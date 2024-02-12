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

#include "spf/geometry/geometry.hpp"
#include "spf/data_types/Buffer.hpp"

namespace spf
{
namespace data_types
{
template <typename T>
struct PointXYZ
{
  using ScalarType = T;
  using VecType = typename geometry::Vec3<T>;
  static constexpr bool hasColors() { return false; }
  static constexpr bool hasNormals() { return false; }
  static constexpr ScalarType maxScalar = std::numeric_limits<ScalarType>::max();

  VecType xyz;

  // Basic constructors
  PointXYZ() = default;
  PointXYZ(const PointXYZ &cp) noexcept = default;
  PointXYZ(PointXYZ &&cp) noexcept = default;
  PointXYZ &operator=(const PointXYZ &cp) noexcept = default;
  PointXYZ &operator=(PointXYZ &&cp) noexcept = default;

  PointXYZ(VecType const &v) : xyz{v} {}

  static constexpr PointXYZ InvalidPoint() { return PointXYZ{VecType{maxScalar}}; }

  friend inline bool operator==(const PointXYZ &p0, const PointXYZ &p1)
  {
    return (p0.xyz == p1.xyz);
  }
  friend inline bool operator!=(const PointXYZ &p0, const PointXYZ &p1)
  {
    return (p0.xyz != p1.xyz);
  }
  inline bool isValid() const { return *this != InvalidPoint(); }

  // Conversion operators
  template <typename PointType>
  operator PointType()
  {
    PointType ret{};
    ret.xyz = xyz;
    return ret;
  }
};

template <typename T>
struct PointXYZRGB
{
  using ScalarType = T;
  using VecType = typename geometry::Vec3<T>;
  static constexpr bool hasColors() { return true; }
  static constexpr bool hasNormals() { return false; }
  static constexpr ScalarType maxScalar = std::numeric_limits<ScalarType>::max();

  VecType xyz;
  VecType rgb;

  // Basic constructors
  PointXYZRGB() = default;
  PointXYZRGB(const PointXYZRGB &cp) noexcept = default;
  PointXYZRGB(PointXYZRGB &&cp) noexcept = default;
  PointXYZRGB &operator=(const PointXYZRGB &cp) noexcept = default;
  PointXYZRGB &operator=(PointXYZRGB &&cp) noexcept = default;

  PointXYZRGB(VecType const &pos) : xyz{pos}, rgb{} {}
  PointXYZRGB(VecType const &pos, const VecType &col) : xyz{pos}, rgb{col} {}

  static constexpr PointXYZRGB InvalidPoint()
  {
    return PointXYZRGB{VecType{maxScalar}, VecType{0}};
  }

  friend inline bool operator==(const PointXYZRGB &p0, const PointXYZRGB &p1)
  {
    return (p0.xyz == p1.xyz) && (p0.rgb == p1.rgb);
  }
  friend inline bool operator!=(const PointXYZRGB &p0, const PointXYZRGB &p1)
  {
    return (p0.xyz != p1.xyz) || (p0.rgb != p1.rgb);
  }
  inline bool isValid() const { return *this != InvalidPoint(); }

  // Conversion operators
  template <typename PointType>
  operator PointType()
  {
    PointType ret{};
    ret.xyz = xyz;
    if constexpr(PointType::hasColors())
    {
      ret.rgb = rgb;
    }
  }
};

template <typename T>
struct PointXYZN
{
  using ScalarType = T;
  using VecType = typename geometry::Vec3<T>;
  static constexpr bool hasColors() { return false; }
  static constexpr bool hasNormals() { return true; }
  static constexpr ScalarType maxScalar = std::numeric_limits<ScalarType>::max();

  VecType xyz;
  VecType n;

  // Basic constructors
  PointXYZN() = default;
  PointXYZN(const PointXYZN &cp) noexcept = default;
  PointXYZN(PointXYZN &&cp) noexcept = default;
  PointXYZN &operator=(const PointXYZN &cp) noexcept = default;
  PointXYZN &operator=(PointXYZN &&cp) noexcept = default;

  PointXYZN(VecType const &xyz) : xyz{xyz}, n{} {}
  PointXYZN(VecType const &xyz, VecType const &n) : xyz{xyz}, n{n} {}

  static constexpr PointXYZN InvalidPoint() { return PointXYZN{VecType{maxScalar}, VecType{0}}; }

