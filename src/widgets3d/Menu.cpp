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
    Menu::Menu() : mIsOpen(true), mProgram(nullptr) {}

    Menu::~Menu() {}

    void Menu::Create(float width, float height)
    {
        mWidth = width;
        mHeight = height;

        mBaseColor.SetWrapModeS(gl::Texture::WrapMode::CLAMP_TO_EDGE);
        mBaseColor.SetWrapModeT(gl::Texture::WrapMode::CLAMP_TO_EDGE);
        mBaseColor.SetMinFilterMode(gl::Texture::FilterMode::LINEAR);
        mBaseColor.SetMagFilterMode(gl::Texture::FilterMode::LINEAR);
        mFBO.SetSize(mWidth, mHeight);
        mFBO.AttachTexture2D(gl::FrameBuffer::Attachment::COLOR0, &mBaseColor);
        // mBaseColor.GenerateMipmaps();
        assert(mFBO.IsComplete());

        const float coef = 3.f;                  // Xmm per 1px
        float plane_w = (width * coef) / 1000.f; // convert from mm to m
        float plane_h = (height * coef) / 1000.f;
        mPlane.Generate(plane_w, plane_h, 1.f, 1.f, util::Plane::Direction::OZ_POS);

        mVBO.SetData(sizeof(util::Plane::Vertex) * mPlane.GetVertexCount(), mPlane.GetVertexData());
        mEBO.Data(sizeof(uint32_t) * mPlane.GetIndicesCount(), mPlane.GetIndicesData(), gl::DataType::UNSIGNED_INT);
        mVAO.AddVertexLayout(mVBO, {gl::VertexAttribute::Entry<glm::vec3>(), gl::VertexAttribute::Entry<glm::vec2>()});
        mVAO.LinkIndexBuffer(mEBO);
    }

    void Menu::Compose(WebXRInputSource *inputSource, const glm::mat4 &model, const char *name, ComposeFun gui_fun)
    {
        ImVec2 mouse_pos = ImVec2(-1.f, -1.f);
        if (inputSource)
        {
            float distance;
#ifdef __EMSCRIPTEN__
            glm::vec3 &input_pose = inputSource->rigidTransform.position;
            glm::vec3 input_dir = glm::rotate(inputSource->rigidTransform.orientation, glm::vec3(0.f, 0.f, -1.f));
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
            if (ImGui::Begin(name, &mIsOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav))
            {
                ImDrawList *draw_list = ImGui::GetWindowDrawList();
                draw_list->AddCircleFilled(ImVec2(mouse_pos.x, mouse_pos.y), 10.f, ImColor(ImVec4(0.f, 1.f, 1.f, 0.5f)));

                gui_fun();
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        mFBO.UnBind();
    }

    void Menu::Draw(const glm::mat4 &model)
    {
        gl::Program *menu_prog = util::ResourceManager::GetShaders()->GetMenuProg();
        menu_prog->Use();
        if (mProgram != menu_prog)
        {
            mUniformLocations.model = menu_prog->Uniform("model");
            // must set once
            menu_prog->SetInt(menu_prog->Uniform("uAlbedo"), 0);
            mProgram = menu_prog;
        }
        menu_prog->SetMatrix4(mUniformLocations.model, model);
        mBaseColor.Activate(0);
        mVAO.Bind();
        gl::Pipeline::EnableBlending();
        gl::Render::DrawIndices(gl::Primitive::TRIANGLES, mPlane.GetIndicesCount(), mEBO.GetDataType(), 0);
    }
} // namespace w3d