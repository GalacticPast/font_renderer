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
    f32        em_to_px_scale;
    s32        curves_start_index;
    s32        curves_end_index;
    db_vector4 half_extent;
} glyph_data;

typedef struct
{
    const char     *font_name;
    glyph_data      data[94]; // most of the ascii coverage. From '!'(33) to '~'(126)
    db_array_curves curves;
} glyphs;

void text_load_font(db_arena *arena, const char *file_path);
