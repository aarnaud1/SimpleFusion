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

#pragma once

#include <vector>
#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "spf/geometry/geometry.hpp"

#include "spf/Types.hpp"
#include "spf/data_types/PointCloud.hpp"
#include "spf/data_types/OrderedPointCloud.hpp"
#include "spf/data_types/Mesh.hpp"
#include "spf/data_types/RGBDFrame.hpp"
#include "spf/fusion/VoxelBlock.hpp"
#include "spf/fusion/Volume.hpp"

namespace spf
{
namespace fusion
{
using namespace data_types;
class Fusion
{
public:
  using IntrinsicsType = CameraIntrinsics<float>;
  using FrameType = RGBDFrame<uint16_t, uint8_t>;
  using PointType = PointXYZRGB<float>;
  using OPCPointType = PointXYZRGBN<float>;
  using PointCloudType = PointCloud<PointType>;
  using OPCType = OrderedPointCloud<OPCPointType>;
  using MeshType = typename Volume::MeshType;

  Fusion(
      const float voxelRes, const float integrationDistance, const size_t maxDepthMapWidth,
      const size_t maxDepthMapHeight);

  ~Fusion();

  void IntegrateDepthMap(
      const FrameType &depthMap, const IntrinsicsType &intinsics, const Mat4f &transform,
      const float near = 0.0f, const size_t far = 5.0f);

  void IntegrateDepthMapOrdered(
      const FrameType &depthMap, const IntrinsicsType &interinsics, const Mat4f &transform,
      const float near = 0.0f, const size_t far = 5.0f);

  void UpdateMeshes();

  void RecomputeMeshes();

  void ExportMesh(const char *filename);

  void DumpAllBlocks(const char *dir);

  void PreloadBlocks(const char *dir);

  void ClearData();

  std::vector<std::pair<BlockId, MeshType *>> GetMeshesForDisplay(
      Mat4f const &transform, const float near, const float far, const float fov,
      Mat4f const &OPENGL_TO_CAM);

private:
  float voxelRes_;
  float tau_;
  size_t maxDepthMapWidth_;
  size_t maxDepthMapHeight_;

  Volume volume_;
  BlockUpdateList intersectingBlocks_;
  BlockIdList newBlocks_;

  void GetBlocksIntersecting(PointCloudType const &pointCloud, const Point3f &cameraCenter);

  void GetBlocksIntersecting(OPCType const &opc);

  void IntegratePointCloud(PointCloudType const &pointCloud, const Point3f &cameraCenter);

  void IntegratePointCloud(OPCType const &opc);

  void UpdateGradients();

  void UpdateAllGradients();

  void RaycastVoxels(const Index3d &minId, const Index3d &maxId, std::set<Index3d> &foundIds);

  void PackTSDF(const BlockId &blockId, float *packedTSDF);
};
} // namespace fusion
} // namespace spf
