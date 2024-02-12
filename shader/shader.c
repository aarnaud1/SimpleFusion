/*
 * Copyright (C) 2024  Adrien ARNAUD
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

#include "shader.h"

GLuint shader_createProgram(const char *vtxSrc, const char *fragSrc)
{
  GLuint vtxShader = 0;
  GLuint fragShader = 0;
  GLuint program = 0;
  GLint linked = GL_FALSE;

  vtxShader = shader_createShader(GL_VERTEX_SHADER, vtxSrc);
  if (!vtxShader)
    goto exit;

  fragShader = shader_createShader(GL_FRAGMENT_SHADER, fragSrc);
  if (!fragShader)
    goto exit;

  program = glCreateProgram();
  if (!program)
  {
    goto exit;
  }

  glAttachShader(program, vtxShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  
  if (!linked)
  {
    fprintf(stderr, "Could not link program");
    GLint infoLogLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
    if (infoLogLen)
    {
      GLchar *infoLog = (GLchar *) malloc(infoLogLen);
      if (infoLog)
      {
        glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
        fprintf(stderr, "Could not link program:\n%s\n", infoLog);
        free(infoLog);
      }
    }
    glDeleteProgram(program);
    program = 0;
  }

  exit:
  glDeleteShader(vtxShader);
  glDeleteShader(fragShader);
  return program;
}

GLuint shader_create_compute_program(const char *kernel)
{
  GLuint program_id = 0;
  GLuint compute_shader = 0;
  GLint linked = GL_FALSE;

  compute_shader = shader_createShader(GL_COMPUTE_SHADER, kernel);
  if (!compute_shader) goto exit;

  program_id = glCreateProgram();
  if (!program_id) goto exit;

  glAttachShader(program_id, compute_shader);

  glLinkProgram(program_id);
  glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
  if (!linked)
  {
    fprintf(stderr, "Could not link compute program");
    GLint infoLogLen = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLen);
    if (infoLogLen)
    {
      GLchar *infoLog = (GLchar *) malloc((size_t) infoLogLen);
      if (infoLog)
      {
        glGetProgramInfoLog(program_id, infoLogLen, NULL, infoLog);
        fprintf(stderr, "Could not link compute program:\n%s\n", infoLog);
        free(infoLog);
      }
    }
    glDeleteProgram(program_id);
    program_id = 0;
  }

  exit:
  glDeleteShader(compute_shader);
  return program_id;
}

GLuint shader_createShader(GLenum shaderType, const char *src)
{
  GLuint shader = glCreateShader(shaderType);
  if (!shader)
  {
    fprintf(stderr, "Problem shader");
    return 0;
  }
  glShaderSource(shader, 1, &src, NULL);

  GLint compiled = GL_FALSE;
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
  {
    GLint infoLogLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
    if (infoLogLen > 0)
    {
      GLchar *infoLog = (GLchar *) malloc(infoLogLen);
      if (infoLog)
      {
        glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);

        fprintf(stderr,
          "Could not compile %s shader:\n%s\n",
          shaderType == GL_VERTEX_SHADER
          ? "vertex"
          : (shaderType == GL_FRAGMENT_SHADER
             ? "fragment"
             : "compute"),
          infoLog
        );

        free(infoLog);
      }
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}
