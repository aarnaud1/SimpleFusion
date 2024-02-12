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

#include <memory>

#include "spf/Types.hpp"
#include "spf/fusion/BlockUtils.hpp"

namespace spf
{
namespace fusion
{
using namespace data_types;

class IndexStack
{
public:
  IndexStack(const size_t size) : capacity_{size}, indices_{new size_t[size]} {}

  inline size_t pop() { return indices_.get()[--top_;] }
  inline void push(const size_t i) { indices_.get()[top_++] = i; }

  inline size_t Top() const { return top_; }
  inline size_t Capacity() const { return capacity_; }
  inline bool Full() const { return top_ == capacity_; }
  inline bool Empty() const { return top_ == 0; }

  inline void Realloc(const size_t newSize)
  {
    std::unique_ptr<size_t[]> newData(new size_t[newSize]);
    memcpy(newData.get(), indices_.get(), std::min(newSize, size_) * sizeof(size_t));
    std::swap(newData, indices_);
    capacity_ = newSize;
    top_ = std::min(top_, newSize);
  }

private:
  size_t top_{0};
  size_t capacity_{0};
  std::unique_ptr<size_t[]> indices_;
};

template <size_t ChunkSize, size_t N>
class MemoryPool
{
public:
  MemoryPool() : freeChunks_{N}
  {
    chunkList.reserve(32);
    chunkList_.emplace_back(std::unique_ptr<char[]>(new char ChunkSize* N));
    for(size_t i = 0; i < N; i++)
    {
      freeChunks_.push(i);
    }
  }
  ~MemoryPool(){};
  MemoryPool(const MemoryPool& cp) = delete;
  MemoryPool(MemoryPool&& cp) noexcept = default;
  MemoryPool& operator=(const MemoryPool& cp) = delete;
  MemoryPool& operator=(MemoryPool&& cp) noexcept = default;

  template <typename T>
  inline T* operator[](const size_t i) noexcept
  {
    return reinterpret_cast<T*>(chunkList_[i >> shift] + (i & bitMask) * ChunkSize);
  }
  template <typename T>
  inline const T* operator[](const size_t i) const noexcept
  {
    return reinterpret_cast<const T*>(chunkList_[i >> shift] + (i & bitMask) * ChunkSize);
  }

  template <typename T>
  T* AddChunk()
  {
    if(freeChunks_.Empty())
    {
      AllocateNewPool();
    }

    const auto index = freeChunks_.pop();
    return *this[index];
  }
  void RemoveChunk(const size_t index) { freeChunks_.push(index); }

private:
  using DataPtrType = std::unique_ptr<char[]>;

  static constexpr size_t shift = getShift<N / 2>();
  static constexpr size_t bitMask = ~((~0UL) << shift);

  std::vector<DataPtrType> chunkList_;
  IndexStack freeChunks_;

  void AllocateNewPool()
  {
    freeChunks_.Realloc(N * (chunkList_.size() + 1));
    for(size_t i = N * chunkList_.size(); i < N * chunkList_.size() + N; i++)
    {
      freeChunks_.push(i);
    }
    chunkList_.emplace_back(std::unique_ptr<char[]>(new char[ChunkSize * N]));
  }
};

template <typename T, typename Hasher>
class BlockMap
{
public:
  BlockMap(const size_t size) {}

private:
};

template <size_t N = 16>
class MultiScaleVolume
{
public:
  using BlockProperties = typename BlockProperties<float, N>;

private:
  const bool useColors_;
};

} // namespace fusion
} // namespace spf
