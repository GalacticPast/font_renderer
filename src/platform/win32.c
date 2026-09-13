#ifdef PLATFORM_WINDOWS

#include "core/event.hpp"
#include "core/input.hpp"
#include "platform.hpp"

#include <windows.h>
#include <windowsx.h> // param input extraction

#include <fstream>
#include <sstream>

#include "opengl/opengl_context.hpp"

static f64 clock_frequency;
static LARGE_INTEGER start_time;

typedef struct internal_state
{
    HINSTANCE h_instance;
    HWND hwnd;
    HDC hdc;
    HGLRC opengl_context;

    s32 width;
    s32 height;
} internal_state;

// Clock

static bool opengl_initialized = false;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);
keys translate_keycode(u32 key);

void clock_setup(void)
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        // Notify the OS that erasing will be handled by the application to prevent flicker.
        return 1;
    case WM_CLOSE:
    {
        event_context context = {0};
        event_fire(EVENT_CODE_APPLICATION_QUIT, nullptr, context);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
    {
        // Get the updated size.
        RECT r;
        GetClientRect(hwnd, &r);
        u32 width = r.right - r.left;
        u32 height = r.bottom - r.top;

        event_context context = {0};
        context.data.u32[0] = width;
        context.data.u32[1] = height;
        event_fire(EVENT_CODE_RESIZED, nullptr, context);
    }
    break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        // Key pressed/released
        bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        keys key = (keys)w_param;

        input_process_key(key, pressed);
    }
    break;
    case WM_MOUSEMOVE:
    {
        // Mouse move
        s16 x_pos = GET_X_LPARAM(l_param);
        s16 y_pos = GET_Y_LPARAM(l_param);

        input_process_mouse_move(x_pos, y_pos);
    }
    break;
    case WM_MOUSEWHEEL:
    {
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
    }
    break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

bool platform_startup(platform_context *plat_state, std::string application_name, s32 x, s32 y, s32 width, s32 height)
{
    DINFO("initializing windows platform startup...");
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)plat_state->internal_state;

    state->h_instance = GetModuleHandleA(0);

    clock_setup();

    // Setup and register window class.
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; // Get double-clicks
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL; // Manage the cursor manually
    wc.hbrBackground = NULL;                  // Transparent
    wc.lpszClassName = "learning_opengl_class";

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    u32 client_screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
    u32 client_screen_height = GetSystemMetrics(SM_CYFULLSCREEN);

    // Create window
    u32 client_x = x == 0 ? (client_screen_width / 2 - width / 2) : x;
    u32 client_y = y == 0 ? (client_screen_height / 2 - height / 2) : y;
    u32 client_width = width;
    u32 client_height = height;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Obtain the size of the border.
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // In this case, the border rectangle is negative.
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Grow by the size of the OS border.
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(window_ex_style, "learning_opengl_class", application_name.c_str(), window_style, window_x, window_y, window_width, window_height, 0, 0, state->h_instance, 0);

    if (handle == 0)
    {
        MessageBoxA(NULL, "Window creation failed!", "ERROR!", MB_ICONEXCLAMATION | MB_OK);
        DFATAL("Window creation failed!");
        return false;
    }
    else
    {
        state->hwnd = handle;
    }

    // Show the window
    return true;
}
void platform_shutdown(platform_context *plat_state)
{
    internal_state *state = (internal_state *)plat_state->internal_state;

    if (state->hwnd)
    {
        wglDeleteContext(state->opengl_context);
        ReleaseDC(state->hwnd, state->hdc);
        DestroyWindow(state->hwnd);

        state->hwnd = 0;
    }
}
bool platform_pump_messages(platform_context *plat_state)
{
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
}

