#include "renderer.h"
#include "gl.h"
#include "platform/platform.h"
#include "text/text.h"

#define MAX_RENDER_BATCHES 12
typedef struct
{
    // logical, as the compositor lays the window out
    u32 width;
    u32 height;
    f32 display_scale;
    // width/height * display_scale: the framebuffer size. Viewport and projection both use these
    u32 physical_width;
    u32 physical_height;

    // resets every frame
    u64             batch_count;
    db_render_batch batches[MAX_RENDER_BATCHES];
} renderer_state;

static renderer_state *r_state;

b8 renderer_init(db_arena *arena)
{
    r_state = db_arena_alloc(arena, sizeof(renderer_state));
    ASSERT(r_state);
    b8 success = platform_renderer_init(arena);
    ASSERT_WITH_MSG(success, "Renderer failed Initilaizing");

    // window size and scale are read every frame in renderer_start_frame
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

// subsystems that own GL objects (text) must be shut down before this, while the context still exists
void renderer_shutdown()
{
    platform_renderer_shutdown();
}

void renderer_start_frame(db_arena *arena)
{
    // the scale is only known after the first frame and changes between monitors, so read it every frame
    platform_get_window_dimensions(&r_state->width, &r_state->height);
    r_state->display_scale = platform_get_display_scale();

    // truncated the same way the platform sizes the EGL window
    r_state->physical_width  = (u32)(r_state->width * r_state->display_scale);
    r_state->physical_height = (u32)(r_state->height * r_state->display_scale);
    glViewport(0, 0, (s32)r_state->physical_width, (s32)r_state->physical_height);
}

b8 renderer_start_layer()
{

    return true;
}
void renderer_end_layer()
{
}

void renderer_push_quad(db_rect2 *quad, db_vector4 *color)
{
}
void renderer_push_text(db_string *string, db_vector2 *position, db_vector3 *color, f32 font_size, f32 wrap_width)
{
    f32        scale             = r_state->display_scale;
    db_vector2 physical_position = db_vector2_make(position->x * scale, position->y * scale);
    text_push_string(string, &physical_position, color, font_size * scale, wrap_width * scale);
}

// for now we only have the text's shader so
void renderer_end_frame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    text_flush(r_state->physical_width, r_state->physical_height);

    platform_swap_buffers();
}
