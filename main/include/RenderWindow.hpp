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

#ifndef __RENDER_WINDOW_HPP__
#define __RENDER_WINDOW_HPP__

#include <vector>
#include <glad/glad.h>

#include "spf/utils.hpp"
#include "BasicRenderer.hpp"

class RenderWindow
{
public:
  RenderWindow(const int w, const int h, const int nRows, const int nCols);

  ~RenderWindow();

  void Resize(const int w, const int h);

  void Draw();

  void AddRenderer(const BasicRenderer *renderer, const int row, const int col);

private:
  int windowWidth_;
  int windowHeight_;
  int nRows_;
  int nCols_;

  std::vector<BasicRenderer *> rendererList_;
};

#endif // __RENDER_WINDOW_HPP__
