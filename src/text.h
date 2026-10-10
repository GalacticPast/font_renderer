#include "defines.h"

typedef struct
{
    db_vector4 p0;
    db_vector4 p1; // control
    db_vector4 p2;
} curve;

db_array_decl(curves, curve);

#define NUMBER_OF_BANDS 8
#define MAX_GLYPHS 4096
// glyph table is indexed directly by ASCII code. Control codes (0-31, 127) stay empty on purpose
#define TEXT_GLYPH_COUNT 128
// a tab advances to the next multiple of this many spaces
#define TEXT_TAB_WIDTH 4

typedef struct
{
    db_matrix4 translation_matrix;
    db_vector2 curves_indicies;
    db_aabb2   aabb;
    f32        advance; // em
    // first 8 is the start and end for the horizontal
    db_vector2 bands_loc[NUMBER_OF_BANDS * 2];
} glyph_data;

typedef struct
{
    f32             funit_to_em; // usually 2048
    f32             ascent;
    f32             descent;
    f32             line_gap;
    db_array_curves curves;
    db_array_s32    horizontal_bands;
    db_array_s32    vertical_bands;
    glyph_data      data[TEXT_GLYPH_COUNT]; // all of ascii
    const char     *font_name;
} glyphs;

// vec4s written per glyph by text_prepare_render_buffer: pen + curve range, then the 4 matrix columns
#define TEXT_VEC4S_PER_GLYPH 13

void       text_init(db_arena *arena);
void       text_deinit();
glyphs    *text_load_font(db_arena *arena, const char *file_path);
void       text_prepare_render_buffer(db_string *string, db_array_vector4 *buffer, f32 font_size, f32 wrap_width);
db_vector2 text_calculate_size(const char *string, s_size length, f32 font_size);
void       text_render(f32 font_size);

// internal
void __text_upload_to_gpu(glyphs *g);
void __text_update_gpu_buffer(void *data, s_size size);
