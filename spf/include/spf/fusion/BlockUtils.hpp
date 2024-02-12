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

#include <limits>

namespace spf
{
namespace fusion
{
template <size_t N>
static constexpr size_t getShift()
{
  if constexpr((N & (N - 1)) != 0)
  {
    static_assert("N must be a power of two");
  }
  if constexpr(N == 0)
  {
    return 0;
  }
  else
  {
    return 1 + getShift<N / 2>();
  }
}

template <typename T, size_t N = 16>
struct BlockProperties
{
  static constexpr T invalidTsdf = std::numeric_limits<T>::max();
  static constexpr size_t blockSize = N;
  static constexpr size_t blockShift = getShift<N / 2>();
  static constexpr size_t blockVolume = blockSize * blockSize * blockSize;
};

struct Index3d
{
  int x;
  int y;
  int z;

  inline Index3d(const int x, const int y, const int z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  inline Index3d(const int val)
  {
    this->x = val;
    this->y = val;
    this->z = val;
  }

  inline bool operator==(Index3d const& other) const
  {
    return this->x == other.x && this->y == other.y && this->z == other.z;
  }

  inline bool operator<(Index3d const& other) const
  {
    if(x < other.x)
    {
      return true;
    }
    else if(x == other.x)
    {
      if(y < other.y)
      {
        return true;
      }
      else if(y == other.y)
      {
        if(z < other.z)
        {
          return true;
        }
      }
    }
    return false;
  }
};

typedef Index3d BlockId;
typedef Index3d VoxelId;

static inline Index3d operator+(Index3d const& i0, Index3d const& i1)
{
  return Index3d(i0.x + i1.x, i0.y + i1.y, i0.z + i1.z);
}

static inline Index3d operator-(Index3d const& i0, Index3d const& i1)
{
  return Index3d(i0.x - i1.x, i0.y - i1.y, i0.z - i1.z);
}

static inline Index3d operator*(const int k, Index3d const& id)
{
  return Index3d(k * id.x, k * id.y, k * id.z);
}

static inline Index3d operator*(Index3d const& id, const int k)
{
  return Index3d(k * id.x, k * id.y, k * id.z);
}

static inline int Dot(Index3d const& id0, Index3d const& id1)
{
  return id0.x * id1.x + id0.y * id1.y + id0.z * id1.z;
}

static inline int Mod(const int a, const int b) { return (a % b + b) % b; }

static inline Index3d Mod(Index3d const& index, const int val)
{
  return Index3d(Mod(index.x, val), Mod(index.y, val), Mod(index.z, val));
}

static inline int Div(const int a, const int b) { return (a - Mod(a, b)) / b; }

static inline Index3d Div(Index3d const& index, const int val)
{
  return Index3d(Div(index.x, val), Div(index.y, val), Div(index.z, val));
}

template <size_t N = 16>
static inline BlockId GetId(Point3f const& v, const float voxelRes)
{
  const int x = (int) floorf(v.x / voxelRes) >> BlockProperties<float, N>::blockShift;
  const int y = (int) floorf(v.y / voxelRes) >> BlockProperties<float, N>::blockShift;
  const int z = (int) floorf(v.z / voxelRes) >> BlockProperties<float, N>::blockShift;
  return BlockId(x, y, z);
}

template <size_t N = 16>
static inline Index3d GetVoxelAbsolutePos(BlockId const& blockId, Index3d const& voxelId)
{
  return BlockProperties<float, N>::blockSize * blockId + voxelId;
}

static inline Point3f GetVoxelPos(const Index3d& id, const float voxelRes)
{
  return voxelRes * Point3f((float) id.x, (float) id.y, (float) id.z);
}

template <size_t N = 16>
static inline Index3d GetVoxelId(const Point3f& p, const float voxelRes)
{
  const int x = Mod((int) floorf(p.x / voxelRes), BlockProperties<float, N>::blockSize);
  const int y = Mod((int) floorf(p.y / voxelRes), BlockProperties<float, N>::blockSize);
  const int z = Mod((int) floorf(p.z / voxelRes), BlockProperties<float, N>::blockSize);
  return Index3d(x, y, z);
}

struct ChunkHasher
{
  static constexpr size_t p1 = 73856093;
  static constexpr size_t p2 = 19349663;
  static constexpr size_t p3 = 83492791;

  inline std::size_t operator()(const BlockId& key) const
  {
    return (key.x * p1 ^ key.y * p2 ^ key.z * p3);
  }
};
} // namespace fusion
} // namespace spf
