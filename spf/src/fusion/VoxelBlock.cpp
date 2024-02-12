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

#include "spf/fusion/VoxelBlock.hpp"

namespace spf
{
namespace fusion
{
VoxelBlock::VoxelBlock(const float voxelRes, bool useColor) :
    voxelRes_(voxelRes),
    blockVolume_(BlockProperties<float, 16>::blockVolume),
    useColor_(useColor),
    tsdf_(new float[BlockProperties<float, 16>::blockVolume]),
    weights_(new float[BlockProperties<float, 16>::blockVolume]),
    gradients_(new Point3f[BlockProperties<float, 16>::blockVolume]),
    colors_(useColor ? new Color3f[BlockProperties<float, 16>::blockVolume] : nullptr)
{
  // Init all values
  for(size_t i = 0; i < this->blockVolume_; i++)
  {
    tsdf_[i] = BlockProperties<float, 16>::invalidTsdf;
  }
  memset(weights_.get(), 0, blockVolume_ * sizeof(float));

  if(useColor_)
  {
    memset((unsigned char *) colors_.get(), 0, blockVolume_ * sizeof(Vec3f));
  }
}

void VoxelBlock::Clear()
{
  for(size_t i = 0; i < this->blockVolume_; i++)
  {
    tsdf_[i] = BlockProperties<float, 16>::invalidTsdf;
    weights_[i] = 0.0f;
    gradients_[i] = Vec3f(0.0f);
    if(useColor_)
    {
      colors_[i] = Vec3f(0.0f);
    }
  }
}
} // namespace fusion
} // namespace spf
