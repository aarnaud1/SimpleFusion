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

#include <numeric>
#include <algorithm>

#include "spf/Types.hpp"
#include "spf/data_types/Buffer.hpp"
#include "spf/data_types/PointTypes.hpp"

namespace spf
{
namespace data_types
{
template <typename T>
class PointCloud
{
public:
  using PointType = T;
  using ScalarType = typename PointType::ScalarType;
  using VecType = typename PointType::VecType;
  using TransformType = typename geometry::Mat4<ScalarType>;

  PointCloud() = default;
  PointCloud(const size_t size) : points_{size} {}
  PointCloud(const PointCloud &cp) noexcept = default;
  PointCloud(PointCloud &&cp) noexcept = default;
  PointCloud &operator=(const PointCloud &cp) noexcept = default;
  PointCloud &operator=(PointCloud &&cp) noexcept = default;

  // Accessors
  auto &PointData() { return points_; }
  const auto &PointData() const { return points_; }

  // Access point data
  auto &Points() { return points_.Points(); }
  const auto &Points() const { return points_.Points(); }
  auto &Points(const size_t i) { return points_.Points[i]; }
  const auto &Points(const size_t i) const { return points_.Points[i]; }
  auto *RawPoints() { return points_.Points().Data(); }
  const auto *RawPoints() const { return points_.Points().Data(); }

  // Access color data
  auto &Colors()
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors();
  }
  const auto &Colors() const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors();
  }
  auto &Colors(const size_t i)
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors()[i];
  }
  const auto &Colors(const size_t i) const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors()[i];
  }
  auto *RawColors()
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors().Data();
  }
  const auto *RawColors() const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors().Data();
  }

  // Access normal data
  auto &Normals()
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals();
  }
  const auto &Normals() const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals();
  }
  auto &Normals(const size_t i)
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals()[i];
  }
  const auto &Normals(const size_t i) const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals()[i];
  }
  auto *RawNormals()
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals().Data();
  }
  const auto *RawNormals() const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals().Data();
  }

  auto operator[](const size_t i) { return points_[i]; }
  const auto operator[](const size_t i) const { return points_[i]; }

  // Add points
  void AddPoint(const PointType &point) { points_.AddPoint(point); }
  void SetPoint(const PointType &point, const size_t i)
  {
    Points(i) = point.xyz;
    if constexpr(PointType::hasColors())
    {
      Points(i) = point.rgb;
    }
    if constexpr(PointType::hasNormals())
    {
      Normals(i) = point.n;
    }
  }

  // Resize and clear operators
  void Resize(const size_t newSize) { points_.Resize(newSize); }
  void Realloc(const size_t newSize) { points_.Realloc(newSize); }
  void Clear() { points_.Clear(); }
  void Release() { points_.Release(); }
  void Fill(const PointType &value) { std::fill(this->begin(), this->end(), value); }

  auto Size() const { return points_.Size(); }
  auto Capacity() const { return points_.Capacity(); }

  // Point cloud related functions
  VecType Centroid() const
  {
    const auto &points = this->Points();
    auto n = static_cast<ScalarType>(this->size());
    auto ret = std::accumulate(points.begin(), points.end(), VecType{0}) / n;
    return ret;
  }

  void Transform(const TransformType &M)
  {
    auto &points = this->Points();
    std::for_each(points.begin(), points.end(), [M](auto &xyz) { xyz *= M; });
    if constexpr(PointType::hasNormals())
    {
      auto &normals = this->Normals();
      auto R = TransformType::Identity();
      R.SetRotation(M.GetRotation());
      std::for_each(normals.begin(), normals.end(), [R](auto &n) { n *= R; });
    }
  }

  void Translate(const VecType &t)
  {
    auto &points = this->Points();
    std::for_each(points.begin(), points.end(), [t](auto &xyz) { xyz += t.xyz(); });
  }

  void ExportPLY(const char *filename)
  {
    FILE *fp = fopen(filename, "w+");
    if(!fp)
    {
      char msg[512];
      sprintf(msg, "Error opening %s : %s\n", filename, strerror(errno));
      throw std::runtime_error(msg);
    }

    fprintf(fp, "ply\n");
    fprintf(fp, "format ascii 1.0\n");
    fprintf(fp, "element vertex %lu\n", points_.Size());
    fprintf(fp, "property float x\n");
    fprintf(fp, "property float y\n");
    fprintf(fp, "property float z\n");
    if constexpr(PointType::hasNormals())
    {
      fprintf(fp, "property float nx\n");
      fprintf(fp, "property float ny\n");
      fprintf(fp, "property float nz\n");
    }
    if constexpr(PointType::hasColors())
    {
      fprintf(fp, "property float red\n");
      fprintf(fp, "property float green\n");
      fprintf(fp, "property float blue\n");
      fprintf(fp, "property float alpha\n");
    }
    fprintf(fp, "end_header\n");

    for(const auto &point : points_)
    {
      if constexpr(PointType::hasNormals() && PointType::hasColors())
      {
        fprintf(
            fp, "%f %f %f %f %f %f %f %f %f %f\n", static_cast<float>(point.xyz.x),
            static_cast<float>(point.xyz.y), static_cast<float>(point.xyz.z),
            static_cast<float>(point.n.x), static_cast<float>(point.n.y),
            static_cast<float>(point.n.y), static_cast<float>(point.rgb.x),
            static_cast<float>(point.rgb.y), static_cast<float>(point.rgb.z), 1.0f);
      }
      else if constexpr(PointType::hasNormals())
      {
        fprintf(
            fp, "%f %f %f %f %f %f\n", static_cast<float>(point.xyz.x),
            static_cast<float>(point.xyz.y), static_cast<float>(point.xyz.z),
            static_cast<float>(point.n.x), static_cast<float>(point.n.y),
            static_cast<float>(point.n.y));
      }
      else if constexpr(PointType::hasColors())
      {
        fprintf(
            fp, "%f %f %f %f %f %f %f\n", static_cast<float>(point.xyz.x),
            static_cast<float>(point.xyz.y), static_cast<float>(point.xyz.z),
            static_cast<float>(point.rgb.x), static_cast<float>(point.rgb.y),
            static_cast<float>(point.rgb.z), 1.0f);
      }
      else
      {
        fprintf(
            fp, "%f %f %f\n", static_cast<float>(point.xyz.x), static_cast<float>(point.xyz.y),
            static_cast<float>(point.xyz.z));
      }
    }
  }

protected:
  using PointListType = PointList<PointType>;
  PointListType points_{};
};
} // namespace data_types
} // namespace spf
