#define DB_IMPLEMENTATION
#define DB_MATH_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "../vendor/stb/stb_truetype.h"
#include "db.h"
#include "db_math.h"
#include "gl.h"
#include "input.h"
#include "platform/platform.h"

#define ONE_TWENTIETH 0.05f

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define FONT_SIZE 300

typedef struct camera
{
    db_vector3 position;
    db_vector3 orientation;
    db_vector3 up;

    f32 speed;
    f32 sensitivity;
    f32 yaw;
    f32 pitch;
    f32 fov;
} camera;

db_array_decl(vector2, db_vector2);
db_array_decl(s32, s32);
db_array_decl(f32, f32);

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
    s32        index;
    s32        curves_start_index;
    s32        curves_end_index;
    db_vector4 half_extent;
} glyph_data;

typedef struct
{
    glyph_data      data[26];
    db_array_curves curves;
} glyphs;

void   camera_set_matrix(camera *camera, shader *shader, db_vector3 glyph_scale, f32 near_plane, f32 far_plane);
b8     update(db_arena *main_arena);
glyphs load_font(db_arena *arena);

static inline db_matrix4 mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip);
static inline db_matrix4 mat4_look_at(db_vector3 position, db_vector3 target, db_vector3 up);

b8 opengl_startup(db_arena *main_arena)
{
    b8 success = platform_startup(main_arena, "slug", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!success)
        return false;
    input_initialize(main_arena);
    return true;
}

// clang-format off
f32 vertices[] =
{ //     COORDINATES
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
};
u32 indices[] =
{
	0, 1, 2,
	0, 2, 3,
};
// clang-format on

void center_glyphs(glyphs *g)
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

int main()
{
    // startup the opengl context;
    db_arena_params params     = {.type = TYPE_ARENA_LINEAR, .chunk_size = 0};
    db_arena        main_arena = db_arena_init_with_size(&params, MB(10));
    b8              success    = opengl_startup(&main_arena);
    if (!success)
        return 0;

    camera camera      = {0};
    camera.position    = db_vector3_make(0.0f, 0.0f, 3.0f);
    camera.orientation = db_vector3_make(0.0f, 0.0f, -1.0f);
    camera.up          = db_vector3_make(0.0f, 1.0f, 0.0f);
    camera.yaw         = -90.0f;
    camera.pitch       = 0.0f;
    camera.fov         = 45.0f;
    camera.sensitivity = 0.1f; // change this value to your liking

    shader shader = {0};
    b8     a = shader_create(&main_arena, &shader, "../assets/shaders/vertex.glsl", "../assets/shaders/fragment.glsl");

    if (!a)
        return false;

    glyphs glyphs = load_font(&main_arena);
    center_glyphs(&glyphs);

    // @debug: temporary - dump 'Q' curve data to find the horizontal-band bug. DELETE after fixed.
    {
        const char *dbg_letters[] = {"M", "J"};
        for (s32 li = 0; li < 2; li++)
        {
            glyph_data *dbg = &glyphs.data[dbg_letters[li][0] - 'A'];
            printf("[DEBUG GLYPH] %s\n", dbg_letters[li]);
            for (s32 ci = dbg->curves_start_index; ci < dbg->curves_end_index; ci++)
            {
                curve *c = &glyphs.curves.data[ci];
                printf("[DEBUG CURVE] %d: p0=(%.3f, %.3f) p1=(%.3f, %.3f) p2=(%.3f, %.3f)\n", ci, c->p0.x, c->p0.y,
                       c->p1.x, c->p1.y, c->p2.x, c->p2.y);
            }
        }
    }

    VAO  b_vao;
    VBO  b_vbo;
    EBO  b_ebo;
    SSBO b_ssbo;

    vao_create(&b_vao);
    vao_bind(&b_vao);

    vbo_create(&b_vbo, vertices, sizeof(vertices));
    vao_link_vbo_attribs(&b_vao, &b_vbo, 0, 3, GL_FLOAT, 3 * sizeof(f32), (void *)0);

    ebo_create(&b_ebo, indices, sizeof(indices));
    ebo_bind(&b_ebo);
    // for now render A

    vao_unbind();

    ssbo_create(&b_ssbo, 1, glyphs.curves.data, sizeof(curve), glyphs.curves.length);
    ssbo_bind(&b_ssbo);

    b8  run = true;
    s32 i   = 0;
    while (run)
    {
        input_update(0);
        if (!platform_pump_messages())
            break;
        run = update(&main_arena);
        if (input_is_key_down(KEY_D) && !input_was_key_down(KEY_D))
        {
            i++;
            i %= 26;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader_use(&shader);

        // quad vertices span [-0.5, 0.5], so scaling by 2x the half-extent
        // makes the quad's world-space half-width/half-height match the glyph's.
        db_vector4 half_extent = glyphs.data[i].half_extent;
        db_vector3 glyph_scale = db_vector3_make(half_extent.x * 2.0f, half_extent.y * 2.0f, 1.0f);

        camera_set_matrix(&camera, &shader, glyph_scale, 0.1f, 100.0f);

        // uniform vec2 curve_indicies;
        u32        curve_loc = glGetUniformLocation(shader.program, "curve_indicies");
        db_vector2 indicies  = db_vector2_make(glyphs.data[i].curves_start_index, glyphs.data[i].curves_end_index);
        glUniform2fv(curve_loc, 1, indicies.data);

        vao_bind(&b_vao);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        platform_swap_buffers();
    }
    shader_destroy(&shader);
    platform_shutdown();
}

static inline db_matrix4 mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip)
{
    f32        half_tan_fov = tanf(fov_radians * 0.5f);
    db_matrix4 out_matrix   = {0};
    out_matrix.data[0]      = 1.0f / (aspect_ratio * half_tan_fov);
    out_matrix.data[5]      = 1.0f / half_tan_fov;
    out_matrix.data[10]     = -((far_clip + near_clip) / (far_clip - near_clip));
    out_matrix.data[11]     = -1.0f;
    out_matrix.data[14]     = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
    return out_matrix;
}

