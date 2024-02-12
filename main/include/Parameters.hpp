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

#include <string>

#include "spf/utils.hpp"
#include "spf/geometry/geometry.hpp"
#include "spf/data_types/PointCloud.hpp"
#include "spf/data_types/RGBDFrame.hpp"
#include "spf/fusion/Fusion.hpp"
#include "spf/fusion/Volume.hpp"

#define KINECT_IMG_H 480
#define KINECT_IMG_W 640
#define KINECT_IMG_RES (KINECT_IMG_H * KINECT_IMG_W)

#define INTRINSICS 525.0f, 525.0f, 319.5f, 239.5f
#define ICL_INTRINSICS_1 481.20f, -480.0f, 319.5f, 239.5f

#define SYNTH_W 640
#define SYNTH_H 480
#define SYNTH_TAN_FOV tan((50.0 * M_PI) / 180.0)
#define SYNTH_FX (float(SYNTH_W / 2) * SYNTH_TAN_FOV)
#define SYNTH_FY (float(SYNTH_H / 2) * SYNTH_TAN_FOV)
#define SYNTH_CX (float) (SYNTH_W / 2)
#define SYNTH_CY (float) (SYNTH_H / 2)
#define SYNTHETIC_0 SYNTH_FX, SYNTH_FY, SYNTH_CX, SYNTH_CY

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

struct CameraParameters
{
  size_t cameraWidth;
  size_t cameraHeight;
  spf::data_types::CameraIntrinsics<float> depthIntrinsics;
  spf::Mat4f OPENGL_TO_CAMERA;
  spf::Mat4f AXIS_PERMUT;
  spf::Mat4f MODEL_TO_OPENGL;

  CameraParameters() = default;

  CameraParameters& operator=(CameraParameters const& cp) = default;
};

struct FusionParameters
{
  float tau;
  float voxelRes;
};

static const CameraParameters FR1_PARAMS = {
    KINECT_IMG_W,
    KINECT_IMG_H,
    {INTRINSICS},
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, -1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, 1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, -1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f))};

static const CameraParameters ICL1_PARAMS = {
    KINECT_IMG_W,
    KINECT_IMG_H,
    {ICL_INTRINSICS_1},
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f))};

static const CameraParameters SYNTHETIC_0_PARAMS = {
    KINECT_IMG_W,
    KINECT_IMG_H,
    {SYNTHETIC_0},
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
    spf::Mat4f(
        spf::Vec4f(1.0f, 0.0f, 0.0f, 0.0f), spf::Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
        spf::Vec4f(0.0f, 0.0f, -1.0f, 0.0f), spf::Vec4f(0.0f, 0.0f, 0.0f, 1.0f))};

static CameraParameters getParams(const std::string& datasetType)
{
  CameraParameters ret;
  if(datasetType == "fr1")
  {
    utils::Log::Info("Application", "Loading fr1 params");
    ret = FR1_PARAMS;
  }
  else if(datasetType == "icl1")
  {
    utils::Log::Info("Application", "Loading ICL data");
    ret = ICL1_PARAMS;
  }
  else if(datasetType == "synthetic0")
  {
    utils::Log::Info("Application", "Loading synthetic 0 data");
    ret = SYNTHETIC_0_PARAMS;
  }
  else
  {
    throw std::runtime_error("Unknown dataset type");
  }

  return ret;
}

#pragma GCC diagnostic pop
