#version 450

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

layout(std140, row_major, binding = 0) uniform MatrixBlock
{
  mat4 projection;
  mat4 transform;
  mat4 camera;
}
matrixBlock;

out struct VS_OUT
{
  vec4 pos;
  vec4 color;
} vsOut;

void main()
{
  gl_Position =
      matrixBlock.projection * matrixBlock.camera * matrixBlock.transform * pos;
  vsOut.pos = pos;
  vsOut.color = color;
}
