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

#include "spf/utils.hpp"
#include "spf/data_types/Image.hpp"
#include "spf/data_types/PointCloud.hpp"
#include "spf/data_types/OrderedPointCloud.hpp"
#include "spf/math/ImgProc.hpp"

namespace spf
{
namespace data_types
{
template <typename T>
struct CameraIntrinsics
{
  T fx;
  T fy;
  T cx;
  T cy;

  CameraIntrinsics(const T fx, const T fy, const T cx, const T cy)
  {
    this->fx = fx;
    this->fy = fy;
    this->cx = cx;
    this->cy = cy;
  }

  CameraIntrinsics() = default;
  CameraIntrinsics(const CameraIntrinsics &cp) = default;
  CameraIntrinsics(CameraIntrinsics &&cp) = default;
  inline CameraIntrinsics &operator=(const CameraIntrinsics &cp) = default;
  inline CameraIntrinsics &operator=(CameraIntrinsics &&cp) = default;
};

template <typename DepthType, typename ColorType>
class RGBDFrame
{
public:
  RGBDFrame() = default;
  RGBDFrame(const size_t width, const size_t height) :
      width_{width},
      height_{height},
      depthImage_{width_, height_, 1},
      filteredDepthImage_{width_, height_, 1},
      colorImage_{width_, height_, 3}
  {}
  RGBDFrame(const RGBDFrame &cp) noexcept = default;
  RGBDFrame(RGBDFrame &&cp) noexcept = default;
  RGBDFrame &operator=(const RGBDFrame &cp) noexcept = default;
  RGBDFrame &operator=(RGBDFrame &&cp) noexcept = default;

  size_t Width() const { return width_; }
  size_t Height() const { return height_; }

  inline auto &DepthImage() { return depthImage_; }
  inline const auto &DepthImage() const { return depthImage_; }
  inline auto &ColorImage() { return colorImage_; }
  inline const auto &ColorImage() const { return colorImage_; }

  inline DepthType *Depth() { return depthImage_.Data(); }
  inline const DepthType *Depth() const { return depthImage_.Data(); }
  inline ColorType *Color() { return colorImage_.Data(); }
  inline const ColorType *Color() const { return colorImage_.Data(); }

  void Clear()
  {
    depthImage_.Clear();
    colorImage_.Clear();
  }

  void Resize(const size_t width, const size_t height)
  {
    width_ = width;
    height_ = height;
    depthImage_.Resize(width, height);
    colorImage_.Resize(width, height);
    filteredDepthImage_.Resize(width_, height_);
  }

  template <typename PointType, typename T>
  void ExtractPoints(
      PointCloud<PointType> &cloud, const CameraIntrinsics<T> &intrinsics, const T near,
      const T far, const T scale = T{1}) const
  {
    using PointCloudType = PointCloud<PointType>;
    using ScalarType = typename PointCloudType::ScalarType;
    using VecType = typename PointCloudType::VecType;
    static_assert(std::is_same_v<T, ScalarType>, "Types must be the same for scalar type");
    auto getPosition = [this, &intrinsics, near, far, scale](const size_t u, const size_t v) {
      const ScalarType invFy = ScalarType(1) / intrinsics.fy;
      const ScalarType invFx = ScalarType(1) / intrinsics.fx;
      const DepthType depth = depthImage_(v, u, 0);
      const auto z = ScalarType(depth) / scale;
      if(depth == DepthType(0))
      {
        return VecType{std::numeric_limits<ScalarType>::max()};
      }
      if(z <= near || z > far)
      {
        return VecType{std::numeric_limits<ScalarType>::max()};
      }
      const auto x = (ScalarType(u) - intrinsics.cx) * (z * invFx);
      const auto y = (ScalarType(v) - intrinsics.cy) * (z * invFy);
      return VecType({x, y, z});
    };
    auto getColor = [this](const size_t u, const size_t v) {
      const size_t index = v * width_ + u;
      const auto r = ScalarType(colorImage_.Data()[3 * index]);
      const auto g = ScalarType(colorImage_.Data()[3 * index + 1]);
      const auto b = ScalarType(colorImage_.Data()[3 * index + 2]);
      return VecType(r, g, b) / ScalarType(255);
    };

    for(size_t v = 0; v < height_; v++)
    {
      for(size_t u = 0; u < width_; u++)
      {
        const auto p = PointType{getPosition(u, v), getColor(u, v)};
        if(p.isValid())
        {
          cloud.AddPoint(p);
        }
      }
    }
  }

  template <typename PointType, typename T>
  void ExtractPoints(
      OrderedPointCloud<PointType> &cloud, const CameraIntrinsics<T> &intrinsics, const T near,
      const T far, const T scale = T{1}) const
  {
    using PointCloudType = OrderedPointCloud<PointType>;
    using ScalarType = typename PointCloudType::ScalarType;
    using VecType = typename PointCloudType::VecType;
    static_assert(std::is_same_v<T, ScalarType>, "Types must be the same for scalar type");
    auto getPosition = [this, &intrinsics, near, far, scale](const size_t u, const size_t v) {
      const ScalarType invFy = ScalarType(1) / intrinsics.fy;
      const ScalarType invFx = ScalarType(1) / intrinsics.fx;
      const DepthType depth = depthImage_(v, u, 0);
      const auto z = ScalarType(depth) / scale;
      if(depth == DepthType(0))
      {
        return VecType{std::numeric_limits<ScalarType>::max()};
      }
      if(z <= near || z > far)
      {
        return VecType{std::numeric_limits<ScalarType>::max()};
      }
      const auto x = (ScalarType(u) - intrinsics.cx) * (z * invFx);
      const auto y = (ScalarType(v) - intrinsics.cy) * (z * invFy);
      return VecType({x, y, z});
    };
    auto getColor = [this](const size_t u, const size_t v) {
      const size_t index = v * width_ + u;
      const auto r = ScalarType(colorImage_.Data()[3 * index]);
      const auto g = ScalarType(colorImage_.Data()[3 * index + 1]);
      const auto b = ScalarType(colorImage_.Data()[3 * index + 2]);
      return VecType(r, g, b) / ScalarType(255);
    };

    for(size_t v = 0; v < height_; v++)
    {
      for(size_t u = 0; u < width_; u++)
      {
        const auto p = PointType{getPosition(u, v), getColor(u, v)};
        cloud.SetPoint(p, v, u);
      }
    }
  }

  void FilterData()
  {
    static_assert(std::is_same_v<DepthType, uint16_t>, "Filtering not implemented for other types");

    static constexpr float sigmaSpace = 4.5f;
    static constexpr float sigmaDepth = 300.0f;
    static constexpr size_t erodeWidth = 8;
    filteredDepthImage_.Clear();
    math::BilateralFilter(
        depthImage_.Data(), filteredDepthImage_.Data(), width_, height_, width_, sigmaSpace,
        sigmaDepth);
    depthImage_.Clear();
    math::ErosionFilter(
        filteredDepthImage_.Data(), depthImage_.Data(), width_, height_, erodeWidth);
  }

private:
  size_t width_{0};
  size_t height_{0};
  Image<DepthType> depthImage_;
  Image<DepthType> filteredDepthImage_;
  Image<ColorType> colorImage_;
};
} // namespace data_types
} // namespace spf
