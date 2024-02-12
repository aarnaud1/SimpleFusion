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

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstddef>
#include <memory>
#include <iterator>

#include "spf/Types.hpp"

namespace spf
{
namespace data_types
{
// Simple buffer class that could be derived into GPU buffer, or specialized
// for SIMD processing.
template <typename T>
class Buffer
{
public:
  Buffer() : size_{0}, capacity_{0}, data_{nullptr} {}
  Buffer(const size_t capacity) : size_{0}, capacity_{capacity}, data_{new T[capacity_]} {}
  Buffer(const Buffer &cp) noexcept :
      size_{cp.size_}, capacity_{cp.capacity_}, data_{new T[capacity_]}
  {
    memcpy(data_.get(), cp.data_, capacity_ * sizeof(T));
  }
  Buffer(Buffer &&cp) noexcept :
      size_{std::move(cp.size_)}, capacity_{std::move(cp.capacity_)}, data_{std::move(cp.data_)}
  {}
  Buffer &operator=(const Buffer &cp) noexcept
  {
    this->size_ = cp.size_;
    this->capacity_ = cp.capacity_;
    this->data_ = std::unique_ptr<T[]>(new T[this->capacity_]);
    memcpy(this->data_.get(), cp.data_.get(), this->capacity_ * sizeof(T));
    return *this;
  }
  Buffer &operator=(Buffer &&cp) noexcept
  {
    this->size_ = std::move(cp.size_);
    this->capacity_ = std::move(cp.capacity_);
    this->data_ = std::move(cp.data_);
    return *this;
  }

  virtual ~Buffer(){};

  inline void IncSize(const size_t n = 1) { size_ += n; }
  void Resize(const size_t size)
  {
    if(size > capacity_)
    {
      throw std::runtime_error("Buffer capacity exeeded, you must allocate more space");
    }
    size_ = size;
  }

  void Realloc(const size_t capacity)
  {
    size_ = 0;
    capacity_ = capacity;
    data_.reset();
    data_ = std::unique_ptr<T[]>(new T[capacity_]);
  }

  inline void Clear() { size_ = 0; }
  inline void Release()
  {
    size_ = 0;
    capacity_ = 0;
    data_.release();
  }

  inline size_t Size() const { return size_; }
  inline size_t SizeBytes() const { return size_ * sizeof(T); }
  inline size_t Capacity() const { return capacity_; }

  inline T *Data() { return data_.get(); };
  inline const T *Data() const { return static_cast<T *>(data_.get()); }

  inline T &operator[](const size_t i) { return data_[i]; }
  inline const T &operator[](const size_t i) const { return data_[i]; }

  inline T &at(const size_t i)
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing Buffer data");
    }
    return data_[i];
  }
  inline const T &at(const size_t i) const
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing Buffer data");
    }
    return data_[i];
  }

  inline T *begin() { return data_.get(); }
  inline T *end() { return data_.get() + size_; }
  inline const T *cbegin() const { return data_.get(); }
  inline const T *cend() const { return data_.get() + size_; }

protected:
  size_t size_{0};
  size_t capacity_{0};
  std::unique_ptr<T[]> data_{nullptr};
};
} // namespace data_types
} // namespace spf