  friend inline bool operator==(const PointXYZN &p0, const PointXYZN &p1)
  {
    return (p0.xyz == p1.xyz) && (p0.n == p1.n);
  }
  friend inline bool operator!=(const PointXYZN &p0, const PointXYZN &p1)
  {
    return (p0.xyz != p1.xyz) || (p0.n != p1.n);
  }
  inline bool isValid() const { return *this != InvalidPoint(); }

  // Conversion operators
  template <typename PointType>
  operator PointType()
  {
    PointType ret{};
    ret.xyz = xyz;
    if constexpr(T::hasNormals())
    {
      ret.n = n;
    }
    return ret;
  }
};

template <typename T>
struct PointXYZRGBN
{
  using ScalarType = T;
  using VecType = typename geometry::Vec3<T>;
  static constexpr bool hasColors() { return true; }
  static constexpr bool hasNormals() { return true; }
  static constexpr ScalarType maxScalar = std::numeric_limits<ScalarType>::max();

  VecType xyz;
  VecType rgb;
  VecType n;

  // Basic constructors
  PointXYZRGBN() = default;
  PointXYZRGBN(const PointXYZRGBN &cp) noexcept = default;
  PointXYZRGBN(PointXYZRGBN &&cp) noexcept = default;
  PointXYZRGBN &operator=(const PointXYZRGBN &cp) noexcept = default;
  PointXYZRGBN &operator=(PointXYZRGBN &&cp) noexcept = default;

  PointXYZRGBN(VecType const &xyz) : xyz{xyz}, rgb{}, n{} {}
  PointXYZRGBN(VecType const &xyz, VecType const &rgb) : xyz{xyz}, rgb{rgb}, n{} {}
  PointXYZRGBN(VecType const &xyz, VecType const &rgb, VecType const &n) : xyz{xyz}, rgb{rgb}, n{n}
  {}

  static constexpr PointXYZRGBN InvalidPoint()
  {
    return PointXYZRGBN{VecType{maxScalar}, VecType{0}, VecType{0}};
  }

  friend inline bool operator==(const PointXYZRGBN &p0, const PointXYZRGBN &p1)
  {
    return (p0.xyz == p1.xyz) && (p0.rgb == p1.rgb) && (p0.n == p1.n);
  }
  friend inline bool operator!=(const PointXYZRGBN &p0, const PointXYZRGBN &p1)
  {
    return (p0.xyz != p1.xyz) || (p0.rgb != p1.rgb) || (p0.n != p1.n);
  }
  inline bool isValid() const { return *this != InvalidPoint(); }

  // Conversion operators
  template <typename PointType>
  operator PointType()
  {
    PointType ret{};
    ret.xyz = xyz;
    if constexpr(PointType::hasColors())
    {
      ret.rgb = rgb;
    }
    if constexpr(PointType::hasNormals())
    {
      ret.n = n;
    }
    return ret;
  }
};

// -----------------------------------------------------------------------------
// For processing it is better to deal with a structure of arrays rather than an
// array of structures.

template <typename PointType>
class PointList
{};

template <typename T>
class PointList<PointXYZ<T>>
{
public:
  using ScalarType = T;
  using PointType = PointXYZ<ScalarType>;
  using VecType = typename PointType::VecType;
  using BufferType = Buffer<VecType>;

  PointList() {}
  PointList(const size_t capacity) : size_{0}, capacity_{capacity}, xyzData_{capacity_} {};
  PointList(const PointList &cp) noexcept = default;
  PointList(PointList &&cp) noexcept = default;
  PointList &operator=(const PointList &cp) noexcept = default;
  PointList &operator=(PointList &&cp) noexcept = default;

  virtual ~PointList() = default;

  void AddPoint(const PointType &p)
  {
    xyzData_.IncSize();
    xyzData_[size_] = p.xyz;
    size_++;
  }

  // Access data
  inline BufferType &Points() { return xyzData_; }
  inline const BufferType &Points() const { return xyzData_; }

  inline void Resize(const size_t size)
  {
    if(size > capacity_)
    {
      throw std::runtime_error("PointList capacity exceded");
    }
    size_ = size;
    xyzData_.Resize(size_);
  }
  inline void Realloc(const size_t capacity)
  {
    size_ = 0;
    capacity_ = capacity;
    xyzData_.Realloc(capacity);
  }
  inline void Clear()
  {
    size_ = 0;
    xyzData_.Clear();
  }
  inline void Release()
  {
    size_ = 0;
    capacity_ = 0;
    xyzData_.Release();
  }

  inline size_t Size() const { return size_; }
  inline size_t Capacity() const { return capacity_; }

