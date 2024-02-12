#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out struct VS_OUT
{
  vec2 uvCoords;
}
vs_out_data;

void main()
{
  vs_out_data.uvCoords = texCoords;
  gl_Position = vec4(position, 1.0f);
}
