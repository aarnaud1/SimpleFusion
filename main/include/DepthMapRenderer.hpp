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

#ifndef __DEPTH_MAP_RENDERER_HPP__
#define __DEPTH_MAP_RENDERER_HPP__

#include <glad/glad.h>
#include <shader.h>
#include <vector>

#include "Global.hpp"
#include "BasicRenderer.hpp"

#include "spf/geometry/geometry.hpp"
#include "spf/fusion/VoxelBlock.hpp"
#include "spf/fusion/Volume.hpp"
#include "spf/data_types/Mesh.hpp"

class DepthMapRenderer : public BasicRenderer
{
public:
  typedef spf::Point4f Point4f;
  typedef spf::Vec4f Vec4f;
  typedef spf::Vec3f Vec3f;
  typedef spf::Color4f Color4f;
  typedef spf::Color3f Color3f;
  typedef spf::Mat4f Mat4f;
  typedef spf::fusion::BlockId BlockId;

  DepthMapRenderer(const size_t w, const size_t h);

  ~DepthMapRenderer();

  void UpdateImg(const uint16_t *depthData);

  void Init() override;

  void Resize(const GLsizei w, const GLsizei h) override;

  void Draw() override;

  void Destroy() override;

private:
  size_t width_;
  size_t height_;

  GLuint texture_;
  GLuint program_;
  GLuint xyzVBO_;
  GLuint uvVBO_;
};
#endif // __DEPTH_MAP_RENDERER_HPP__
