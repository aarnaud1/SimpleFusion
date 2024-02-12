#version 450

uniform sampler2D image;

in struct VS_OUT
{
  vec2 uvCoords;
} vs_out_data;

layout(location = 0) out vec4 fragColor;

void main() { fragColor = texture(image, vs_out_data.uvCoords); }
