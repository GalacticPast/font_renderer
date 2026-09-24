#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include "../../vendor/glad/glad.h"
#include "../input.h"
#include "platform.h"
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

static internal_state storage;
static internal_state *state = &storage;
static b8 running;
static b8 input_ready;
static b8 init_openGL(void);

// Clock

static b8 opengl_initialized = false;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

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
        running = false;
        return 0;
    }
    case WM_DESTROY:
        running = false;
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
    {
        // Get the updated size.
        RECT r;
        GetClientRect(hwnd, &r);
        u32 width = r.right - r.left;
        u32 height = r.bottom - r.top;

        state->width = width;
        state->height = height;
        if (opengl_initialized) glViewport(0, 0, width, height);
    }
    break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        // Key pressed/released
        b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        keys key = (keys)w_param;

        if (input_ready && w_param < 256) input_process_key(key, pressed);
    }
    break;
    case WM_MOUSEMOVE:
    {
        // Mouse move
        s16 x_pos = GET_X_LPARAM(l_param);
        s16 y_pos = GET_Y_LPARAM(l_param);

        if (input_ready) input_process_mouse_move(x_pos, y_pos);
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

b8 platform_startup(db_arena *arena, char *application_name, s32 x, s32 y, s32 width, s32 height)
{
    printf("Initializing Windows platform...\n");
    state->width = width;
    state->height = height;

    state->h_instance = GetModuleHandleA(0);

    clock_setup();

    // Setup and register window class.
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS | CS_OWNDC; // Get double-clicks
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

    HWND handle = CreateWindowExA(window_ex_style, "learning_opengl_class", application_name, window_style, window_x, window_y, window_width, window_height, 0, 0, state->h_instance, 0);

    if (handle == 0)
    {
        MessageBoxA(NULL, "Window creation failed!", "ERROR!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    else
    {
        state->hwnd = handle;
    }

    if (!init_openGL())
    {
        platform_shutdown();
        return false;
    }
    running = true;
    return true;
}
void platform_shutdown(void)
{

    if (state->hwnd)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(state->opengl_context);
        ReleaseDC(state->hwnd, state->hdc);
        DestroyWindow(state->hwnd);

        state->hwnd = 0;
    }
}
b8 platform_pump_messages(void)
{
    input_ready = true;
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT) running = false;
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return running;
}

static b8 init_openGL(void)
{
    printf("Initializing OpenGL...\n");

    state->hdc = GetDC(state->hwnd);
    if (state->hdc == NULL)
    {
        MessageBox(NULL, ("Failed to get window's device context!"), "DERROR", MB_ICONERROR);
        return false;
    }

    PIXELFORMATDESCRIPTOR pixel_format_des = {};
    pixel_format_des.nSize = sizeof(pixel_format_des);
    pixel_format_des.nVersion = 1;
    pixel_format_des.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pixel_format_des.iPixelType = PFD_TYPE_RGBA;
    pixel_format_des.cColorBits = 32;
    pixel_format_des.cDepthBits = 32;
    pixel_format_des.iLayerType = PFD_MAIN_PLANE;

    u32 format = ChoosePixelFormat(state->hdc, &pixel_format_des);

    if (format == 0 || SetPixelFormat(state->hdc, format, &pixel_format_des) == FALSE)
    {
        MessageBox(NULL, ("Failed to set a compatible format"), "DERROR", MB_ICONERROR);
        return false;
    }

    HGLRC temp_context = NULL;
    temp_context = wglCreateContext(state->hdc);
    if (temp_context == NULL)
    {
        MessageBox(NULL, ("Failed to create the initial rendereing context"), "DERROR", MB_ICONERROR);
        return false;
    }

    if (!wglMakeCurrent(state->hdc, temp_context))
    {
        wglDeleteContext(temp_context);
        return false;
    }

    typedef HGLRC (WINAPI *create_context_proc)(HDC, HGLRC, const int *);
    create_context_proc create_context = (create_context_proc)wglGetProcAddress("wglCreateContextAttribsARB");
    // WGL major version, minor version, profile mask, core profile bit.
    const int attributes[] = {0x2091, 3, 0x2092, 3, 0x9126, 1, 0};
    if (create_context)
        state->opengl_context = create_context(state->hdc, NULL, attributes);

    if (state->opengl_context == NULL)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(temp_context);
        MessageBox(NULL, ("OpenGL 3.3 core context unavailable"), "DERROR", MB_ICONERROR);
        return false;
    }

    wglMakeCurrent(NULL, NULL);                        // Remove the temporary context from being active
    wglDeleteContext(temp_context);                    // Delete the temporary OpenGL context
    if (!wglMakeCurrent(state->hdc, state->opengl_context))
        return false;

    // Glad Loader!
    if (!gladLoadGL() || !GLAD_GL_VERSION_3_3)
    {
        MessageBox(NULL, ("Glad Loader failed!"), "DERROR", MB_ICONERROR);
        return false;
    }

    b8 should_activate = 1; // TODO: if the window should not accept input, this should be false.
    s32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(state->hwnd, show_window_command_flags);

    opengl_initialized = true;
    glViewport(0, 0, state->width, state->height);

    return true;
}
void platform_swap_buffers(void)
{

    SwapBuffers(state->hdc);
}
void platform_set_viewport(void *data, u16 width, u16 height)
{
    if (opengl_initialized)
    {

        state->width = width;
        state->height = height;
        glViewport(0, 0, width, height);
    }
}

void platform_get_window_dimensions(u32 *width, u32 *height)
{

    *width = state->width;
    *height = state->height;
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
