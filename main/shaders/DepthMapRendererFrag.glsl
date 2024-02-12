#version 450

uniform sampler2D tex;
layout(location = 0) out vec4 fragColor;

in struct VS_OUT
{
  vec3 xyz;
  vec2 uv;
} vsOut;

void main()
{
  const float depth = texture(tex, vsOut.uv).r;
  if(depth == 0.0f)
  {
    fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }
  else
  {
    fragColor = vec4(
        mix(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), depth * 5.f), 1.0f);
  }
}
