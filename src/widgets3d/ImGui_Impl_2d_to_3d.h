#pragma once
#include <imgui.h>

enum Key
{
	KEY_TAB,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_INSERT,
	KEY_DELETE,
	KEY_BACKSPACE,
	KEY_SPACE,
	KEY_ENTER,
	KEY_ESCAPE,
	KEY_KP_ENTER,
	KEY_A,
	KEY_C,
	KEY_V,
	KEY_X,
	KEY_Y,
	KEY_Z
};

enum MouseButton
{
	MOUSEBUTTON_LEFT = 0,
	MOUSEBUTTON_RIGHT = 1,
	MOUSEBUTTON_MIDDLE = 2
};

enum Action
{
	RELEASE,
	PRESS,
	REPEAT
};



IMGUI_IMPL_API bool     ImGui_Impl_2d_to_3d_InitForOpenGL();
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_Shutdown();
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_NewFrame(const ImVec2& win_size, const ImVec2& mouse_pos);

//call backs
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_WindowFocusCallback(int focused);
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_MouseButtonCallback(int button, int action, int mods);
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_ScrollCallback(double xoffset, double yoffset);
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_KeyCallback(int key, int scancode, int action, int mods);
IMGUI_IMPL_API void     ImGui_Impl_2d_to_3d_CharCallback(unsigned int c);