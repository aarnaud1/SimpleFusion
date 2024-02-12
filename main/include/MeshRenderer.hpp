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

#ifndef __MESH_RENDERER_HPP__
#define __MESH_RENDERER_HPP__

#include <glad/glad.h>
#include <shader.h>
#include <vector>
#include <unordered_map>

#include "Global.hpp"
#include "BasicRenderer.hpp"
#include "spf/geometry/geometry.hpp"
#include "spf/data_types/Mesh.hpp"
#include "spf/fusion/VoxelBlock.hpp"
#include "spf/fusion/Volume.hpp"

class MeshRenderer : public BasicRenderer
{
public:
  typedef spf::Point4f Point4f;
  typedef spf::Vec4f Vec4f;
  typedef spf::Vec3f Vec3f;
  typedef spf::Color4f Color4f;
  typedef spf::Color3f Color3f;
  typedef spf::Mat4f Mat4f;
  typedef spf::fusion::BlockId BlockId;
  using PointType = spf::data_types::PointXYZRGBN<float>;
  using MeshType = spf::data_types::Mesh<PointType>;

  struct MeshVertex
  {
    Vec3f pos;
    Vec3f norm;
    Vec3f col;
  };

  MeshRenderer(Mat4f const &modelToOpenGL);

  ~MeshRenderer();

  void UpdateMeshData(const std::vector<std::pair<BlockId, MeshType *>> &meshList);

  void UpdateTransform(const Mat4f &transform);

  void Init() override;

  void Resize(const GLsizei w, const GLsizei h) override;

  void Draw() override;

  void Destroy() override;

private:
  typedef spf::fusion::ChunkHasher ChunkHasher;

  // std::unordered_map<BlockId, MeshStorageInfo, ChunkHasher> meshes_;
  std::unordered_map<BlockId, bool, ChunkHasher> meshStatus_;

  GLuint programId_;
  GLuint matrixUBO_;

  GLuint verticesSSBO_;
  std::vector<MeshVertex> vertices_;

  struct
  {
    Mat4f projection;
    Mat4f transform_;
    Mat4f camera;
  } matrixBlock_;
};

#endif // __MESH_RENDERER_HPP__
