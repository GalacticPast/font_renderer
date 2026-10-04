#include "text.h"
#include "../vendor/stb/stb_truetype.h"
#include "db_math.h"
#include "gl.h"

#define FONT_SIZE 300

void text_center_glyphs(glyphs *g);

typedef struct
{
    SSBO   gpu_data;
    glyphs glyphs; // for now there's only one
} text_state;

text_state *state;

void text_load_font(db_arena *arena, const char *file_path)
{
    db_file_contents font_content = db_file_read_contents(arena, db_file_mode_rb, 0, file_path);
    ASSERT_WITH_MSG(font_content.size, "couldnt read the font.");

    stbtt_fontinfo font_info = {};
    b8             res       = stbtt_InitFont(&font_info, font_content.data, 0);
    ASSERT_WITH_MSG(res, "font loeading failed");

    state = db_arena_alloc(arena, sizeof(text_state));

    glyphs glyphs = {};
    glyphs.curves = db_array_curves_init(arena);

    for (s32 i = 33; i < 127; i++)
    {
        s32 glyph_index = stbtt_FindGlyphIndex(&font_info, (s32)('!' + i));

        stbtt_vertex *vertices      = NULL;
        b32           vertices_size = stbtt_GetGlyphShape(&font_info, glyph_index, &vertices);

        glyph_data *g_data = &glyphs.data[i];

        g_data->em_to_px_scale     = stbtt_ScaleForMappingEmToPixels(&font_info, FONT_SIZE);
        g_data->curves_start_index = glyphs.curves.length;
        db_vector4 curr_point      = db_vector4_zero();

        for (int i = 0; i < vertices_size; i++)
        {
            switch (vertices[i].type)
            {
                case STBTT_vmove: {
                    curr_point.x = vertices[i].x;
                    curr_point.y = vertices[i].y;
                }
                break;
                case STBTT_vline: {
                    db_vector4 p_2 = db_vector4_make(vertices[i].x, vertices[i].y, 0.0, 0.0);
                    db_vector4 control_p =
                        db_vector4_make((p_2.x + curr_point.x) * 0.5f, (p_2.y + curr_point.y) * 0.5f, 0.0, 0.0);
                    curve c = {.p0 = curr_point, .p1 = control_p, .p2 = p_2};
                    db_array_curves_append(&glyphs.curves, c);
                    curr_point = p_2;
                }
                break;
                case STBTT_vcurve: {
                    db_vector4 p_2       = db_vector4_make(vertices[i].x, vertices[i].y, 0.0, 0.0);
                    db_vector4 control_p = db_vector4_make(vertices[i].cx, vertices[i].cy, 0.0, 0.0);
                    curve      c         = {.p0 = curr_point, .p1 = control_p, .p2 = p_2};
                    db_array_curves_append(&glyphs.curves, c);
                    curr_point = p_2;
                }
                break;
                case STBTT_vcubic: {
                }
                break;
            }
        }
        g_data->curves_end_index = glyphs.curves.length;
        stbtt_FreeShape(&font_info, vertices);
    }
    // I might not need this though
    text_center_glyphs(&glyphs);
    // upload to gpu.
    ssbo_create(&(state->gpu_data), 1, glyphs.curves.data, sizeof(curve), glyphs.curves.length);
    ssbo_bind(&(state->gpu_data));

    return;
}

void text_center_glyphs(glyphs *g)
{
    for (s32 j = 0; j < 26; j++)
    {
        glyph_data *b   = &g->data[j];
        db_vector4  min = db_vector4_make(DB_MATH_F32_MAX, DB_MATH_F32_MAX, 0.0, 0.0);
        db_vector4  max = db_vector4_make(DB_MATH_F32_MIN, DB_MATH_F32_MIN, 0.0, 0.0);

        for (s32 i = b->curves_start_index; i < b->curves_end_index; i++)
        {
            db_vector4 *p0 = &g->curves.data[i].p0;
            db_vector4 *p1 = &g->curves.data[i].p1;
            db_vector4 *p2 = &g->curves.data[i].p2;

            db_vector4 p0_px = db_vector4_multiply(*p0, b->em_to_px_scale);
            db_vector4 p1_px = db_vector4_multiply(*p1, b->em_to_px_scale);
            db_vector4 p2_px = db_vector4_multiply(*p2, b->em_to_px_scale);

            min.x = db_min(db_min3(p0_px.x, p1_px.x, p2_px.x), min.x);
            min.y = db_min(db_min3(p0_px.y, p1_px.y, p2_px.y), min.y);

            max.x = db_max(db_max3(p0_px.x, p1_px.x, p2_px.x), max.x);
            max.y = db_max(db_max3(p0_px.y, p1_px.y, p2_px.y), max.y);
        }
        db_vector4 center = db_vector4_multiply(db_vector4_add(min, max), 0.5f);
        b->half_extent    = db_vector4_multiply(db_vector4_subtract(max, min), 0.5f);

        // center it
        for (s32 i = b->curves_start_index; i < b->curves_end_index; i++)
        {
            db_vector4 *p0 = &g->curves.data[i].p0;
            db_vector4 *p1 = &g->curves.data[i].p1;
            db_vector4 *p2 = &g->curves.data[i].p2;

            db_vector4 p0_px = db_vector4_multiply(*p0, b->em_to_px_scale);
            db_vector4 p1_px = db_vector4_multiply(*p1, b->em_to_px_scale);
            db_vector4 p2_px = db_vector4_multiply(*p2, b->em_to_px_scale);

            *p0 = db_vector4_subtract(p0_px, center);
            *p1 = db_vector4_subtract(p1_px, center);
            *p2 = db_vector4_subtract(p2_px, center);
        }
    }
}
