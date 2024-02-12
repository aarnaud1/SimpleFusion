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

#ifndef __BASIC_TEXTURE_RENDERER_HPP__
#define __BASIC_TEXTURE_RENDERER_HPP__

#include <glad/glad.h>
#include <shader.h>
#include <string.h>

#include "Global.hpp"
#include "BasicRenderer.hpp"

class BasicTextureRenderer : public BasicRenderer
{
public:
  BasicTextureRenderer()
  {
    width_ = 640;
    height_ = 480;
    pixels_ = new GLubyte[3 * 640 * 480];
  };

  ~BasicTextureRenderer() { delete[] pixels_; };

  void Init();

  void Resize(const GLsizei w, const GLsizei h);

  void UpdateTextureData(const void *image, const GLuint w, const GLuint h);

  void Draw();

  void Destroy();

private:
  GLuint programId_;
  GLuint texId_;
  GLuint xyzSSBO_;
  GLuint uvSSBO_;

  GLuint width_;
  GLuint height_;
  GLubyte *pixels_;
};

#endif // __BASIC_TEXTURE_RENDERER_HPP__
