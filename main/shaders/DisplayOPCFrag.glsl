#version 450

layout(location = 0) out vec4 fragColor;

in struct VS_OUT
{
  vec4 pos;
  vec4 color;
  vec4 normal;
} vsOut;

void main()
{
  fragColor =
      vec4(abs(vsOut.normal.x), abs(vsOut.normal.y), abs(vsOut.normal.z), 1.0f);
}
