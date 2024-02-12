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

#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <map>
#include <memory>

#include "spf/Types.hpp"
#include "spf/data_types/PointCloud.hpp"
#include "spf/data_types/Mesh.hpp"
#include "spf/fusion/BlockUtils.hpp"

namespace spf
{
namespace fusion
{
using namespace data_types;

class VoxelBlock
{
public:
  VoxelBlock(const float voxelRes, const bool useColor = true);

  ~VoxelBlock(){};

  void Clear();

  inline bool UseColor() const { return useColor_; }
  inline float* TSDF() const { return tsdf_.get(); }
  inline float* Weights() const { return weights_.get(); }
  inline Color3f* Colors() const { return colors_.get(); }
  inline Vec3f* Gradients() const { return gradients_.get(); }

  inline float TSDFAt(const Index3d& index) const
  {
    return tsdf_
        [index.x + index.y * BlockProperties<float, 16>::blockSize
         + index.z * BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize];
  }

  inline float WeightAt(const Index3d& index) const
  {
    return weights_
        [index.x + index.y * BlockProperties<float, 16>::blockSize
         + index.z * BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize];
  }

  inline Vec3f GradientAt(const Index3d& index) const
  {
    return gradients_
        [index.x + index.y * BlockProperties<float, 16>::blockSize
         + index.z * BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize];
  }

  inline Color3f ColorAt(const Index3d& index) const
  {
    return colors_
        [index.x + index.y * BlockProperties<float, 16>::blockSize
         + index.z * BlockProperties<float, 16>::blockSize * BlockProperties<float, 16>::blockSize];
  }

  static constexpr size_t BlockSize() { return BlockProperties<float, 16>::blockSize; }
  static constexpr size_t BlockVolume() { return BlockProperties<float, 16>::blockVolume; }

private:
  float voxelRes_;
  size_t blockVolume_;
  bool useColor_;

  std::unique_ptr<float[]> tsdf_;
  std::unique_ptr<float[]> weights_;
  std::unique_ptr<Vec3f[]> gradients_;
  std::unique_ptr<Color3f[]> colors_;
};
} // namespace fusion
} // namespace spf
