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

#include "SliceRenderer.hpp"

using namespace spf::fusion;

SliceRenderer::SliceRenderer(const size_t w, const size_t h, spf::fusion::Volume &volume) :
    height_(h), width_(w), volume_(volume), tsdfImg_(new Color3u[w * h])
{}

void SliceRenderer::Render(const Vec3 &center, const Vec3 & /*axis*/)
{
  const auto centerBlockId = GetId(center, voxelRes_);
  const auto centerVoxId = GetVoxelId(center, voxelRes_);
  const auto centerPos = GetVoxelAbsolutePos(centerBlockId, centerVoxId);

#pragma omp parallel for
  for(size_t v = 0; v < height_; v++)
  {
    for(size_t u = 0; u < width_; u++)
    {
      const auto absVoxelPos = centerPos - spf::fusion::Index3d(u - width_ / 2, v - height_ / 2, 0);
      const auto blockId = Div(absVoxelPos, spf::fusion::BLOCK_SIZE);
      const auto voxelId = Mod(absVoxelPos, spf::fusion::BLOCK_SIZE);

      const auto *block = volume_.GetBlock(blockId);
      if(block != nullptr)
      {
        const auto tsdf = block->TSDFAt(voxelId);
        const auto pxValue = ColorTSDF(tsdf);
        const size_t index = v * width_ + u;
        tsdfImg_[index].r = uint8_t(255.0f * pxValue.x);
        tsdfImg_[index].g = uint8_t(255.0f * pxValue.y);
        tsdfImg_[index].b = uint8_t(255.0f * pxValue.z);
      }
    }
  }
}
