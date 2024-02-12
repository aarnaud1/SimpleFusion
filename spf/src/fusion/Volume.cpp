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

#include "spf/fusion/Volume.hpp"
#include <dirent.h>
#include <zlib.h>

namespace spf
{
namespace fusion
{
Volume::Volume(const float voxelRes)
{
  this->nextBlockIndex_ = 0;
  this->voxelRes_ = voxelRes;
}

bool Volume::AddBlock(const BlockId &blockId)
{
  if(blockIds_.find(blockId) != blockIds_.end())
  {
    return false;
  }

  blockIds_[blockId] = nextBlockIndex_;
  nextBlockIndex_++;

  voxelBlocks_.push_back(BlockPtrType(new VoxelBlock(voxelRes_)));
  meshes_.push_back(MeshPtrType(nullptr));

  return true;
}

size_t Volume::AddBlocks(const BlockIdList &blockList)
{
  size_t numAllocated = 0;
  for(size_t i = 0; i < blockList.size(); i++)
  {
    const BlockId &blockId = blockList[i];
    if(blockIds_.find(blockId) != blockIds_.end())
    {
      continue;
    }

    blockIds_[blockId] = nextBlockIndex_;
    nextBlockIndex_++;

    voxelBlocks_.push_back(BlockPtrType(new VoxelBlock(voxelRes_)));
    meshes_.push_back(MeshPtrType(nullptr));
    numAllocated++;
  }

  return numAllocated;
}

Volume::MeshType *Volume::GetMesh(const BlockId &blockId)
{
  if(blockIds_.find(blockId) == blockIds_.end())
  {
    return nullptr;
  }
  return meshes_[blockIds_[blockId]].get();
}

VoxelBlock *Volume::GetBlock(const BlockId &blockId)
{
  if(blockIds_.find(blockId) == blockIds_.end())
  {
    return nullptr;
  }
  return voxelBlocks_[blockIds_[blockId]].get();
}

BlockIdList Volume::GetAllIds() const
{
  BlockIdList ret;
  for(const auto &id : blockIds_)
  {
    ret.push_back(id.first);
  }
  return ret;
}

void Volume::RecomputeMeshes(const BlockIdList &blockList)
{
  START_CHRONO("Update meshes");
#pragma omp parallel shared(blockList)
  {
    MeshType tmp{3 * maxMeshSize_, maxMeshSize_};

#pragma omp for schedule(dynamic)
    for(size_t numBlock = 0; numBlock < blockList.size(); numBlock++)
    {
      ComputeMesh(blockList[numBlock], tmp);
    }
  }
  STOP_CHRONO();
}

void Volume::RecomputeAllMeshes()
{
  START_CHRONO("Update all meshes");
  BlockIdList idList;
  for(auto &entry : blockIds_)
  {
    idList.emplace_back(entry.first);
  }

#pragma omp parallel shared(idList)
  {
    MeshType tmp{3 * maxMeshSize_, maxMeshSize_};

#pragma omp for schedule(dynamic)
    for(size_t numBlock = 0; numBlock < idList.size(); numBlock++)
    {
      ComputeMesh(idList[numBlock], tmp);
    }
  }
  STOP_CHRONO();
}

void Volume::ExportMeshes(const char *filename)
{
  size_t numTriangles = 0;

  utils::Log::Info("Volume", "Recomputing meshes for all blocks...\n");
  for(size_t i = 0; i < blockIds_.size(); i++)
  {
    auto *mesh = meshes_[i].get();
    if(mesh == nullptr)
      continue;

    numTriangles += mesh->NumTriangles();
  }

  FILE *fp = fopen(filename, "w+");
  if(!fp)
  {
    fprintf(stderr, "Error opening %s : %s\n", filename, strerror(errno));
    return;
  }

  fprintf(fp, "ply\n");
  fprintf(fp, "format ascii 1.0\n");
  fprintf(fp, "element vertex %lu\n", 3 * numTriangles);
  fprintf(fp, "property float x\n");
  fprintf(fp, "property float y\n");
  fprintf(fp, "property float z\n");
  fprintf(fp, "property float nx\n");
  fprintf(fp, "property float ny\n");
  fprintf(fp, "property float nz\n");
  fprintf(fp, "property uchar blue\n");
  fprintf(fp, "property uchar green\n");
  fprintf(fp, "property uchar red\n");
  fprintf(fp, "property uchar alpha\n");
  fprintf(fp, "element face %lu\n", numTriangles);
  fprintf(fp, "property list uchar int vertex_index\n");
  fprintf(fp, "end_header\n");

  for(size_t i = 0; i < blockIds_.size(); i++)
  {
    if(i % 100 == 0)
    {
      utils::Log::Info(
          "Volume", "Exporting blocks %lu - %lu over %lu\n", i, std::min(i + 100, blockIds_.size()),
          blockIds_.size());
    }

    MeshType *mesh = meshes_[i].get();
    if(mesh == nullptr)
      continue;

    const Vec3f *__restrict__ vertices = mesh->RawPoints();
    const Vec3f *__restrict__ normals = mesh->RawNormals();
    const Vec3f *__restrict__ colors = mesh->RawColors();

    for(size_t vertexId = 0; vertexId < 3 * mesh->NumTriangles(); vertexId++)
    {
      fprintf(
          fp, "%f %f %f %f %f %f %u %u %u 255\n", vertices[vertexId].x, vertices[vertexId].y,
          vertices[vertexId].z, normals[vertexId].x, normals[vertexId].y, normals[vertexId].z,
          (unsigned char) (255.0f * colors[vertexId].x),
          (unsigned char) (255.0f * colors[vertexId].y),
          (unsigned char) (255.0f * colors[vertexId].z));
    }
  }

  size_t triangleOffset = 0;
  for(size_t i = 0; i < blockIds_.size(); i++)
  {
    MeshType *mesh = meshes_[i].get();
    if(mesh == nullptr)
      continue;

    for(size_t face = triangleOffset; face < triangleOffset + mesh->NumTriangles(); face++)
    {
      fprintf(fp, "3 %lu %lu %lu\n", 3 * face, 3 * face + 1, 3 * face + 2);
    }

    triangleOffset += mesh->NumTriangles();
  }

  fclose(fp);
}

#define WRITE_BLOCK(FP, DATA, T)                                                                   \
  if(gzfwrite(DATA, sizeof(T), BlockProperties<float, 16>::blockVolume, FP)                        \
     != BlockProperties<float, 16>::blockVolume)                                                   \
  {                                                                                                \
    utils::Log::Error("Writing block", "Error writing in %s\n", filename);                         \
    return;                                                                                        \
  }

void Volume::DumpAllBlocks(const char *dir)
{
  for(const auto &id : blockIds_)
  {
    auto *block = voxelBlocks_[id.second].get();
    if(block == nullptr)
    {
      continue;
    }

    char filename[512];
    sprintf(filename, "%s/%d_%d_%d.gz", dir, id.first.x, id.first.y, id.first.z);
    gzFile fp = gzopen(filename, "w6h");
    if(!fp)
    {
      utils::Log::Error("Writing block", "Could not open %s : %s\n", filename, strerror(errno));
      return;
    }

    const bool useColor = block->UseColor();
    gzfwrite(&useColor, 1, sizeof(bool), fp);
    WRITE_BLOCK(fp, block->TSDF(), float);
    WRITE_BLOCK(fp, block->Weights(), float);
    WRITE_BLOCK(fp, block->Gradients(), Vec3f);
    if(useColor)
    {
      WRITE_BLOCK(fp, block->Colors(), Color3f);
    }

    gzclose(fp);
  }
}

#define READ_BLOCK(FP, DATA, T)                                                                    \
  if(gzfread(DATA, sizeof(T), BlockProperties<float, 16>::blockVolume, FP)                         \
     != BlockProperties<float, 16>::blockVolume)                                                   \
  {                                                                                                \
    utils::Log::Error("Reading block", "Error reading in %s\n", filename.c_str());                 \
    return;                                                                                        \
  }

void Volume::PreloadBlocks(const char *dirName)
{
  struct dirent *ent;
  DIR *dir = opendir(dirName);
  if(!dir)
  {
    utils::Log::Error("Preload", "Error : %s is not a valid directory\n", dirName);
    return;
  }

  std::vector<std::string> filenames;
  while((ent = readdir(dir)) != NULL)
  {
    filenames.push_back(std::string(dirName) + "/" + std::string(ent->d_name));
  }
  closedir(dir);

  auto splitString = [](std::string input, std::string delim) -> std::vector<std::string> {
    auto start = 0U;
    auto end = input.find(delim);

    std::vector<std::string> ret;
    while(end != std::string::npos)
    {
      ret.push_back(input.substr(start, end - start));
      start = end + delim.length();
      end = input.find(delim, start);
    }
    ret.push_back(input.substr(start, end - start));
    return ret;
  };

  for(const auto &filename : filenames)
  {
    auto splittedName = splitString(filename, std::string("."));
    if(splittedName.back() != std::string("gz"))
    {
      continue;
    }

    auto stringId = splitString(splittedName[0], std::string("/"));
    stringId = splitString(stringId.back(), "_");
    const int x = std::stoi(stringId[0]);
    const int y = std::stoi(stringId[1]);
    const int z = std::stoi(stringId[2]);
    const BlockId blockId(x, y, z);

    utils::Log::Info("Preload", "Reading block : %d %d %d\n", x, y, z);
    AddBlock(blockId);

    auto *block = GetBlock(blockId);
    if(block == nullptr)
    {
      continue;
    }

    gzFile fp = gzopen(filename.c_str(), "rb");
    if(!fp)
    {
      utils::Log::Error(
          "Reading block", "Could not open %s : %s\n", filename.c_str(), strerror(errno));
      return;
    }

    bool useColor;
    gzfread(&useColor, sizeof(bool), 1, fp);
    READ_BLOCK(fp, block->TSDF(), float);
    READ_BLOCK(fp, block->Weights(), float);
    READ_BLOCK(fp, block->Gradients(), Vec3f);
    if(useColor)
    {
      READ_BLOCK(fp, block->Colors(), Color3f);
    }

    gzclose(fp);
  }
}

size_t Volume::ComputeMesh(const BlockId &blockId, MeshType &tmp)
{
  const size_t id = blockIds_[blockId];
  const BlockId b0 = blockId + BlockId(0, 0, 0);
  const Vec3f org =
      (float) BlockProperties<float, 16>::blockSize * voxelRes_ * Vec3f(b0.x, b0.y, b0.z);

  const BlockId bxx = blockId + BlockId(1, 0, 0);
  const BlockId byy = blockId + BlockId(0, 1, 0);
  const BlockId bzz = blockId + BlockId(0, 0, 1);
  const BlockId bxy = blockId + BlockId(1, 1, 0);
  const BlockId bxz = blockId + BlockId(1, 0, 1);
  const BlockId byz = blockId + BlockId(0, 1, 1);
  const BlockId bxyz = blockId + BlockId(1, 1, 1);

  // Empty block
  if(voxelBlocks_[id] == nullptr)
  {
    return 0;
  }

  float *points = reinterpret_cast<float *>(tmp.RawPoints());
  float *colors = reinterpret_cast<float *>(tmp.RawColors());
  float *normals = reinterpret_cast<float *>(tmp.RawNormals());

  const float *tsdf = voxelBlocks_[id]->TSDF();
  const float *rgb = (float *) voxelBlocks_[id]->Colors();
  const float *grad = (float *) voxelBlocks_[id]->Gradients();

  float *xx = nullptr;
  float *yy = nullptr;
  float *zz = nullptr;
  float *xy = nullptr;
  float *xz = nullptr;
  float *yz = nullptr;
  float *xyz = nullptr;

  float *cxx = nullptr;
  float *cyy = nullptr;
  float *czz = nullptr;
  float *cxy = nullptr;
  float *cxz = nullptr;
  float *cyz = nullptr;
  float *cxyz = nullptr;

  float *gxx = nullptr;
  float *gyy = nullptr;
  float *gzz = nullptr;
  float *gxy = nullptr;
  float *gxz = nullptr;
  float *gyz = nullptr;
  float *gxyz = nullptr;

  if(blockIds_.find(bxx) != blockIds_.end())
  {
    xx = voxelBlocks_[blockIds_[bxx]]->TSDF();
    cxx = (float *) voxelBlocks_[blockIds_[bxx]]->Colors();
    gxx = (float *) voxelBlocks_[blockIds_[bxx]]->Gradients();
  }

  if(blockIds_.find(byy) != blockIds_.end())
  {
    yy = voxelBlocks_[blockIds_[byy]]->TSDF();
    cyy = (float *) voxelBlocks_[blockIds_[byy]]->Colors();
    gyy = (float *) voxelBlocks_[blockIds_[byy]]->Gradients();
  }

  if(blockIds_.find(bzz) != blockIds_.end())
  {
    zz = voxelBlocks_[blockIds_[bzz]]->TSDF();
    czz = (float *) voxelBlocks_[blockIds_[bzz]]->Colors();
    gzz = (float *) voxelBlocks_[blockIds_[bzz]]->Gradients();
  }

  if(blockIds_.find(bxy) != blockIds_.end())
  {
    xy = voxelBlocks_[blockIds_[bxy]]->TSDF();
    cxy = (float *) voxelBlocks_[blockIds_[bxy]]->Colors();
    gxy = (float *) voxelBlocks_[blockIds_[bxy]]->Gradients();
  }

  if(blockIds_.find(bxz) != blockIds_.end())
  {
    xz = voxelBlocks_[blockIds_[bxz]]->TSDF();
    cxz = (float *) voxelBlocks_[blockIds_[bxz]]->Colors();
    gxz = (float *) voxelBlocks_[blockIds_[bxz]]->Gradients();
  }

  if(blockIds_.find(byz) != blockIds_.end())
  {
    yz = voxelBlocks_[blockIds_[byz]]->TSDF();
    cyz = (float *) voxelBlocks_[blockIds_[byz]]->Colors();
    gyz = (float *) voxelBlocks_[blockIds_[byz]]->Gradients();
  }

  if(blockIds_.find(bxyz) != blockIds_.end())
  {
    xyz = voxelBlocks_[blockIds_[bxyz]]->TSDF();
    cxyz = (float *) voxelBlocks_[blockIds_[bxyz]]->Colors();
    gxyz = (float *) voxelBlocks_[blockIds_[bxyz]]->Gradients();
  }

  const size_t numTriangles = spf::mc::extractMesh(
      tsdf, xx, yy, zz, xy, xz, yz, xyz, rgb, cxx, cyy, czz, cxy, cxz, cyz, cxyz, grad, gxx, gyy,
      gzz, gxy, gxz, gyz, gxyz, points, colors, normals, BlockProperties<float, 16>::blockSize,
      voxelRes_, (float *) &org);
  tmp.Resize(3 * numTriangles, numTriangles);

  if(meshes_[id].get())
  {
    meshes_[id].reset();
  }

  if(numTriangles > 0)
  {
    meshes_[id] = std::unique_ptr<MeshType>(new MeshType(3 * numTriangles, numTriangles));
    meshes_[id]->Resize(3 * numTriangles, numTriangles);
    memcpy(
        reinterpret_cast<float *>(meshes_[id]->RawPoints()),
        reinterpret_cast<float *>(tmp.RawPoints()), 3 * numTriangles * sizeof(Point3f));
    memcpy(
        reinterpret_cast<float *>(meshes_[id]->RawColors()),
        reinterpret_cast<float *>(tmp.RawColors()), 3 * numTriangles * sizeof(Color3f));
    memcpy(
        reinterpret_cast<float *>(meshes_[id]->RawNormals()),
        reinterpret_cast<float *>(tmp.RawNormals()), 3 * numTriangles * sizeof(Vec3f));
  }

  return numTriangles;
}
} // namespace fusion
} // namespace spf
