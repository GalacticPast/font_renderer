#include "gl.h"
#include "input.h"
#include "platform/platform.h"
#include "text.h"

#define ONE_TWENTIETH 0.05f

// logical size: what the compositor lays the window out with

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
b8   update(db_arena *frame_arena, f32 *font_size);
void render_text(db_string *string, f32 font_size);

b8 opengl_startup(db_arena *main_arena)
{
    b8 success = platform_startup(main_arena, "slug", 0, 0, 600, 400);
    if (!success)
        return false;
    input_initialize(main_arena);
    return true;
}

int main()
{
    // startup the opengl context;
    db_arena_params params     = {.type = TYPE_ARENA_LINEAR, .chunk_size = 0};
    db_arena        main_arena = db_arena_init_with_size(&params, MB(10));
    b8              success    = opengl_startup(&main_arena);
    if (!success)
        return 0;

    text_init(&main_arena);

    // the scale is known once the platform has talked to the compositor
    display_scale = platform_get_display_scale();
    u32 width, height;
    platform_get_window_dimensions(&width, &height);

    glViewport(0, 0, (s32)(width * display_scale), (s32)(height * display_scale));

    // UI and text share the same camera
    camera camera      = {0};
    camera.position    = db_vector3_make(0.0f, 0.0f, 3.0f);
    camera.orientation = db_vector3_make(0.0f, 0.0f, -1.0f);
    camera.up          = db_vector3_make(0.0f, 1.0f, 0.0f);
    camera.yaw         = -90.0f;
    camera.pitch       = 0.0f;
    camera.fov         = 45.0f;
    camera.sensitivity = 0.1f; // change this value to your liking

    glyphs *glyphs = text_load_font(&main_arena, "../assets/font/Archivo-Regular.ttf");

    f32 font_size = 24.0f * display_scale; // 12 logical px, in physical pixels

    // for now render A

    vao_unbind();

    // glyph edges now output fractional alpha for anti-aliasing; without
    // blending enabled that alpha is ignored and edges stay hard.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    b8  run = true;
    s32 i   = 0;

    db_arena frame_arena = db_arena_init_with_size(NULL, MB(1));

    while (run)
    {
        input_update(0);
        if (!platform_pump_messages())
            break;

        run = update(&frame_arena, &font_size);

        db_string str = db_string_make(
            &frame_arena,
            "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
            ")");
        db_array_vector4 txt_buffer = db_array_vector4_init(&frame_arena);
        text_prepare_render_buffer(&str, &txt_buffer, font_size, width * display_scale);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        text_render(font_size);

        platform_swap_buffers();
        db_arena_reset(&frame_arena);
    }
    text_deinit();
    platform_shutdown();
}

//@note:
// this is temperory
b8 update(db_arena *arena, f32 *font_size)
{
    if (input_was_key_down(KEY_ESCAPE))
    {
        return false;
    }
    if (input_is_key_down(KEY_D))
    {
        *font_size += 2.0f;
    }
    if (input_is_key_down(KEY_A))
    {
        *font_size -= 2.0f;
    }
    *font_size = db_max(4.0, *font_size);
    return true;
}