  inline PointType operator[](const size_t i) { return {xyzData_[i]}; }
  inline const PointType operator[](const size_t i) const { return {xyzData_[i]}; }
  inline auto at(const size_t i)
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing PointList data");
    }
    return {xyzData_[i]};
  }
  inline const auto at(const size_t i) const
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing PointList data");
    }
    return {xyzData_[i]};
  }

  template <typename ElementType>
  struct Iterator
  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = size_t;

    inline explicit Iterator(PointList &list) : list_{list}, pos_{0} {}
    inline explicit Iterator(PointList &list, const size_t pos) : list_{list}, pos_{pos} {}

    inline ElementType operator*() const { return list_[pos_]; }
    inline Iterator &operator++()
    {
      pos_++;
      return *this;
    }
    inline Iterator &operator+=(const size_t n)
    {
      pos_ += n;
      return *this;
    }
    inline bool operator==(const Iterator &other)
    {
      return this->list_[this->pos_] == other.list_[other.pos_];
    }
    inline bool operator!=(const Iterator &other)
    {
      return this->list_[this->pos_] != other.list_[other.pos_];
    }

  private:
    PointList &list_;
    size_t pos_{0};
  };

  inline auto begin() { return Iterator<PointType>(*this, 0); }
  inline auto end() { return Iterator<PointType>(*this, size_); }
  inline auto cbegin() { return Iterator<const PointType>(*this, 0); }
  inline auto cend() { return Iterator<const PointType>(*this, size_); }

protected:
  size_t size_{0};
  size_t capacity_{0};
  BufferType xyzData_{};
};

template <typename T>
class PointList<PointXYZRGB<T>>
{
public:
  using ScalarType = T;
  using PointType = PointXYZRGB<ScalarType>;
  using VecType = typename PointType::VecType;
  using BufferType = Buffer<VecType>;

  PointList() {}
  PointList(const size_t capacity) :
      size_{0}, capacity_{capacity}, xyzData_{capacity_}, rgbData_{capacity_} {};
  PointList(const PointList &cp) noexcept = default;
  PointList(PointList &&cp) noexcept = default;
  PointList &operator=(const PointList &cp) noexcept = default;
  PointList &operator=(PointList &&cp) noexcept = default;

  virtual ~PointList() = default;

  void AddPoint(const PointType &p)
  {
    xyzData_.IncSize();
    rgbData_.IncSize();
    xyzData_[size_] = p.xyz;
    rgbData_[size_] = p.rgb;
    size_++;
  }

  // Access data
  inline BufferType &Points() { return xyzData_; }
  inline const BufferType &Points() const { return xyzData_; }
  inline BufferType &Colors() { return rgbData_; }
  inline const BufferType &Colors() const { return rgbData_; }

  inline void Resize(const size_t size)
  {
    if(size > capacity_)
    {
      throw std::runtime_error("PointList capacity exeeded");
    }
    size_ = size;
    xyzData_.Resize(size_);
    rgbData_.Resize(size_);
  }
  inline void Realloc(const size_t capacity)
  {
    size_ = 0;
    capacity_ = capacity;
    xyzData_.Realloc(capacity_);
    rgbData_.Realloc(capacity_);
  }
  inline void Clear()
  {
    size_ = 0;
    xyzData_.Clear();
    rgbData_.Clear();
  }
  inline void Release()
  {
    size_ = 0;
    capacity_ = 0;
    xyzData_.Release();
    rgbData_.Release();
  }

  inline size_t Size() const { return size_; }
  inline size_t Capacity() const { return capacity_; }

  inline PointType operator[](const size_t i) { return {xyzData_[i], rgbData_[i]}; }
  inline const PointType operator[](const size_t i) const { return {xyzData_[i], rgbData_[i]}; }
  inline auto at(const size_t i)
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing PointList data");
    }
    return {xyzData_[i], rgbData_[i]};
  }
  inline const auto at(const size_t i) const
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing PointList data");
    }
    return {xyzData_[i], rgbData_[i]};
  }

  template <typename ElementType>
  struct Iterator
  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = size_t;

    inline explicit Iterator(PointList &list) : list_{list}, pos_{0} {}
    inline explicit Iterator(PointList &list, const size_t pos) : list_{list}, pos_{pos} {}

    inline ElementType operator*() const { return list_[pos_]; }
    inline Iterator &operator++()
    {
      pos_++;
      return *this;
    }
    inline Iterator &operator+=(const size_t n)
    {
      pos_ += n;
      return *this;
    }
    inline bool operator==(const Iterator &other)
    {
      return this->list_[this->pos_] == other.list_[other.pos_];
    }
    inline bool operator!=(const Iterator &other)
    {
      return this->list_[this->pos_] != other.list_[other.pos_];
    }

  private:
    PointList &list_;
    size_t pos_{0};
  };

  inline auto begin() { return Iterator<PointType>(*this, 0); }
  inline auto end() { return Iterator<PointType>(*this, size_); }
  inline auto cbegin() { return Iterator<const PointType>(*this, 0); }
  inline auto cend() { return Iterator<const PointType>(*this, size_); }

