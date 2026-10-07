#include "defines.h"

typedef struct
{
    db_vector4 p0;
    db_vector4 p1; // control
    db_vector4 p2;
} curve;

db_array_decl(curves, curve);

typedef struct
{
    db_matrix4 translation_matrix;
    db_vector2 curves_indicies;
    db_aabb2   aabb;
    f32        advance; // em
} glyph_data;

typedef struct
{
    const char     *font_name;
    f32             funit_to_em; // usually 2048
    glyph_data      data[95];    // most of the ascii coverage. From ' '(32) to '~'(126)
    db_array_curves curves;
} glyphs;

// vec4s written per glyph by text_prepare_render_buffer: pen + curve range, then the 4 matrix columns
#define TEXT_VEC4S_PER_GLYPH 5

glyphs *text_load_font(db_arena *arena, const char *file_path);
void    text_prepare_render_buffer(db_string *string, db_array_vector4 *buffer, f32 font_size, f32 wrap_width);
