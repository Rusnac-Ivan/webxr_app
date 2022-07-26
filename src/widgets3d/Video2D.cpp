#include "Video2D.h"

#include <opengl/Render.h>
#include <opengl/Pipeline.h>
#include <backends/imgui_impl_opengl3.h>
#include <utilities/Resources/ResourceManager.h>
#include "ImGui_Impl_2d_to_3d.h"
#include "utilities/imgui_tools/imgui_util.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/intersect.hpp>

namespace w3d
{
    Video2D::Video2D(/* args */) : mIsReady(false)
    {
    }

    Video2D::~Video2D()
    {
    }

    void Video2D::Create(const char *url)
    {
#ifdef __EMSCRIPTEN__
        mEMVideo.AddListener(this);
        mEMVideo.Load(url);
        mEMVideo.SetAutoplay(false);
        mEMVideo.SetMute(true);
        mEMVideo.SetLoop(false);
        // mEMVideo.Play();

#else
        printf("Video2D::operator()\n");
        mWidth = 800.f;
        mHeight = 500.f;

        const void *pixels[3] = {0, 0, 0};
        mVideoCol.LoadData(0, gl::Texture::Format::RGB, 1, 1, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, pixels);

        gl::Texture2D::Sampler sam;
        sam.wrapS = gl::Texture::WrapMode::CLAMP_TO_EDGE;
        sam.wrapT = gl::Texture::WrapMode::CLAMP_TO_EDGE;
        sam.minFilter = gl::Texture::FilterMode::LINEAR;
        sam.magFilter = gl::Texture::FilterMode::LINEAR;
        mVideoCol.SetSampler(sam);
        mResultCol.SetSampler(sam);

        mFBO.SetSize(mWidth, mHeight);
        mFBO.AttachTexture2D(gl::FrameBuffer::Attachment::COLOR0, &mResultCol);
        assert(mFBO.IsComplete());

        const float coef = 2.f;                   // Xmm per 1px
        float plane_w = (mWidth * coef) / 1000.f; // convert from mm to m
        float plane_h = (mHeight * coef) / 1000.f;
        mPlane.Generate(plane_w, plane_h, 1.f, 1.f, util::Plane<OPTIONS>::Direction::OZ_POS);

        mIsReady = true;
#endif
    }

    void Video2D::operator()(IEvent &event)
    {
#ifdef __EMSCRIPTEN__
        em::MediaEvent &media_event = dynamic_cast<em::MediaEvent &>(event);

        if (media_event.GetType() == em::MediaEvent::Type::ON_CANPLAY)
        {
            printf("Video2D::operator()\n");
            mWidth = mEMVideo.GetWidth();
            mHeight = mEMVideo.GetHeight();

            const void *pixels[3] = {0, 0, 0};
            mVideoCol.LoadData(0, gl::Texture::Format::RGB, 1, 1, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, pixels);

            gl::Texture2D::Sampler sam;
            sam.wrapS = gl::Texture::WrapMode::CLAMP_TO_EDGE;
            sam.wrapT = gl::Texture::WrapMode::CLAMP_TO_EDGE;
            sam.minFilter = gl::Texture::FilterMode::LINEAR;
            sam.magFilter = gl::Texture::FilterMode::LINEAR;
            mVideoCol.SetSampler(sam);
            mResultCol.SetSampler(sam);

            mFBO.SetSize(mWidth, mHeight);
            mFBO.AttachTexture2D(gl::FrameBuffer::Attachment::COLOR0, &mResultCol);
            assert(mFBO.IsComplete());

            const float coef = 3.f;                   // Xmm per 1px
            float plane_w = (mWidth * coef) / 1000.f; // convert from mm to m
            float plane_h = (mHeight * coef) / 1000.f;
            mPlane.Generate(plane_w, plane_h, 1.f, 1.f, util::Plane<OPTIONS>::Direction::OZ_POS);

            mIsReady = true;
        }
#endif
    }

