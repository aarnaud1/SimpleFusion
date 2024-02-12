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

#include <cassert>
#include <cmath>
#include <memory>

#include <spf/utils.hpp>
#include <spf/data_types/AlignedImage.hpp>

namespace spf
{
namespace math
{
void BilateralFilter(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const size_t stride,
    const float sigmaSpace, const float sigmaColor);

void ErosionFilter(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const uint16_t thr);

void AnisotropicDiffusion(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const float sigma,
    const float lambda, const size_t n);
} // namespace math
} // namespace spf