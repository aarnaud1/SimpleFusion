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
class OrderedPointCloud
{
public:
  using PointType = T;
  using ScalarType = typename PointType::ScalarType;
  using VecType = typename PointType::VecType;
  using TransformType = typename geometry::Mat4<ScalarType>;

  OrderedPointCloud() = default;
  OrderedPointCloud(const size_t width, const size_t height) :
      width_{width}, height_{height}, points_{width_ * height_}
  {
    points_.Resize(width_ * height_);
  }
  OrderedPointCloud(const OrderedPointCloud& cp) noexcept = default;
  OrderedPointCloud(OrderedPointCloud&& cp) noexcept = default;
  OrderedPointCloud& operator=(const OrderedPointCloud& cp) noexcept = default;
  OrderedPointCloud& operator=(OrderedPointCloud&& cp) noexcept = default;

  // Access points data
  auto& Points() { return points_.Points(); }
  const auto& Points() const { return points_.Points(); }
  auto& Points(const size_t i) { return points_.Points[i]; }
  const auto& Points(const size_t i) const { return points_.Points[i]; }
  auto* RawPoints() { return points_.Points().Data(); }
  const auto* RawPoints() const { return points_.Points().Data(); }
  auto& Points(const size_t i, const size_t j) { return points_.Points()[i * width_ + j]; }
  const auto& Points(const size_t i, const size_t j) const
  {
    return points_.Points()[i * width_ + j];
  }

