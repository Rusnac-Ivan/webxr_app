#include "ImGui_Impl_2d_to_3d.h"
#include <GLFW/glfw3.h>

typedef void (*Windowfocusfun)(int);
typedef void (*Cursorenterfun)(int);
typedef void (*Mousebuttonfun)(int, int, int);
typedef void (*Scrollfun)(double, double);
typedef void (*Keyfun)(int, int, int, int);
typedef void (*Charfun)(unsigned int);

constexpr uint32_t MAX_CONTEXT_COUNT = 10;

static double g_Time = 0.0f;
static ImGuiContext *g_Context[MAX_CONTEXT_COUNT];
static bool g_MouseJustPressed[ImGuiMouseButton_COUNT] = {};

/*struct ImGui_Impl_2d_to_3d_Data
{
    double Time;

    bool MouseJustPressed[ImGuiMouseButton_COUNT];
    bool InstalledCallbacks;

    // callbacks: our callbacks will call the user's previously installed callbacks, if any.
    Windowfocusfun PrevUserCallbackWindowFocus;
    Cursorenterfun PrevUserCallbackCursorEnter;
    Mousebuttonfun PrevUserCallbackMousebutton;
    Scrollfun PrevUserCallbackScroll;
    Keyfun PrevUserCallbackKey;
    Charfun PrevUserCallbackChar;

    ImGui_Impl_2d_to_3d_Data() { memset(this, 0, sizeof(*this)); }
};*/

/*static ImGui_Impl_2d_to_3d_Data* ImGui_Impl_2d_to_3d_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_Impl_2d_to_3d_Data *)ImGui::GetIO().BackendPlatformUserData : NULL;
}*/

void ImGui_Impl_2d_to_3d_Shutdown()
{
    ImGuiIO &io = ImGui::GetIO();
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    // IM_DELETE(bd);
}

bool ImGui_Impl_2d_to_3d_InitForOpenGL()
{
    ImGuiIO &io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    // Setup backend capabilities flags
    // ImGui_Impl_2d_to_3d_Data* bd = IM_NEW(ImGui_Impl_2d_to_3d_Data)();
    io.BackendPlatformUserData = (void *)"imGui_impl_2d_to_3d";
    io.BackendPlatformName = "imGui_impl_2d_to_3d";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)

    g_Time = 0.0;

    // Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = KEY_HOME;
    io.KeyMap[ImGuiKey_End] = KEY_END;
    io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = KEY_A;
    io.KeyMap[ImGuiKey_C] = KEY_C;
    io.KeyMap[ImGuiKey_V] = KEY_V;
    io.KeyMap[ImGuiKey_X] = KEY_X;
    io.KeyMap[ImGuiKey_Y] = KEY_Y;
    io.KeyMap[ImGuiKey_Z] = KEY_Z;

    return true;
}

static void ImGui_Impl_2d_to_3d_UpdateMousePosAndButtons(const ImVec2 &mouse_pos)
{
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    ImGuiIO &io = ImGui::GetIO();

    const ImVec2 mouse_pos_prev = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        io.MouseDown[i] = g_MouseJustPressed[i];
    }

    io.MousePos = mouse_pos;
}

void ImGui_Impl_2d_to_3d_NewFrame(const ImVec2 &win_size, const ImVec2 &mouse_pos)
{
    // width height
    ImGuiIO &io = ImGui::GetIO();
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    // IM_ASSERT(bd != NULL && "Did you call ImGui_Impl_2d_to_3d_InitForOpenGL()?");

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)win_size.x, (float)win_size.y);
    io.DisplayFramebufferScale = ImVec2(1.f, 1.f);

    /*// Setup time step
    double current_time = glfwGetTime();
    io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time = current_time;*/

    ImGui_Impl_2d_to_3d_UpdateMousePosAndButtons(mouse_pos);
}

void ImGui_Impl_2d_to_3d_WindowFocusCallback(int focused)
{
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    // if (bd->PrevUserCallbackWindowFocus != NULL)
    // bd->PrevUserCallbackWindowFocus(focused);

    ImGuiIO &io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

void ImGui_Impl_2d_to_3d_MouseButtonCallback(int button, int action, int mods)
{
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    // if (bd->PrevUserCallbackMousebutton != NULL)
    // bd->PrevUserCallbackMousebutton(button, action, mods);

    if (action == PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;

    if (action == RELEASE && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = false;
}
void ImGui_Impl_2d_to_3d_ScrollCallback(double xoffset, double yoffset)
{
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    // if (bd->PrevUserCallbackScroll != NULL)
    // bd->PrevUserCallbackScroll(xoffset, yoffset);

    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}
void ImGui_Impl_2d_to_3d_KeyCallback(int key, int scancode, int action, int mods)
{
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    // if (bd->PrevUserCallbackKey != NULL)
    // bd->PrevUserCallbackKey(key, scancode, action, mods);

    ImGuiIO &io = ImGui::GetIO();
    if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
    {
        if (action == PRESS)
            io.KeysDown[key] = true;
        if (action == RELEASE)
            io.KeysDown[key] = false;
    }
}
void ImGui_Impl_2d_to_3d_CharCallback(unsigned int c)
{
    // ImGui_Impl_2d_to_3d_Data *bd = ImGui_Impl_2d_to_3d_GetBackendData();
    // if (bd->PrevUserCallbackChar != NULL)
    // bd->PrevUserCallbackChar(c);

    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(c);
}