#include "Menu.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <opengl/Render.h>

namespace w3d
{
    Menu::Menu(/* args */) : mIsOpen(true)
    {
        mFBO.
    }

    Menu::~Menu()
    {
    }

    void Menu::Compose(const char* name, ImGuiWindowFlags flags, ComposeFun fun)
    {
        mFBO.Bind();
        gl::Render::SetClearColor(0.f, 0.f, 0.f, 0.f);
        gl::Render::Clear(gl::BufferBit::COLOR);

        if (ImGui::Begin(name, &mIsOpen, flags))
        {
            fun();
            ImGui::End();
        }
        mFBO.UnBind();
    }

    void Menu::Draw(const glm::mat4& view, const glm::mat4& proj)
    {

    }
} // namespace w3d