static inline db_matrix4 mat4_look_at(db_vector3 position, db_vector3 target, db_vector3 up)
{
    db_matrix4 out_matrix;
    db_vector3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis            = db_vector3_normalize(z_axis);
    db_vector3 x_axis = db_vector3_normalize(db_vector3_cross(z_axis, up));
    db_vector3 y_axis = db_vector3_cross(x_axis, z_axis);

    out_matrix.data[0]  = x_axis.x;
    out_matrix.data[1]  = y_axis.x;
    out_matrix.data[2]  = -z_axis.x;
    out_matrix.data[3]  = 0;
    out_matrix.data[4]  = x_axis.y;
    out_matrix.data[5]  = y_axis.y;
    out_matrix.data[6]  = -z_axis.y;
    out_matrix.data[7]  = 0;
    out_matrix.data[8]  = x_axis.z;
    out_matrix.data[9]  = y_axis.z;
    out_matrix.data[10] = -z_axis.z;
    out_matrix.data[11] = 0;
    out_matrix.data[12] = -db_vector3_dot_multiplication(x_axis, position);
    out_matrix.data[13] = -db_vector3_dot_multiplication(y_axis, position);
    out_matrix.data[14] = db_vector3_dot_multiplication(z_axis, position);
    out_matrix.data[15] = 1.0f;

    return out_matrix;
}

void camera_set_matrix(camera *camera, shader *shader, db_vector3 glyph_scale, f32 near_plane, f32 far_plane)
{
    u32 projection_loc = glGetUniformLocation(shader->program, "projection");
    u32 model_loc      = glGetUniformLocation(shader->program, "model");
    u32 view_loc       = glGetUniformLocation(shader->program, "view");

    db_matrix4 ortho;
    db_matrix4_ortho2d(&ortho, -(f32)WINDOW_WIDTH / 2.0f, (f32)WINDOW_WIDTH / 2.0f, -(f32)WINDOW_HEIGHT / 2.0f,
                       (f32)WINDOW_HEIGHT / 2.0f);

    db_matrix4 view;
    db_matrix4_identity(&view);

    db_matrix4 model;
    db_matrix4_identity(&model);
    db_matrix4_scale(&model, glyph_scale);

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.data);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, ortho.data);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model.data);
}
//@note:
// this is temperory
b8 update(db_arena *main_arena)
{
    if (input_was_key_down(KEY_ESCAPE))
    {
        return false;
    }
    return true;
}

glyphs load_font(db_arena *arena)
{
    db_file_contents font_content =
        db_file_read_contents(arena, db_file_mode_rb, 0, "../assets/font/Archivo-Regular.ttf");
    ASSERT_WITH_MSG(font_content.size, "couldnt read the font.");

    stbtt_fontinfo font_info = {};
    b8             res       = stbtt_InitFont(&font_info, font_content.data, 0);
    ASSERT_WITH_MSG(res, "font loeading failed");

    // Just checking "B"

    glyphs glyphs = {};
    glyphs.curves = db_array_curves_init(arena);

    for (s32 i = 0; i < 26; i++)
    {
        s32 glyph_index = stbtt_FindGlyphIndex(&font_info, (s32)('A' + i));

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
    return glyphs;
}
