#include "gl.h"
#include "input.h"
#include "platform/platform.h"
#include "text.h"

#define ONE_TWENTIETH 0.05f

// logical size: what the compositor lays the window out with
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

// physical pixels per logical unit. Set from the compositor after startup
static f32 display_scale = 1.0f;

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
b8   update(db_arena *main_arena);
void render_text(db_string *string, f32 font_size);

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

    // the scale is known once the platform has talked to the compositor
    display_scale = platform_get_display_scale();
    glViewport(0, 0, (s32)(WINDOW_WIDTH * display_scale), (s32)(WINDOW_HEIGHT * display_scale));

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

    glyphs *glyphs = text_load_font(&main_arena, "../assets/font/Archivo-Regular.ttf");

    //@info:   temp
    f32       font_size = 8.0f * display_scale; // 12 logical px, in physical pixels
    db_string str       = db_string_make(
        &main_arena, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut "
                     "labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco "
                     "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in "
                     "voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat "
                     "non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

    db_array_vector4 txt_buffer = db_array_vector4_init(&main_arena);
    text_prepare_render_buffer(&str, &txt_buffer, font_size, WINDOW_WIDTH * display_scale);

    VAO  b_vao;
    VBO  b_vbo;
    EBO  b_ebo;
    SSBO b_ssbo;

    VBO instanced_vbo;

    vao_create(&b_vao);
    vao_bind(&b_vao);

    vbo_create(&b_vbo, vertices, sizeof(vertices));
    vao_link_vbo_attribs(&b_vao, &b_vbo, 0, 3, GL_FLOAT, 3 * sizeof(f32), (void *)0);

    vbo_create(&instanced_vbo, (GLfloat *)txt_buffer.data, txt_buffer.length * txt_buffer.type_size);
    vao_link_vbo_attribs(&b_vao, &instanced_vbo, 1, 2, GL_FLOAT, 20 * sizeof(f32), (void *)0);
    vao_link_vbo_attribs(&b_vao, &instanced_vbo, 2, 2, GL_FLOAT, 20 * sizeof(f32), (void *)(2 * sizeof(f32)));

    // mat4 takes locations 3..6, one vec4 column each
    vao_link_vbo_attribs(&b_vao, &instanced_vbo, 3, 4, GL_FLOAT, 20 * sizeof(f32), (void *)(4 * sizeof(f32)));
    vao_link_vbo_attribs(&b_vao, &instanced_vbo, 4, 4, GL_FLOAT, 20 * sizeof(f32), (void *)(8 * sizeof(f32)));
    vao_link_vbo_attribs(&b_vao, &instanced_vbo, 5, 4, GL_FLOAT, 20 * sizeof(f32), (void *)(12 * sizeof(f32)));
    vao_link_vbo_attribs(&b_vao, &instanced_vbo, 6, 4, GL_FLOAT, 20 * sizeof(f32), (void *)(16 * sizeof(f32)));
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

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
    s32 mod = '~' - '!';

    while (run)
    {
        input_update(0);
        if (!platform_pump_messages())
            break;
        run = update(&main_arena);
        if (input_is_key_down(KEY_D) && !input_was_key_down(KEY_D))
        {
            i++;
            i %= mod;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        shader_use(&shader);

        glUniform1f(glGetUniformLocation(shader.program, "font_px"), font_size);

        camera_set_matrix(&camera, &shader, 0.1f, 100.0f);

        vao_bind(&b_vao);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, txt_buffer.length / TEXT_VEC4S_PER_GLYPH);

        platform_swap_buffers();
    }
    shader_destroy(&shader);
    platform_shutdown();
}

void camera_set_matrix(camera *camera, shader *shader, f32 near_plane, f32 far_plane)
{
    u32 projection_loc = glGetUniformLocation(shader->program, "projection");
    u32 model_loc      = glGetUniformLocation(shader->program, "model");
    u32 view_loc       = glGetUniformLocation(shader->program, "view");

    db_matrix4 ortho;
    // the projection works in physical pixels, matching the buffer and font_px
    db_matrix4_ortho2d(&ortho, 0, (f32)WINDOW_WIDTH * display_scale, (f32)WINDOW_HEIGHT * display_scale, 0);

    db_matrix4 view;
    db_matrix4_identity(&view);

    db_matrix4 model;
    db_matrix4_identity(&model);

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

void render_text(db_string *string, f32 font_size)
{
}
