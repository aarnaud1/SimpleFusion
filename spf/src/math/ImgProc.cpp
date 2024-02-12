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

#include "spf/math/ImgProc.hpp"
#include "spf/math/simd.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void BilateralFilter_(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const size_t stride,
    const float sigmaSpace, const float sigmaColor);

static void BilateralFilterAvx_(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const size_t stride,
    const float sigmaSpace, const float sigmaColor);

static void AnisotropicDiffusionInnerKernel(
    const float* src, float* dst, const size_t w, const size_t h, const float sigma,
    const float lambda);

namespace spf
{
using namespace data_types;

namespace math
{
void BilateralFilter(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const size_t stride,
    const float sigmaSpace, const float sigmaColor)
{
  BilateralFilterAvx_(src, dst, w, h, stride, sigmaSpace, sigmaColor);
}

void ErosionFilter(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const uint16_t thr)
{
  constexpr size_t k_h = 4;
  constexpr size_t k_w = 4;

  START_CHRONO("Erosion filter");
#pragma omp parallel for
  for(size_t i = k_h; i < h - k_h; i++)
  {
    for(size_t j = k_w; j < w - k_w; j++)
    {
      bool check = true;
      for(size_t ii = 0; ii <= k_h; ii++)
      {
        for(size_t jj = 0; jj <= k_w; jj++)
        {
          if(src[(i + ii - k_h) * w + j + jj - k_w] < thr)
          {
            check = false;
            goto next;
          }
        }
      }
    next:
      dst[i * w + j] = check ? src[i * w + j] : 0;
    }
  }
  STOP_CHRONO();
}

void AnisotropicDiffusion(
    const uint16_t* src, uint16_t* dst, const size_t w, const size_t h, const float sigma,
    const float lambda, const size_t n)
{
  START_CHRONO("Anisitropic diffusion");
  std::unique_ptr<float[]> tmp(new float[w * h * (n + 1)]);
  for(size_t idx = 0; idx < w * h; idx++)
  {
    tmp.get()[idx] = (float) src[idx];
  }

  for(size_t p = 0; p < n; p++)
  {
    AnisotropicDiffusionInnerKernel(
        tmp.get() + p * (w * h), tmp.get() + (p + 1) * (w * h), w, h, sigma, lambda);
  }

  for(size_t idx = 0; idx < w * h; idx++)
  {
    dst[idx] = (uint16_t) (tmp.get() + n * w * h)[idx];
  }
  STOP_CHRONO();
}
} // namespace math
} // namespace spf

static void BilateralFilter_(
    const uint16_t* __restrict__ src, uint16_t* __restrict__ dst, const size_t w, const size_t h,
    const size_t stride, const float sigmaSpace, const float sigmaColor)
{
  constexpr size_t D = 13;
  constexpr size_t k_h = D / 2;
  constexpr size_t k_w = D / 2;

  const float sigma_space2_inv_half = 0.5f / (sigmaSpace * sigmaSpace);
  const float sigma_color2_inv_half = 0.5f / (sigmaColor * sigmaColor);

  START_CHRONO("Bilateral filter");
#pragma omp parallel for
  for(size_t i = k_h; i < (h - k_h); i++)
  {
    for(size_t j = k_w; j < (w - k_w); j++)
    {
      int value = src[i * stride + j];

      float sum1 = 0.0f;
      float sum2 = 0.0f;

      for(size_t ii = 0; ii < D; ii++)
      {
        for(size_t jj = 0; jj < D; jj++)
        {
          int tmp = src[(i + ii - k_h) * stride + (j + jj - k_w)];
          const float dc = value - tmp;
          const float dx = (float) jj - (float) k_w;
          const float dy = (float) ii - (float) k_h;
          const float space2 = dx * dx + dy * dy;
          const float color2 = dc * dc;
          const float weight =
              std::exp(-(space2 * sigma_space2_inv_half + color2 * sigma_color2_inv_half));

          sum1 += tmp * weight;
          sum2 += weight;
        }
      }

      uint16_t res(sum1 / sum2);
      dst[i * stride + j] = res;
    }
  }
  STOP_CHRONO();
}

