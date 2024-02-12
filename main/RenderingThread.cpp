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

#include "RenderingThread.hpp"

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void error_callback(int error, const char *description);

static void windowResizeCallback(GLFWwindow *window, int w, int h);

void *mainLoop(void *);

static BasicTextureRenderer renderer_; // TODO : make it an attribute
static pthread_mutex_t dataMutex_;

RenderingThread::RenderingThread(const char *windowName) { stopped_ = 1u; }

RenderingThread::~RenderingThread() {}

void RenderingThread::Start()
{
  __sync_fetch_and_and(&stopped_, 1u);
  if(pthread_create(&mainThread_, NULL, &mainLoop, (void *) this) != 0)
  {
    exitError("could not create rendering main thread");
  }
}

void RenderingThread::Stop()
{
  __sync_fetch_and_and(&stopped_, 0u);
  pthread_join(mainThread_, NULL);
}

void RenderingThread::SetData(const void *data)
{
  pthread_mutex_lock(&dataMutex_);
  renderer_.UpdateTextureData(data, 0, 0);
  pthread_mutex_unlock(&dataMutex_);
}

// -----------------------------------------------------------------------------

void *mainLoop(void *ths)
{
  RenderingThread *data = (RenderingThread *) ths;
  unsigned char stopped = 0u;
  GLFWwindow *mainWindow_ = NULL;

  // Init GLFW and OpenGL
  glfwSetErrorCallback(error_callback);

  if(!glfwInit())
  {
    exitError("could not initialize GLDW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  const int INITIAL_WINDOW_W = 640;
  const int INITIAL_WINDOW_H = 480;
  mainWindow_ = glfwCreateWindow(INITIAL_WINDOW_W, INITIAL_WINDOW_H, "Main window", NULL, NULL);
  if(!mainWindow_)
  {
    glfwTerminate();
    exitError("could not create a window");
  }

  glfwSetWindowSizeCallback(mainWindow_, windowResizeCallback);
  glfwSetKeyCallback(mainWindow_, keyCallback);

  glfwMakeContextCurrent(mainWindow_);
  gladLoadGL();
  glfwSwapInterval(1);

  renderer_.Init();
  renderer_.Resize(640, 480);
  while(!stopped)
  {
    pthread_mutex_lock(&dataMutex_);
    renderer_.Draw();
    pthread_mutex_unlock(&dataMutex_);
    glFlush();
    glFinish();
    glfwSwapBuffers(mainWindow_);
    glfwPollEvents();
    stopped = glfwWindowShouldClose(mainWindow_);
    // stopped |= __sync_fetch_and_or(&data->stopped_, 0u);
  }
  renderer_.Destroy();

  glfwDestroyWindow(mainWindow_);
  glfwTerminate();

  __sync_fetch_and_and(&data->stopped_, 1u);
  return NULL;
}

// -----------------------------------------------------------------------------

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  fprintf(stdout, "Something happens...\n");
  switch(key)
  {
    case GLFW_KEY_ESCAPE:
      if(action == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        fprintf(stdout, "Window should close now...\n");
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

static void windowResizeCallback(GLFWwindow *window, int w, int h) {}
