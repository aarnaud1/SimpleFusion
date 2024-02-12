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

#include <cstdlib>

namespace spf
{
namespace data_types
{
template <typename T>
class AlignedImage
{
public:
  AlignedImage(const size_t width, const size_t height, const size_t channels = 1) :
      width_(width), height_(height), channels_(channels)
  {
    const size_t imgStride = width_ * sizeof(T);
    const size_t pad = alignment_ - (imgStride % alignment_);
    strideBytes_ = imgStride + pad;
    stride_ = strideBytes_ / sizeof(T);
    data_ = (T *) aligned_alloc(alignment_, channels_ * height_ * strideBytes_);
  }

  AlignedImage(const AlignedImage &cp) :
      width_(cp.width_),
      height_(cp.height_),
      channels_(cp.channels_),
      strideBytes_(cp.strideBytes_),
      stride_(cp.stride_)
  {
    data_ = (T *) aligned_alloc(alignment_, channels_ * height_ * strideBytes_);
    memcpy(data_, cp.data_, channels_ * height_ * strideBytes_);
  }

  AlignedImage &operator=(const AlignedImage &cp)
  {
    width_ = cp.width_;
    height_ = cp.height_;
    channels_ = cp.channels_;
    strideBytes_ = cp.strideBytes_;
    stride_ = cp.stride_;
    data_ = (T *) aligned_alloc(alignment_, channels_ * height_ * strideBytes_);
    memcpy(data_, cp.data_, channels_ * height_ * strideBytes_);
    return *this;
  }

  ~AlignedImage() { free(data_); }

  void Clear() { memset(data_, 0, channels_ * height_ * strideBytes_); }

  void Copy(const T *data)
  {
    for(size_t j = 0; j < height_; j++)
    {
      auto *line = data_ + j * channels_ * stride_;
      memcpy(line, data + j * channels_ * width_, channels_ * width_ * sizeof(T));
    }
  }

  void Copy(AlignedImage &src) { memcpy(data_, src.Data(), height_ * channels_ * strideBytes_); }

  // inline T operator[](const size_t i) const { return data_[i]; }

  inline T At(const size_t u, const size_t v, const size_t c = 0) const
  {
    return data_[channels_ * (v * stride_ + u) + c];
  }

  inline T *Data() const { return data_; }

  // inline T *GetRow(const size_t v) const
  // {
  //   return data_ + v * channels_ * stride_;
  // }

  inline size_t Width() const { return width_; }
  inline size_t Height() const { return height_; }
  inline size_t Channels() const { return channels_; }
  inline size_t StrideBytes() const { return strideBytes_; }
  inline size_t Stride() const { return stride_; }

private:
  static constexpr size_t alignment_ = 32;
  size_t width_;
  size_t height_;
  size_t channels_;
  size_t strideBytes_;
  size_t stride_;

  T *data_;
};
} // namespace data_types
} // namespace spf
