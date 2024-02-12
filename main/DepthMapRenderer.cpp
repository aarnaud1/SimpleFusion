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

#include "DepthMapRenderer.hpp"
#include "shader_sources.h"

static const GLfloat XYZ[] = {-1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  0.0f,
                              -1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, -1.0f, 0.0f};

static const GLfloat UV[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                             0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};

DepthMapRenderer::DepthMapRenderer(const size_t w, const size_t h) : width_(w), height_(h) {}

DepthMapRenderer::~DepthMapRenderer() {}

void DepthMapRenderer::UpdateImg(const uint16_t *depthData)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_UNSIGNED_SHORT, depthData);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void DepthMapRenderer::Init()
{
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenBuffers(1, &xyzVBO_);
  glGenBuffers(1, &uvVBO_);

  glBindBuffer(GL_ARRAY_BUFFER, xyzVBO_);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (6 * 3 * sizeof(float)), XYZ, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uvVBO_);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (6 * 2 * sizeof(float)), UV, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  program_ = shader_createProgram(__DepthMapRendererVert, __DepthMapRendererFrag);
}

void DepthMapRenderer::Resize(const GLsizei /*w*/, const GLsizei /*h*/) {}

void DepthMapRenderer::Draw()
{
  glUseProgram(program_);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, xyzVBO_);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uvVBO_);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glUseProgram(0);
}

void DepthMapRenderer::Destroy()
{
  glDeleteBuffers(1, &xyzVBO_);
  glDeleteBuffers(1, &uvVBO_);
  glDeleteTextures(1, &texture_);
  glDeleteProgram(program_);
}
