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

#include <glad/glad.h>
#include <shader.h>
#include <string.h>
#include <vector>

#include "Global.hpp"
#include "BasicRenderer.hpp"
#include "spf/data_types/OrderedPointCloud.hpp"
#include "spf/Types.hpp"

class OPCFrameRenderer : public BasicRenderer
{
public:
  typedef spf::Point4f Point4f;
  typedef spf::Vec4f Vec4f;
  typedef spf::Vec3f Vec3f;
  typedef spf::Color4f Color4f;
  typedef spf::Color3f Color3f;
  typedef spf::Mat4f Mat4f;

  using PointType = spf::data_types::PointXYZRGBN<float>;
  using OPCType = spf::data_types::OrderedPointCloud<PointType>;

  OPCFrameRenderer(const int w, const int h, Mat4f const &modelToOpenGL);

  ~OPCFrameRenderer();

  void UpdateCloudData(const OPCType &cloud);

  void Resize(const GLsizei w, const GLsizei h) override;

  void Init() override;

  void Draw() override;

  void Destroy() override;

private:
  const size_t numPoints_;
  int width_;
  int height_;

  GLuint programId_;
  GLuint xyzVBO_;
  GLuint rgbVBO_;
  GLuint nxyzVBO_;
  GLuint matrixUBO_;

  std::vector<Point4f> points_;
  std::vector<Color4f> colors_;
  std::vector<Vec3f> normals_;
  struct
  {
    Mat4f projection;
    Mat4f transform_;
    Mat4f camera;
  } matrixBlock_;
};
