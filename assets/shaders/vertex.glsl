#version 330 core
layout (location = 0) in vec2 p0;
layout (location = 1) in vec2 p1;
layout (location = 2) in vec2 p2;
layout (location = 3) in float t;

uniform mat4 projection;
uniform mat4 view;


vec2 bezier_solver()
{
    vec2 ans = (1.0 - t) * (1.0 - t) * p0 + 2.0 * ( 1.0 - t) * t * p1 + t * t * p2;
    return ans; 
}

void main()
{
    vec2 pos = bezier_solver(); 
    gl_Position = projection * view * vec4(vec3(pos, 0.0), 1.0);
}
