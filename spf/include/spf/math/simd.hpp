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

#include <x86intrin.h>
#include <cstdint>

static inline __m256 loadU162Float(const uint16_t *ptr)
{
  return _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(_mm_loadu_si128((const __m128i_u *) ptr)));
}

static inline void storeFloat2U16(uint16_t *ptr, __m256 value)
{
  __m256i tmp = _mm256_cvtps_epi32(value);
  tmp = _mm256_packs_epi32(tmp, _mm256_setzero_si256());
  tmp = _mm256_permute4x64_epi64(tmp, 0xD8);
  _mm_storeu_si128((__m128i_u *) ptr, _mm256_castsi256_si128(tmp));
}

// https://stackoverflow.com/questions/48863719/fastest-implementation-of-
// exponential-function-using-avx
static inline __m256 exp256_ps(__m256 x)
{
  /* Modified code. The original code is here:
    https://github.com/reyoung/avx_mathfun

     AVX implementation of exp
     Based on "sse_mathfun.h", by Julien Pommier
     http://gruntthepeon.free.fr/ssemath/
     Copyright (C) 2012 Giovanni Garberoglio
     Interdisciplinary Laboratory for Computational Science (LISC)
     Fondazione Bruno Kessler and University of Trento
     via Sommarive, 18
     I-38123 Trento (Italy)
    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
    (this is the zlib license)
  */
  const __m256 exp_hi = _mm256_set1_ps(88.3762626647949f);
  const __m256 exp_lo = _mm256_set1_ps(-88.3762626647949f);

  const __m256 cephes_LOG2EF = _mm256_set1_ps(1.44269504088896341);
  const __m256 cephes_exp_C1 = _mm256_set1_ps(0.693359375);
  const __m256 cephes_exp_C2 = _mm256_set1_ps(-2.12194440e-4);

  const __m256 cephes_exp_p0 = _mm256_set1_ps(1.9875691500E-4);
  const __m256 cephes_exp_p1 = _mm256_set1_ps(1.3981999507E-3);
  const __m256 cephes_exp_p2 = _mm256_set1_ps(8.3334519073E-3);
  const __m256 cephes_exp_p3 = _mm256_set1_ps(4.1665795894E-2);
  const __m256 cephes_exp_p4 = _mm256_set1_ps(1.6666665459E-1);
  const __m256 cephes_exp_p5 = _mm256_set1_ps(5.0000001201E-1);

  __m256 tmp = _mm256_setzero_ps(), fx;
  __m256i imm0;
  __m256 one = _mm256_set1_ps(1.0f);

  x = _mm256_min_ps(x, exp_hi);
  x = _mm256_max_ps(x, exp_lo);

  /* express exp(x) as exp(g + n*log(2)) */
  fx = _mm256_fmadd_ps(x, cephes_LOG2EF, _mm256_set1_ps(0.5f));
  tmp = _mm256_floor_ps(fx);
  __m256 mask = _mm256_cmp_ps(tmp, fx, _CMP_GT_OS);
  mask = _mm256_and_ps(mask, one);
  fx = _mm256_sub_ps(tmp, mask);
  tmp = _mm256_mul_ps(fx, cephes_exp_C1);
  __m256 z = _mm256_mul_ps(fx, cephes_exp_C2);
  x = _mm256_sub_ps(x, tmp);
  x = _mm256_sub_ps(x, z);
  z = _mm256_mul_ps(x, x);

  __m256 y = cephes_exp_p0;
  y = _mm256_fmadd_ps(y, x, cephes_exp_p1);
  y = _mm256_fmadd_ps(y, x, cephes_exp_p2);
  y = _mm256_fmadd_ps(y, x, cephes_exp_p3);
  y = _mm256_fmadd_ps(y, x, cephes_exp_p4);
  y = _mm256_fmadd_ps(y, x, cephes_exp_p5);
  y = _mm256_fmadd_ps(y, z, x);
  y = _mm256_add_ps(y, one);

  /* build 2^n */
  imm0 = _mm256_cvttps_epi32(fx);
  imm0 = _mm256_add_epi32(imm0, _mm256_set1_epi32(0x7f));
  imm0 = _mm256_slli_epi32(imm0, 23);
  __m256 pow2n = _mm256_castsi256_ps(imm0);
  y = _mm256_mul_ps(y, pow2n);
  return y;
}