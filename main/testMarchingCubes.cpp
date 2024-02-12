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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "spf/marching_cubes/MarchingCubes.hpp"

#define BLOCK_SIZE 64
#define SPHERE_RADIUS 32.0f

using namespace spf::mc;

int main()
{
  float* TSDF = (float*) malloc(BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE * sizeof(float));
  float* triangles = (float*) malloc(12 * 3 * BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE * sizeof(float));

  for(size_t i = 0; i < BLOCK_SIZE; i++)
  {
    for(size_t j = 0; j < BLOCK_SIZE; j++)
    {
      for(size_t k = 0; k < BLOCK_SIZE; k++)
      {
        const float x = (float) i;
        const float y = (float) j;
        const float z = (float) k;
        const float dist = sqrtf(x * x + y * y + z * z);
        TSDF[i + j * BLOCK_SIZE + k * BLOCK_SIZE * BLOCK_SIZE] = dist - SPHERE_RADIUS;
      }
    }
  }

  float blockPos[3] = {0, 0, 0};
  const size_t numTriangles = extractMesh(
      TSDF, NULL, NULL, NULL, NULL, NULL, NULL, NULL, triangles, BLOCK_SIZE, 1.0f, blockPos);
  fprintf(stdout, "extracted %lu triangles\n", numTriangles);

  const char* filename = "test-mc-output.ply";
  FILE* fp = fopen(filename, "w+");
  if(!fp)
  {
    fprintf(stderr, "Error opening %s : %s\n", filename, strerror(errno));
    return EXIT_FAILURE;
  }

  fprintf(fp, "ply\n");
  fprintf(fp, "format ascii 1.0\n");
  fprintf(fp, "element vertex %lu\n", 3 * numTriangles);
  fprintf(fp, "property float x\n");
  fprintf(fp, "property float y\n");
  fprintf(fp, "property float z\n");
  fprintf(fp, "element face %lu\n", numTriangles);
  fprintf(fp, "property list uchar int vertex_index\n");
  fprintf(fp, "end_header\n");
  for(size_t i = 0; i < 3 * numTriangles; i++)
  {
    fprintf(fp, "%f %f %f\n", triangles[3 * i + 0], triangles[3 * i + 1], triangles[3 * i + 2]);
  }

  for(size_t i = 0; i < numTriangles; i++)
  {
    fprintf(fp, "3 %lu %lu %lu\n", 3 * i, 3 * i + 1, 3 * i + 2);
  }

  fclose(fp);

  free(TSDF);
  free(triangles);
  return EXIT_SUCCESS;
}
