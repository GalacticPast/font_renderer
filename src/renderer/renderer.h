#pragma once

#include "core/defines.h"

b8   renderer_init(db_arena *arena);
void renderer_shutdown();

void renderer_start_frame(db_arena *arena);
void renderer_push_quad(db_rect2 *quad, db_vector4 *color);
// all lengths are logical units; wrap_width is measured from position.x, 0 disables wrapping
void renderer_push_text(db_string *string, db_vector2 *position, db_vector3 *color, f32 font_size, f32 wrap_width);
void renderer_end_frame();
