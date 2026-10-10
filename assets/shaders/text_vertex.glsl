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
layout(std430, binding = 2) buffer h_bands{
    int horizontal_bands[];
};
layout(std430, binding = 3) buffer v_bands{
    int vertical_bands[];
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (location = 0) in vec3 position;

// instanced
layout (location = 1) in vec2 pen_offset;
layout (location = 2) in vec2 curve_indicies;
layout (location = 3) in mat4 translation_matrix;
// bands_loc[0..7] (horizontal), two bands packed per column: col c holds bands b=2c (.xy) and b=2c+1 (.zw)
layout (location = 7)  in mat4 h_band_loc;
// bands_loc[8..15] (vertical), same packing
layout (location = 11) in mat4 v_band_loc;

flat out vec2 frag_curve_indicies;
flat out mat4 frag_h_band_loc;
flat out mat4 frag_v_band_loc;
flat out vec2 frag_glyph_min;
flat out vec2 frag_glyph_max;

uniform float font_px; // pixels per em

out vec2 frag_pos;

void main()
{
    // glyph space, in em: same units as the curves. Must not include pen_offset
    vec4 glyph_pos = (model * translation_matrix) * vec4(position, 1.0);
    // dilate the quad by half a pixel so edge pixels are shaded (the paper's bounding-box expansion)
    glyph_pos.xy += sign(position.xy) * (0.5 / font_px);
    frag_pos = glyph_pos.xy;
    frag_curve_indicies = curve_indicies;
    frag_h_band_loc = h_band_loc;
    frag_v_band_loc = v_band_loc;

    // translation_matrix = translate(center) * scale(half_size * 2); recover the glyph's em-space bbox
    // from it directly (model is identity in this renderer, so skipping it here is safe)
    vec2 glyph_center    = translation_matrix[3].xy;
    vec2 glyph_half_size = vec2(translation_matrix[0].x, translation_matrix[1].y) * 0.5;
    frag_glyph_min = glyph_center - glyph_half_size;
    frag_glyph_max = glyph_center + glyph_half_size;

    // em -> pixels, for placing the vertex on screen only. The screen is top-left origin (y down) but font
    // outlines are y up, so the glyph's y is flipped here, not in the projection, to keep the UI convention
    vec2 pixel_xy = vec2(glyph_pos.x + pen_offset.x, pen_offset.y - glyph_pos.y) * font_px;
    vec4 pixel_pos = vec4(pixel_xy, 0.0, 1.0);
    gl_Position    = projection * view * pixel_pos;
}
