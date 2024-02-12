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

#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#define BUFFER_SIZE(N) ((GLsizeiptr) (N))
#define BUFFER_OFFSET(N) ((char *) NULL + N)

static inline void exitError(const char *msg = "")
{
  fprintf(stderr, "Error : %s\n", msg);
  exit(EXIT_FAILURE);
}
#endif // __GLOBAL_HPP__