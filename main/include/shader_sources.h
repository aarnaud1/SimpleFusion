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

#pragma once

#include <glad/glad.h>

static const GLchar __DisplayOPCFrag[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) out vec4 fragColor;                                           \n"
  "                                                                                    \n"
  " in struct VS_OUT                                                                   \n"
  " {                                                                                  \n"
  "   vec4 pos;                                                                        \n"
  "   vec4 color;                                                                      \n"
  "   vec4 normal;                                                                     \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   fragColor =                                                                      \n"
  "       vec4(abs(vsOut.normal.x), abs(vsOut.normal.y), abs(vsOut.normal.z), 1.0f);   \n"
  " }                                                                                  \n"
};

static const GLchar __StreamingDisplayVert[] =
{
  " #version 450 core                                                                  \n"
  "                                                                                    \n"
  " layout(location = 0) in vec3 position;                                             \n"
  " layout(location = 1) in vec2 texCoords;                                            \n"
  "                                                                                    \n"
  " out struct VS_OUT                                                                  \n"
  " {                                                                                  \n"
  "   vec2 uvCoords;                                                                   \n"
  " }                                                                                  \n"
  " vs_out_data;                                                                       \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   vs_out_data.uvCoords = texCoords;                                                \n"
  "   gl_Position = vec4(position, 1.0f);                                              \n"
  " }                                                                                  \n"
};

static const GLchar __StreamingDisplayCloudFrag[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) out vec4 fragColor;                                           \n"
  "                                                                                    \n"
  " in struct VS_OUT                                                                   \n"
  " {                                                                                  \n"
  "   vec4 pos;                                                                        \n"
  "   vec4 color;                                                                      \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   fragColor = vsOut.color;                                                         \n"
  " }                                                                                  \n"
};

static const GLchar __MeshRendererFrag[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) out vec4 fragColor;                                           \n"
  "                                                                                    \n"
  " layout(std140, row_major, binding = 0) uniform MatrixBlock                         \n"
  " {                                                                                  \n"
  "   mat4 projection;                                                                 \n"
  "   mat4 transform;                                                                  \n"
  "   mat4 camera;                                                                     \n"
  " }                                                                                  \n"
  " matrixBlock;                                                                       \n"
  "                                                                                    \n"
  " in struct VS_OUT                                                                   \n"
  " {                                                                                  \n"
  "   vec4 pos;                                                                        \n"
  "   vec4 color;                                                                      \n"
  "   vec4 normal;                                                                     \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   const vec3 V = -normalize(vsOut.pos.xyz);                                        \n"
  "   const vec3 N = vsOut.normal.xyz;                                                 \n"
  "   const vec3 L =                                                                   \n"
  "       normalize((vec4(1.0f, 1.0f, 1.0f, 1.0f)).xyz);                               \n"
  "   const vec3 R = reflect(L, N);                                                    \n"
  "                                                                                    \n"
  "   const float id = dot(L, N);                                                      \n"
  "   const float is = pow(dot(R, V), 1000.0f);                                        \n"
  "                                                                                    \n"
  "   const vec3 ambiant = vsOut.color.bgr;                                            \n"
  "   const vec3 diffuse = (id > 0.0f ? id : 0.0f) * vec3(1.0f, 1.0f, 1.0f);           \n"
  "   const vec3 specular = (is > 0.0f ? is : 0.0f) * vec3(1.0f, 1.0f, 1.0f);          \n"
  "   const vec3 rgb = 0.8f * ambiant + 0.2f * diffuse + 0.1f * specular;              \n"
  "   fragColor = vec4(rgb, 1.0f);                                                     \n"
  " }                                                                                  \n"
};

static const GLchar __DepthMapRendererVert[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) in vec3 xyz;                                                  \n"
  " layout(location = 1) in vec2 uv;                                                   \n"
  "                                                                                    \n"
  " out struct VS_OUT                                                                  \n"
  " {                                                                                  \n"
  "   vec3 xyz;                                                                        \n"
  "   vec2 uv;                                                                         \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   gl_Position = vec4(xyz.x, -xyz.y, xyz.z, 1.0f);                                  \n"
  "   vsOut.xyz = xyz;                                                                 \n"
  "   vsOut.uv = uv;                                                                   \n"
  " }                                                                                  \n"
};