bool init_openGL(platform_context *plat_state)
{
    DINFO("Initializing OpenGL...");

    internal_state *state = (internal_state *)plat_state->internal_state;

    state->hdc = GetDC(state->hwnd);
    if (state->hdc == NULL)
    {
        MessageBox(NULL, ("Failed to get window's device context!"), "DERROR", MB_ICONERROR);
        DFATAL("Failed to get window's device context!");
        return false;
    }

    PIXELFORMATDESCRIPTOR pixel_format_des = {};
    pixel_format_des.nSize = sizeof(pixel_format_des);
    pixel_format_des.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pixel_format_des.iPixelType = PFD_TYPE_RGBA;
    pixel_format_des.cColorBits = 32;
    pixel_format_des.cDepthBits = 32;
    pixel_format_des.iLayerType = PFD_MAIN_PLANE;

    u32 format = ChoosePixelFormat(state->hdc, &pixel_format_des);

    if (format == 0 || SetPixelFormat(state->hdc, format, &pixel_format_des) == FALSE)
    {
        MessageBox(NULL, ("Failed to set a compatible format"), "DERROR", MB_ICONERROR);
        DFATAL("Failed to set a compatible format");
        return false;
    }

    HGLRC temp_context = NULL;
    temp_context = wglCreateContext(state->hdc);
    if (temp_context == NULL)
    {
        MessageBox(NULL, ("Failed to create the initial rendereing context"), "DERROR", MB_ICONERROR);
        DFATAL("Failed to create the initial rendereing context");
        return false;
    }

    wglMakeCurrent(state->hdc, temp_context);

    gladLoaderLoadWGL(state->hdc);

    state->opengl_context = wglCreateContext(state->hdc);

    if (state->opengl_context == NULL)
    {
        MessageBox(NULL, ("Failed to create the final rendering context!"), "DERROR", MB_ICONERROR);
        DFATAL("Failed to create the final rendereing context");
        return false;
    }

    wglMakeCurrent(NULL, NULL);                        // Remove the temporary context from being active
    wglDeleteContext(temp_context);                    // Delete the temporary OpenGL context
    wglMakeCurrent(state->hdc, state->opengl_context); // Make our OpenGL 3.2 context current

    // Glad Loader!
    if (!gladLoaderLoadGL())
    {
        MessageBox(NULL, ("Glad Loader failed!"), "DERROR", MB_ICONERROR);
        DFATAL("Glad Loader failed!");
        return false;
    }

    bool should_activate = 1; // TODO: if the window should not accept input, this should be false.
    s32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(state->hwnd, show_window_command_flags);

    opengl_initialized = true;

    return true;
}
void platform_swap_buffers(platform_context *plat_state)
{
    internal_state *state = (internal_state *)plat_state->internal_state;
    SwapBuffers(state->hdc);
}
void platform_set_viewport(void *data, u16 width, u16 height)
{
    if (opengl_initialized)
    {
        platform_context *plat_context = (platform_context *)data;
        internal_state *state = (internal_state *)plat_context->internal_state;

        state->width = width;
        state->height = height;
        glViewport(0, 0, width, height);
        glViewport(0, 0, width, height);
    }
}

void platform_get_window_dimensions(platform_context *plat_state, s32 *width, s32 *height)
{
    internal_state *state = (internal_state *)plat_state->internal_state;
    *width = state->width;
    *height = state->height;
}
// memory
void *platform_allocate(u64 size, bool aligned)
{
    return malloc(size);
}
void platform_free(void *block, bool aligned)
{
    free(block);
}
void *platform_zero_memory(void *block, u64 size)
{
    return memset(block, 0, size);
}
void *platform_copy_memory(void *dest, const void *source, u64 size)
{
    return memcpy(dest, source, size);
}
void *platform_set_memory(void *dest, s32 value, u64 size)
{
    return memset(dest, value, size);
}

void platform_load_file(const char *filepath, std::string *str)
{
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        DFATAL("Failed to open file.");
    }

    std::ostringstream file_stream;
    file_stream << file.rdbuf();
    *str = file_stream.str();
    file.close();
}

void platform_get_shaders(std::string *vertex_shader_source, std::string *fragment_shader_source)
{
    char vertex_path[] = "../assets/shaders/vert.glsl";
    char frag_path[] = "../assets/shaders/frag.glsl";

    platform_load_file(vertex_path, vertex_shader_source);
    platform_load_file(frag_path, fragment_shader_source);
}

void platform_log_message(const char *buffer, log_levels level, u32 max_chars)
{

    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // DFATAL,DERROR,WARN,DINFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[level]);

    u64 length = strlen(buffer);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, (DWORD)length, number_written, 0);
}
f64 platform_get_absolute_time(void)
{
    if (!clock_frequency)
    {
        clock_setup();
    }

    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64)now_time.QuadPart * clock_frequency;
}
void platform_sleep(u64 ms)
{
    Sleep(ms);
}

#endif
