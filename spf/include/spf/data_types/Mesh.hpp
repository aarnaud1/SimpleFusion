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

#include <algorithm>
#include <vector>

#include "spf/geometry/geometry.hpp"
#include "spf/data_types/Buffer.hpp"
#include "spf/data_types/PointTypes.hpp"

namespace spf
{
namespace data_types
{
template <typename T>
class Mesh
{
public:
  using PointType = T;
  using SclarType = typename PointType::ScalarType;
  using VecType = typename PointType::VecType;
  using IndexType = geometry::Vec3<size_t>;

  Mesh() = default;
  Mesh(const size_t numPoints, const size_t numTriangles) :
      points_{numPoints}, triangles_{numTriangles}
  {}
  Mesh(const Mesh &cp) noexcept = default;
  Mesh(Mesh &&cp) noexcept = default;
  Mesh &operator=(const Mesh &cp) noexcept = default;
  Mesh &operator=(Mesh &&cp) noexcept = default;

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

  auto &Triangles() { return triangles_; }
  const auto &Triangles() const { return triangles_; }
  auto &Triangles(const size_t i) { return triangles_[i]; }
  const auto &Triangles(const size_t i) const { return triangles_[i]; }
  auto *RawTriangles() { return triangles_.Data(); }
  const auto *RawTriangles() const { return triangles_.Data(); }

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

  // Add triangles
  void AddTriangle(const IndexType &triangle)
  {
    triangles_[triangles_.Size()] = triangle;
    triangles_.IncSize();
  }
  void SetTriangle(const IndexType &triangle, const size_t i) { triangles_[i] = triangle; }

  // Resize and clear operators
  void Resize(const size_t pointsSize, const size_t trianglesSize)
  {
    points_.Resize(pointsSize);
    triangles_.Resize(trianglesSize);
  }
  void Realloc(const size_t pointsSize, const size_t trianglesSize)
  {
    points_.Realloc(pointsSize);
    triangles_.Realloc(trianglesSize);
  }
  void Clear()
  {
    points_.Clear();
    triangles_.Clear();
  }
  void Release()
  {
    points_.Release();
    triangles_.Release();
  }

  auto NumPoints() const { return points_.Size(); }
  auto NumTriangles() const { return triangles_.Size(); }

private:
  using PointListType = PointList<PointType>;
  using IndexList = Buffer<IndexType>;
  PointListType points_{};
  IndexList triangles_{};
};
} // namespace data_types
} // namespace spf
