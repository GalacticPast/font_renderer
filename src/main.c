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
    f32              em_to_px_scale;
    db_array_s32     contours_start_indicies;
    db_array_vector2 contours;
} glyph_data;

typedef struct
{
    glyph_data data[26];
} glyphs;

void   camera_set_matrix(camera *camera, shader *shader, f32 near_plane, f32 far_plane);
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
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};
// clang-format on

db_vector2 bezier_solver(db_vector2 *p0, db_vector2 *p1, db_vector2 *p3, f32 t)
{
    f32        a      = (1.0 - t) * (1.0 - t);
    db_vector2 f_term = db_vector2_multiply(*p0, a);
    f32        b      = 2.0 * (1 - t) * t;
    db_vector2 s_term = db_vector2_multiply(*p1, b);
    f32        c      = t * t;
    db_vector2 t_term = db_vector2_multiply(*p3, c);

    db_vector2 ans = db_vector2_add(f_term, s_term);
    ans            = db_vector2_add(ans, t_term);
    return ans;
}

void gl_load_debug_glyph_outline(glyph_data *b, db_array_f32 *lines, db_array_s32 *contour_vertex_counts)
{
    printf("contours %ld\n", b->contours_start_indicies.length);
    db_vector2 min = db_vector2_make(DB_MATH_F32_MAX, DB_MATH_F32_MAX);
    db_vector2 max = db_vector2_make(DB_MATH_F32_MIN, DB_MATH_F32_MIN);

    s32 contour_count = b->contours_start_indicies.length;

    // Pass 1: bounding box across every contour, so every contour is centered
    // against the same glyph-wide origin instead of a running/partial one.
    for (s32 i = 0; i < b->contours.length; i += 3)
    {
        db_vector2 *p0 = &b->contours.data[i];
        db_vector2 *p1 = &b->contours.data[i + 1]; // control point
        db_vector2 *p2 = &b->contours.data[i + 2];

        db_vector2 p0_px = db_vector2_multiply(*p0, b->em_to_px_scale);
        db_vector2 p1_px = db_vector2_multiply(*p1, b->em_to_px_scale);
        db_vector2 p2_px = db_vector2_multiply(*p2, b->em_to_px_scale);

        min.x = db_min(db_min3(p0_px.x, p1_px.x, p2_px.x), min.x);
        min.y = db_min(db_min3(p0_px.y, p1_px.y, p2_px.y), min.y);

        max.x = db_max(db_max3(p0_px.x, p1_px.x, p2_px.x), max.x);
        max.y = db_max(db_max3(p0_px.y, p1_px.y, p2_px.y), max.y);
    }

    db_vector2 center = db_vector2_multiply(db_vector2_add(min, max), 0.5f);

    // Pass 2: tessellate each contour separately and record how many vertices
    // it produced, so each contour can be drawn as its own line strip instead
    // of one strip that stitches every contour together with stray edges.
    // for (s32 c = 0; c < contour_count; c++)
    // {
    //     s32 outline_start = b->contours_start_indicies.data[c];
    //     s32 outline_end   = (c + 1 < contour_count) ? b->contours_start_indicies.data[c + 1] : b->contours.length;
    //
    //     s32 contour_vertex_count = 0;
    //     for (s32 i = outline_start; i < outline_end; i += 3)
    //     {
    //         db_vector2 *p0 = &b->contours.data[i];
    //         db_vector2 *p1 = &b->contours.data[i + 1]; // control point
    //         db_vector2 *p2 = &b->contours.data[i + 2];
    //
    //         db_vector2 p0_px = db_vector2_multiply(*p0, b->em_to_px_scale);
    //         db_vector2 p1_px = db_vector2_multiply(*p1, b->em_to_px_scale);
    //         db_vector2 p2_px = db_vector2_multiply(*p2, b->em_to_px_scale);
    //
    //         p0_px = db_vector2_subtract(p0_px, center);
    //         p1_px = db_vector2_subtract(p1_px, center);
    //         p2_px = db_vector2_subtract(p2_px, center);
    //
    //         db_vector2 temp = {};
    //         for (s32 t = 0; t <= 20; t++)
    //         {
    //             temp = bezier_solver(&p0_px, &p1_px, &p2_px, ONE_TWENTIETH * t);
    //             db_array_f32_append(lines, temp.x);
    //             db_array_f32_append(lines, temp.y);
    //             contour_vertex_count++;
    //         }
    //     }
    //     db_array_s32_append(contour_vertex_counts, contour_vertex_count);
    // }
    //
    for (s32 c = 0; c < contour_count; c++)
    {
        s32 outline_start = b->contours_start_indicies.data[c];
        s32 outline_end   = (c + 1 < contour_count) ? b->contours_start_indicies.data[c + 1] : b->contours.length;

        s32 contour_edge = 0;
        for (s32 i = outline_start; i < outline_end; i += 3)
        {
            db_vector2 *p0 = &b->contours.data[i];
            db_vector2 *p1 = &b->contours.data[i + 1]; // control point
            db_vector2 *p2 = &b->contours.data[i + 2];

            db_vector2 p0_px = db_vector2_multiply(*p0, b->em_to_px_scale);
            db_vector2 p1_px = db_vector2_multiply(*p1, b->em_to_px_scale);
            db_vector2 p2_px = db_vector2_multiply(*p2, b->em_to_px_scale);

            p0_px = db_vector2_subtract(p0_px, center);
            p1_px = db_vector2_subtract(p1_px, center);
            p2_px = db_vector2_subtract(p2_px, center);

            for (s32 j = 0; j <= 20; j++)
            {
                db_array_f32_append(lines, p0_px.x);
                db_array_f32_append(lines, p0_px.y);
                db_array_f32_append(lines, p1_px.x);
                db_array_f32_append(lines, p1_px.y);
                db_array_f32_append(lines, p2_px.x);
                db_array_f32_append(lines, p2_px.y);
                db_array_f32_append(lines, ONE_TWENTIETH * j);
                contour_edge++;
            }
        }
        db_array_s32_append(contour_vertex_counts, contour_edge);
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

    // Isolated Loop-Blinn curve-fill test: one quadratic Bezier triangle,
    // rendered separately from the debug line outline above.
    struct shader curve_shader = {0};
    b8            b            = shader_create(&main_arena, &curve_shader, "../assets/shaders/curve_vertex.glsl",
                                               "../assets/shaders/curve_fragment.glsl");

    if (!b)
        return false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VAO vao;
    VBO vbo;
    EBO ebo;

    // vao_create(&vao);
    // vao_bind(&vao);
    //
    // vbo_create(&vbo, vertices, sizeof(vertices));
    // ebo_create(&ebo, indices, sizeof(indices));
    //
    // vao_link_vbo_attribs(&vao, &vbo, 0, 3, GL_FLOAT, 8 * sizeof(f32), (void *)0);
    // vao_link_vbo_attribs(&vao, &vbo, 1, 3, GL_FLOAT, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
    // vao_link_vbo_attribs(&vao, &vbo, 2, 2, GL_FLOAT, 8 * sizeof(f32), (void *)(6 * sizeof(f32)));
    //
    // vao_unbind();

    db_array_f32 b_vertices            = db_array_f32_init(&main_arena);
    db_array_s32 contour_vertex_counts = db_array_s32_init(&main_arena);

    glyphs glyphs = load_font(&main_arena);

    VAO b_vao;
    VBO b_vbo;

    vao_create(&b_vao);
    vao_bind(&b_vao);

    gl_load_debug_glyph_outline(&glyphs.data[0], &b_vertices, &contour_vertex_counts);
    vbo_create(&b_vbo, b_vertices.data, b_vertices.length * sizeof(f32));
    vao_link_vbo_attribs(&b_vao, &b_vbo, 0, 2, GL_FLOAT, 7 * sizeof(f32), (void *)0);
    vao_link_vbo_attribs(&b_vao, &b_vbo, 1, 2, GL_FLOAT, 7 * sizeof(f32), (void *)(2 * sizeof(f32)));
    vao_link_vbo_attribs(&b_vao, &b_vbo, 2, 2, GL_FLOAT, 7 * sizeof(f32), (void *)(4 * sizeof(f32)));
    vao_link_vbo_attribs(&b_vao, &b_vbo, 3, 1, GL_FLOAT, 7 * sizeof(f32), (void *)(6 * sizeof(f32)));

    // clang-format off
    // One hardcoded quadratic curve triangle: p0, control, p2, each with the
    // Loop-Blinn uv (p0 -> (0,0), control -> (0.5,0), p2 -> (1,1)).
    f32 curve_triangle[] =
    {
        // position         uv
        -150.0f, -100.0f,   0.0f, 0.0f, // p0
           0.0f,  100.0f,   0.5f, 0.0f, // control
         150.0f, -100.0f,   1.0f, 1.0f, // p2
    };
    // clang-format on

    VAO curve_vao;
    VBO curve_vbo;

    vao_create(&curve_vao);
    vao_bind(&curve_vao);
    vbo_create(&curve_vbo, curve_triangle, sizeof(curve_triangle));
    vao_link_vbo_attribs(&curve_vao, &curve_vbo, 0, 2, GL_FLOAT, 4 * sizeof(f32), (void *)0);
    vao_link_vbo_attribs(&curve_vao, &curve_vbo, 1, 2, GL_FLOAT, 4 * sizeof(f32), (void *)(2 * sizeof(f32)));

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
            vbo_delete(&b_vbo);
            db_array_f32_clear(&b_vertices);
            db_array_s32_clear(&contour_vertex_counts);

            i++;
            i %= 26;
            gl_load_debug_glyph_outline(&glyphs.data[i], &b_vertices, &contour_vertex_counts);

            vbo_create(&b_vbo, b_vertices.data, b_vertices.length * sizeof(f32));
            vao_link_vbo_attribs(&b_vao, &b_vbo, 0, 2, GL_FLOAT, 7 * sizeof(f32), (void *)0);
            vao_link_vbo_attribs(&b_vao, &b_vbo, 1, 2, GL_FLOAT, 7 * sizeof(f32), (void *)(2 * sizeof(f32)));
            vao_link_vbo_attribs(&b_vao, &b_vbo, 2, 2, GL_FLOAT, 7 * sizeof(f32), (void *)(4 * sizeof(f32)));
            vao_link_vbo_attribs(&b_vao, &b_vbo, 3, 1, GL_FLOAT, 7 * sizeof(f32), (void *)(6 * sizeof(f32)));
        }

        glClearColor(0.1f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader_use(&shader);

        camera_set_matrix(&camera, &shader, 0.1f, 100.0f);

        vao_bind(&b_vao);

        s32 offset = 0;
        for (s32 i = 0; i < contour_vertex_counts.length; i++)
        {
            glDrawArrays(GL_LINE_STRIP, offset, contour_vertex_counts.data[i]);
            offset += contour_vertex_counts.data[i];
        }

        shader_use(&curve_shader);
        camera_set_matrix(&camera, &curve_shader, 0.1f, 100.0f);
        vao_bind(&curve_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        platform_swap_buffers();
    }
    shader_destroy(&shader);
    shader_destroy(&curve_shader);
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

void camera_set_matrix(camera *camera, shader *shader, f32 near_plane, f32 far_plane)
{

    db_matrix4 ortho;
    db_matrix4_ortho2d(&ortho, -(f32)WINDOW_WIDTH / 2.0f, (f32)WINDOW_WIDTH / 2.0f, -(f32)WINDOW_HEIGHT / 2.0f,
                       (f32)WINDOW_HEIGHT / 2.0f);

    u32 projection_loc = glGetUniformLocation(shader->program, "projection");

    db_matrix4 view;
    db_matrix4_identity(&view);
    u32 view_loc = glGetUniformLocation(shader->program, "view");

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.data);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, ortho.data);
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

    for (s32 i = 0; i < 26; i++)
    {
        s32 glyph_index = stbtt_FindGlyphIndex(&font_info, (s32)('A' + i));

        stbtt_vertex *vertices      = NULL;
        b32           vertices_size = stbtt_GetGlyphShape(&font_info, glyph_index, &vertices);

        glyph_data *g_data = &glyphs.data[i];

        g_data->contours                = db_array_vector2_init(arena);
        g_data->contours_start_indicies = db_array_s32_init(arena);
        g_data->em_to_px_scale          = stbtt_ScaleForMappingEmToPixels(&font_info, FONT_SIZE);

        db_vector2 curr_point = db_vector2_zero();

        for (int i = 0; i < vertices_size; i++)
        {
            switch (vertices[i].type)
            {
                case STBTT_vmove: {
                    db_array_s32_append(&g_data->contours_start_indicies, db_array_vector2_length(&g_data->contours));
                    curr_point.x = vertices[i].x;
                    curr_point.y = vertices[i].y;
                }
                break;
                case STBTT_vline: {
                    db_vector2 p2  = db_vector2_make(vertices[i].x, vertices[i].y);
                    db_vector2 mid = db_vector2_make((p2.x + curr_point.x) * 0.5f, (p2.y + curr_point.y) * 0.5f);
                    db_array_vector2_append(&g_data->contours, curr_point);
                    db_array_vector2_append(&g_data->contours, mid);
                    db_array_vector2_append(&g_data->contours, p2);
                    curr_point = p2;
                }
                break;
                case STBTT_vcurve: {
                    db_vector2 p2        = db_vector2_make(vertices[i].x, vertices[i].y);
                    db_vector2 control_p = db_vector2_make(vertices[i].cx, vertices[i].cy);
                    db_array_vector2_append(&g_data->contours, curr_point);
                    db_array_vector2_append(&g_data->contours, control_p);
                    db_array_vector2_append(&g_data->contours, p2);
                    curr_point = p2;
                }
                break;
                case STBTT_vcubic: {
                }
                break;
            }
        }
        stbtt_FreeShape(&font_info, vertices);
    }

    return glyphs;
}
