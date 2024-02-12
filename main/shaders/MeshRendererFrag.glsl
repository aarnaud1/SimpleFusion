#version 450

layout(location = 0) out vec4 fragColor;

layout(std140, row_major, binding = 0) uniform MatrixBlock
{
  mat4 projection;
  mat4 transform;
  mat4 camera;
}
matrixBlock;

in struct VS_OUT
{
  vec4 pos;
  vec4 color;
  vec4 normal;
} vsOut;

void main()
{
  const vec3 V = -normalize(vsOut.pos.xyz);
  const vec3 N = vsOut.normal.xyz;
  const vec3 L =
      normalize((vec4(1.0f, 1.0f, 1.0f, 1.0f)).xyz);
  const vec3 R = reflect(L, N);

  const float id = dot(L, N);
  const float is = pow(dot(R, V), 1000.0f);

  const vec3 ambiant = vsOut.color.bgr;
  const vec3 diffuse = (id > 0.0f ? id : 0.0f) * vec3(1.0f, 1.0f, 1.0f);
  const vec3 specular = (is > 0.0f ? is : 0.0f) * vec3(1.0f, 1.0f, 1.0f);
  const vec3 rgb = 0.8f * ambiant + 0.2f * diffuse + 0.1f * specular;
  fragColor = vec4(rgb, 1.0f);
}
