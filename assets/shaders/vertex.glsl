#version 330 core
layout (location = 0) in vec2 a_pos;

void main()
{
    vec2 scaled_pos = a_pos * 0.001;
    
    scaled_pos.x -= 0.5; 
    scaled_pos.y -= 0.5;

    gl_Position = vec4(scaled_pos, 0.0, 1.0);
}
