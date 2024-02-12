#version 450

layout(location = 0) out vec4 fragColor;

in struct VS_OUT
{
  vec4 pos;
  vec4 color;
} vsOut;

void main()
{
  fragColor = vsOut.color;
}
