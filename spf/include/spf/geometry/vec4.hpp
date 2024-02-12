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
struct Vec4
{
  using DataType = T;

  T x;
  T y;
  T z;
  T t;

  inline Vec4() : x(0), y(0), z(0), t(0) {}

  template <typename U>
  inline Vec4(const U x, const U y, const U z, const U t) : x(T(x)), y(T(y)), z(T(z)), t(T(t))
  {}

  template <typename U>
  inline Vec4(Vec3<U> const &pos, const U t) : x(T(pos.x)), y(T(pos.y)), z(T(pos.z)), t(T(t))
  {}

  template <typename U>
  inline Vec4(Vec4<U> const &cp) : x(T(cp.x)), y(T(cp.y)), z(T(cp.z)), t(T(cp.t))
  {}

  template <typename U>
  inline Vec4(const U val)
  {
    this->x = T(val);
    this->y = T(val);
    this->z = T(val);
    this->t = T(val);
  }

  template <typename U>
  inline Vec4(Vec3<U> const &v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), t(T(1))
  {}

  inline bool operator==(Vec4 const &v0) const
  {
    return (x == v0.x) && (y == v0.y) && (z == v0.z) && (t == v0.t);
  }
  inline bool operator!=(Vec4 const &v0) const
  {
    return (x != v0.x) || (y != v0.y) || (z != v0.z) || (t != v0.t);
  }

  // Swizzle
  inline Vec3<T> xyz() const { return Vec3<T>(x, y, z); }

  template <typename U>
  inline Vec4 &operator=(Vec4<U> const &v0)
  {
    this->x = T(v0.x);
    this->y = T(v0.y);
    this->z = T(v0.z);
    this->t = T(v0.t);
    return *this;
  }

  inline Vec4 operator+(Vec4 const &v0)
  {
    Vec4 ret;
    ret.x = this->x + v0.x;
    ret.y = this->y + v0.y;
    ret.z = this->z + v0.z;
    ret.t = this->t + v0.t;
    return ret;
  }

  inline Vec4 &operator+=(Vec4 const &v0)
  {
    *this = *this + v0;
    return *this;
  }

  inline Vec4 operator-(Vec4 const &v0)
  {
    Vec4 ret;
    ret.x = this->x - v0.x;
    ret.y = this->y - v0.y;
    ret.z = this->z - v0.z;
    ret.t = this->t - v0.t;
    return ret;
  }

  inline Vec4 &operator-=(Vec4 const &v0)
  {
    *this = *this - v0;
    return *this;
  }

  inline Vec4 operator*(const T k)
  {
    Vec4 ret;
    ret.x = k * this->x;
    ret.y = k * this->y;
    ret.z = k * this->z;
    ret.t = k * this->t;
    return ret;
  }

  // Allow matrix - vector multiplication in the form V *= M
  template <typename U>
  inline Vec4 &operator*=(const U &m)
  {
    *this = m * *this;
    return *this;
  }

  inline Vec4 &operator*=(const T k)
  {
    *this = *this * k;
    return *this;
  }

  inline Vec4 operator/(const T k)
  {
    Vec4 ret;
    ret.x = this->x / k;
    ret.y = this->y / k;
    ret.z = this->z / k;
    ret.t = this->t / k;
    return ret;
  }

  inline Vec4 operator/=(const T k)
  {
    *this = *this / k;
    return *this;
  }

  inline T Len() { return std::sqrt(x * x + y * y + z * z + t * t); }

  inline T Dist(Vec4 const &v0)
  {
    const T dx = this->x - v0.x;
    const T dy = this->y - v0.y;
    const T dz = this->z - v0.z;
    const T dt = this->t - v0.t;
    return std::sqrt(dx * dx + dy * dy + dz * dz + dt * dt);
  }

  static inline T Len(Vec4 const &v0)
  {
    return std::sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z + v0.t * v0.t);
  }

  inline void Normalize()
  {
    *this = *this
            * (T(1)
               / std::sqrt(
                   this->x * this->x + this->y * this->y + this->z * this->z + this->t * this->t));
  }

  static inline T Dist(Vec4 const &v0, Vec4 const &v1)
  {
    const T dx = v0.x - v1.x;
    const T dy = v0.y - v1.y;
    const T dz = v0.z - v1.z;
    const T dt = v0.t - v1.t;
    return std::sqrt(dx * dx + dy * dy + dz * dz + dt * dt);
  }

  static inline Vec4 Normalize(Vec4 const &v0)
  {
    return Vec4(v0) * (T(1) / std::sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z + v0.t * v0.t));
  }

  static inline T Dot(Vec4 const &v0, Vec4 const &v1)
  {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.t * v1.t;
  }
};

template <typename T>
static inline Vec4<T> operator+(const Vec4<T> &v0, const Vec4<T> &v1)
{
  Vec4<T> ret;
  ret.x = v0.x + v1.x;
  ret.y = v0.y + v1.y;
  ret.z = v0.z + v1.z;
  ret.t = v0.t + v1.t;
  return ret;
}

template <typename T>
static inline Vec4<T> operator-(const Vec4<T> &v0, const Vec4<T> &v1)
{
  Vec4<T> ret;
  ret.x = v0.x - v1.x;
  ret.y = v0.y - v1.y;
  ret.z = v0.z - v1.z;
  ret.t = v0.t - v1.t;
  return ret;
}

template <typename T>
static inline Vec4<T> operator*(const Vec4<T> &v0, const T k)
{
  Vec4<T> ret;
  ret.x = k * v0.x;
  ret.y = k * v0.y;
  ret.z = k * v0.z;
  ret.t = k * v0.t;
  return ret;
}

// -----------------------------------------------------------------------------

template <typename T>
static inline std::ostream &operator<<(std::ostream &os, const Vec4<T> &v)
{
  os << "|" << v.x << " " << v.y << " " << v.z << " " << v.t << "|";
  return os;
}

template <typename T>
static inline std::istream &operator>>(std::istream &is, Vec4<T> &v)
{
  is >> v.x >> v.y >> v.z >> v.t;
  return is;
}

} // namespace geometry
} // namespace spf