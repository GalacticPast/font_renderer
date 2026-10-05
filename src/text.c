#define STB_TRUETYPE_IMPLEMENTATION

#include "text.h"
#include "../vendor/stb/stb_truetype.h"
#include "db_math.h"
#include "gl.h"

void text_compute_glyph_bbox(glyphs *g);

typedef struct
{
    SSBO   gpu_data;
    glyphs glyphs; // for now there's only one
} text_state;

text_state *state;

glyphs *text_load_font(db_arena *arena, const char *file_path)
{
    db_file_contents font_content = db_file_read_contents(arena, db_file_mode_rb, 0, file_path);
    ASSERT_WITH_MSG(font_content.size, "couldnt read the font.");

    stbtt_fontinfo font_info = {};
    b8             res       = stbtt_InitFont(&font_info, font_content.data, 0);
    ASSERT_WITH_MSG(res, "font loeading failed");

    state = db_arena_alloc(arena, sizeof(text_state));

    glyphs glyphs      = {};
    glyphs.curves      = db_array_curves_init(arena);
    glyphs.funit_to_em = stbtt_ScaleForMappingEmToPixels(&font_info, 1.0);

    for (s32 i = 0; i <= ('~' - '!'); i++)
    {
        s32 glyph_index = stbtt_FindGlyphIndex(&font_info, (s32)('!' + i));

        stbtt_vertex *vertices      = NULL;
        b32           vertices_size = stbtt_GetGlyphShape(&font_info, glyph_index, &vertices);

        glyph_data *g_data = &glyphs.data[i];

        s32 advance_funits    = 0;
        s32 left_side_bearing = 0;
        stbtt_GetGlyphHMetrics(&font_info, glyph_index, &advance_funits, &left_side_bearing);
        g_data->advance = advance_funits * glyphs.funit_to_em;

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

    // transform to em
    curve *itr = NULL;
    s64    i   = 0;

    f32 funit_to_em = glyphs.funit_to_em;
    db_array_for_each_ptr(glyphs.curves, i, itr)
    {
        itr->p0 = db_vector4_multiply(itr->p0, funit_to_em);
        itr->p1 = db_vector4_multiply(itr->p1, funit_to_em);
        itr->p2 = db_vector4_multiply(itr->p2, funit_to_em);
    }

    text_compute_glyph_bbox(&glyphs);

    // upload to gpu.
    ssbo_create(&(state->gpu_data), 1, glyphs.curves.data, sizeof(curve), glyphs.curves.length);
    ssbo_bind(&(state->gpu_data));

    state->glyphs = glyphs;

    return &state->glyphs;
}

static f32 solve_bezier(f32 a, f32 b, f32 c, f32 t)
{
    // (1 - t)^2 a, + 2 * t * ( 1 - t) * b + t * t * c
    f32 ans = (1 - t) * (1 - t) * a + 2 * t * (1 - t) * b + t * t * c;
    return ans;
}

void text_compute_glyph_bbox(glyphs *g)
{

    s32 len = g->curves.length;

    for (s32 i = 0; i < 94; i++)
    {
        db_vector2 min = db_vector2_make(DB_MATH_F32_MAX, DB_MATH_F32_MAX);
        db_vector2 max = db_vector2_make(DB_MATH_F32_MIN, DB_MATH_F32_MIN);

        s32 start = g->data[i].curves_start_index;
        s32 end   = g->data[i].curves_end_index;

        for (s32 j = start; j < end; j++)
        {
            curve *c = &g->curves.data[j];
            // p1 is the control point
            min.x    = db_min3(c->p0.x, c->p2.x, min.x);
            min.y    = db_min3(c->p0.y, c->p2.y, min.y);

            max.x = db_max3(c->p0.x, c->p2.x, max.x);
            max.y = db_max3(c->p0.y, c->p2.y, max.y);

            // Find internal x extremum
            f32 denom_x = c->p0.x - 2 * c->p1.x + c->p2.x;
            if (denom_x != 0.0)
            {
                f32 t_x = (c->p0.x - c->p1.x) / denom_x;
                if (t_x > 0.0 && t_x < 1.0)
                {
                    // solve the bezier at t_x
                    f32 c_x = solve_bezier(c->p0.x, c->p1.x, c->p2.x, t_x);
                    min.x   = db_min(c_x, min.x);
                    max.x   = db_max(c_x, max.x);
                }
            }

            // Find internal y extremum
            f32 denom_y = c->p0.y - 2 * c->p1.y + c->p2.y;
            if (denom_y != 0.0)
            {
                f32 t_y = (c->p0.y - c->p1.y) / denom_y;
                if (t_y > 0.0 && t_y < 1.0)
                {
                    // solve the bezier at t_y
                    f32 c_y = solve_bezier(c->p0.y, c->p1.y, c->p2.y, t_y);
                    min.y   = db_min(c_y, min.y);
                    max.y   = db_max(c_y, max.y);
                }
            }
        }
        db_vector2 center         = db_vector2_multiply(db_vector2_add(min, max), 0.5f);
        db_vector2 half_size      = db_vector2_multiply(db_vector2_subtract(max, min), 0.5f);
        g->data[i].aabb.center    = center;
        g->data[i].aabb.half_size = half_size;
    }
}
