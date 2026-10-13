#pragma once

#include "core/defines.h"

typedef enum
{
    DB_RENDER_TEXT = bit1,
    DB_RENDER_QUAD = bit2,
} db_render_type;

typedef struct
{
    db_render_type type;
    u64            instance_index;
    u64            instance_count;
} db_render_batch;

b8   renderer_init(db_arena *arena);
void renderer_shutdown();

void renderer_start_frame(db_arena *arena);

b8   renderer_start_layer();
void renderer_end_layer();

void renderer_push_quad(db_rect2 *quad, db_vector4 *color);
// all lengths are logical units; wrap_width is measured from position.x, 0 disables wrapping
void renderer_push_text(db_string *string, db_vector2 *position, db_vector3 *color, f32 font_size, f32 wrap_width);

#define renderer_layer_batch() defer_loop(renderer_start_layer(), renderer_end_layer())

void renderer_end_frame();
