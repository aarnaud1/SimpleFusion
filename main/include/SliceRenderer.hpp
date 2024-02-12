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

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <spf/utils.hpp>
#include <spf/fusion/VoxelBlock.hpp>
#include <spf/fusion/Volume.hpp>

struct Color3u
{
  uint8_t b, g, r;
};

class SliceRenderer
{
public:
  typedef spf::Vec3f Vec3;

  SliceRenderer(const size_t w, const size_t h, spf::fusion::Volume& volume);

  void Render(const Vec3& center, const Vec3& axis);

  void Show()
  {
    auto* dataPtr = (void*) tsdfImg_.get();
    cv::Mat img(height_, width_, CV_8UC3, dataPtr);
    cv::imshow("TSDF", img);
  }

private:
  size_t width_;
  size_t height_;
  float voxelRes_;

  spf::fusion::Volume& volume_;
  std::unique_ptr<Color3u[]> tsdfImg_;

  inline Vec3 ColorTSDF(const float tsdf)
  {
    constexpr float eps = 1.0f;

    Vec3 ret(0.5f);

    if(std::abs(tsdf) <= eps)
    {
      ret = Vec3(1.0f);
    }

    if(tsdf == spf::fusion::INVALID_TSDF)
    {
      ret = Vec3(0.5f, 0.0f, 0.0f);
    }

    return ret;
  }
};
