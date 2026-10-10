#define STB_TRUETYPE_IMPLEMENTATION

#include "text.h"
#include "../vendor/stb/stb_truetype.h"
#include "db_math.h"
#include "gl.h"

void text_compute_glyph_bbox(glyphs *g);

typedef struct
{
    glyphs glyphs; // for now there's only one
} text_state;

text_state *t_state;

static f32 text_line_height()
{
    return t_state->glyphs.ascent - t_state->glyphs.descent + t_state->glyphs.line_gap;
}

// bytes outside ASCII (e.g. UTF-8 lead bytes, negative as signed char) fall back to '?'
static glyph_data *text_get_glyph(char c)
{
    u8 code = (u8)c;
    if (code >= TEXT_GLYPH_COUNT)
        code = '?';
    return &t_state->glyphs.data[code];
}

static f32 text_next_tab_stop(f32 pen_x, f32 font_size)
{
    f32 tab = TEXT_TAB_WIDTH * t_state->glyphs.data[' '].advance * font_size;
    return (db_floor(pen_x / tab) + 1.0f) * tab;
}

// no wrapping: only '\n' starts a new line
db_vector2 text_calculate_size(const char *string, s_size length, f32 font_size)
{
    ASSERT_WITH_MSG(string, "String is empty");

    f32 line_width = 0.0f;
    f32 max_width  = 0.0f;
    s32 line_count = 1;

    for (s64 i = 0; i < (s64)length; i++)
    {
        char c = string[i];
        if (c == '\n')
        {
            max_width  = db_max(max_width, line_width);
            line_width = 0.0f;
            line_count++;
            continue;
        }
        if (c == '\t')
        {
            line_width = text_next_tab_stop(line_width, font_size);
            continue;
        }
        if ((u8)c < ' ' || c == 127)
            continue;

        line_width += text_get_glyph(c)->advance * font_size;
    }
    max_width = db_max(max_width, line_width);

    return db_vector2_make(max_width, line_count * text_line_height() * font_size);
}

void text_prepare_render_buffer(db_string *string, db_array_vector4 *buffer, f32 font_size, f32 wrap_width)
{
    db_vector2 pen_pos = db_vector2_make(0.0f, t_state->glyphs.ascent * font_size);
    for (s32 i = 0; i < string->length; i++)
    {
        char c = string->data[i];
        if (c == '\n')
        {
            pen_pos.x  = 0.0f;
            pen_pos.y += text_line_height() * font_size;
            continue;
        }
        if (c == '\t')
        {
            pen_pos.x = text_next_tab_stop(pen_pos.x, font_size);
            continue;
        }
        // '\r' and every other control code: no glyph, no advance
        if ((u8)c < ' ' || c == 127)
            continue;

        glyph_data *g = text_get_glyph(c);
        if (pen_pos.x + g->advance * font_size > wrap_width)
        {
            pen_pos.x  = 0.0f;
            pen_pos.y += text_line_height() * font_size;
        }

        // pen goes to em here, so only the pen is scaled. The matrix is already in em
        db_vector4 v =
            db_vector4_make(pen_pos.x / font_size, pen_pos.y / font_size, g->curves_indicies.x, g->curves_indicies.y);
        db_vector4 fr_row = db_vector4_make(g->translation_matrix.data[0], g->translation_matrix.data[1],
                                            g->translation_matrix.data[2], g->translation_matrix.data[3]);
        db_vector4 s_row  = db_vector4_make(g->translation_matrix.data[4], g->translation_matrix.data[5],
                                            g->translation_matrix.data[6], g->translation_matrix.data[7]);
        db_vector4 t_row  = db_vector4_make(g->translation_matrix.data[8], g->translation_matrix.data[9],
                                            g->translation_matrix.data[10], g->translation_matrix.data[11]);
        db_vector4 fo_row = db_vector4_make(g->translation_matrix.data[12], g->translation_matrix.data[13],
                                            g->translation_matrix.data[14], g->translation_matrix.data[15]);

        db_array_vector4_append(buffer, v);
        db_array_vector4_append(buffer, fr_row);
        db_array_vector4_append(buffer, s_row);
        db_array_vector4_append(buffer, t_row);
        db_array_vector4_append(buffer, fo_row);

        for (s32 j = 0; j < NUMBER_OF_BANDS * 2; j += 2)
        {
            db_vector4 bands_ind =
                db_vector4_make(g->bands_loc[j].x, g->bands_loc[j].y, g->bands_loc[j + 1].x, g->bands_loc[j + 1].y);
            db_array_vector4_append(buffer, bands_ind);
        }

        pen_pos.x += g->advance * font_size;
    }
    __text_update_gpu_buffer(buffer->data, buffer->length * buffer->type_size);
}

