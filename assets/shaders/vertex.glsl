#version 430 core

struct Curve
{
    vec4 p0;
    vec4 p1; // control 
    vec4 p2;
};

layout(std430, binding = 1) buffer curve_buffer{
    Curve curves[];
};

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec2 curve_indicies;

out vec2 frag_pos;

void main()
{
    vec4 world_pos = model * vec4(position, 1.0);
    frag_pos = world_pos.xy;
    gl_Position = projection * view * world_pos;
}
