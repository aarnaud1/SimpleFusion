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

#include <iostream>
#include <stdio.h>
#include <chrono>
#include <unordered_map>

#define DEBUG_GRAPHICS

#define LOG_LEVEL_VERBOSE 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_CRITICAL 3

#ifndef NO_DEBUG
#  ifndef LOG_LEVEL
#    define LOG_LEVEL LOG_LEVEL_VERBOSE
#  endif
#else
#  define LOG_LEVEL -1
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wformat-security"

namespace utils
{
class Log
{
public:
  template <typename... Args>
  static void Message(const char* format, Args... args)
  {
    if constexpr(LogLevel >= 0)
    {
      fprintf(stdout, format, args...);
    }
  }
  template <typename... Args>
  static void Info(const char* tag, const char* format, Args... args)
  {
    if constexpr(LogLevel <= LOG_LEVEL_VERBOSE)
    {
      fprintf(stdout, "\033[0;34m");
      fprintf(stdout, "[%s]: ", tag);
      fprintf(stdout, format, args...);
      fprintf(stdout, "\033[0m");
    }
  }
  template <typename... Args>
  static void Warning(const char* tag, const char* format, Args... args)
  {
    if constexpr(LogLevel <= LOG_LEVEL_WARNING)
    {
      fprintf(stdout, "\033[0;33m");
      fprintf(stdout, "[%s]: ", tag);
      fprintf(stdout, format, args...);
      fprintf(stdout, "\033[0m");
      fflush(stdout);
    }
  }
  template <typename... Args>
  static void Error(const char* tag, const char* format, Args... args)
  {
    if constexpr(LogLevel <= LOG_LEVEL_ERROR)
    {
      fprintf(stderr, "\033[0;31m");
      fprintf(stderr, "[%s]: ", tag);
      fprintf(stderr, format, args...);
      fprintf(stderr, "\033[0m");
    }
  }
  template <typename... Args>
  static void Critical(const char* tag, const char* format, Args... args)
  {
    fprintf(stderr, "\033[0;31m");
    fprintf(stderr, "[%s]: ", tag);
    fprintf(stderr, format, args...);
    fprintf(stderr, "\033[0m");
    fflush(stderr);
  }

private:
  static constexpr int LogLevel = LOG_LEVEL;
  Log() = default;
};
} // namespace utils
#pragma GCC diagnostic pop

#define CHRONO(f)                                                                                  \
  {                                                                                                \
    auto start = std::chrono::steady_clock::now();                                                 \
    f;                                                                                             \
    auto stop = std::chrono::steady_clock::now();                                                  \
    const double t = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();  \
    utils::Log::Info("Timing", "%s took : %f ms\n", #f, t);                                        \
  }

#define START_CHRONO(M)                                                                            \
  {                                                                                                \
    const char* msg = M;                                                                           \
    auto start = std::chrono::steady_clock::now();

#define STOP_CHRONO()                                                                              \
  auto stop = std::chrono::steady_clock::now();                                                    \
  const double t = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();    \
  utils::Log::Info("Timing", "%s took : %f ms\n", msg, t);                                         \
  }

#ifdef DEBUG_GRAPHICS
#  include <glad/glad.h>

#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wunused-variable"
static void checkGLError(const char* message)
{
  GLenum err = glGetError();
  switch(err)
  {
    case GL_NO_ERROR:
      break;
    case GL_INVALID_ENUM:
      utils::Log::Error("GL check", "%s GL error : INVALID ENUM\n", message);
      break;
    case GL_INVALID_VALUE:
      utils::Log::Error("GL check", "%s GL error : INVALID VALUE\n", message);
      break;
    case GL_INVALID_OPERATION:
      utils::Log::Error("GL check", "%s GL error : INVALID OPERATION\n", message);
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      utils::Log::Error("GL cherck", "%s GL error : INVALID FRAMEBUFFER OPERATION\n", message);
      break;
    case GL_OUT_OF_MEMORY:
      utils::Log::Error("GL check", "%s GL error : OUT OF MEMORY\n", message);
      break;
    case GL_STACK_OVERFLOW:
      utils::Log::Error("GL check", "%s GL error : STACK OVERFLOW\n", message);
      break;
    case GL_STACK_UNDERFLOW:
      utils::Log::Error("GL check", "%s GL error : STACK UNDERFLOW\n", message);
      break;
    default:
      utils::Log::Error("GL check", "%s GL error : UNKNOWN ERROR\n", message);
      break;
  }
}
#  pragma GCC diagnostic pop

#  define CHECK_GL(f)                                                                              \
    {                                                                                              \
      glGetError();                                                                                \
      f;                                                                                           \
      checkGLError(#f);                                                                            \
    }
#else
#  define CHECK_GL(f) f
#endif
