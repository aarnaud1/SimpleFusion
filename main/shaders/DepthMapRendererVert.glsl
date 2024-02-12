#version 450

layout(location = 0) in vec3 xyz;
layout(location = 1) in vec2 uv;

out struct VS_OUT
{
  vec3 xyz;
  vec2 uv;
} vsOut;

void main()
{
  gl_Position = vec4(xyz.x, -xyz.y, xyz.z, 1.0f);
  vsOut.xyz = xyz;
  vsOut.uv = uv;
}
