#include "Menu.h"

#include <opengl/Render.h>
#include <backends/imgui_impl_glfw.h>
#include "ImGui_Impl_2d_to_3d.h"
#include <backends/imgui_impl_opengl3.h>
#include <utilities/Resources/ResourceManager.h>
#include <opengl/Render.h>
#include <opengl/Pipeline.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/intersect.hpp>

namespace w3d
{

    Menu::Menu() : mIsOpen(true), mProgram(nullptr)
    {
        mModel = glm::mat4(1.f);
        // mImGuiContext = ImGui::CreateContext();

        /*ImGuiIO& io = mImGuiContext->IO;

        StyleColors(mImGuiContext->Style);

        ImFontConfig imFontConf = {};
        imFontConf.FontDataOwnedByAtlas = false;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(__Helvetica_ttf, __Helvetica_ttf_len, 17.f, &imFontConf);*/
    }

    Menu::~Menu()
    {
        // ImGui::DestroyContext(mImGuiContext);
    }

    void Menu::Create(float width, float height, float mm_per_px)
    {
        printf("Menu::Create width: %f, height: %f\n", width, height);
        mWidth = width;
        mHeight = height;

        gl::Texture2D::Sampler sam;
        sam.wrapS = gl::Texture::WrapMode::CLAMP_TO_EDGE;
        sam.wrapT = gl::Texture::WrapMode::CLAMP_TO_EDGE;
        sam.minFilter = gl::Texture::FilterMode::LINEAR;
        sam.magFilter = gl::Texture::FilterMode::LINEAR;
        mBaseColor.SetSampler(sam);

        mFBO.SetSize(mWidth, mHeight);
        mFBO.AttachTexture2D(gl::FrameBuffer::Attachment::COLOR0, &mBaseColor);
        // mBaseColor.GenerateMipmaps();
        assert(mFBO.IsComplete());

        const float coef = mm_per_px;            // Xmm per 1px
        float plane_w = (width * coef) / 1000.f; // convert from mm to m
        float plane_h = (height * coef) / 1000.f;
        mPlane.Generate(plane_w, plane_h, 1.f, 1.f, util::Plane<OPTIONS>::Direction::OZ_POS);
    }

    void Menu::Compose(const glm::vec3 &cont_pos, const glm::quat &cont_rot, const glm::mat4 &model, const char *name, ComposeFun gui_fun, ImGuiWindowFlags window_flags)
    {
        return;
        mModel = model;
        bool is_hovered = false;
        ImVec2 mouse_pos = ImVec2(-1.f, -1.f);
        // if (inputSource)
        {
            float distance;
#ifdef __EMSCRIPTEN__
            glm::vec3 input_pose = cont_pos;
            glm::vec3 input_dir = glm::rotate(cont_rot, glm::vec3(0.f, 0.f, -1.f));
#else
            glm::vec3 &input_pose = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 input_dir = glm::vec3(0.f, 0.f, -1.f);
#endif
            glm::vec3 plane_origin = glm::vec3(model * glm::vec4(mPlane.GetOrigin(), 1.f));
            glm::vec3 plane_normal = glm::mat3(model) * mPlane.GetNormal();
            glm::vec3 plane_up = glm::mat3(model) * mPlane.GetUp();
            glm::vec3 plane_right = glm::mat3(model) * mPlane.GetRight();
            if (glm::intersectRayPlane(input_pose, input_dir, plane_origin, plane_normal, distance))
            {
                glm::vec3 inter_pos = input_pose + input_dir * distance;

                float h = glm::dot(inter_pos - plane_origin, plane_right);
                float v = glm::dot(inter_pos - plane_origin, plane_up);

                mouse_pos.x = mWidth / mPlane.GetWidth() * h;
                mouse_pos.y = mHeight - mHeight / mPlane.GetHeight() * v;

                is_hovered = true;
            }
        }

        mFBO.Bind();

        gl::Render::SetClearColor(0.f, 0.f, 0.f, 0.f);
        gl::Render::Clear(gl::BufferBit::COLOR);

        if (mIsOpen)
        {
            ImGui_Impl_2d_to_3d_NewFrame(ImVec2(mWidth, mHeight), mouse_pos);
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(mWidth, mHeight), ImGuiCond_Always);

            ImGuiContext &g = *GImGui;
            g.ActiveIdNoClearOnFocusLoss = true;
            if (ImGui::Begin(name, &mIsOpen, window_flags))
            {
                if (is_hovered)
                    g.HoveredWindow = ImGui::GetCurrentWindow();

                // g.HoveredIdAllowOverlap = true;
                // g.ActiveIdAllowOverlap = true;

                gui_fun();

                ImDrawList *draw_list = ImGui::GetWindowDrawList();
                if (is_hovered)
                    draw_list->AddCircleFilled(ImVec2(mouse_pos.x, mouse_pos.y), 10.f, ImColor(ImVec4(1.f, 0.f, 1.f, 1.f)));

                g.HoveredWindow = nullptr;
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        mFBO.UnBind();
    }

    void Menu::Draw()
    {
        return;
        gl::Program *menu_prog = util::ResourceManager::GetShaders()->GetMenuProg();
        menu_prog->Use();
        if (mProgram != menu_prog)
        {
            mUniformLocations.model = menu_prog->Uniform("model");
            // must set once
            menu_prog->SetInt(menu_prog->Uniform("uAlbedo"), 0);
            mProgram = menu_prog;
        }
        menu_prog->SetMatrix4(mUniformLocations.model, mModel);
        mBaseColor.Activate(0);
        gl::Pipeline::EnableBlending();
        mPlane.Draw();
    }
} // namespace w3d