protected:
  size_t size_{0};
  size_t capacity_{0};
  BufferType xyzData_{};
  BufferType rgbData_{};
};

template <typename T>
class PointList<PointXYZN<T>>
{
public:
  using ScalarType = T;
  using PointType = PointXYZN<ScalarType>;
  using VecType = typename PointType::VecType;
  using BufferType = Buffer<VecType>;

  PointList() {}
  PointList(const size_t capacity) :
      size_{0}, capacity_{0}, xyzData_{capacity_}, nData_{capacity_} {};
  PointList(const PointList &cp) noexcept = default;
  PointList(PointList &&cp) noexcept = default;
  PointList &operator=(const PointList &cp) noexcept = default;
  PointList &operator=(PointList &&cp) noexcept = default;

  virtual ~PointList() = default;

  void AddPoint(const PointType &p)
  {
    xyzData_.IncSize();
    nData_.IncSize();
    xyzData_[size_] = p.xyz;
    nData_[size_] = p.n;
    size_++;
  }

  // Access data
  inline BufferType &Points() { return xyzData_; }
  inline const BufferType &Points() const { return xyzData_; }
  inline BufferType &Normals() { return nData_; }
  inline const BufferType &Normals() const { return nData_; }

  inline void Resize(const size_t size)
  {
    if(size > capacity_)
    {
      throw std::runtime_error("PointList capacity exeeded");
    }
    size_ = size;
    xyzData_.Resize(size);
    nData_.Resize(size);
  }
  inline void Realloc(const size_t capacity)
  {
    size_ = 0;
    capacity_ = capacity;
    xyzData_.Realloc(capacity_);
    nData_.Realloc(capacity_);
  }
  inline void Clear()
  {
    size_ = 0;
    xyzData_.Clear();
    nData_.Clear();
  }
  inline void Release()
  {
    size_ = 0;
    capacity_ = 0;
    xyzData_.Release();
    nData_.Release();
  }

  inline size_t Size() const { return size_; }

  inline PointType operator[](const size_t i) { return {xyzData_[i], nData_[i]}; }
  inline const PointType operator[](const size_t i) const { return {xyzData_[i], nData_[i]}; }
  inline auto at(const size_t i)
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing PointList data");
    }
    return {xyzData_[i], nData_[i]};
  }
  const inline auto at(const size_t i) const
  {
    if(i >= size_)
    {
      throw std::out_of_range("Bad index when accessing PointList data");
    }
    return {xyzData_[i], nData_[i]};
  }

  template <typename ElementType>
  struct Iterator
  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = size_t;

    inline explicit Iterator(PointList &list) : list_{list}, pos_{0} {}
    inline explicit Iterator(PointList &list, const size_t pos) : list_{list}, pos_{pos} {}

    inline ElementType operator*() const { return list_[pos_]; }
    inline Iterator &operator++()
    {
      pos_++;
      return *this;
    }
    inline Iterator &operator+=(const size_t n)
    {
      pos_ += n;
      return *this;
    }
    inline bool operator==(const Iterator &other)
    {
      return this->list_[this->pos_] == other.list_[other.pos_];
    }
    inline bool operator!=(const Iterator &other)
    {
      return this->list_[this->pos_] != other.list_[other.pos_];
    }

  private:
    PointList &list_;
    size_t pos_{0};
  };

  inline auto begin() { return Iterator<PointType>(*this, 0); }
  inline auto end() { return Iterator<PointType>(*this, size_); }
  inline auto cbegin() { return Iterator<const PointType>(*this, 0); }
  inline auto cend() { return Iterator<const PointType>(*this, size_); }

protected:
  size_t size_{0};
  size_t capacity_{0};
  BufferType xyzData_{};
  BufferType nData_{};
};