glyphs *text_load_font(db_arena *arena, const char *file_path)
{
    db_file_contents font_content = db_file_read_contents(arena, db_file_mode_rb, 0, file_path);
    ASSERT_WITH_MSG(font_content.size, "couldnt read the font.");

    stbtt_fontinfo font_info = {};
    b8             res       = stbtt_InitFont(&font_info, font_content.data, 0);
    ASSERT_WITH_MSG(res, "font loeading failed");

    t_state = db_arena_alloc(arena, sizeof(text_state));

    glyphs glyphs           = {};
    glyphs.curves           = db_array_curves_init(arena);
    glyphs.horizontal_bands = db_array_s32_init(arena);
    glyphs.vertical_bands   = db_array_s32_init(arena);
    glyphs.funit_to_em      = stbtt_ScaleForMappingEmToPixels(&font_info, 1.0);

    s32 ascent   = 0;
    s32 descent  = 0;
    s32 line_gap = 0;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
    glyphs.ascent   = ascent * glyphs.funit_to_em;
    glyphs.descent  = descent * glyphs.funit_to_em;
    glyphs.line_gap = line_gap * glyphs.funit_to_em;

    for (s32 i = 0; i < TEXT_GLYPH_COUNT; i++)
    {
        // control codes map to .notdef (a box) in most fonts; leave them empty instead
        if (i < ' ' || i == 127)
            continue;

        s32         glyph_index = stbtt_FindGlyphIndex(&font_info, (s32)i);
        glyph_data *g_data      = &glyphs.data[i];

        // the advance is needed even when there is no outline (space)
        s32 advance_funits    = 0;
        s32 left_side_bearing = 0;
        stbtt_GetGlyphHMetrics(&font_info, glyph_index, &advance_funits, &left_side_bearing);
        g_data->advance = advance_funits * glyphs.funit_to_em;

        stbtt_vertex *vertices      = NULL;
        b32           vertices_size = stbtt_GetGlyphShape(&font_info, glyph_index, &vertices);

        g_data->curves_indicies.x = glyphs.curves.length;
        db_vector4 curr_point     = db_vector4_zero();

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
        g_data->curves_indicies.y = glyphs.curves.length;

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

    __text_upload_to_gpu(&glyphs);

    t_state->glyphs = glyphs;

    return &t_state->glyphs;
}

static f32 solve_bezier(f32 a, f32 b, f32 c, f32 t)
{
    // (1 - t)^2 a, + 2 * t * ( 1 - t) * b + t * t * c
    f32 ans = (1 - t) * (1 - t) * a + 2 * t * (1 - t) * b + t * t * c;
    return ans;
}

b8 compare_s32(s32 *a, s32 *b)
{
    return (*a > *b) - (*a < *b); // -1, 0, or 1
}

void text_compute_glyph_bbox(glyphs *g)
{
    for (s32 i = 0; i < TEXT_GLYPH_COUNT; i++)
    {
        db_vector2 min = db_vector2_make(DB_MATH_F32_MAX, DB_MATH_F32_MAX);
        db_vector2 max = db_vector2_make(DB_MATH_F32_MIN, DB_MATH_F32_MIN);

        s32 start = g->data[i].curves_indicies.x;
        s32 end   = g->data[i].curves_indicies.y;

        if (start == end)
        {
            // g->data[i].aabb.center     = db_vector2_zero();
            // g->data[i].aabb.half_size  = db_vector2_zero();
            // g->data[i].curves_indicies = db_vector2_zero();
            // db_matrix4_identity(&g->data[i].translation_matrix);
            continue;
        }

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
        db_matrix4_identity(&g->data[i].translation_matrix);

        db_vector3 glyph_scale  = db_vector3_make(half_size.x * 2.0f, half_size.y * 2.0f, 1.0f);
        db_vector3 glyph_center = db_vector3_make(center.x, center.y, 0.0f);

        db_matrix4 scale_matrix;
        db_matrix4_scale(&scale_matrix, glyph_scale);

        db_matrix4 translate_matrix;
        db_matrix4_translate(&translate_matrix, glyph_center);

        db_matrix4_mul(&g->data[i].translation_matrix, &translate_matrix, &scale_matrix);

        s32 band_count = NUMBER_OF_BANDS;
        f32 eps        = 1.0f / 1024.0f;

        // horizontal banding
        f32 h_band_height = (max.y - min.y) / ((f32)band_count);
        for (s32 b = 0; b < band_count; b++)
        {
            f32 band_min_y = min.y + (f32)b * h_band_height - eps;
            f32 band_max_y = min.y + (f32)(b + 1) * h_band_height + eps;

            g->data[i].bands_loc[b].x = g->horizontal_bands.length;
            for (int j = start; j < end; j++)
            {
                curve *c = &g->curves.data[j];
                // check if it is horizontal
                if (fabs(c->p0.y - c->p2.y) < 1e-6 && fabs(c->p1.y - (c->p0.y + c->p2.y) * 0.5) < 1e-6)
                {
                    continue;
                }
                // check if the curve overlaps the band
                if (db_min3(c->p0.y, c->p1.y, c->p2.y) <= band_max_y &&
                    db_max3(c->p0.y, c->p1.y, c->p2.y) >= band_min_y)
                {
                    db_array_s32_append(&g->horizontal_bands, j);
                }
            }
            g->data[i].bands_loc[b].y = g->horizontal_bands.length;
        }

        f32 v_band_height = (max.x - min.x) / ((f32)band_count);
        for (s32 b = 0; b < band_count; b++)
        {
            f32 band_min_x = min.x + (f32)b * v_band_height - eps;
            f32 band_max_x = min.x + (f32)(b + 1) * v_band_height + eps;

            g->data[i].bands_loc[b + band_count].x = g->vertical_bands.length;

            for (int j = start; j < end; j++)
            {
                curve *c = &g->curves.data[j];
                // check if it vertical
                if (fabs(c->p0.x - c->p2.x) < 1e-5 && fabs(c->p1.x - (c->p0.x + c->p2.x) * 0.5) < 1e-5)
                {
                    continue;
                }
                // check if the curve overlaps the band
                if (db_min3(c->p0.x, c->p1.x, c->p2.x) <= band_max_x &&
                    db_max3(c->p0.x, c->p1.x, c->p2.x) >= band_min_x)
                {
                    db_array_s32_append(&g->vertical_bands, j);
                }
            }
            g->data[i].bands_loc[b + band_count].y = g->vertical_bands.length;
        }
    }
}
