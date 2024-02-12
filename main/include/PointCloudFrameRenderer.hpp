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

#ifndef __POINT_CLOUD_RENDERER__
#define __POINT_CLOUD_RENDERER__

#include <glad/glad.h>
#include <shader.h>
#include <string.h>
#include <vector>

#include "Global.hpp"
#include "BasicRenderer.hpp"
#include "spf/data_types/PointCloud.hpp"
#include "spf/Types.hpp"

class PointCloudFrameRenderer : public BasicRenderer
{
public:
  typedef spf::Point3f Point3f;
  typedef spf::Vec4f Vec4f;
  typedef spf::Vec3f Vec3f;
  typedef spf::Color4f Color4f;
  typedef spf::Color3f Color3f;
  typedef spf::Mat4f Mat4f;

  using PointType = spf::data_types::PointXYZRGB<float>;
  using PointCloudType = spf::data_types::PointCloud<PointType>;

  PointCloudFrameRenderer(const int numPoints, Mat4f const& modelToOpenGL);

  ~PointCloudFrameRenderer();

  void UpdateCloudData(const PointCloudType& cloud);

  void Resize(const GLsizei w, const GLsizei h) override;

  void Init() override;

  void Draw() override;

  void Destroy() override;

private:
  const int maxNumPoints_;
  GLuint programId_;
  GLuint xyzVBO_;
  GLuint rgbVBO_;
  GLuint matrixUBO_;

  std::vector<Vec4f> points_;
  std::vector<Vec4f> colors_;
  struct
  {
    Mat4f projection;
    Mat4f transform_;
    Mat4f camera;
  } matrixBlock_;
};

#endif // __BASIC_POINT_CLOUD_RENDERER__