  // Access color data
  auto& Colors()
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors();
  }
  const auto& Colors() const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors();
  }
  auto& Colors(const size_t i)
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors()[i];
  }
  const auto& Colors(const size_t i) const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors()[i];
  }
  auto* RawColors()
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors().Data();
  }
  const auto* RawColors() const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors().Data();
  }
  auto& Colors(const size_t i, const size_t j)
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors()[i * width_ + j];
  }
  const auto& Colors(const size_t i, const size_t j) const
  {
    static_assert(PointType::hasColors(), "PointType must have colors");
    return points_.Colors()[i * width_ + j];
  }

  // Access normal data
  auto& Normals()
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals();
  }
  const auto& Normals() const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals();
  }
  auto& Normals(const size_t i)
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals()[i];
  }
  const auto& Normals(const size_t i) const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals()[i];
  }
  auto* RawNormals()
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals().Data();
  }
  const auto* RawNormals() const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals().Data();
  }
  auto& Normals(const size_t i, const size_t j)
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals()[i * width_ + j];
  }
  const auto& Normals(const size_t i, const size_t j) const
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");
    return points_.Normals()[i * width_ + j];
  }

  auto operator[](const size_t i) { return points_[i]; }
  const auto operator[](const size_t i) const { return points_[i]; }

  void SetPoint(const PointType& point, const size_t i, const size_t j)
  {
    Points(i, j) = point.xyz;
    if constexpr(PointType::hasColors())
    {
      Colors(i, j) = point.rgb;
    }
    if constexpr(PointType::hasNormals())
    {
      Normals(i, j) = point.n;
    }
  }

  // Resize and clear operators
  void Resize(const size_t width, const size_t height)
  {
    width_ = width;
    height_ = height;
    points_.Resize(width_ * height_);
    org_ = {0};
  }
  void Realloc(const size_t newSize)
  {
    points_.Realloc(newSize);
    org_ = {0};
  }
  void Clear()
  {
    points_.Clear();
    org_ = {0};
  }
  void Release()
  {
    points_.Release();
    org_ = {0};
  }
  void Fill(const PointType& value) { std::fill(points_.begin(), points_.end(), value); }

  auto Width() const { return width_; }
  auto Height() const { return height_; }
  auto Res() const { return width_ * height_; }
  auto Capacity() const { return points_.capacity(); }

  VecType Centroid() const
  {
    const auto& points = this->Points();
    size_t n = 0;
    VecType ret{0};
    std::for_each(points.begin(), points.end(), [&ret, &n](const auto& p) {
      if(p != VecType{PointType::maxScalar})
      {
        ret += p;
        n++;
      }
    });
    if(n > 0)
    {
      ret /= static_cast<ScalarType>(n);
    }
  }

  void Transform(const TransformType& M)
  {
    auto& points = this->Points();
    std::for_each(points.begin(), points.end(), [M](auto& p) {
      if(p != VecType{PointType::maxScalar})
      {
        p *= M;
      }
    });
    if constexpr(PointType::hasNormals())
    {
      auto& normals = this->Normals();
      auto R = TransformType::Identity();
      R.SetRotation(M.GetRotation());
      std::for_each(normals.begin(), normals.end(), [R](auto& n) {
        if(n != VecType{0})
        {
          n *= R;
        }
      });
    }
    org_ *= M;
  }

  void Translate(const VecType& t)
  {
    auto& points = this->Points();
    std::for_each(points.begin(), points.end(), [t](auto& p) {
      if(p != VecType{PointType::maxScalar})
      {
        p += t;
      }
    });
    org_ += t;
  }

  void EstimateNormals(const float distThr)
  {
    static_assert(PointType::hasNormals(), "PointType must have normals");

    auto isValid = [](const VecType& v) {
      return v != VecType{std::numeric_limits<ScalarType>::max()};
    };
    auto correctNormal = [this](VecType const& n, VecType const& p) -> VecType {
      return VecType::Dot(n, p - org_) < ScalarType(0) ? -n : n;
    };

    for(size_t i = 0; i < height_; i++)
    {
      for(size_t j = 1; j < width_ - 1; j++)
      {
        const auto& p = Points(i, j);
        const auto& tmp00 = Points((i - 1), j);
        const auto& tmp01 = Points((i + 1), j);
        const auto& tmp10 = Points(i, j - 1);
        const auto& tmp11 = Points(i, (j + 1));

        if(!isValid(p))
        {
          Normals(i, j) = VecType{0};
          continue;
        }

        VecType norm{0};
        int n = 0;
        const bool validTmp00 = isValid(tmp00) && VecType::Dist(tmp00, p) <= distThr;
        const bool validTmp01 = isValid(tmp01) && VecType::Dist(tmp01, p) <= distThr;
        const bool validTmp10 = isValid(tmp10) && VecType::Dist(tmp10, p) <= distThr;
        const bool validTmp11 = isValid(tmp11) && VecType::Dist(tmp11, p) <= distThr;

        if(validTmp11 && validTmp10)
        {
          norm += correctNormal(VecType::Cross(tmp11 - p, tmp01 - p), p);
          n++;
        }

        if(validTmp01 && validTmp10)
        {
          norm += correctNormal(VecType::Cross(tmp01 - p, tmp10 - p), p);
          n++;
        }

        if(validTmp10 && validTmp00)
        {
          norm += correctNormal(VecType::Cross(tmp10 - p, tmp00 - p), p);
          n++;
        }

        if(validTmp00 && validTmp11)
        {
          norm += correctNormal(VecType::Cross(tmp00 - p, tmp11 - p), p);
          n++;
        }

        if(n == 0)
        {
          Normals(i, j) = VecType{0};
        }
        else
        {
          Normals(i, j) = VecType::Normalize(norm);
        }
      }
    }

    // Clean points with empty normals
    for(size_t i = 0; i < width_ * height_; i++)
    {
      if(Normals()[i] == VecType{0})
      {
        Points()[i] = VecType{std::numeric_limits<ScalarType>::max()};
      }
    }
  }

  void ExportPLY(const char* filename)
  {
    std::vector<PointType> validPoints;
    for(size_t i = 0; i < width_ * height_; i++)
    {
      if(Points()[i] != VecType{PointType::maxScalar})
      {
        validPoints.emplace_back(points_[i]);
      }
    }

    FILE* fp = fopen(filename, "w+");
    if(!fp)
    {
      char msg[512];
      sprintf(msg, "Error opening %s : %s\n", filename, strerror(errno));
      throw std::runtime_error(msg);
    }

    fprintf(fp, "ply\n");
    fprintf(fp, "format ascii 1.0\n");
    fprintf(fp, "element vertex %lu\n", validPoints.size());
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

    for(const auto& point : validPoints)
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

    fclose(fp);
  }

private:
  using PointTypeList = PointList<PointType>;

  size_t width_;
  size_t height_;
  PointTypeList points_;
  VecType org_;
};
} // namespace data_types
} // namespace spf
