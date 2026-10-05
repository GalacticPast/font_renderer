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
uniform float font_px; // pixels per em
uniform vec2  pen_offset; // em, where this glyph's origin sits on the line

out vec2 frag_pos;

void main()
{
    // glyph space, in em: same units as the curves. Must not include pen_offset
    vec4 glyph_pos = model * vec4(position, 1.0);
    frag_pos = glyph_pos.xy;

    // em -> pixels, for placing the vertex on screen only
    vec4 pixel_pos = vec4((glyph_pos.xy + pen_offset) * font_px, 0.0, 1.0);
    gl_Position    = projection * view * pixel_pos;
}
