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

#include "MeshRenderer.hpp"
#include "shader_sources.h"

#include "spf/utils.hpp"

MeshRenderer::MeshRenderer(Mat4f const &modelToOpenGL)
{
  matrixBlock_.projection = Mat4f::Identity();
  matrixBlock_.transform_ = Mat4f(modelToOpenGL);
  matrixBlock_.camera = Mat4f::Identity();
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::UpdateMeshData(
    const std::vector<std::pair<BlockId, MeshRenderer::MeshType *>> &meshList)
{
  vertices_.clear();

  // Copy data to GPU
  for(const auto &p : meshList)
  {
    // const BlockId id = p.first;
    const MeshType *mesh = p.second;

    const Vec3f *pos = mesh->RawPoints();
    const Vec3f *norm = mesh->RawNormals();
    const Vec3f *col = mesh->RawColors();
    for(size_t i = 0; i < 3 * mesh->NumTriangles(); i++)
    {
      const MeshVertex v = {pos[i], norm[i], col[i]};
      vertices_.emplace_back(v);
    }
  }
}

void MeshRenderer::UpdateTransform(const Mat4f &transform) { matrixBlock_.transform_ = transform; }

void MeshRenderer::Init()
{
  programId_ = shader_createProgram(__MeshRendererVert, __MeshRendererFrag);

  if(!programId_)
  {
    utils::Log::Error("Rendering", "Error creating shaders\n");
    exit(-1);
  }

  glGenBuffers(1, &matrixUBO_);
  glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO_);
  glBufferData(GL_UNIFORM_BUFFER, BUFFER_SIZE(3 * sizeof(Mat4f)), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glGenBuffers(1, &verticesSSBO_);
  glBindBuffer(GL_ARRAY_BUFFER, verticesSSBO_);
  glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE(sizeof(MeshVertex)), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshRenderer::Resize(const GLsizei w, const GLsizei h)
{
  matrixBlock_.projection = Mat4f::Perspective(45.0f, (float) w / (float) h, 0.50f, 10.0f);
}

void MeshRenderer::Draw()
{
  glEnable(GL_DEPTH_TEST);

  static GLsync fence;

  glBindBuffer(GL_ARRAY_BUFFER, verticesSSBO_);
  glBufferData(
      GL_ARRAY_BUFFER, BUFFER_SIZE(sizeof(MeshVertex) * vertices_.size()), NULL, GL_DYNAMIC_DRAW);
  void *gpuData;
  CHECK_GL(gpuData = glMapBufferRange(
               GL_ARRAY_BUFFER, 0, sizeof(MeshVertex) * vertices_.size(),
               GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
           glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED));

  if(gpuData == NULL)
  {
    utils::Log::Error("Rendering", "Error gpuData is NULL\n");
    return;
  }
  memcpy(gpuData, (void *) vertices_.data(), sizeof(MeshVertex) * vertices_.size());

  glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, sizeof(MeshVertex) * vertices_.size());

  if(glUnmapBuffer(GL_ARRAY_BUFFER) != GL_TRUE)
  {
    utils::Log::Error("Rendering", "Error when unmapping buffer\n");
    return;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO_);
  glBufferSubData(
      GL_UNIFORM_BUFFER, (GLintptr) (BUFFER_OFFSET(0)), BUFFER_SIZE(3 * sizeof(Mat4f)),
      &matrixBlock_);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glUseProgram(programId_);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrixUBO_);

  glBindBuffer(GL_ARRAY_BUFFER, verticesSSBO_);
  CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0)));

  CHECK_GL(glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat))));

  CHECK_GL(glVertexAttribPointer(
      2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(6 * sizeof(float))));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  CHECK_GL(glDrawArrays(GL_TRIANGLES, 0, vertices_.size()));
  fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glUseProgram(0);
  // glFlush();
}

void MeshRenderer::Destroy()
{
  glDeleteBuffers(1, &verticesSSBO_);
  meshStatus_.clear();
}