static void BilateralFilterAvx_(
    const uint16_t* __restrict__ src, uint16_t* __restrict__ dst, const size_t w, const size_t h,
    const size_t stride, const float sigmaSpace, const float sigmaColor)
{
  constexpr size_t D = 13;
  constexpr size_t k_h = D / 2;
  constexpr size_t k_w = D / 2;

  const float sigma_space2_inv_half = 0.5f / (sigmaSpace * sigmaSpace);
  const float sigma_color2_inv_half = 0.5f / (sigmaColor * sigmaColor);

  START_CHRONO("Bilateral filter with AVX");
#pragma omp parallel for
  for(size_t i = 0; i < (h - D); i++)
  {
    for(size_t j = 0; j < ((w - D) / 8) * 8; j += 8)
    {
      const __m256 value = loadU162Float(src + (i + k_h) * stride + j + k_w);

      __m256 sum1 = _mm256_set1_ps(0.0f);
      __m256 sum2 = _mm256_set1_ps(0.0f);
      for(size_t ii = 0; ii < D; ii++)
      {
        for(size_t jj = 0; jj < D; jj++)
        {
          const __m256 tmp = loadU162Float(src + (i + ii) * stride + (j + jj));
          const __m256 dx = _mm256_set1_ps((float) jj - (float) k_w);
          const __m256 dy = _mm256_set1_ps((float) ii - (float) k_h);
          const __m256 dc = _mm256_sub_ps(value, tmp);
          const __m256 space2 = _mm256_add_ps(_mm256_mul_ps(dx, dx), _mm256_mul_ps(dy, dy));
          const __m256 color2 = _mm256_mul_ps(dc, dc);
          const __m256 x = _mm256_add_ps(
              _mm256_mul_ps(space2, _mm256_set1_ps(-sigma_space2_inv_half)),
              _mm256_mul_ps(color2, _mm256_set1_ps(-sigma_color2_inv_half)));
          const __m256 weight = exp256_ps(x);

          sum1 = _mm256_add_ps(_mm256_mul_ps(tmp, weight), sum1);
          sum2 = _mm256_add_ps(weight, sum2);
        }
      }
      __m256 res = _mm256_div_ps(sum1, sum2);
      storeFloat2U16(dst + (i + k_h) * stride + j + k_w, res);
    }
  }
  STOP_CHRONO();
}

static void AnisotropicDiffusionInnerKernel(
    const float* __restrict__ src, float* __restrict__ dst, const size_t w, const size_t h,
    const float sigma, const float lambda)
{
  const float sigma2_inv = 1.0f / (sigma * sigma);

#pragma omp parallel for
  for(size_t i = 1; i < h - 1; i++)
  {
    for(size_t j = 1; j < w - 1; j++)
    {
      const float ii = src[i * w + j];
      const float i0 = src[i * w + j - 1];
      const float i1 = src[i * w + j + 1];
      const float i2 = src[(i - 1) * w + j];
      const float i3 = src[(i + 1) * w + j];

      const float g0 = ii - i0;
      const float g1 = i1 - ii;
      const float g2 = ii - i2;
      const float g3 = i3 - ii;

      const float c0 = std::exp(-sigma2_inv * g0 * g0);
      const float c1 = std::exp(-sigma2_inv * g1 * g1);
      const float c2 = std::exp(-sigma2_inv * g2 * g2);
      const float c3 = std::exp(-sigma2_inv * g3 * g3);

      dst[i * w + j] = ii * (1.0f - lambda * (c0 + c1 + c2 + c3))
                       + lambda * (c0 * i0 + c1 * i1 + c2 * i2 + c3 * i3);
    }
  }
}
#pragma GCC diagnostic pop
