#define DB_IMPLEMENTATION

#include "core/input.h"
#include "platform/platform.h"
#include "renderer/renderer.h"
#include "text/text.h"

#define ONE_TWENTIETH 0.05f

b8 update(db_arena *frame_arena);

b8 application_startup(db_arena *main_arena)
{
    b8 success = platform_startup(main_arena, "slug", 0, 0, 600, 400);
    ASSERT_WITH_MSG(success, "Platform Startup failed");
    success = renderer_init(main_arena);

    input_initialize(main_arena);
    text_init(main_arena);
    glyphs *glyphs = text_load_font(main_arena, "../assets/font/Archivo-Regular.ttf");

    return true;
}

// reverse order of application_startup
void application_shutdown(db_arena *main_arena)
{
    text_deinit();
    input_shutdown(main_arena);
    renderer_shutdown();
    platform_shutdown();
}

int main()
{
    db_arena_params params     = {.type = TYPE_ARENA_LINEAR, .chunk_size = 0};
    db_arena        main_arena = db_arena_init_with_size(&params, MB(10));

    b8 success = application_startup(&main_arena);
    if (!success)
        return 0;

    b8 run = true;

    db_vector2 pos   = db_vector2_make(10, 10);
    db_vector3 color = db_vector3_make(0.5, 0.5, 0.0);

    u32 width  = 0;
    u32 height = 0;

    db_arena frame_arena = db_arena_init_with_size(NULL, MB(1));
    while (run)
    {
        input_update(0);
        if (!platform_pump_messages())
            break;
        platform_get_window_dimensions(&width, &height);
        run = update(&frame_arena);

        db_string str = db_string_make(
            &frame_arena,
            "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
            ")");

        renderer_start_frame(&frame_arena);
        // wrap at the window's right edge
        renderer_push_text(&str, &pos, &color, 24.0f, (f32)width - pos.x);

        renderer_end_frame();
        db_arena_reset(&frame_arena);
    }
    application_shutdown(&main_arena);
}

//@note:
// this is temperory
b8 update(db_arena *arena)
{
    if (input_is_key_down(KEY_ESCAPE))
    {
        return false;
    }
    return true;
}
