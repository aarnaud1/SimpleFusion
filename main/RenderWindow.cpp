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

#include "RenderWindow.hpp"

RenderWindow::RenderWindow(const int w, const int h, const int nRows, const int nCols) :
    windowWidth_(w), windowHeight_(h), nRows_(nRows), nCols_(nCols), rendererList_(nRows * nCols)
{
  for(size_t i = 0; i < rendererList_.size(); i++)
  {
    rendererList_[i] = NULL;
  }
}

RenderWindow::~RenderWindow() {}

void RenderWindow::Resize(const int w, const int h)
{
  windowWidth_ = w;
  windowHeight_ = h;
  const int xStep = w / nCols_;
  const int yStep = h / nRows_;
  for(size_t i = 0; i < rendererList_.size(); i++)
  {
    if(rendererList_[i] != NULL)
    {
      rendererList_[i]->Resize(xStep, yStep);
    }
  }
}

void RenderWindow::Draw()
{
  size_t index = 0;
  const int xStep = windowWidth_ / nCols_;
  const int yStep = windowHeight_ / nRows_;

  // TODO : Make an Init() funtion
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(int row = 0; row < nRows_; row++)
  {
    for(int col = 0; col < nCols_; col++)
    {
      glViewport(col * xStep, row * yStep, xStep, yStep);
      BasicRenderer *renderer = rendererList_[index++];
      if(renderer != NULL)
      {
        renderer->Draw();
        glFinish();
      }
    }
  }
}

void RenderWindow::AddRenderer(const BasicRenderer *renderer, const int row, const int col)
{
  if(row * nCols_ + col < (int) rendererList_.size())
  {
    rendererList_[row * nCols_ + col] = (BasicRenderer *) renderer;
  }
}