static const GLchar __StreamingDisplayCloudVert[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) in vec4 pos;                                                  \n"
  " layout(location = 1) in vec4 color;                                                \n"
  "                                                                                    \n"
  " layout(std140, row_major, binding = 0) uniform MatrixBlock                         \n"
  " {                                                                                  \n"
  "   mat4 projection;                                                                 \n"
  "   mat4 transform;                                                                  \n"
  "   mat4 camera;                                                                     \n"
  " }                                                                                  \n"
  " matrixBlock;                                                                       \n"
  "                                                                                    \n"
  " out struct VS_OUT                                                                  \n"
  " {                                                                                  \n"
  "   vec4 pos;                                                                        \n"
  "   vec4 color;                                                                      \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   gl_Position =                                                                    \n"
  "       matrixBlock.projection * matrixBlock.camera * matrixBlock.transform * pos;   \n"
  "   vsOut.pos = pos;                                                                 \n"
  "   vsOut.color = color;                                                             \n"
  " }                                                                                  \n"
};

static const GLchar __StreamingDisplayFrag[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " uniform sampler2D image;                                                           \n"
  "                                                                                    \n"
  " in struct VS_OUT                                                                   \n"
  " {                                                                                  \n"
  "   vec2 uvCoords;                                                                   \n"
  " } vs_out_data;                                                                     \n"
  "                                                                                    \n"
  " layout(location = 0) out vec4 fragColor;                                           \n"
  "                                                                                    \n"
  " void main() { fragColor = texture(image, vs_out_data.uvCoords); }                  \n"
};

static const GLchar __DepthMapRendererFrag[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " uniform sampler2D tex;                                                             \n"
  " layout(location = 0) out vec4 fragColor;                                           \n"
  "                                                                                    \n"
  " in struct VS_OUT                                                                   \n"
  " {                                                                                  \n"
  "   vec3 xyz;                                                                        \n"
  "   vec2 uv;                                                                         \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   const float depth = texture(tex, vsOut.uv).r;                                    \n"
  "   if(depth == 0.0f)                                                                \n"
  "   {                                                                                \n"
  "     fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);                                      \n"
  "   }                                                                                \n"
  "   else                                                                             \n"
  "   {                                                                                \n"
  "     fragColor = vec4(                                                              \n"
  "         mix(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), depth * 5.f), 1.0f);   \n"
  "   }                                                                                \n"
  " }                                                                                  \n"
};

static const GLchar __DisplayOPCVert[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) in vec4 pos;                                                  \n"
  " layout(location = 1) in vec4 color;                                                \n"
  " layout(location = 2) in vec3 normal;                                               \n"
  "                                                                                    \n"
  " layout(std140, row_major, binding = 0) uniform MatrixBlock                         \n"
  " {                                                                                  \n"
  "   mat4 projection;                                                                 \n"
  "   mat4 transform;                                                                  \n"
  "   mat4 camera;                                                                     \n"
  " }                                                                                  \n"
  " matrixBlock;                                                                       \n"
  "                                                                                    \n"
  " out struct VS_OUT                                                                  \n"
  " {                                                                                  \n"
  "   vec4 pos;                                                                        \n"
  "   vec4 color;                                                                      \n"
  "   vec4 normal;                                                                     \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   gl_Position =                                                                    \n"
  "       matrixBlock.projection * matrixBlock.camera * matrixBlock.transform * pos;   \n"
  "   vsOut.pos = pos;                                                                 \n"
  "   vsOut.color = color;                                                             \n"
  "   vsOut.normal =                                                                   \n"
  "       (matrixBlock.camera * matrixBlock.transform * vec4(normal.xyz, 0.0f));       \n"
  " }                                                                                  \n"
};

static const GLchar __MeshRendererVert[] =
{
  " #version 450                                                                       \n"
  "                                                                                    \n"
  " layout(location = 0) in vec3 pos;                                                  \n"
  " layout(location = 1) in vec3 normal;                                               \n"
  " layout(location = 2) in vec3 color;                                                \n"
  "                                                                                    \n"
  " layout(std140, row_major, binding = 0) uniform MatrixBlock                         \n"
  " {                                                                                  \n"
  "   mat4 projection;                                                                 \n"
  "   mat4 transform;                                                                  \n"
  "   mat4 camera;                                                                     \n"
  " }                                                                                  \n"
  " matrixBlock;                                                                       \n"
  "                                                                                    \n"
  " out struct VS_OUT                                                                  \n"
  " {                                                                                  \n"
  "   vec4 pos;                                                                        \n"
  "   vec4 color;                                                                      \n"
  "   vec4 normal;                                                                     \n"
  " } vsOut;                                                                           \n"
  "                                                                                    \n"
  " void main()                                                                        \n"
  " {                                                                                  \n"
  "   gl_Position = matrixBlock.projection * matrixBlock.camera                        \n"
  "                 * matrixBlock.transform * vec4(pos, 1.0f);                         \n"
  "   vsOut.pos = vec4(pos, 1.0f);                                                     \n"
  "   vsOut.normal = vec4(normal, 1.0f);                                               \n"
  "   vsOut.color = vec4(color, 1.0f);                                                 \n"
  " }                                                                                  \n"
};

  