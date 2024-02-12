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

#include "spf/geometry/geometry.hpp"

namespace spf
{
typedef spf::geometry::Vec4<float> Vec4f;
typedef spf::geometry::Vec4<float> Point4f;
typedef spf::geometry::Vec4<float> Color4f;
typedef spf::geometry::Vec4<float> Normal4f;

typedef spf::geometry::Vec3<float> Vec3f;
typedef spf::geometry::Vec3<float> Point3f;
typedef spf::geometry::Vec3<float> Color3f;
typedef spf::geometry::Vec3<float> Normal3f;

typedef spf::geometry::Mat4<float> Mat4f;
typedef spf::geometry::Mat3<float> Mat3f;

typedef spf::geometry::Vec4<double> Vec4d;
typedef spf::geometry::Vec4<double> Point4d;
typedef spf::geometry::Vec4<double> Color4d;
typedef spf::geometry::Vec4<double> Normal4d;

typedef spf::geometry::Vec3<double> Vec3d;
typedef spf::geometry::Vec3<double> Point3d;
typedef spf::geometry::Vec3<double> Color3d;
typedef spf::geometry::Vec3<double> Normal3d;

typedef spf::geometry::Mat4<double> Mat4d;
typedef spf::geometry::Mat3<double> Mat3d;
} // namespace spf