template <typename T>
class PointList<PointXYZRGBN<T>>
{
public:
  using ScalarType = T;
  using PointType = PointXYZRGBN<ScalarType>;
  using VecType = typename PointType::VecType;
  using BufferType = Buffer<VecType>;

  PointList() {}
  PointList(const size_t capacity) :
      size_{0}, capacity_{capacity}, xyzData_{capacity_}, rgbData_{capacity_}, nData_{capacity_} {};
  PointList(const PointList &cp) noexcept = default;
  PointList(PointList &&cp) noexcept = default;
  PointList &operator=(const PointList &cp) noexcept = default;
  PointList &operator=(PointList &&cp) noexcept = default;

  virtual ~PointList() = default;

  void AddPoint(const PointType &p)
  {
    xyzData_.IncSize();
    rgbData_.IncSize();
    nData_.IncSize();
    xyzData_[size_] = p.xyz;
    rgbData_[size_] = p.rgb;
    nData_[size_] = p.n;
    size_++;
  }

  // Access data
  inline BufferType &Points() { return xyzData_; }
  inline const BufferType &Points() const { return xyzData_; }
  inline BufferType &Colors() { return rgbData_; }
  inline const BufferType &Colors() const { return rgbData_; }
  inline BufferType &Normals() { return nData_; }
  inline const BufferType &Normals() const { return nData_; }

  inline void Resize(const size_t size)
  {
    if(size > capacity_)
    {
      throw std::runtime_error("PointList capacity exeeded");
    }
    size_ = size;
    xyzData_.Resize(size);
    rgbData_.Resize(size);
    nData_.Resize(size);
  }
  inline void Realloc(const size_t capacity)
  {
    size_ = 0;
    capacity_ = capacity;
    xyzData_.Realloc(capacity_);
    rgbData_.Realloc(capacity_);
    nData_.Realloc(capacity_);
  }
  inline void Clear()
  {
    size_ = 0;
    xyzData_.Clear();
    rgbData_.Clear();
    nData_.Clear();
  }
  inline void Release()
  {
    size_ = 0;
    capacity_ = 0;
    xyzData_.Release();
    rgbData_.Release();
    nData_.Release();
  }

  inline size_t Size() const { return size_; }
  inline size_t Capacity() const { return capacity_; }

  inline PointType operator[](const size_t i) { return {xyzData_[i], rgbData_[i], nData_[i]}; }
  inline const PointType operator[](const size_t i) const
  {
    return {xyzData_[i], rgbData_[i], nData_[i]};
  }
  // inline auto at(const size_t i)
  // {
  //   if(i >= size_)
  //   {
  //     throw std::out_of_range("Bad index when accessing PointList data");
  //   }
  //   return {xyzData_[i], rgbData_[i], nData_[i]};
  // }
  // inline const auto at(const size_t i) const
  // {
  //   if(i >= size_)
  //   {
  //     throw std::out_of_range("Bad index when accessing PointList data");
  //   }
  //   return {xyzData_[i], rgbData_[i], nData_[i]};
  // }

  // template <typename ElementType>
  // struct Iterator
  // {
  //   using iterator_category = std::forward_iterator_tag;
  //   using difference_type = size_t;
  //
  //   inline explicit Iterator(PointList &list) : list_{list}, pos_{0} {}
  //   inline explicit Iterator(PointList &list, const size_t pos) :
  //       list_{list}, pos_{pos}
  //   {}
  //
  //   inline ElementType operator*() const { return list_[pos_]; }
  //   inline Iterator &operator++()
  //   {
  //     pos_++;
  //     return *this;
  //   }
  //   inline Iterator &operator+=(const size_t n)
  //   {
  //     pos_ += n;
  //     return *this;
  //   }
  //   inline bool operator==(const Iterator &other) const
  //   {
  //     return std::addressof(list_) == std::addressof(other.list_)
  //            && pos_ == other.pos_;
  //   }
  //   inline bool operator!=(const Iterator &other) const
  //   {
  //     return !this->operator==(other);
  //   }
  //
  // private:
  //   PointList &list_;
  //   size_t pos_{0};
  // };

  // inline auto begin() { return Iterator<PointType>(*this, 0); }
  // inline auto end() { return Iterator<PointType>(*this, size_); }
  // inline auto cbegin() { return Iterator<const PointType>(*this, 0); }
  // inline auto cend() { return Iterator<const PointType>(*this, size_); }

protected:
  size_t size_{0};
  size_t capacity_{0};
  BufferType xyzData_{};
  BufferType rgbData_{};
  BufferType nData_{};
};

} // namespace data_types
} // namespace spf
