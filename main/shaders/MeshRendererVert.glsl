#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

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
  vec4 normal;
} vsOut;

void main()
{
  gl_Position = matrixBlock.projection * matrixBlock.camera
                * matrixBlock.transform * vec4(pos, 1.0f);
  vsOut.pos = vec4(pos, 1.0f);
  vsOut.normal = vec4(normal, 1.0f);
  vsOut.color = vec4(color, 1.0f);
}
