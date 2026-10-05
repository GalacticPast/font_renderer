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

void camera_set_matrix(camera *camera, shader *shader, db_vector3 glyph_center, db_vector3 glyph_scale, f32 near_plane,
                       f32 far_plane);
b8   update(db_arena *main_arena);

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

    glyphs *glyphs = text_load_font(&main_arena, "../assets/font/Archivo-Regular.ttf");

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

    b8  run     = true;
    s32 i       = 0;
    s32 mod     = '~' - '!';
    f32 font_px = 40.0f;

    const char *string = "ABCDEFGHIJ";

    f32 pen_pos[2] = {0, 40}; // px from the top-left, baseline sits 100 px down

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

        glUniform1f(glGetUniformLocation(shader.program, "font_px"), font_px);

        // pen cursor for this frame, in px like the other UI positions. Converted to em only when uploaded
        db_vector2 pen = db_vector2_make(pen_pos[0], pen_pos[1]);

        for (s32 j = 0; j < 11; j++)
        {
            s32        c            = string[j] - '!';
            db_vector2 half_extent  = glyphs->data[c].aabb.half_size;
            db_vector2 center       = glyphs->data[c].aabb.center;
            db_vector3 glyph_scale  = db_vector3_make(half_extent.x * 2.0f, half_extent.y * 2.0f, 1.0f);
            db_vector3 glyph_center = db_vector3_make(center.x, center.y, 0.0f);

            camera_set_matrix(&camera, &shader, glyph_center, glyph_scale, 0.1f, 100.0f);

            // the shader works in em, so convert the px pen before uploading it
            glUniform2f(glGetUniformLocation(shader.program, "pen_offset"), pen.x / font_px, pen.y / font_px);

            // uniform vec2 curve_indicies;
            u32        curve_loc = glGetUniformLocation(shader.program, "curve_indicies");
            db_vector2 indicies = db_vector2_make(glyphs->data[c].curves_start_index, glyphs->data[c].curves_end_index);
            glUniform2fv(curve_loc, 1, indicies.data);

            vao_bind(&b_vao);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            pen.x += glyphs->data[c].advance * font_px;
        }

        platform_swap_buffers();
    }
    shader_destroy(&shader);
    platform_shutdown();
}

void camera_set_matrix(camera *camera, shader *shader, db_vector3 glyph_center, db_vector3 glyph_scale, f32 near_plane,
                       f32 far_plane)
{
    u32 projection_loc = glGetUniformLocation(shader->program, "projection");
    u32 model_loc      = glGetUniformLocation(shader->program, "model");
    u32 view_loc       = glGetUniformLocation(shader->program, "view");

    db_matrix4 ortho;
    db_matrix4_ortho2d(&ortho, 0, (f32)WINDOW_WIDTH, (f32)WINDOW_HEIGHT, 0);

    db_matrix4 view;
    db_matrix4_identity(&view);

    // model = translate(center) * scale(size): scale the unit quad first, then move it onto the ink box
    db_matrix4 scale_matrix;
    db_matrix4_scale(&scale_matrix, glyph_scale);
    db_matrix4 translate_matrix;
    db_matrix4_translate(&translate_matrix, glyph_center);

    db_matrix4 model;
    db_matrix4_mul(&model, &translate_matrix, &scale_matrix);

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
