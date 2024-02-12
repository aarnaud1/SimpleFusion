/*
 * Copyright (C) 2024  Adrien ARNAUD
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

#include <stdint.h>
#include <math.h>
#include <string.h>

#include "spf/geometry/vec3.hpp"
#include "spf/geometry/vec4.hpp"
#include "spf/geometry/mat3.hpp"

namespace spf
{
namespace geometry
{
template <typename T>
struct Mat4
{
  using DataType = T;

  T c00, c01, c02, c03;
  T c10, c11, c12, c13;
  T c20, c21, c22, c23;
  T c30, c31, c32, c33;

  inline Mat4()
  {
    c00 = T(0);
    c01 = T(0);
    c02 = T(0);
    c03 = T(0);

    c10 = T(0);
    c11 = T(0);
    c12 = T(0);
    c13 = T(0);

    c20 = T(0);
    c21 = T(0);
    c22 = T(0);
    c23 = T(0);

    c30 = T(0);
    c31 = T(0);
    c32 = T(0);
    c33 = T(0);
  }

  template <typename U>
  inline Mat4(U coeffs[16])
  {
    c00 = T(coeffs[0]);
    c01 = T(coeffs[1]);
    c02 = T(coeffs[2]);
    c03 = T(coeffs[3]);

    c10 = T(coeffs[4]);
    c11 = T(coeffs[5]);
    c12 = T(coeffs[6]);
    c13 = T(coeffs[7]);

    c20 = T(coeffs[8]);
    c21 = T(coeffs[9]);
    c22 = T(coeffs[10]);
    c23 = T(coeffs[11]);

    c30 = T(coeffs[12]);
    c31 = T(coeffs[13]);
    c32 = T(coeffs[14]);
    c33 = T(coeffs[15]);
  }

  template <typename U>
  inline Mat4(Vec4<U> const &v0, Vec4<U> const &v1, Vec4<U> const &v2, Vec4<U> const &v3)
  {
    this->c00 = T(v0.x);
    this->c01 = T(v0.y);
    this->c02 = T(v0.z);
    this->c03 = T(v0.t);

    this->c10 = T(v1.x);
    this->c11 = T(v1.y);
    this->c12 = T(v1.z);
    this->c13 = T(v1.t);

    this->c20 = T(v2.x);
    this->c21 = T(v2.y);
    this->c22 = T(v2.z);
    this->c23 = T(v2.t);

    this->c30 = T(v3.x);
    this->c31 = T(v3.y);
    this->c32 = T(v3.z);
    this->c33 = T(v3.t);
  }

  template <typename U>
  inline Mat4(Mat4<U> const &cp)
  {
    this->c00 = T(cp.c00);
    this->c01 = T(cp.c01);
    this->c02 = T(cp.c02);
    this->c03 = T(cp.c03);

    this->c10 = T(cp.c10);
    this->c11 = T(cp.c11);
    this->c12 = T(cp.c12);
    this->c13 = T(cp.c13);

    this->c20 = T(cp.c20);
    this->c21 = T(cp.c21);
    this->c22 = T(cp.c22);
    this->c23 = T(cp.c23);

    this->c30 = T(cp.c30);
    this->c31 = T(cp.c31);
    this->c32 = T(cp.c32);
    this->c33 = T(cp.c33);
  }

  template <typename U>
  inline Mat4 &operator=(Mat4<U> const &m)
  {
    this->c00 = T(m.c00);
    this->c01 = T(m.c01);
    this->c02 = T(m.c02);
    this->c03 = T(m.c03);

    this->c10 = T(m.c10);
    this->c11 = T(m.c11);
    this->c12 = T(m.c12);
    this->c13 = T(m.c13);

    this->c20 = T(m.c20);
    this->c21 = T(m.c21);
    this->c22 = T(m.c22);
    this->c23 = T(m.c23);

    this->c30 = T(m.c30);
    this->c31 = T(m.c31);
    this->c32 = T(m.c32);
    this->c33 = T(m.c33);
    return *this;
  }

  inline Mat4 &operator+=(Mat4 const &m)
  {
    this->c00 += m.c00;
    this->c01 += m.c01;
    this->c02 += m.c02;
    this->c03 += m.c03;

    this->c10 += m.c10;
    this->c11 += m.c11;
    this->c12 += m.c12;
    this->c13 += m.c13;

    this->c20 += m.c20;
    this->c21 += m.c21;
    this->c22 += m.c22;
    this->c23 += m.c23;

    this->c30 += m.c30;
    this->c31 += m.c31;
    this->c32 += m.c32;
    this->c33 += m.c33;
    return *this;
  }

  inline Mat4 &operator-=(Mat4 const &m)
  {
    this->c00 -= m.c00;
    this->c01 -= m.c01;
    this->c02 -= m.c02;
    this->c03 -= m.c03;

    this->c10 -= m.c10;
    this->c11 -= m.c11;
    this->c12 -= m.c12;
    this->c13 -= m.c13;

    this->c20 -= m.c20;
    this->c21 -= m.c21;
    this->c22 -= m.c22;
    this->c23 -= m.c23;

    this->c30 -= m.c30;
    this->c31 -= m.c31;
    this->c32 -= m.c32;
    this->c33 -= m.c33;
    return *this;
  }

  // this <- m * this
  inline Mat4 &operator*=(Mat4 const &m)
  {
    this->c00 = m.c00 * this->c00 + m.c01 * this->c10 + m.c02 * this->c20 + m.c03 * this->c30;
    this->c01 = m.c00 * this->c01 + m.c01 * this->c11 + m.c02 * this->c21 + m.c03 * this->c31;
    this->c02 = m.c00 * this->c02 + m.c01 * this->c12 + m.c02 * this->c22 + m.c03 * this->c32;
    this->c03 = m.c00 * this->c03 + m.c01 * this->c13 + m.c02 * this->c23 + m.c03 * this->c33;

    this->c10 = m.c10 * this->c00 + m.c11 * this->c10 + m.c12 * this->c20 + m.c13 * this->c30;
    this->c11 = m.c10 * this->c01 + m.c11 * this->c11 + m.c12 * this->c21 + m.c13 * this->c31;
    this->c12 = m.c10 * this->c02 + m.c11 * this->c12 + m.c12 * this->c22 + m.c13 * this->c32;
    this->c13 = m.c10 * this->c03 + m.c11 * this->c13 + m.c12 * this->c23 + m.c13 * this->c33;

    this->c20 = m.c20 * this->c00 + m.c21 * this->c10 + m.c22 * this->c20 + m.c23 * this->c30;
    this->c21 = m.c20 * this->c01 + m.c21 * this->c11 + m.c22 * this->c21 + m.c23 * this->c31;
    this->c22 = m.c20 * this->c02 + m.c21 * this->c12 + m.c22 * this->c22 + m.c23 * this->c32;
    this->c23 = m.c20 * this->c03 + m.c21 * this->c13 + m.c22 * this->c23 + m.c23 * this->c33;

    this->c30 = m.c30 * this->c00 + m.c31 * this->c10 + m.c32 * this->c20 + m.c33 * this->c30;
    this->c31 = m.c30 * this->c01 + m.c31 * this->c11 + m.c32 * this->c21 + m.c33 * this->c31;
    this->c32 = m.c30 * this->c02 + m.c31 * this->c12 + m.c32 * this->c22 + m.c33 * this->c32;
    this->c33 = m.c30 * this->c03 + m.c31 * this->c13 + m.c32 * this->c23 + m.c33 * this->c33;

    return *this;
  }

  inline Mat4 &operator*=(const T k)
  {
    this->c00 *= k;
    this->c01 *= k;
    this->c02 *= k;
    this->c03 *= k;

    this->c10 *= k;
    this->c11 *= k;
    this->c12 *= k;
    this->c13 *= k;

    this->c20 *= k;
    this->c21 *= k;
    this->c22 *= k;
    this->c23 *= k;

    this->c30 *= k;
    this->c31 *= k;
    this->c32 *= k;
    this->c33 *= k;

    return *this;
  }

  inline Mat4 operator+(Mat4 const &m) const
  {
    Mat4 ret;

    ret.c00 = this->c00 + m.c00;
    ret.c01 = this->c01 + m.c01;
    ret.c02 = this->c02 + m.c02;
    ret.c03 = this->c03 + m.c03;
    ret.c10 = this->c10 + m.c10;
    ret.c11 = this->c11 + m.c11;
    ret.c12 = this->c12 + m.c12;
    ret.c13 = this->c13 + m.c13;
    ret.c20 = this->c20 + m.c20;
    ret.c21 = this->c21 + m.c21;
    ret.c22 = this->c22 + m.c22;
    ret.c23 = this->c23 + m.c23;
    ret.c30 = this->c30 + m.c30;
    ret.c31 = this->c31 + m.c31;
    ret.c32 = this->c32 + m.c32;
    ret.c33 = this->c33 + m.c33;

    return ret;
  }

  inline Mat4 operator-(Mat4 const &m) const
  {
    Mat4 ret;

    ret.c00 = this->c00 - m.c00;
    ret.c01 = this->c01 - m.c01;
    ret.c02 = this->c02 - m.c02;
    ret.c03 = this->c03 - m.c03;
    ret.c10 = this->c10 - m.c10;
    ret.c11 = this->c11 - m.c11;
    ret.c12 = this->c12 - m.c12;
    ret.c13 = this->c13 - m.c13;
    ret.c20 = this->c20 - m.c20;
    ret.c21 = this->c21 - m.c21;
    ret.c22 = this->c22 - m.c22;
    ret.c23 = this->c23 - m.c23;
    ret.c30 = this->c30 - m.c30;
    ret.c31 = this->c31 - m.c31;
    ret.c32 = this->c32 - m.c32;
    ret.c33 = this->c33 - m.c33;

    return ret;
  }

  inline Mat4 &operator-()
  {
    this->c00 = -this->c00;
    this->c01 = -this->c01;
    this->c02 = -this->c02;
    this->c03 = -this->c03;
    this->c10 = -this->c10;
    this->c11 = -this->c11;
    this->c12 = -this->c12;
    this->c13 = -this->c13;
    this->c20 = -this->c20;
    this->c21 = -this->c21;
    this->c22 = -this->c22;
    this->c23 = -this->c23;
    this->c30 = -this->c30;
    this->c31 = -this->c31;
    this->c32 = -this->c32;
    this->c33 = -this->c33;

    return *this;
  }

  inline Mat4 operator/(const T k) const
  {
    Mat4 ret;

    ret.c00 = this->c00 / k;
    ret.c01 = this->c01 / k;
    ret.c02 = this->c02 / k;
    ret.c03 = this->c03 / k;
    ret.c10 = this->c10 / k;
    ret.c11 = this->c11 / k;
    ret.c12 = this->c12 / k;
    ret.c13 = this->c13 / k;
    ret.c20 = this->c20 / k;
    ret.c21 = this->c21 / k;
    ret.c22 = this->c22 / k;
    ret.c23 = this->c23 / k;
    ret.c30 = this->c30 / k;
    ret.c31 = this->c31 / k;
    ret.c32 = this->c32 / k;
    ret.c33 = this->c33 / k;

    return ret;
  }

  inline Mat4 operator*(const T k) const
  {
    Mat4 ret;

    ret.c00 = this->c00 * k;
    ret.c01 = this->c01 * k;
    ret.c02 = this->c02 * k;
    ret.c03 = this->c03 * k;
    ret.c10 = this->c10 * k;
    ret.c11 = this->c11 * k;
    ret.c12 = this->c12 * k;
    ret.c13 = this->c13 * k;
    ret.c20 = this->c20 * k;
    ret.c21 = this->c21 * k;
    ret.c22 = this->c22 * k;
    ret.c23 = this->c23 * k;
    ret.c30 = this->c30 * k;
    ret.c31 = this->c31 * k;
    ret.c32 = this->c32 * k;
    ret.c33 = this->c33 * k;

    return ret;
  }

  inline Mat4 operator*(Mat4 const &m1) const
  {
    Mat4 ret;

    ret.c00 = this->c00 * m1.c00 + this->c01 * m1.c10 + this->c02 * m1.c20 + this->c03 * m1.c30;
    ret.c01 = this->c00 * m1.c01 + this->c01 * m1.c11 + this->c02 * m1.c21 + this->c03 * m1.c31;
    ret.c02 = this->c00 * m1.c02 + this->c01 * m1.c12 + this->c02 * m1.c22 + this->c03 * m1.c32;
    ret.c03 = this->c00 * m1.c03 + this->c01 * m1.c13 + this->c02 * m1.c23 + this->c03 * m1.c33;
    ret.c10 = this->c10 * m1.c00 + this->c11 * m1.c10 + this->c12 * m1.c20 + this->c13 * m1.c30;
    ret.c11 = this->c10 * m1.c01 + this->c11 * m1.c11 + this->c12 * m1.c21 + this->c13 * m1.c31;
    ret.c12 = this->c10 * m1.c02 + this->c11 * m1.c12 + this->c12 * m1.c22 + this->c13 * m1.c32;
    ret.c13 = this->c10 * m1.c03 + this->c11 * m1.c13 + this->c12 * m1.c23 + this->c13 * m1.c33;
    ret.c20 = this->c20 * m1.c00 + this->c21 * m1.c10 + this->c22 * m1.c20 + this->c23 * m1.c30;
    ret.c21 = this->c20 * m1.c01 + this->c21 * m1.c11 + this->c22 * m1.c21 + this->c23 * m1.c31;
    ret.c22 = this->c20 * m1.c02 + this->c21 * m1.c12 + this->c22 * m1.c22 + this->c23 * m1.c32;
    ret.c23 = this->c20 * m1.c03 + this->c21 * m1.c13 + this->c22 * m1.c23 + this->c23 * m1.c33;
    ret.c30 = this->c30 * m1.c00 + this->c31 * m1.c10 + this->c32 * m1.c20 + this->c33 * m1.c30;
    ret.c31 = this->c30 * m1.c01 + this->c31 * m1.c11 + this->c32 * m1.c21 + this->c33 * m1.c31;
    ret.c32 = this->c30 * m1.c02 + this->c31 * m1.c12 + this->c32 * m1.c22 + this->c33 * m1.c32;
    ret.c33 = this->c30 * m1.c03 + this->c31 * m1.c13 + this->c32 * m1.c23 + this->c33 * m1.c33;

    return ret;
  }

  inline Vec4<T> operator*(Vec4<T> const &v) const
  {
    Vec4<T> ret;

    ret.x = this->c00 * v.x + this->c01 * v.y + this->c02 * v.z + this->c03 * v.t;
    ret.y = this->c10 * v.x + this->c11 * v.y + this->c12 * v.z + this->c13 * v.t;
    ret.z = this->c20 * v.x + this->c21 * v.y + this->c22 * v.z + this->c23 * v.t;
    ret.t = this->c30 * v.x + this->c31 * v.y + this->c32 * v.z + this->c33 * v.t;

    return ret;
  }

  inline void Transpose()
  {
    const T swap10 = this->c10;
    this->c10 = this->c01;
    this->c01 = swap10;

    const T swap20 = this->c20;
    this->c20 = this->c02;
    this->c02 = swap20;

    const T swap30 = this->c30;
    this->c30 = this->c03;
    this->c03 = swap30;

    const T swap21 = this->c21;
    this->c21 = this->c12;
    this->c12 = swap21;

    const T swap31 = this->c31;
    this->c31 = this->c13;
    this->c13 = swap31;

    const T swap32 = this->c32;
    this->c32 = this->c23;
    this->c23 = swap32;
  }

  inline T Det() const
  {
    const T SubFactor00 = this->c22 * this->c33 - this->c32 * this->c23;
    const T SubFactor01 = this->c21 * this->c33 - this->c31 * this->c23;
    const T SubFactor02 = this->c21 * this->c32 - this->c31 * this->c22;
    const T SubFactor03 = this->c20 * this->c33 - this->c30 * this->c23;
    const T SubFactor04 = this->c20 * this->c32 - this->c30 * this->c22;
    const T SubFactor05 = this->c20 * this->c31 - this->c30 * this->c21;

    const T tmpc00 = +(this->c11 * SubFactor00 - this->c12 * SubFactor01 + this->c13 * SubFactor02);
    const T tmpc01 = -(this->c10 * SubFactor00 - this->c12 * SubFactor03 + this->c13 * SubFactor04);
    const T tmpc02 = +(this->c10 * SubFactor01 - this->c11 * SubFactor03 + this->c13 * SubFactor05);
    const T tmpc03 = -(this->c10 * SubFactor02 - this->c11 * SubFactor04 + this->c12 * SubFactor05);

    return this->c00 * tmpc00 + this->c01 * tmpc01 + this->c02 * tmpc02 + this->c03 * tmpc03;
  }

  inline void Inverse()
  {
    const T SubFactor00 = this->c22 * this->c33 - this->c32 * this->c23;
    const T SubFactor01 = this->c21 * this->c33 - this->c31 * this->c23;
    const T SubFactor02 = this->c21 * this->c32 - this->c31 * this->c22;
    const T SubFactor03 = this->c20 * this->c33 - this->c30 * this->c23;
    const T SubFactor04 = this->c20 * this->c32 - this->c30 * this->c22;
    const T SubFactor05 = this->c20 * this->c31 - this->c30 * this->c21;
    const T SubFactor06 = this->c12 * this->c33 - this->c32 * this->c13;
    const T SubFactor07 = this->c11 * this->c33 - this->c31 * this->c13;
    const T SubFactor08 = this->c11 * this->c32 - this->c31 * this->c12;
    const T SubFactor09 = this->c10 * this->c33 - this->c30 * this->c13;
    const T SubFactor10 = this->c10 * this->c32 - this->c30 * this->c12;
    const T SubFactor11 = this->c11 * this->c33 - this->c31 * this->c13;
    const T SubFactor12 = this->c10 * this->c31 - this->c30 * this->c11;
    const T SubFactor13 = this->c12 * this->c23 - this->c22 * this->c13;
    const T SubFactor14 = this->c11 * this->c23 - this->c21 * this->c13;
    const T SubFactor15 = this->c11 * this->c22 - this->c21 * this->c12;
    const T SubFactor16 = this->c10 * this->c23 - this->c20 * this->c13;
    const T SubFactor17 = this->c10 * this->c22 - this->c20 * this->c12;
    const T SubFactor18 = this->c10 * this->c21 - this->c20 * this->c11;

    Mat4 tmp;

    tmp.c00 = +(this->c11 * SubFactor00 - this->c12 * SubFactor01 + this->c13 * SubFactor02);
    tmp.c01 = -(this->c10 * SubFactor00 - this->c12 * SubFactor03 + this->c13 * SubFactor04);
    tmp.c02 = +(this->c10 * SubFactor01 - this->c11 * SubFactor03 + this->c13 * SubFactor05);
    tmp.c03 = -(this->c10 * SubFactor02 - this->c11 * SubFactor04 + this->c12 * SubFactor05);
    tmp.c10 = -(this->c01 * SubFactor00 - this->c02 * SubFactor01 + this->c03 * SubFactor02);
    tmp.c11 = +(this->c00 * SubFactor00 - this->c02 * SubFactor03 + this->c03 * SubFactor04);
    tmp.c12 = -(this->c00 * SubFactor01 - this->c01 * SubFactor03 + this->c03 * SubFactor05);
    tmp.c13 = +(this->c00 * SubFactor02 - this->c01 * SubFactor04 + this->c02 * SubFactor05);
    tmp.c20 = +(this->c01 * SubFactor06 - this->c02 * SubFactor07 + this->c03 * SubFactor08);
    tmp.c21 = -(this->c00 * SubFactor06 - this->c02 * SubFactor09 + this->c03 * SubFactor10);
    tmp.c22 = +(this->c00 * SubFactor11 - this->c01 * SubFactor09 + this->c03 * SubFactor12);
    tmp.c23 = -(this->c00 * SubFactor08 - this->c01 * SubFactor10 + this->c02 * SubFactor12);
    tmp.c30 = -(this->c01 * SubFactor13 - this->c02 * SubFactor14 + this->c03 * SubFactor15);
    tmp.c31 = +(this->c00 * SubFactor13 - this->c02 * SubFactor16 + this->c03 * SubFactor17);
    tmp.c32 = -(this->c00 * SubFactor14 - this->c01 * SubFactor16 + this->c03 * SubFactor18);
    tmp.c33 = +(this->c00 * SubFactor15 - this->c01 * SubFactor17 + this->c02 * SubFactor18);

    const T det =
        this->c00 * tmp.c00 + this->c01 * tmp.c01 + this->c02 * tmp.c02 + this->c03 * tmp.c03;

    tmp.c00 /= det;
    tmp.c01 /= det;
    tmp.c02 /= det;
    tmp.c03 /= det;
    tmp.c10 /= det;
    tmp.c11 /= det;
    tmp.c12 /= det;
    tmp.c13 /= det;
    tmp.c20 /= det;
    tmp.c21 /= det;
    tmp.c22 /= det;
    tmp.c23 /= det;
    tmp.c30 /= det;
    tmp.c31 /= det;
    tmp.c32 /= det;
    tmp.c33 /= det;

    tmp.Transpose();
    *this = tmp;
  }

  inline void SetRotation(const T *rot)
  {
    this->c00 = rot[0];
    this->c01 = rot[1];
    this->c02 = rot[2];
    this->c10 = rot[3];
    this->c11 = rot[4];
    this->c12 = rot[5];
    this->c20 = rot[6];
    this->c21 = rot[7];
    this->c22 = rot[8];
  }

  inline void SetRotation(Mat3<T> const &rot)
  {
    this->c00 = rot.c00;
    this->c01 = rot.c01;
    this->c02 = rot.c02;
    this->c10 = rot.c10;
    this->c11 = rot.c11;
    this->c12 = rot.c12;
    this->c20 = rot.c20;
    this->c21 = rot.c21;
    this->c22 = rot.c22;
  }

  inline void SetRotation(Mat4 const &rot)
  {
    this->c00 = rot.c00;
    this->c01 = rot.c01;
    this->c02 = rot.c02;
    this->c10 = rot.c10;
    this->c11 = rot.c11;
    this->c12 = rot.c12;
    this->c20 = rot.c20;
    this->c21 = rot.c21;
    this->c22 = rot.c22;
  }

  inline void SetTranslation(Vec3<T> const &t)
  {
    this->c03 = t.x;
    this->c13 = t.y;
    this->c23 = t.z;
  }

  inline void SetTranslation(Vec4<T> const &t)
  {
    this->c03 = t.x;
    this->c13 = t.y;
    this->c23 = t.z;
  }

  inline Mat3<T> GetRotation() const
  {
    Mat3<T> ret;
    ret.c00 = this->c00;
    ret.c01 = this->c01;
    ret.c02 = this->c02;

    ret.c10 = this->c10;
    ret.c11 = this->c11;
    ret.c12 = this->c12;

    ret.c20 = this->c20;
    ret.c21 = this->c21;
    ret.c22 = this->c22;

    return ret;
  }

  inline Vec4<T> GetTranslation() const
  {
    Vec4<T> ret;
    ret.x = this->c03;
    ret.y = this->c13;
    ret.z = this->c23;
    ret.t = T(1);
    return ret;
  }

  // Static functions

  static inline Mat4 Identity()
  {
    Mat4 ret;
    ret.c00 = T(1);
    ret.c01 = T(0);
    ret.c02 = T(0);
    ret.c03 = T(0);

    ret.c10 = T(0);
    ret.c11 = T(1);
    ret.c12 = T(0);
    ret.c13 = T(0);

    ret.c20 = T(0);
    ret.c21 = T(0);
    ret.c22 = T(1);
    ret.c23 = T(0);

    ret.c30 = T(0);
    ret.c31 = T(0);
    ret.c32 = T(0);
    ret.c33 = T(1);
    return ret;
  }

  static inline Mat4 FromQuat(Vec4<T> const &quat)
  {
    Mat4 ret = Mat4::Identity();

    const T w = quat.x;
    const T x = quat.y;
    const T y = quat.z;
    const T z = quat.t;

    const T qxx = x * x;
    const T qyy = y * y;
    const T qzz = z * z;
    const T qxz = x * z;
    const T qxy = x * y;
    const T qyz = y * z;
    const T qwx = w * x;
    const T qwy = w * y;
    const T qwz = w * z;

    ret.c00 = T(1) - T(2) * (qyy + qzz);
    ret.c01 = T(2) * (qxy + qwz);
    ret.c02 = T(2) * (qxz - qwy);

    ret.c10 = T(2) * (qxy - qwz);
    ret.c11 = T(1) - T(2) * (qxx + qzz);
    ret.c12 = T(2) * (qyz + qwx);

    ret.c20 = T(2) * (qxz + qwy);
    ret.c21 = T(2) * (qyz - qwx);
    ret.c22 = T(1) - T(2) * (qxx + qyy);

    ret.c33 = T(1);

    return ret;
  }

  static inline Mat4 FromQuat(const T w, const T x, const T y, const T z)
  {
    return Mat4::FromQuat(Vec4<T>(w, x, y, z));
  }

  static inline Mat4 Rotation(Vec3<T> const &axis, const T theta)
  {
    Mat4 ret;

    const Vec3<T> axis_ = Vec3<T>::Normalize(axis);

    const T x = axis_.x;
    const T y = axis_.y;
    const T z = axis_.z;
    const T c = cosf(theta);
    const T s = sin(theta);

    ret.c00 = x * x * (T(1) - c) + c;
    ret.c01 = x * y * (T(1) - c) - z * s;
    ret.c02 = x * z * (T(1) - c) + y * s;

    ret.c10 = x * y * (T(1) - c) + z * s;
    ret.c11 = y * y * (T(1) - c) + c;
    ret.c12 = y * z * (T(1) - c) - x * s;

    ret.c20 = x * z * (T(1) - c) - y * s;
    ret.c21 = y * z * (T(1) - c) + x * s;
    ret.c22 = z * z * (T(1) - c) + c;

    ret.c33 = T(1);

    return ret;
  }

  static inline Mat4 Affine(Vec3<T> const &axis, const T theta, Vec3<T> const &t)
  {
    Mat4 ret = Mat4::Rotation(axis, theta);
    ret.c03 = t.x;
    ret.c13 = t.y;
    ret.c23 = t.z;
    return ret;
  }

  static inline Mat4 Affine(Vec4<T> const &quat, Vec3<T> const &trans)
  {
    Mat4 ret = Mat4::FromQuat(quat);
    ret.SetTranslation(trans);
    return ret;
  }

  static inline Mat4 Affine(Mat3<T> const &r, Vec3<T> const &t)
  {
    Mat4 ret = Mat4::Identity();
    ret.SetTranslation(t);
    ret.SetRotation(r);
    return ret;
  }

  static inline Mat4 Affine(Vec3<T> const &t, Mat3<T> const &r)
  {
    Mat4 ret = Mat4::Identity();
    ret.SetTranslation(t);
    ret.SetRotation(r);
    return ret;
  }

  static inline Mat4 Perspective(const T fovy, const T aspect, const T near, const T far)
  {
    Mat4 ret = Mat4::Identity();
    const T theta = (T) M_PI * fovy * T(0.5) / T(180);
    const T range = far - near;
    const T invtan = T(1) / tanf(theta);

    ret.c00 = invtan / aspect;
    ret.c11 = invtan;
    ret.c22 = -(near + far) / range;
    ret.c32 = -T(1);
    ret.c23 = -T(2) * near * far / range;

    return ret;
  }

  static inline Mat4 LookAt(Vec3<T> const &position, Vec3<T> const &direction, Vec3<T> const &up)
  {
    Mat4 ret = Mat4::Identity();
    const Vec3<T> forward = Vec3<T>::Normalize(position - direction);
    const Vec3<T> right = Vec3<T>::Cross(Vec3<T>::Normalize(up), forward);
    const Vec3<T> u_ = Vec3<T>::Cross(forward, right);
    const Vec3<T> de = -T(1) * position;

    ret.c00 = right.x;
    ret.c10 = right.y;
    ret.c20 = right.z;

    ret.c01 = u_.x;
    ret.c11 = u_.y;
    ret.c21 = u_.z;

    ret.c02 = forward.x;
    ret.c12 = forward.y;
    ret.c22 = forward.z;

    ret.Inverse();

    ret.c03 = Vec3<T>::Dot(de, right);
    ret.c13 = Vec3<T>::Dot(de, u_);
    ret.c23 = Vec3<T>::Dot(de, forward);
    return ret;
  }

  static inline Mat4 Transpose(Mat4 const &m)
  {
    Mat4 ret(m);
    ret.Transpose();
    return ret;
  }

  static inline Mat4 Inverse(Mat4 const &m)
  {
    Mat4 ret(m);
    ret.Inverse();
    return ret;
  }
};

template <typename T>
static inline Vec4<T> &operator*=(Vec4<T> &v, Mat4<T> const &m)
{
  v = Vec4<T>(m * v);
  return v;
}

template <typename T>
static inline Vec3<T> operator*(Mat4<T> const &m, Vec3<T> const &v)
{
  return (m * Vec4<T>(v)).xyz();
}

template <typename T>
static inline Vec3<T> &operator*=(Vec3<T> &v, Mat4<T> const &m)
{
  v = (m * Vec4<T>(v)).xyz();
  return v;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, Mat4<T> const &m)
{
  os << "|" << m.c00 << " " << m.c01 << " " << m.c02 << " " << m.c03 << "|\n";
  os << "|" << m.c10 << " " << m.c11 << " " << m.c12 << " " << m.c13 << "|\n";
  os << "|" << m.c20 << " " << m.c21 << " " << m.c22 << " " << m.c23 << "|\n";
  os << "|" << m.c30 << " " << m.c31 << " " << m.c32 << " " << m.c33 << "|";
  return os;
}

template <typename T>
inline std::istream &operator>>(std::istream &is, Mat4<T> &m)
{
  is >> m.c00 >> m.c01 >> m.c02 >> m.c03 >> m.c10 >> m.c11 >> m.c12 >> m.c13 >> m.c20 >> m.c21
      >> m.c22 >> m.c23 >> m.c30 >> m.c31 >> m.c32 >> m.c33;
  return is;
}
} // namespace geometry
} // namespace spf
