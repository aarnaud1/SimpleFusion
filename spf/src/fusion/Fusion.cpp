/*
 * Copyright (C) 2024 Adrien ARNAUD
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "spf/fusion/Fusion.hpp"
#include "spf/utils.hpp"

#include <omp.h>

namespace spf
{
namespace fusion
{
Fusion::Fusion(
    const float voxelRes, const float integrationDistance, const size_t maxDepthMapWidth,
    const size_t maxDepthMapHeight) :
    voxelRes_(voxelRes),
    tau_(integrationDistance),
    maxDepthMapWidth_(maxDepthMapWidth),
    maxDepthMapHeight_(maxDepthMapHeight),
    volume_(voxelRes_)
{}

Fusion::~Fusion() {}

void Fusion::IntegrateDepthMap(
    const FrameType &depthMap, const IntrinsicsType &intrinsics, const Mat4f &transform,
    const float near, const size_t far)
{
  utils::Log::Info("Fusion", "Integrating point cloud\n");
  static PointCloudType inputCloud(maxDepthMapWidth_ * maxDepthMapHeight_);
  inputCloud.Clear();

  newBlocks_.clear();
  intersectingBlocks_.clear();

  CHRONO((depthMap.ExtractPoints<PointType, float>(inputCloud, intrinsics, near, far, 5000.0f)));
  inputCloud.Transform(transform);

  const Point3f c = transform * Point3f(0.0f, 0.0f, 0.0f);
  GetBlocksIntersecting(inputCloud, c);

  for(const BlockId &id : intersectingBlocks_)
  {
    newBlocks_.emplace_back(id);
  }

  const size_t numAllocated = volume_.AddBlocks(newBlocks_);
  utils::Log::Info("Fusion", "There are %lu blocks intersecting\n", newBlocks_.size());
  utils::Log::Info("Fusion", "Allocated %lu new blocks\n", numAllocated);
  utils::Log::Info("Fusion", "Total blocks stored : %lu\n", volume_.NumBlocks());

  IntegratePointCloud(inputCloud, c);
  // UpdateGradients();
}

void Fusion::IntegrateDepthMapOrdered(
    const FrameType &depthMap, const IntrinsicsType &intrinsics, const Mat4f &transform,
    const float near, const size_t far)
{
  utils::Log::Info("Fusion", "Integrating OPC\n");
  OPCType inputCloud(depthMap.Width(), depthMap.Height());

  newBlocks_.clear();
  intersectingBlocks_.clear();

  CHRONO((depthMap.ExtractPoints<OPCPointType, float>(inputCloud, intrinsics, near, far, 5000.0f)));
  CHRONO(inputCloud.Transform(transform));
  CHRONO(inputCloud.EstimateNormals(5.0f * voxelRes_));

  GetBlocksIntersecting(inputCloud);

  START_CHRONO("Inserting new blocks");
  for(const BlockId &id : intersectingBlocks_)
  {
    newBlocks_.emplace_back(id);
  }
  STOP_CHRONO();

  const size_t numAllocated = volume_.AddBlocks(newBlocks_);
  utils::Log::Info("Fusion", "There are %lu blocks intersecting\n", newBlocks_.size());
  utils::Log::Info("Fusion", "Allocated %lu new blocks\n", numAllocated);
  utils::Log::Info("Fusion", "Total blocks stored : %lu\n", volume_.NumBlocks());

  IntegratePointCloud(inputCloud);
  // UpdateGradients();
}

void Fusion::UpdateMeshes()
{
  UpdateGradients();
  volume_.RecomputeMeshes(newBlocks_);
}

void Fusion::RecomputeMeshes()
{
  UpdateAllGradients();
  volume_.RecomputeAllMeshes();
}

void Fusion::ExportMesh(const char *filename) { volume_.ExportMeshes(filename); }

std::vector<std::pair<BlockId, Fusion::MeshType *>> Fusion::GetMeshesForDisplay(
    Mat4f const &transform, const float near, const float far, const float fov,
    Mat4f const &OPENGL_TO_CAM)
{
  std::vector<std::pair<BlockId, MeshType *>> ret;

  const float tanFov = std::tan(0.5f * (fov * M_PI) / 180.0f);
  const Vec4f dir(0.0f, 0.0f, -1.0f, 0.0f);

  Vec4f corners[8];

  corners[0] = dir * near + Vec4f(-far * tanFov, -far * tanFov, 0.0f, 1.0f);
  corners[1] = dir * near + Vec4f(far * tanFov, -far * tanFov, 0.0f, 1.0f);
  corners[2] = dir * near + Vec4f(-far * tanFov, far * tanFov, 0.0f, 1.0f);
  corners[3] = dir * near + Vec4f(far * tanFov, far * tanFov, 0.0f, 1.0f);

  corners[4] = dir * far + Vec4f(-far * tanFov, -far * tanFov, 0.0f, 1.0f);
  corners[5] = dir * far + Vec4f(far * tanFov, -far * tanFov, 0.0f, 1.0f);
  corners[6] = dir * far + Vec4f(-far * tanFov, far * tanFov, 0.0f, 1.0f);
  corners[7] = dir * far + Vec4f(far * tanFov, far * tanFov, 0.0f, 1.0f);

  // Compute all the frustum points
  corners[0] = transform * OPENGL_TO_CAM * corners[0];
  corners[1] = transform * OPENGL_TO_CAM * corners[1];
  corners[2] = transform * OPENGL_TO_CAM * corners[2];
  corners[3] = transform * OPENGL_TO_CAM * corners[3];
  corners[4] = transform * OPENGL_TO_CAM * corners[4];
  corners[5] = transform * OPENGL_TO_CAM * corners[5];
  corners[6] = transform * OPENGL_TO_CAM * corners[6];
  corners[7] = transform * OPENGL_TO_CAM * corners[7];

  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float minZ = std::numeric_limits<float>::max();

  float maxX = -std::numeric_limits<float>::max();
  float maxY = -std::numeric_limits<float>::max();
  float maxZ = -std::numeric_limits<float>::max();

#pragma GCC unroll 8
  for(size_t i = 0; i < 8; i++)
  {
    minX = std::min(minX, corners[i].x);
    minY = std::min(minY, corners[i].y);
    minZ = std::min(minZ, corners[i].z);

    maxX = std::max(maxX, corners[i].x);
    maxY = std::max(maxY, corners[i].y);
    maxZ = std::max(maxZ, corners[i].z);
  }

  const BlockId b0 = GetId(Vec3f(minX, minY, minZ), voxelRes_);
  const BlockId b1 = GetId(Vec3f(maxX, maxY, maxZ), voxelRes_);

  ret.clear();
  for(int i = b0.x; i <= b1.x; i++)
  {
    for(int j = b0.y; j <= b1.y; j++)
    {
      for(int k = b0.z; k <= b1.z; k++)
      {
        const BlockId id(i, j, k);
        MeshType *ptr = volume_.GetMesh(id);
        if(ptr != NULL)
        {
          if(ptr->NumTriangles() > 0)
          {
            ret.emplace_back(std::pair<BlockId, MeshType *>(id, ptr));
          }
        }
      }
    }
  }

  return ret;
}

void Fusion::DumpAllBlocks(const char *dir) { volume_.DumpAllBlocks(dir); }

void Fusion::PreloadBlocks(const char *dir) { volume_.PreloadBlocks(dir); }

void Fusion::ClearData() {}

void Fusion::GetBlocksIntersecting(PointCloudType const &inputCloud, const Point3f &cameraCenter)
{
  START_CHRONO("Get blocks intersecting");
#pragma omp parallel
  {
    std::set<BlockId> foundIds;
#pragma omp for
    for(size_t i = 0; i < inputCloud.Size(); i++)
    {
      const auto org = inputCloud.Points()[i];
      const Vec3f u = Vec3f::Normalize(org - cameraCenter);
      const Point3f first = org - tau_ * u;
      const Point3f last = org + tau_ * u;
      const BlockId minId = GetId(first, voxelRes_);
      const BlockId maxId = GetId(last, voxelRes_);

      RaycastVoxels(minId, maxId, foundIds);
    }

#pragma omp critical
    {
      intersectingBlocks_.insert(foundIds.begin(), foundIds.end());
    }
  } // omp parallel
  STOP_CHRONO();
}

void Fusion::GetBlocksIntersecting(OPCType const &opc)
{
  START_CHRONO("Get blocks intersecting OPC");
#pragma omp parallel
  {
    std::set<BlockId> foundIds;
#pragma omp for
    for(size_t i = 0; i < opc.Height(); i++)
    {
      for(size_t j = 0; j < opc.Width(); j++)
      {
        const Point3f p = opc.Points(i, j);
        const Vec3f n = opc.Normals(i, j);

        if(p.x == FLT_MAX)
        {
          continue;
        }

        if(n.x == 0 && n.y == 0 && n.z == 0)
        {
          continue;
        }

        const Point3f first = p + tau_ * n;
        const Point3f last = p - tau_ * n;
        const BlockId minId = GetId(first, voxelRes_);
        const BlockId maxId = GetId(last, voxelRes_);

        RaycastVoxels(minId, maxId, foundIds);
      }
    }

#pragma omp critical
    {
      intersectingBlocks_.insert(foundIds.begin(), foundIds.end());
    }
  } // omp parallel
  STOP_CHRONO();
}

void Fusion::IntegratePointCloud(PointCloudType const &inputCloud, const Point3f &cameraCenter)
{
  START_CHRONO("Integrate point cloud");
  const float step = 0.5f * voxelRes_;
  const float sigma = tau_;
  const float tsdfFact = 1.0f / (2.0f * sigma * sigma);
  const float coeff = 1.0f / (sigma * sqrtf(2 * M_PI));

#pragma omp parallel for schedule(static)
  for(size_t i = 0; i < inputCloud.Size(); i++)
  {
    const Point3f org = inputCloud.Points()[i];
    const Color3f rgb = inputCloud.Colors()[i];
    const Vec3f u = Vec3f::Normalize(org - cameraCenter);

    for(float dist = tau_; dist > -tau_; dist -= step)
    {
      const Point3f pos = org - dist * u;
      const BlockId id = GetId(pos, voxelRes_);
      const Index3d voxelId = GetVoxelId(pos, voxelRes_);
      const Point3f voxelPos = GetVoxelPos(GetVoxelAbsolutePos(id, voxelId), voxelRes_);
      const float tsdf = Vec3f::Dot(u, org - voxelPos) >= 0.0f ? Point3f::Dist(voxelPos, org)
                                                               : -Point3f::Dist(voxelPos, org);

      // Update volume TSDF
      VoxelBlock *voxelBlock = volume_.GetBlock(id);
      if(voxelBlock == NULL)
      {
        continue;
      }
      const size_t offset = voxelId.x + voxelId.y * BlockProperties<float, 16>::blockSize
                            + voxelId.z * BlockProperties<float, 16>::blockSize
                                  * BlockProperties<float, 16>::blockSize;

      const float weight = coeff * expf(-(tsdf * tsdf) * tsdfFact);
      float *__restrict tsdfPtr = voxelBlock->TSDF();
      Color3f *__restrict colorsPtr = voxelBlock->Colors();
      float *__restrict weightsPtr = voxelBlock->Weights();

      const float weightSum = weight + weightsPtr[offset];
      tsdfPtr[offset] = (weightsPtr[offset] * tsdfPtr[offset] + weight * tsdf) / weightSum;
      colorsPtr[offset] = (weightsPtr[offset] * colorsPtr[offset] + weight * rgb) / weightSum;
      weightsPtr[offset] += weight;
    }
  }
  STOP_CHRONO();
}

void Fusion::IntegratePointCloud(OPCType const &opc)
{
  START_CHRONO("Integrate OPC");
  const float step = 0.5f * voxelRes_;
  const float sigma = tau_;
  const float tsdfFact = 1.0f / (2.0f * sigma * sigma);
  const float coeff = 1.0f / (sigma * sqrtf(2 * M_PI));

#pragma omp parallel for schedule(static)
  for(size_t i = 0; i < opc.Height(); i++)
  {
    for(size_t j = 0; j < opc.Width(); j++)
    {
      const Point3f org = opc.Points(i, j);
      const Vec3f u = opc.Normals(i, j);

      if(org.x == FLT_MAX)
      {
        continue;
      }

      if(u.x == 0 && u.y == 0 && u.z == 0)
      {
        continue;
      }

      if(u.x == FLT_MAX || u.y == FLT_MAX || u.z == FLT_MAX)
      {
        continue;
      }

      const Color3f rgb = opc.Colors(i, j);

      for(float dist = tau_; dist > -tau_; dist -= step)
      {
        const Point3f pos = org - dist * u;
        const BlockId id = GetId(pos, voxelRes_);
        const Index3d voxelId = GetVoxelId(pos, voxelRes_);
        const Point3f voxelPos = GetVoxelPos(GetVoxelAbsolutePos(id, voxelId), voxelRes_);
        const float tsdf = Vec3f::Dot(u, org - voxelPos) >= 0.0f ? Point3f::Dist(voxelPos, org)
                                                                 : -Point3f::Dist(voxelPos, org);

        // Update volume TSDF
        VoxelBlock *voxelBlock = volume_.GetBlock(id);
        if(voxelBlock == NULL)
        {
          continue;
        }
        const size_t offset = voxelId.x + voxelId.y * BlockProperties<float, 16>::blockSize
                              + voxelId.z * BlockProperties<float, 16>::blockSize
                                    * BlockProperties<float, 16>::blockSize;
        const float weight = coeff * expf(-(tsdf * tsdf) * tsdfFact);

        float *__restrict tsdfPtr = voxelBlock->TSDF();
        Color3f *__restrict colorsPtr = voxelBlock->Colors();
        float *__restrict weightsPtr = voxelBlock->Weights();

        const float weightSum = weight + weightsPtr[offset];
        tsdfPtr[offset] = (weightsPtr[offset] * tsdfPtr[offset] + weight * tsdf) / weightSum;
        colorsPtr[offset] = (weightsPtr[offset] * colorsPtr[offset] + weight * rgb) / weightSum;
        weightsPtr[offset] += weight;
      }
    }
  }
  STOP_CHRONO();
}

void Fusion::UpdateGradients()
{
  START_CHRONO("Update gradients");
  static const Index3d BLOCK_DIM(
      1, BlockProperties<float, 16>::blockSize,
      BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize);
  static const Index3d PBLOCK_SIZE(
      1, BlockProperties<float, 16>::blockSize + 2,
      (BlockProperties<float, 16>::blockSize + 2) * (BlockProperties<float, 16>::blockSize + 2));

#pragma omp parallel
  {
    float *packedTSDF = (float *) malloc(
        ((BlockProperties<float, 16>::blockSize + 2) * (BlockProperties<float, 16>::blockSize + 2)
         * (BlockProperties<float, 16>::blockSize + 2))
        * sizeof(float));

#pragma omp for
    for(size_t id = 0; id < newBlocks_.size(); id++)
    {
      const BlockId &blockId = newBlocks_[id];
      if(volume_.GetBlock(blockId) == NULL)
      {
        continue;
      }

      Vec3f *gradPtr = volume_.GetBlock(blockId)->Gradients();

      PackTSDF(blockId, packedTSDF);

      for(size_t k = 1; k < BlockProperties<float, 16>::blockSize + 1; k++)
      {
        for(size_t j = 1; j < BlockProperties<float, 16>::blockSize + 1; j++)
        {
          for(size_t i = 1; i < BlockProperties<float, 16>::blockSize + 1; i++)
          {
            const float dx = packedTSDF[Dot(Index3d(i + 1, j, k), PBLOCK_SIZE)]
                             - packedTSDF[Dot(Index3d(i - 1, j, k), PBLOCK_SIZE)];
            const float dy = packedTSDF[Dot(Index3d(i, j + 1, k), PBLOCK_SIZE)]
                             - packedTSDF[Dot(Index3d(i, j - 1, k), PBLOCK_SIZE)];
            const float dz = packedTSDF[Dot(Index3d(i, j, k + 1), PBLOCK_SIZE)]
                             - packedTSDF[Dot(Index3d(i, j, k - 1), PBLOCK_SIZE)];

            gradPtr[Dot(Index3d(i - 1, j - 1, k - 1), BLOCK_DIM)] = Vec3f(dx, dy, dz) / voxelRes_;
          }
        }
      }
    }

    free(packedTSDF);
  } // omp parallel
  STOP_CHRONO();
}

void Fusion::UpdateAllGradients()
{
  START_CHRONO("Update all gradients");
  static const Index3d BLOCK_DIM(
      1, BlockProperties<float, 16>::blockSize,
      BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize);
  static const Index3d PBLOCK_SIZE(
      1, BlockProperties<float, 16>::blockSize + 2,
      (BlockProperties<float, 16>::blockSize + 2) * (BlockProperties<float, 16>::blockSize + 2));

  BlockIdList allBlocks = volume_.GetAllIds();
#pragma omp parallel shared(allBlocks)
  {
    float *packedTSDF = (float *) malloc(
        ((BlockProperties<float, 16>::blockSize + 2) * (BlockProperties<float, 16>::blockSize + 2)
         * (BlockProperties<float, 16>::blockSize + 2))
        * sizeof(float));

#pragma omp for
    for(size_t id = 0; id < allBlocks.size(); id++)
    {
      const BlockId &blockId = allBlocks[id];
      if(volume_.GetBlock(blockId) == NULL)
      {
        continue;
      }

      Vec3f *gradPtr = volume_.GetBlock(blockId)->Gradients();

      PackTSDF(blockId, packedTSDF);

      for(size_t k = 1; k < BlockProperties<float, 16>::blockSize + 1; k++)
      {
        for(size_t j = 1; j < BlockProperties<float, 16>::blockSize + 1; j++)
        {
          for(size_t i = 1; i < BlockProperties<float, 16>::blockSize + 1; i++)
          {
            const float dx = packedTSDF[Dot(Index3d(i + 1, j, k), PBLOCK_SIZE)]
                             - packedTSDF[Dot(Index3d(i - 1, j, k), PBLOCK_SIZE)];
            const float dy = packedTSDF[Dot(Index3d(i, j + 1, k), PBLOCK_SIZE)]
                             - packedTSDF[Dot(Index3d(i, j - 1, k), PBLOCK_SIZE)];
            const float dz = packedTSDF[Dot(Index3d(i, j, k + 1), PBLOCK_SIZE)]
                             - packedTSDF[Dot(Index3d(i, j, k - 1), PBLOCK_SIZE)];

            gradPtr[Dot(Index3d(i - 1, j - 1, k - 1), BLOCK_DIM)] = Vec3f(dx, dy, dz) / voxelRes_;
          }
        }
      }
    }

    free(packedTSDF);
  } // omp parallel
  STOP_CHRONO();
}

void Fusion::PackTSDF(const BlockId &blockId, float *__restrict__ packedTSDF)
{
  const Index3d BLOCK_DIM(
      1, BlockProperties<float, 16>::blockSize,
      BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize);
  const Index3d PBLOCK_SIZE(
      1, BlockProperties<float, 16>::blockSize + 2,
      (BlockProperties<float, 16>::blockSize + 2) * (BlockProperties<float, 16>::blockSize + 2));

  const BlockId bMinusX = blockId - BlockId(1, 0, 0);
  const BlockId bPlusX = blockId + BlockId(1, 0, 0);
  const BlockId bMinusY = blockId - BlockId(0, 1, 0);
  const BlockId bPlusY = blockId + BlockId(0, 1, 0);
  const BlockId bMinusZ = blockId - BlockId(0, 0, 1);
  const BlockId bPlusZ = blockId + BlockId(0, 0, 1);

  const float *TSDF = volume_.GetBlock(blockId)->TSDF();
  const float *mXTSDF =
      volume_.GetBlock(bMinusX) != NULL ? volume_.GetBlock(bMinusX)->TSDF() : NULL;
  const float *pXTSDF = volume_.GetBlock(bPlusX) != NULL ? volume_.GetBlock(bPlusX)->TSDF() : NULL;
  const float *mYTSDF =
      volume_.GetBlock(bMinusY) != NULL ? volume_.GetBlock(bMinusY)->TSDF() : NULL;
  const float *pYTSDF = volume_.GetBlock(bPlusY) != NULL ? volume_.GetBlock(bPlusY)->TSDF() : NULL;
  const float *mZTSDF =
      volume_.GetBlock(bMinusZ) != NULL ? volume_.GetBlock(bMinusZ)->TSDF() : NULL;
  const float *pZTSDF = volume_.GetBlock(bPlusZ) != NULL ? volume_.GetBlock(bPlusZ)->TSDF() : NULL;

  // Along X axis
  if(mXTSDF != NULL)
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
      {
        packedTSDF[Dot(Index3d(0, j + 1, k + 1), PBLOCK_SIZE)] =
            mXTSDF[Dot(Index3d(BlockProperties<float, 16>::blockSize - 1, j, k), BLOCK_DIM)];
      }
    }
  }
  else
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
      {
        packedTSDF[Dot(Index3d(0, j + 1, k + 1), PBLOCK_SIZE)] = 0;
      }
    }
  }

  if(pXTSDF != NULL)
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
      {
        packedTSDF[Dot(
            Index3d(BlockProperties<float, 16>::blockSize + 1, j + 1, k + 1), PBLOCK_SIZE)] =
            pXTSDF[Dot(Index3d(0, j, k), BLOCK_DIM)];
      }
    }
  }
  else
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
      {
        packedTSDF[Dot(
            Index3d(BlockProperties<float, 16>::blockSize + 1, j + 1, k + 1), PBLOCK_SIZE)] = 0;
      }
    }
  }

  // Along Y axis
  if(mYTSDF != NULL)
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(Index3d(i + 1, 0, k + 1), PBLOCK_SIZE)] =
            mYTSDF[Dot(Index3d(i, BlockProperties<float, 16>::blockSize - 1, k), BLOCK_DIM)];
      }
    }
  }
  else
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(Index3d(i + 1, 0, k + 1), PBLOCK_SIZE)] = 0;
      }
    }
  }

  if(pYTSDF != NULL)
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(
            Index3d(i + 1, BlockProperties<float, 16>::blockSize + 1, k + 1), PBLOCK_SIZE)] =
            pYTSDF[Dot(Index3d(i, 0, k), BLOCK_DIM)];
      }
    }
  }
  else
  {
    for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(
            Index3d(i + 1, BlockProperties<float, 16>::blockSize + 1, k + 1), PBLOCK_SIZE)] = 0;
      }
    }
  }

  // Along Z axis
  if(mZTSDF != NULL)
  {
    for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(Index3d(i + 1, j + 1, 0), PBLOCK_SIZE)] =
            mZTSDF[Dot(Index3d(i, j, BlockProperties<float, 16>::blockSize - 1), BLOCK_DIM)];
      }
    }
  }
  else
  {
    for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(Index3d(i + 1, j + 1, 0), PBLOCK_SIZE)] = 0;
      }
    }
  }

  if(pZTSDF != NULL)
  {
    for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(
            Index3d(i + 1, j + 1, BlockProperties<float, 16>::blockSize + 1), PBLOCK_SIZE)] =
            pZTSDF[Dot(Index3d(i, j, 0), BLOCK_DIM)];
      }
    }
  }
  else
  {
    for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(
            Index3d(i + 1, j + 1, BlockProperties<float, 16>::blockSize + 1), PBLOCK_SIZE)] = 0;
      }
    }
  }

  // Inner block
  for(size_t k = 0; k < BlockProperties<float, 16>::blockSize; k++)
  {
    for(size_t j = 0; j < BlockProperties<float, 16>::blockSize; j++)
    {
      for(size_t i = 0; i < BlockProperties<float, 16>::blockSize; i++)
      {
        packedTSDF[Dot(Index3d(i + 1, j + 1, k + 1), PBLOCK_SIZE)] =
            TSDF[Dot(Index3d(i, j, k), BLOCK_DIM)];
      }
    }
  }
}

// From : https://gist.github.com/yamamushi/5823518
void Fusion::RaycastVoxels(const Index3d &minId, const Index3d &maxId, std::set<Index3d> &foundIds)
{
  const int dx = maxId.x - minId.x;
  const int dy = maxId.y - minId.y;
  const int dz = maxId.z - minId.z;

  const int incX = dx == 0 ? 0 : dx < 0 ? -1 : 1;
  const int incY = dy == 0 ? 0 : dy < 0 ? -1 : 1;
  const int incZ = dz == 0 ? 0 : dz < 0 ? -1 : 1;

  const int l = std::abs(dx);
  const int m = std::abs(dy);
  const int n = std::abs(dz);

  const int dx2 = dx << 1;
  const int dy2 = dy << 1;
  const int dz2 = dz << 2;

  int err1;
  int err2;

  BlockId id(minId);

  if(l >= m && l >= n)
  {
    err1 = dy2 - l;
    err2 = dz2 - l;
    for(int i = 0; i <= l; i++)
    {
      foundIds.emplace(id);
      if(err1 > 0)
      {
        id.y += incY;
        err1 -= dx2;
      }
      if(err2 > 0)
      {
        id.z += incZ;
        err2 -= dx2;
      }
      err1 += dy2;
      err2 += dz2;
      id.x += incX;
    }
  }

  else if(m >= l && m >= n)
  {
    err1 = dx2 - m;
    err2 = dz2 - m;
    for(int i = 0; i <= m; i++)
    {
      foundIds.emplace(id);
      if(err1 > 0)
      {
        id.x += incX;
        err1 -= dy2;
      }
      if(err2 > 0)
      {
        id.z += incZ;
        err2 -= dy2;
      }
      err1 += dx2;
      err2 += dz2;
      id.y += incY;
    }
  }
  else
  {
    err1 = dy2 - n;
    err2 = dx2 - n;
    for(int i = 0; i <= n; i++)
    {
      foundIds.emplace(id);
      if(err1 > 0)
      {
        id.y += incY;
        err1 -= dz2;
      }
      if(err2 > 0)
      {
        id.x += incX;
        err2 -= dz2;
      }
      err1 += dy2;
      err2 += dx2;
      id.z += incZ;
    }
  }
  foundIds.emplace(id);
}
} // namespace fusion
} // namespace spf
