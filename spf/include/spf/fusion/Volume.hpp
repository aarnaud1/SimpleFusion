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
#include <set>
#include <memory>
#include <unordered_map>

#include "spf/utils.hpp"
#include "spf/Types.hpp"
#include "spf/data_types/Mesh.hpp"
#include "spf/fusion/BlockUtils.hpp"
#include "spf/fusion/VoxelBlock.hpp"
#include "spf/marching_cubes/MarchingCubes.hpp"

namespace spf
{
namespace fusion
{
using namespace data_types;

using BlockIdList = std::vector<BlockId>;
using BlockIdMap = std::unordered_map<BlockId, int, ChunkHasher>;
using BlockList = std::vector<std::unique_ptr<VoxelBlock>>;
using MeshList = std::vector<std::unique_ptr<data_types::Mesh<data_types::PointXYZRGBN<float>>>>;
using BlockUpdateList = std::set<BlockId>;

class Volume
{
  // TODO : support voxel block suppression
public:
  using MeshType = data_types::Mesh<data_types::PointXYZRGBN<float>>;
  using MeshPtrType = std::unique_ptr<MeshType>;
  using BlockPtrType = std::unique_ptr<VoxelBlock>;

  Volume(const float voxelRes);

  bool AddBlock(const BlockId &blockId);

  size_t AddBlocks(const BlockIdList &blockIds);

  // void RemoveBlock(const BlockId &blockId);

  // void RemoveBlocks(const BlockId &blockIds);

  inline bool Find(const BlockId &blockId) { return blockIds_.find(blockId) != blockIds_.end(); }

  MeshType *GetMesh(const BlockId &blockId);

  inline MeshList &GetMeshes() { return meshes_; }

  VoxelBlock *GetBlock(const BlockId &blockId);

  inline BlockList &GetVoxelBlocks() { return voxelBlocks_; }

  inline size_t NumBlocks() const { return blockIds_.size(); }

  inline float VoxelRes() const { return voxelRes_; }

  BlockIdList GetAllIds() const;

  void RecomputeMeshes(const BlockIdList &blockList);

  void RecomputeAllMeshes();

  void ExportMeshes(const char *filename);

  void DumpAllBlocks(const char *dir);

  void PreloadBlocks(const char *dirName);

  void ClearData();

private:
  static constexpr size_t maxMeshSize_ = 2 * BlockProperties<float, 16>::blockVolume;
  size_t nextBlockIndex_;
  float voxelRes_;

  BlockIdMap blockIds_;
  BlockList voxelBlocks_;
  MeshList meshes_;

  size_t ComputeMesh(const BlockId &blockId, MeshType &tmp);
};
} // namespace fusion
} // namespace spf
