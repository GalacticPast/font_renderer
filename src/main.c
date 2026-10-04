#include "gl.h"
#include "input.h"
#include "platform/platform.h"
#include "text.h"

#define ONE_TWENTIETH 0.05f

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

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

void camera_set_matrix(camera *camera, shader *shader, db_vector3 glyph_scale, f32 near_plane, f32 far_plane);
b8   update(db_arena *main_arena);

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

    text_load_font(&main_arena, "../assets/font/Archivo-Regular.ttf");

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

    // glyph edges now output fractional alpha for anti-aliasing; without
    // blending enabled that alpha is ignored and edges stay hard.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
