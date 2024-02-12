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
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gflags/gflags.h>

#include <spf/utils.hpp>
#include <spf/geometry/geometry.hpp>
#include <spf/data_types/PointCloud.hpp>
#include <spf/data_types/OrderedPointCloud.hpp>
#include <spf/data_types/RGBDFrame.hpp>
#include <spf/fusion/Fusion.hpp>
#include <spf/fusion/Volume.hpp>

#include "Global.hpp"
#include "Parameters.hpp"
#include "DataStreamer.hpp"
#include "RenderWindow.hpp"
#include "PointCloudFrameRenderer.hpp"
#include "MeshRenderer.hpp"
#include "DepthMapRenderer.hpp"
#include "OPCFrameRenderer.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// -----------------------------------------------------------------------------

typedef spf::Mat4f Mat4;
typedef spf::Vec4f Vec4;
typedef spf::Vec3f Vec3;
typedef spf::fusion::Fusion Fusion;

using PointType = spf::data_types::PointXYZRGB<float>;
using PointCloudType = spf::data_types::PointCloud<PointType>;
using OPCPointType = spf::data_types::PointXYZRGBN<float>;
using OPCType = spf::data_types::OrderedPointCloud<OPCPointType>;
using RGBDFrameType = spf::data_types::RGBDFrame<uint16_t, uint8_t>;

struct Instance
{
  CameraParameters params;
  FusionParameters fusionParams;
  std::unique_ptr<IDataStreamer> dataStreamer;
  RenderWindow renderWindow;

  PointCloudFrameRenderer cloudRenderer;
  DepthMapRenderer depthMapRenderer;
  DepthMapRenderer filteredDepthMapRenderer;
  OPCFrameRenderer opcRenderer;

  PointCloudType inputCloud;
  OPCType inputOpc;
  RGBDFrameType rgbd;
  spf::data_types::CameraIntrinsics<float> intrinsics;

  Fusion fusion;

  GLFWwindow *mainWindow;

  Instance(
      const CameraParameters &params, const FusionParameters &fusionParams, const char *dataset) :
      params(params),
      fusionParams(fusionParams),
      dataStreamer(nullptr),
      renderWindow(params.cameraWidth, params.cameraHeight, 2, 2),
      cloudRenderer(params.cameraWidth * params.cameraHeight, params.MODEL_TO_OPENGL),
      depthMapRenderer(params.cameraWidth, params.cameraHeight),
      filteredDepthMapRenderer(params.cameraWidth, params.cameraHeight),
      opcRenderer(params.cameraWidth, params.cameraHeight, params.MODEL_TO_OPENGL),
      inputCloud(params.cameraWidth * params.cameraHeight),
      inputOpc(params.cameraWidth, params.cameraHeight),
      rgbd(params.cameraWidth, params.cameraHeight),
      intrinsics(params.depthIntrinsics),
      fusion(fusionParams.voxelRes, fusionParams.tau, params.cameraWidth, params.cameraHeight)
  {}

  void InitRendering()
  {
    depthMapRenderer.Init();
    cloudRenderer.Init();
    opcRenderer.Init();

    renderWindow.AddRenderer(&depthMapRenderer, 0, 0);
    renderWindow.AddRenderer(&opcRenderer, 0, 1);
    renderWindow.AddRenderer(&cloudRenderer, 1, 0);
    renderWindow.Resize(640, 480);
  }

  void DestroyRendering()
  {
    depthMapRenderer.Destroy();
    cloudRenderer.Destroy();
    opcRenderer.Destroy();
  }
};

// -----------------------------------------------------------------------------

static void onRGBDFrameAvailable(
    const uint16_t *depth, const uint8_t *color, const Vec3 &translation, const Vec4 &rotation,
    const size_t w, const size_t h);

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void error_callback(int error, const char *description);

static void windowResizeCallback(GLFWwindow *window, int w, int h);

static void appMainLoop(void);

// -----------------------------------------------------------------------------

// Parameters
DEFINE_string(datasetType, "fr1", "Type of dataset to use : [fr1, icl1");
DEFINE_string(dataset, "", "Dataset path");
DEFINE_double(voxelRes, 0.01, "Voxel resolution in meters");
DEFINE_double(tau, 0.025, "Truncation distance");
DEFINE_double(maxDist, 2.0, "Max integration distance");
DEFINE_double(minDist, 0.0, "Minimum integration distance");
DEFINE_bool(updateMesh, false, "Update mesh after each level");
DEFINE_bool(useOPC, false, "Use OPC or not for integration");
DEFINE_bool(useHybrid, false, "Hybrid integration (experimental");
DEFINE_bool(noExport, false, "Export final mesh");
DEFINE_bool(dumpBlocks, false, "Dump all blocks at the end");
DEFINE_bool(preload, false, "Preload previously stored blocks");
DEFINE_string(outputDir, "./", "Output directory");
DEFINE_string(outputFile, "fusion-output.ply", "Output .ply file to export");

