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

#include <iostream>

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

namespace spf
{
namespace geometry
{
template <typename T>
struct Vec3
{
  using DataType = T;

  T x;
  T y;
  T z;

  inline Vec3() : x(0), y(0), z(0) {}

  template <typename U>
  inline Vec3(const U x, const U y, const U z) : x(T(x)), y(T(y)), z(T(z))
  {}

  template <typename U>
  inline Vec3(Vec3<U> const &cp) : x(T(cp.x)), y(T(cp.y)), z(T(cp.z))
  {}

  template <typename U>
  inline Vec3(const U val)
  {
    this->x = T(val);
    this->y = T(val);
    this->z = T(val);
  }

  inline bool operator==(Vec3 const &v0) const { return (x == v0.x) && (y == v0.y) && (z == v0.z); }
  inline bool operator!=(Vec3 const &v0) const { return (x != v0.x) || (y != v0.y) || (z != v0.z); }

  template <typename U>
  inline Vec3 &operator=(Vec3<U> const &v0)
  {
    this->x = T(v0.x);
    this->y = T(v0.y);
    this->z = T(v0.z);
    return *this;
  }

  inline Vec3 operator+(Vec3 const &v0)
  {
    Vec3 ret;
    ret.x = this->x + v0.x;
    ret.y = this->y + v0.y;
    ret.z = this->z + v0.z;
    return ret;
  }

  inline Vec3 &operator+=(Vec3 const &v0)
  {
    *this = *this + v0;
    return *this;
  }

  inline Vec3 operator-(Vec3 const &v0) const
  {
    Vec3 ret;
    ret.x = this->x - v0.x;
    ret.y = this->y - v0.y;
    ret.z = this->z - v0.z;
    return ret;
  }

  inline Vec3 operator-() const
  {
    Vec3 ret;
    ret.x = -this->x;
    ret.y = -this->y;
    ret.z = -this->z;
    return ret;
  }

  inline Vec3 &operator-=(Vec3 const &v0)
  {
    *this = *this - v0;
    return *this;
  }

  inline Vec3 operator*(const T k) const
  {
    Vec3 ret;
    ret.x = k * this->x;
    ret.y = k * this->y;
    ret.z = k * this->z;
    return ret;
  }

  inline Vec3 &operator*=(const T k)
  {
    *this = *this * k;
    return *this;
  }

  inline Vec3 operator/(const T k) const
  {
    Vec3 ret;
    ret.x = this->x / k;
    ret.y = this->y / k;
    ret.z = this->z / k;
    return ret;
  }

  inline Vec3 operator/=(const T k)
  {
    *this = *this / k;
    return *this;
  }

  inline T Len() { return std::sqrt(x * x + y * y + z * z); }

  inline T Dist(Vec3 const &v0)
  {
    const T dx = this->x - v0.x;
    const T dy = this->y - v0.y;
    const T dz = this->z - v0.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }

  static inline T Len(Vec3 const &v0) { return std::sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z); }

  inline void Normalize()
  {
    const T l = std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    *this = *this * (T(1) / l);
  }

  static inline T Dist(Vec3 const &v0, Vec3 const &v1)
  {
    const T dx = v0.x - v1.x;
    const T dy = v0.y - v1.y;
    const T dz = v0.z - v1.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }

  static inline Vec3 Normalize(Vec3 const &v0)
  {
    return Vec3(v0) * (T(1) / std::sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z));
  }

  static inline T Dot(Vec3 const &v0, Vec3 const &v1)
  {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
  }

  static inline Vec3 Cross(Vec3 const &v0, Vec3 const &v1)
  {
    Vec3 ret;

    ret.x = v0.y * v1.z - v0.z * v1.y;
    ret.y = v0.z * v1.x - v0.x * v1.z;
    ret.z = v0.x * v1.y - v0.y * v1.x;

    return ret;
  }

  static inline Vec3 Reflect(Vec3 const &I, Vec3 const &N)
  {
    Vec3 ret;
    Vec3 normalized = Vec3::Normalize(N);
    T tmp = 2.0f * Vec3::Dot(normalized, I);
    ret = Vec3(I) - normalized * tmp;
    return ret;
  }
};

template <typename T>
static inline Vec3<T> operator+(const Vec3<T> &v0, const Vec3<T> &v1)
{
  Vec3<T> ret;
  ret.x = v0.x + v1.x;
  ret.y = v0.y + v1.y;
  ret.z = v0.z + v1.z;
  return ret;
}

template <typename T>
static inline Vec3<T> operator-(const Vec3<T> &v0, const Vec3<T> &v1)
{
  Vec3<T> ret;
  ret.x = v0.x - v1.x;
  ret.y = v0.y - v1.y;
  ret.z = v0.z - v1.z;
  return ret;
}

template <typename T>
static inline Vec3<T> operator*(const T k, const Vec3<T> &v0)
{
  Vec3<T> ret;
  ret.x = k * v0.x;
  ret.y = k * v0.y;
  ret.z = k * v0.z;
  return ret;
}

// -----------------------------------------------------------------------------

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vec3<T> &v)
{
  os << "|" << v.x << " " << v.y << " " << v.z << "|";
  return os;
}

template <typename T>
inline std::istream &operator>>(std::istream &is, Vec3<T> &v)
{
  is >> v.x >> v.y >> v.z;
  return is;
}
} // namespace geometry
} // namespace spf