    void Video2D::Compose(const glm::vec3 &cont_pos, const glm::quat &cont_rot, const glm::mat4 &model)
    {
        return;
        mModel = model;
        if (mIsReady)
        {
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

                    // printf("mouse_pos[%.2f, %.2f]\n", mouse_pos.x, mouse_pos.y);
                    is_hovered = true;
                }
            }
            {
#ifdef __EMSCRIPTEN__
                if (mEMVideo.GetMediaState() == em::MediaState::PLAYING)
                    mEMVideo.UpdateFrame(mVideoCol);
#endif

                mFBO.Bind();

                gl::Render::SetClearColor(0.f, 0.f, 0.f, 0.f);
                gl::Render::Clear(gl::BufferBit::COLOR);

                {
                    // ImDrawData *draw_data = ImGui::GetDrawData();
                    // mouse_pos = ImVec2(400.f, 250.f);
                    ImGui_Impl_2d_to_3d_NewFrame(ImVec2(mWidth, mHeight), mouse_pos);
                    ImGui::NewFrame();

                    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
                    ImGui::SetNextWindowSize(ImVec2(mWidth, mHeight), ImGuiCond_Always);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
                    ImGuiContext &g = *GImGui;
                    g.ActiveIdNoClearOnFocusLoss = true;
                    if (ImGui::Begin("Video2D", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
                    {
                        if (is_hovered)
                            g.HoveredWindow = ImGui::GetCurrentWindow();

                        ImVec2 button_size = ImVec2(170.f, 70.f);
#ifdef __EMSCRIPTEN__
                        if (mEMVideo.GetMediaState() != em::MediaState::PLAYING && mEMVideo.GetMediaState() != em::MediaState::ENDED)
                        {
                            ImVec2 m_pos = ImGui::GetMousePos();
                            // printf("mouse_pos[%.2f, %.2f]\n", m_pos.x, m_pos.y);

                            ImGui::SetCursorPos(ImVec2((mWidth - button_size.x) / 2.f, (mHeight - button_size.y) / 2.f));
                            if (ImGui::Button("Play", button_size))
                            {
                                mEMVideo.Play();
                            }
                        }
                        else if (mEMVideo.GetMediaState() == em::MediaState::WAITING)
                        {
                            float dim = mWidth < mHeight ? mWidth : mHeight;
                            float radius = 0.2f * dim;
                            const ImU32 color = ImColor(ImVec4(0.f, 1.f, 0.f, 0.f));
                            const int circle_count = radius > 67 ? 0.12f * radius : 8;
                            ImGui::SetCursorPos(ImVec2(mWidth / 2.f - radius, mHeight / 2.f - radius));
                            ImGui::LoadingIndicatorCircle("Load", radius, ImVec4(1.f, 0.f, 0.f, 1.f), ImVec4(0.f, 0.f, 1.f, 1.f), 20, 12.f);
                        }
                        else if (mEMVideo.GetMediaState() == em::MediaState::PLAYING)
                        {
                            ImGui::Image((ImTextureID)mVideoCol.GetID(), ImVec2(mWidth, mHeight));
                        }
#else
                        ImGui::SetCursorPos(ImVec2((mWidth - button_size.x) / 2.f, (mHeight - button_size.y) / 2.f));
                        if (ImGui::Button("Play", button_size))
                        {
                        }
#endif

                        ImDrawList *draw_list = ImGui::GetWindowDrawList();
                        if (is_hovered)
                            draw_list->AddCircleFilled(ImVec2(mouse_pos.x, mouse_pos.y), 10.f, ImColor(ImVec4(1.f, 0.f, 1.f, 0.7f)));

                        g.HoveredWindow = nullptr;
                    }
                    ImGui::End();
                    ImGui::PopStyleVar(2);

                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                }
                // GL(Flush());
                mFBO.UnBind();
            }
        }
    }

    void Video2D::Draw()
    {
        return;
        if (mIsReady)
        {
            {
                gl::Program *menu_prog = util::ResourceManager::GetShaders()->GetMenuProg();
                menu_prog->Use();
                if (mProgram != menu_prog)
                {
                    mUniformLocations.model = menu_prog->Uniform("model");
                    mUniformLocations.is_video = menu_prog->Uniform("uIsVideo");
                    // must set once
                    menu_prog->SetInt(menu_prog->Uniform("uAlbedo"), 0);
                    mProgram = menu_prog;
                }
                menu_prog->SetMatrix4(mUniformLocations.model, mModel);
                menu_prog->SetInt(mUniformLocations.is_video, 1);
                mResultCol.Activate(0);
                gl::Pipeline::EnableBlending();
                mPlane.Draw();
                menu_prog->SetInt(mUniformLocations.is_video, 0);
            }
        }
    }

}