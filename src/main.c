#define DB_IMPLEMENTATION
#include "db.h"
#include "input.h"
#include "platform/platform.h"

#include <GL/gl.h>

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
    db_arena main_arena = db_arena_init(.type = TYPE_ARENA_LINEAR, MB(10));
    b8       success    = opengl_startup(&main_arena);
    if (!success)
        return 0;
    while (true)
    {
        platform_pump_messages();
    }
}
