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

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <gflags/gflags.h>

#include <spf/utils.hpp>
#include <spf/Types.hpp>

#include <omp.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Parameters.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// -----------------------------------------------------------------------------

typedef spf::geometry::Mat4<float> Mat4;
typedef spf::geometry::Vec4<float> Vec4;
typedef spf::geometry::Vec3<float> Vec3;

enum class PrimitiveType : int
{
  SPHERE,
  PLANE
};

struct DatasetInfo
{
  std::vector<PrimitiveType> primitives;
  std::vector<Vec4> params;
  std::vector<Vec3> positions;
  std::vector<Vec4> rotations;
};

struct Ray
{
  Vec3 org;
  Vec3 dir;
};

// -----------------------------------------------------------------------------

static DatasetInfo readDataset(const std::string &geometry, const std::string &trajectory);

static void renderPrimitives(
    const DatasetInfo &info, const std::string &outputDir, const size_t w, const size_t h,
    const float near, const float far, const bool debug);

static void renderPrimitives(
    uint16_t *depth, uint8_t *color, const std::vector<PrimitiveType> &primitives,
    const std::vector<Vec4> &params, const Mat4 &M, const size_t w, const size_t h,
    const float near, const float far);

// -----------------------------------------------------------------------------

DEFINE_string(output, "./", "Dataset output");
DEFINE_string(geometry, "geometry.txt", "Input geometry file");
DEFINE_string(trajectory, "trajectory.txt", "Input trajectory file");
DEFINE_uint32(width, 640, "Width of the point clouds");
DEFINE_uint32(height, 480, "Height of the point clouds");
DEFINE_double(near, 0.10, "Near plane");
DEFINE_double(far, 50.0, "Far plane");
DEFINE_bool(debug, false, "Debug mode");

int main(int argc, char **argv)
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  gflags::SetUsageMessage("Synthetic dataset generator");

  const auto outputDir = std::string(FLAGS_output);
  const auto geometryFile = std::string(FLAGS_geometry);
  const auto trajectoryFile = std::string(FLAGS_trajectory);
  const auto w = FLAGS_width;
  const auto h = FLAGS_height;
  const auto near = float(FLAGS_near);
  const auto far = float(FLAGS_far);
  const auto debug = FLAGS_debug;

  const auto info = readDataset(geometryFile, trajectoryFile);
  renderPrimitives(info, outputDir, w, h, near, far, debug);

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------

static DatasetInfo readDataset(const std::string &geometry, const std::string &trajectory)
{
  DatasetInfo ret;
  {
    std::string primitiveName;
    Vec4 params;
    std::ifstream ifs(geometry.c_str());
    std::string line;
    while(std::getline(ifs, line))
    {
      std::istringstream iss(line);
      iss >> primitiveName >> params.x >> params.y >> params.z >> params.t;
      ret.primitives.emplace_back(
          primitiveName == std::string("sphere") ? PrimitiveType::SPHERE : PrimitiveType::PLANE);
      ret.params.emplace_back(params);
    }
  }

  {
    float tx, ty, tz;
    float qx, qy, qz, qw;
    std::ifstream ifs(trajectory.c_str());
    std::string line;
    while(std::getline(ifs, line))
    {
      std::istringstream iss(line);
      iss >> tx >> ty >> tz;
      ret.positions.emplace_back(Vec3(tx, ty, tz));
      iss >> qx >> qy >> qz >> qw;
      ret.rotations.emplace_back(Vec4(qw, qx, qy, qz));
    }
  }

  return ret;
}

