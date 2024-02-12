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

#ifndef __RENDERING_THREAD__
#define __RENDERING_THREAD__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifndef _REENTRANT
#  define _REENTRANT
#endif
#include <pthread.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Global.hpp"
#include "BasicTextureRenderer.hpp"

class RenderingThread
{
public:
  RenderingThread(const char *windowName);

  ~RenderingThread();

  void Start();

  void Stop();

  void SetData(const void *data);

private:
  // GLFWwindow *mainWindow_ = NULL;
  pthread_t mainThread_;
  volatile unsigned char stopped_;

  friend void *mainLoop(void *);
};

#endif // __RENDERING_THREAD__