static Instance *instance_ = NULL;

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  gflags::SetUsageMessage("Test fusion program");

  FusionParameters params;
  params.voxelRes = static_cast<float>(FLAGS_voxelRes);
  params.tau = static_cast<float>(FLAGS_tau);

  const char *datasetType = FLAGS_datasetType.c_str();
  const char *datasetDir = FLAGS_dataset.c_str();

  instance_ = new Instance(getParams(datasetType), params, datasetDir);

  if(std::string(datasetType) == std::string("synthetic0"))
  {
    instance_->dataStreamer = std::unique_ptr<IDataStreamer>(new SyntheticDataStreamer(datasetDir));
  }
  else
  {
    instance_->dataStreamer = std::unique_ptr<IDataStreamer>(new DataStreamer(datasetDir));
  }

  // Init GLFW and OpenGL
  glfwSetErrorCallback(error_callback);

  if(!glfwInit())
  {
    exitError("could not initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  const int INITIAL_WINDOW_W = 640;
  const int INITIAL_WINDOW_H = 480;
  instance_->mainWindow =
      glfwCreateWindow(INITIAL_WINDOW_W, INITIAL_WINDOW_H, "Main window", NULL, NULL);
  if(!instance_->mainWindow)
  {
    glfwTerminate();
    exitError("could not create a window");
  }

  glfwSetWindowSizeCallback(instance_->mainWindow, windowResizeCallback);
  glfwSetKeyCallback(instance_->mainWindow, keyCallback);
  glfwMakeContextCurrent(instance_->mainWindow);
  gladLoadGL();
  glfwSwapInterval(1);

  instance_->InitRendering();

  instance_->dataStreamer->RegisterRGBDFrameCallback(onRGBDFrameAvailable);
  instance_->dataStreamer->PrepareStreamingData();

  if(FLAGS_preload)
  {
    utils::Log::Info("Main", "Reading blocks\n");
    instance_->fusion.PreloadBlocks(FLAGS_outputDir.c_str());
  }

  appMainLoop();

  instance_->fusion.RecomputeMeshes();

  if(!FLAGS_noExport)
  {
    char outputFile[512];
    sprintf(outputFile, "%s/%s", FLAGS_outputDir.c_str(), FLAGS_outputFile.c_str());
    instance_->fusion.ExportMesh(outputFile);
  }

  if(FLAGS_dumpBlocks)
  {
    instance_->fusion.DumpAllBlocks(FLAGS_outputDir.c_str());
  }
  instance_->DestroyRendering();

  glfwDestroyWindow(instance_->mainWindow);
  glfwTerminate();

  delete instance_;

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------

static void onRGBDFrameAvailable(
    const uint16_t *depth, const uint8_t *color, const Vec3 &translation, const Vec4 &rotation,
    const size_t w, const size_t h)
{
  const Mat4 axisPermut = instance_->params.AXIS_PERMUT;

  // TODO : for normal dataset : axisPermut * affine
  Mat4 transform;
  if(std::string(FLAGS_datasetType) == std::string("synthetic0"))
  {
    transform = Mat4::Inverse(Mat4::Affine(rotation, translation)) * axisPermut;
  }
  else
  {
    transform = axisPermut * Mat4::Affine(rotation, translation);
  }

  instance_->rgbd.Clear();
  instance_->inputCloud.Clear();

  if(color == nullptr)
  {
    for(size_t i = 0; i < 3 * w * h; i++)
    {
      instance_->rgbd.Color()[i] = 127;
    }
  }
  else
  {
    memcpy(instance_->rgbd.Color(), color, 3 * w * h * sizeof(uint8_t));
  }
  memcpy(instance_->rgbd.Depth(), depth, w * h * sizeof(uint16_t));

  utils::Log::Message("--------------------------------------------------------\n");
  utils::Log::Info("Main", "Integrating a new frame\n");

  const auto minDist = static_cast<float>(FLAGS_minDist);
  const auto maxDist = static_cast<float>(FLAGS_maxDist);

  instance_->depthMapRenderer.UpdateImg(depth);
  instance_->rgbd.FilterData();

  // Extract normal cloud
  instance_->rgbd.ExtractPoints(
      instance_->inputCloud, instance_->intrinsics, minDist, maxDist, 5000.0f);

  // Extract OPC
  instance_->rgbd.ExtractPoints(
      instance_->inputOpc, instance_->intrinsics, minDist, maxDist, 5000.0f);
  instance_->inputOpc.EstimateNormals(5.0f * FLAGS_voxelRes);

  // Update cloud renderers
  instance_->cloudRenderer.UpdateCloudData(instance_->inputCloud);
  instance_->opcRenderer.UpdateCloudData(instance_->inputOpc);

  START_CHRONO("Whole process");

  // Integrate depth map
  if(FLAGS_useOPC)
  {
    instance_->fusion.IntegrateDepthMapOrdered(
        instance_->rgbd, instance_->intrinsics, transform, static_cast<float>(FLAGS_minDist),
        static_cast<float>(FLAGS_maxDist));
  }
  else
  {
    instance_->fusion.IntegrateDepthMap(
        instance_->rgbd, instance_->intrinsics, transform, static_cast<float>(FLAGS_minDist),
        static_cast<float>(FLAGS_maxDist));
  }

  // Update 3D model
  if(FLAGS_updateMesh)
  {
    instance_->fusion.UpdateMeshes();
  }
  STOP_CHRONO();
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  switch(key)
  {
    case GLFW_KEY_ESCAPE:
      if(action == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
      break;
    case GLFW_KEY_RIGHT:
      break;
    case GLFW_KEY_LEFT:
      break;
    default:
      break;
  }
}

static void error_callback(int error, const char *description) {}

static void windowResizeCallback(GLFWwindow *window, int w, int h)
{
  instance_->renderWindow.Resize(w, h);
}

static void appMainLoop()
{
  while(!glfwWindowShouldClose(instance_->mainWindow) && instance_->dataStreamer->StreamNextData())
  {
    instance_->renderWindow.Draw();
    glFlush();
    glFinish();
    glfwSwapBuffers(instance_->mainWindow);
    glfwPollEvents();
  }
}
#pragma GCC diagnostic pop