static void renderPrimitives(
    const DatasetInfo &info, const std::string &outputDir, const size_t w, const size_t h,
    const float near, const float far, const bool debug)
{
  auto saveToPLY = [w, h](const float *points, const uint8_t *bgr, const char *filename) {
    FILE *fp = fopen(filename, "w+");
    if(fp)
    {
      fprintf(fp, "ply\n");
      fprintf(fp, "format ascii 1.0\n");
      fprintf(fp, "element vertex %lu\n", w * h);
      fprintf(fp, "property float x\n");
      fprintf(fp, "property float y\n");
      fprintf(fp, "property float z\n");
      fprintf(fp, "property uchar blue\n");
      fprintf(fp, "property uchar green\n");
      fprintf(fp, "property uchar red\n");
      fprintf(fp, "property uchar alpha\n");
      fprintf(fp, "end_header\n");

      for(size_t i = 0; i < w * h; i++)
      {
        fprintf(
            fp, "%f %f %f %u %u %u %u\n", points[3 * i], points[3 * i + 1], points[3 * i + 2],
            bgr[3 * i], bgr[3 * i + 1], bgr[3 * i + 2], 255);
      }

      fclose(fp);
    }
  };

  auto getPoint = [](const size_t u, const size_t v, const uint16_t d) -> Vec3 {
    constexpr float fx = SYNTH_FX;
    constexpr float fy = SYNTH_FY;
    constexpr float cx = SYNTH_CX;
    constexpr float cy = SYNTH_CY;

    if(d == 0)
    {
      return Vec3(0.0);
    }

    const float z = (float) d / 5000.0;
    const float x = ((float) u - cx) * (z / fx);
    const float y = ((float) v - cy) * (z / fy);
    return SYNTHETIC_0_PARAMS.AXIS_PERMUT * Vec3(x, y, z);
  };

  auto getPoints = [w, h,
                    getPoint](const uint16_t *depth, float *points, const Mat4 &transform) -> void {
    size_t i = 0;
    for(size_t v = 0; v < h; v++)
    {
      for(size_t u = 0; u < w; u++)
      {
        const auto pos = transform * getPoint(u, v, depth[i]);
        points[3 * i] = pos.x;
        points[3 * i + 1] = pos.y;
        points[3 * i + 2] = pos.z;
        i++;
      }
    }
  };

  const auto trajectorySize = info.positions.size();

#pragma omp parallel
  {
    std::unique_ptr<uint16_t[]> depth(new uint16_t[w * h]);
    std::unique_ptr<uint8_t[]> bgr(new uint8_t[3 * w * h]);

#pragma omp for
    for(size_t i = 0; i < trajectorySize; i++)
    {
      const auto &pos = info.positions[i];
      const auto &rot = info.rotations[i];
      const auto transform = Mat4::Affine(rot, pos);
      // const auto transform =
      //     Mat4::LookAt(pos, Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
      renderPrimitives(
          depth.get(), bgr.get(), info.primitives, info.params, transform, w, h, near, far);

      if(!debug) // Write depth maps
      {
        cv::Mat img(h, w, CV_16U, depth.get());
        char filename[512];
        sprintf(filename, "%s/depth%lu.png", outputDir.c_str(), i);
        cv::imwrite(filename, img);
      }
      else // Write PLY files
      {
        std::unique_ptr<float[]> points(new float[3 * w * h]);
        char filename[512];
        sprintf(filename, "%s/depth%lu.ply", outputDir.c_str(), i);
        getPoints(depth.get(), points.get(), Mat4::Inverse(transform));
        saveToPLY(points.get(), bgr.get(), filename);
      }
    }
  } // end of parallel region

  // Export info
  char filename[512];
  sprintf(filename, "%s/output.txt", outputDir.c_str());
  FILE *fp = fopen(filename, "w+");
  if(!fp)
  {
    throw std::runtime_error("Impossible to open output file");
  }

  for(size_t i = 0; i < trajectorySize; i++)
  {
    char depthName[512];
    sprintf(depthName, "depth%lu.png", i);
    const auto &pos = info.positions[i];
    const auto &rot = info.rotations[i];
    fprintf(
        fp, "%s %f %f %f %f %f %f %f\n", depthName, pos.x, pos.y, pos.z, rot.y, rot.z, rot.t,
        rot.x);
  }
  fclose(fp);
}

