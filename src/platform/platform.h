#pragma once
#include "../db.h"

b8   platform_startup(db_arena *arena, char *application_name, s32 x, s32 y, s32 width, s32 height);
void platform_shutdown();
b8   platform_pump_messages();

// void platform_log_message(const char *buffer, log_levels level, u32 max_chars);

void platform_get_window_dimensions(u32 *width, u32 *height);

void platform_swap_buffers();
void platform_set_viewport(void *data, u16 width, u16 height);

void platform_load_file(const char *filepath, db_string *str);

f64  platform_get_absolute_time();
void platform_sleep(u64 ms);
