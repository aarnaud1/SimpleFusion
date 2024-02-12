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

#include "PointCloudFrameRenderer.hpp"
#include "shader_sources.h"

PointCloudFrameRenderer::PointCloudFrameRenderer(const int numPoints, Mat4f const &modelToOpenGL) :
    maxNumPoints_(numPoints), points_(numPoints), colors_(numPoints)
{
  matrixBlock_.projection = Mat4f::Identity();
  matrixBlock_.transform_ = Mat4f(modelToOpenGL);
  matrixBlock_.camera = Mat4f::Identity();
}

PointCloudFrameRenderer::~PointCloudFrameRenderer() {}

void PointCloudFrameRenderer::UpdateCloudData(const PointCloudType &cloud)
{
  const auto *points = cloud.RawPoints();
  const auto *colors = cloud.RawColors();
  points_.clear();
  colors_.clear();
  for(size_t i = 0; i < cloud.Size(); i++)
  {
    points_.push_back(Vec4f(points[i].x, points[i].y, points[i].z, 1.0f));
    colors_.push_back(Vec4f(colors[i].z, colors[i].y, colors[i].x, 1.0f));
  }
}

void PointCloudFrameRenderer::Init()
{
  glGenBuffers(1, &xyzVBO_);
  glGenBuffers(1, &rgbVBO_);
  glGenBuffers(1, &matrixUBO_);
  programId_ = shader_createProgram(__StreamingDisplayCloudVert, __StreamingDisplayCloudFrag);

  glBindBuffer(GL_ARRAY_BUFFER, xyzVBO_);
  glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE(maxNumPoints_ * sizeof(Vec4f)), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, rgbVBO_);
  glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE(maxNumPoints_ * sizeof(Vec4f)), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO_);
  glBufferData(GL_UNIFORM_BUFFER, BUFFER_SIZE(3 * sizeof(Mat4f)), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void PointCloudFrameRenderer::Resize(const GLsizei w, const GLsizei h)
{
  matrixBlock_.projection = Mat4f::Perspective(50.0f, (float) w / (float) h, 0.10f, 10.0f);
}

void PointCloudFrameRenderer::Draw()
{
  glEnable(GL_DEPTH_TEST);

  glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO_);
  glBufferSubData(
      GL_UNIFORM_BUFFER, (GLintptr) (BUFFER_OFFSET(0)), BUFFER_SIZE(3 * sizeof(Mat4f)),
      &matrixBlock_);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glUseProgram(programId_);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrixUBO_);

  glBindBuffer(GL_ARRAY_BUFFER, xyzVBO_);
  glBufferData(
      GL_ARRAY_BUFFER, BUFFER_SIZE(points_.size() * sizeof(Vec4f)), points_.data(),
      GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  glBindBuffer(GL_ARRAY_BUFFER, rgbVBO_);
  glBufferData(
      GL_ARRAY_BUFFER, BUFFER_SIZE(colors_.size() * sizeof(Vec4f)), colors_.data(),
      GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glDrawArrays(GL_POINTS, 0, points_.size());
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glUseProgram(0);
  glFlush();
}

void PointCloudFrameRenderer::Destroy()
{
  glDeleteBuffers(1, &xyzVBO_);
  glDeleteBuffers(1, &rgbVBO_);
  glDeleteBuffers(1, &matrixUBO_);
  glDeleteProgram(programId_);
}