static void renderPrimitives(
    uint16_t *depthData, uint8_t *colors, const std::vector<PrimitiveType> &primitives,
    const std::vector<Vec4> &params, const Mat4 &M, const size_t w, const size_t h,
    const float near, const float far)
{
  auto getRay = [w, h](const size_t u, const size_t v) -> Ray {
    const float fx = SYNTH_FX;
    const float fy = SYNTH_FY;
    const float cx = SYNTH_CX;
    const float cy = SYNTH_CY;
    const Vec3 dir{((float) u - cx) / fx, ((float) v - cy) / fy, -1.0f};
    return Ray{Vec3(0.0f), Vec3::Normalize(dir)};
  };

  auto raycastSphere = [](const Ray &ray, const Vec4 &params, const Mat4 &M) -> Vec3 {
    const Vec3 sOrg = M * Vec3(params.x, params.y, params.z);
    const float rad = params.t;
    const float dist = Vec3::Len(Vec3::Cross(ray.dir, sOrg - ray.org));
    if(dist > rad)
    {
      return Vec3(FLT_MAX);
    }

    const float A = ray.org.x - sOrg.x;
    const float B = ray.org.y - sOrg.y;
    const float C = ray.org.z - sOrg.z;

    const float b = 2.0f * (ray.dir.x * A + ray.dir.y * B + ray.dir.z * C);
    const float c = A * A + B * B + C * C - rad * rad;
    const float delta = b * b - 4.0f * c;
    const float t0 = (-b - std::sqrt(delta)) / 2.0;
    const float t1 = (-b + std::sqrt(delta)) / 2.0;
    const float t = std::min(t0, t1);

    if(t < 0.0)
    {
      return Vec3(FLT_MAX);
    }

    return ray.org + t * ray.dir;
  };

  auto raycastPlane = [](const Ray &ray, const Vec4 &params, const Mat4 &M) -> Vec3 {
    const Vec3 org = Mat4::Inverse(M) * ray.org;
    const Vec3 dir = (Mat4::Inverse(M) * Vec4(ray.dir, 0.0f)).xyz();
    const float a = params.x;
    const float b = params.y;
    const float c = params.z;
    const float d = params.t;

    if(std::abs(Vec3::Dot(dir, Vec3{a, b, c})) <= 0.00001f)
    {
      return Vec3(FLT_MAX);
    }

    const float A = a * dir.x + b * dir.y + c * dir.z;
    const float B = -(a * org.x + b * org.y + c * org.z + d);
    const float t0 = B / A;

    if(t0 <= 0.0)
    {
      return Vec3(FLT_MAX);
    }

    return ray.org + t0 * ray.dir;
  };

  size_t id = 0;
  for(size_t v = 0; v < h; v++)
  {
    for(size_t u = 0; u < w; u++)
    {
      Ray ray = getRay(u, v);
      float depth = FLT_MAX;
      for(size_t i = 0; i < primitives.size(); i++)
      {
        const PrimitiveType primitive = primitives[i];
        const Vec4 primitiveParams = params[i];
        if(primitive == PrimitiveType::SPHERE)
        {
          const auto hit = raycastSphere(ray, primitiveParams, M);
          if(hit.z == FLT_MAX)
            continue;
          depth = std::min(depth, (ray.org - hit).z);
        }
        else if(primitive == PrimitiveType::PLANE)
        {
          const auto hit = raycastPlane(ray, primitiveParams, M);
          if(hit.z == FLT_MAX)
            continue;
          depth = std::min(depth, (ray.org - hit).z);
        }
      }

      if(depth != FLT_MAX && depth >= near && depth <= far)
      {
        depthData[v * w + u] = uint16_t(depth * 5000.0);
        colors[3 * id] = 127;
        colors[3 * id + 1] = 127;
        colors[3 * id + 2] = 127;
      }
      else
      {
        depthData[v * w + u] = 0u;
        colors[3 * id] = 0;
        colors[3 * id + 1] = 0;
        colors[3 * id + 2] = 0;
      }

      id++;
    }
  }
}

#pragma GCC diagnostic pop
