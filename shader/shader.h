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

#ifndef __SHADER_H
#define __SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>

#define LINE_MAX_LENGTH 512

#ifdef __cplusplus
extern "C" {
#endif

extern GLuint shader_createProgram(const char *vtxSrc, const char *fragSrc);

extern GLuint shader_create_compute_program(const char *kernel);

extern GLuint shader_createShader(GLenum shaderType, const char *src);

#ifdef __cplusplus
}
#endif

#endif //_ _SHADER_H
