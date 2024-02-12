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
#include <memory>

namespace spf
{
namespace data_types
{
template <typename T>
class Image
{
public:
  Image() : width_{0}, height_{0}, channels_{0}, data_{nullptr} {}
  Image(const size_t width, const size_t height, const size_t channels) :
      width_{width},
      height_{height},
      channels_{channels},
      data_{new T[channels_ * width_ * height_]}
  {}
  Image(const Image &cp) noexcept :
      width_{cp.width_},
      height_{cp.height_},
      channels_{cp.channels_},
      data_{new T[channels_ * width_ * height_]}
  {
    memcpy(data_.get(), cp.data_.get(), channels_ * width_ * height_ * sizeof(T));
  }
  Image(Image &&cp) noexcept
  {
    width_ = std::move(cp.width_);
    height_ = std::move(cp.height_);
    channels_ = std::move(cp.channels_);
    data_ = std::move(cp.data_);
  }
  Image &operator=(const Image &cp) noexcept
  {
    this->width_ = cp.width_;
    this->height_ = cp.height_;
    this->channels_ = cp.channels_;
    this->data_ = std::unique_ptr<T[]>(new T[channels_ * width_ * height_]);
    memcpy(this->data_.get(), cp.data_.get(), channels_ * width_ * height_ * sizeof(T));
    return *this;
  }
  Image &operator=(Image &&cp) noexcept
  {
    this->width_ = std::move(cp.width_);
    this->height_ = std::move(cp.height_);
    this->channels_ = std::move(cp.channels_);
    this->data_ = std::move(cp.data_);
    return *this;
  }

  virtual ~Image() {}

  inline T *Data() { return data_.get(); }
  inline const T *Data() const { return data_.get(); }
  // inline T &operator[](const size_t i) { return *(data_.get() + i); }
  // inline const T &operator[](const size_t i) const
  // {
  //   return *(data_.get() + i);
  // }
  inline T &operator()(const size_t i, const size_t j, const size_t c)
  {
    return *(data_.get() + c * width_ * height_ + i * width_ + j);
  }
  inline const T &operator()(const size_t i, const size_t j, const size_t c) const
  {
    return *(data_.get() + c * width_ * height_ + i * width_ + j);
  }

  inline size_t Width() const { return width_; }
  inline size_t Height() const { return height_; }
  inline size_t Channels() const { return channels_; }

  void Resize(const size_t width, const size_t height)
  {
    data_.reset();
    width_ = width;
    height_ = height;
    data_ = std::unique_ptr<T[]>(new T[channels_ * width * height]);
  }
  inline void Clear() { memset(data_.get(), 0, channels_ * width_ * height_ * sizeof(T)); }
  // inline void Clear(const T &clearVal)
  // {
  //   sd::fill(this->begin(), this->end(), clearVal);
  // }

  // inline T *begin() { return data_.get(); }
  // inline T *end() { return data_.get() + width_ * height_; }
  // inline const T *cbegin() const { return data_.get(); }
  // inline const T *cend() const { return data_.get() + width_ * height_; }

protected:
  size_t width_;
  size_t height_;
  size_t channels_;
  std::unique_ptr<T[]> data_{nullptr};
};
} // namespace data_types
} // namespace spf
