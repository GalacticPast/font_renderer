#include "input.h"

typedef struct keyboard_state
{
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state
{
    s16 x;
    s16 y;
    s8  z_delta;
    u8  buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state
{
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state    mouse_current;
    mouse_state    mouse_previous;
} input_state;

// Internal input state
static input_state *state;

void input_initialize(db_arena *arena)
{
    ASSERT(arena);
    state = (input_state *)db_arena_alloc(arena, sizeof(input_state));
}

void input_shutdown(db_arena *arena)
{
    // does_nothing actucally
}

void input_update(f64 delta_time)
{
    if (!state)
        return;
    // Copy current states to previous states.
    memcpy(&state->keyboard_previous, &state->keyboard_current, sizeof(keyboard_state));
    memcpy(&state->mouse_previous, &state->mouse_current, sizeof(mouse_state));
}

void input_process_key(keys key, b8 pressed)
{
    // Only handle this if the state actually changed.
    if (state->keyboard_current.keys[key] != pressed)
    {
        state->keyboard_current.keys[key] = pressed;
    }
}

void input_process_button(buttons button, b8 pressed)
{
    // If the state changed, fire an event.
    if (state->mouse_current.buttons[button] != pressed)
    {
        state->mouse_current.buttons[button] = pressed;
    }
}

void input_process_mouse_move(s16 x, s16 y)
{
    // Only process if actually different
    if (state->mouse_current.x != x || state->mouse_current.y != y)
    {
        // NOTE: Enable this if debugging.
        // DDEBUG("Mouse pos: %i, %i!", x, y);

        // Update internal state.
        state->mouse_current.x = x;
        state->mouse_current.y = y;
    }
}

void input_process_mouse_wheel(s8 z_delta)
{
}

b8 input_is_key_down(keys key)
{
    if (!state)
    {
        return false;
    }
    return state->keyboard_current.keys[key] == true;
}

b8 input_is_key_up(keys key)
{
    if (!state)
    {
        return true;
    }
    return state->keyboard_current.keys[key] == false;
}

b8 input_was_key_down(keys key)
{
    if (!state)
    {
        return false;
    }
    return state->keyboard_previous.keys[key] == true;
}

b8 input_was_key_up(keys key)
{
    if (!state)
    {
        return true;
    }
    return state->keyboard_previous.keys[key] == false;
}

// mouse input
b8 input_is_button_down(buttons button)
{
    if (!state)
    {
        return false;
    }
    return state->mouse_current.buttons[button] == true;
}

b8 input_is_button_up(buttons button)
{
    if (!state)
    {
        return true;
    }
    return state->mouse_current.buttons[button] == false;
}

b8 input_was_button_down(buttons button)
{
    if (!state)
    {
        return false;
    }
    return state->mouse_previous.buttons[button] == true;
}

b8 input_was_button_up(buttons button)
{
    if (!state)
    {
        return true;
    }
    return state->mouse_previous.buttons[button] == false;
}

void input_get_mouse_position(s32 *x, s32 *y)
{
    if (!state)
    {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state->mouse_current.x;
    *y = state->mouse_current.y;
}

void input_get_previous_mouse_position(s32 *x, s32 *y)
{
    if (!state)
    {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state->mouse_previous.x;
    *y = state->mouse_previous.y;
}
