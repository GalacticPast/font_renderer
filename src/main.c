#define DB_IMPLEMENTATION
#define DB_MATH_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "../vendor/stb/stb_truetype.h"
#include "db.h"
#include "db_math.h"
#include "gl.h"
#include "input.h"
#include "platform/platform.h"

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

void camera_set_matrix(camera *camera, shader *shader, f32 near_plane, f32 far_plane);
void update(db_arena *main_arena);
void load_font(db_arena *arena);

static inline db_matrix4 mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip);
static inline db_matrix4 mat4_look_at(db_vector3 position, db_vector3 target, db_vector3 up);

b8 opengl_startup(db_arena *main_arena)
{
    b8 success = platform_startup(main_arena, "slug", 0, 0, 600, 400);
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

int main()
{
    // startup the opengl context;
    db_arena_params params     = {.type = TYPE_ARENA_LINEAR, .chunk_size = 0};
    db_arena        main_arena = db_arena_init_with_size(&params, MB(10));
    b8              success    = opengl_startup(&main_arena);
    if (!success)
        return 0;

    camera camera      = {};
    camera.position    = db_vector3_make(0.0f, 0.0f, 3.0f);
    camera.orientation = db_vector3_make(0.0f, 0.0f, -1.0f);
    camera.up          = db_vector3_make(0.0f, 1.0f, 0.0f);
    camera.yaw         = -90.0f;
    camera.pitch       = 0.0f;
    camera.fov         = 45.0f;
    camera.sensitivity = 0.1f; // change this value to your liking

    shader shader = {};
    b8     a      = shader_create(&main_arena, &shader);

    if (!a)
        return false;

    VAO vao;
    VBO vbo;
    EBO ebo;

    vao_bind(&vao);

    vbo_create(&vbo, vertices, sizeof(vertices));
    ebo_create(&ebo, indices, sizeof(indices));

    vao_link_vbo_attribs(&vao, &vbo, 0, 3, GL_FLOAT, 8 * sizeof(f32), (void *)0);
    vao_link_vbo_attribs(&vao, &vbo, 1, 3, GL_FLOAT, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
    vao_link_vbo_attribs(&vao, &vbo, 2, 2, GL_FLOAT, 8 * sizeof(f32), (void *)(6 * sizeof(f32)));

    vao_unbind();

    load_font(&main_arena);

    while (true)
    {
        platform_pump_messages();
        update(&main_arena);

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader_use(&shader);

        camera_set_matrix(&camera, &shader, 0.1f, 100.0f);

        vao_bind(&vao);

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        platform_swap_buffers();
    }
    shader_destroy(&shader);
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

    u32 win_width, win_height;
    platform_get_window_dimensions(&win_width, &win_height);

    db_matrix4 projection =
        mat4_perspective(db_to_radians(camera->fov), (f32)win_width / (f32)win_height, 0.1f, 100.0f);
    u32 projection_loc = glGetUniformLocation(shader->program, "projection");

    db_matrix4 view = mat4_look_at(camera->position, db_vector3_add(camera->position, camera->orientation), camera->up);
    u32        view_loc = glGetUniformLocation(shader->program, "view");

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.data);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection.data);
}
//@note:
// this is temperory
void update(db_arena *main_arena)
{
}

typedef struct
{
    s_size size;
    f32   *vertices;
} glyph_data;

void load_font(db_arena *arena)
{
    db_file_contents font_content =
        db_file_read_contents(arena, db_file_mode_rb, 0, "../assets/font/Archivo-Regular.ttf");
    ASSERT_WITH_MSG(font_content.size, "couldnt read the font.");
    stbtt_fontinfo font_info = {};
    b8             res       = stbtt_InitFont(&font_info, font_content.data, 0);
    ASSERT_WITH_MSG(res, "font loeading failed");

    // Just checking "B"
    s32 glyph_index = stbtt_FindGlyphIndex(&font_info, (s32)'B');

    stbtt_vertex *vertices      = NULL;
    b32           vertices_size = stbtt_GetGlyphShape(&font_info, glyph_index, &vertices);

    glyph_data g_data = {};

    // for (int i = 0; i < vertices_size; i++)
    // {
    //     printf("type: %d, padding: %d, x: %d, y: %d, cx: %d, cy: %d, cx1: %d, cy1: %d \n", vertices[i].type,
    //            vertices[i].padding, vertices[i].x, vertices[i].y, vertices[i].cx, vertices[i].cy, vertices[i].cx1,
    //            vertices[i].cy1);
    // }
}
