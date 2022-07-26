#include "Video3D.h"

#include <opengl/Render.h>
#include <opengl/Pipeline.h>
#include <backends/imgui_impl_opengl3.h>
#include <utilities/Resources/ResourceManager.h>
#include "ImGui_Impl_2d_to_3d.h"

namespace w3d
{
    Video3D::Video3D(/* args */) : mProgram(nullptr), mIsReady(false)
    {
    }

    Video3D::~Video3D()
    {
    }

    void Video3D::Create(const char *url)
    {
#ifdef __EMSCRIPTEN__
        mEMVideo.AddListener(this);
        mEMVideo.Load(url);
        mEMVideo.SetAutoplay(false);
        mEMVideo.SetMute(true);
        mEMVideo.SetLoop(true);
        // mEMVideo.Play();
#else
        mWidth = 500.f;
        mHeight = 500.f;

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

        mSphere.Generate(50.f, 20, 20);

        printf("Video3D::Create\n");
#endif
    }

    void Video3D::operator()(IEvent &event)
    {
#ifdef __EMSCRIPTEN__
        em::MediaEvent &media_event = dynamic_cast<em::MediaEvent &>(event);

        if (media_event.GetType() == em::MediaEvent::Type::ON_CANPLAY)
        {
            printf("Video3D::operator()\n");
            mWidth = mEMVideo.GetWidth();
            mHeight = mEMVideo.GetHeight();

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

            // mResultCol.GenerateMipmaps();

            mSphere.Generate(50.f, 20, 20);

            mIsReady = true;
        }

#endif
    }

    void Video3D::Play()
    {
#ifdef __EMSCRIPTEN__
        mEMVideo.Play();
        printf("Video3D::Play\n");
#endif
    }

    void Video3D::Pause()
    {
#ifdef __EMSCRIPTEN__
        mEMVideo.Pause();
        printf("Video3D::Pause\n");
#endif
    }

    void Video3D::Compose(const glm::vec3 &cont_pos, const glm::quat &cont_rot, const glm::mat4 &model)
    {
        return;
        if (mIsReady)
        {
#ifdef __EMSCRIPTEN__
            if (mEMVideo.GetMediaState() == em::MediaState::PLAYING)
                mEMVideo.UpdateFrame(mVideoCol);
#endif

            mFBO.Bind();

            gl::Render::SetClearColor(1.f, 0.f, 0.f, 1.f);
            gl::Render::Clear(gl::BufferBit::COLOR);

            {
                ImVec2 video_size = ImVec2(mWidth, mHeight);

                ImGui_Impl_2d_to_3d_NewFrame(video_size, ImVec2(-1.f, -1.f));
                ImGui::NewFrame();

                ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
                ImGui::SetNextWindowSize(video_size, ImGuiCond_Always);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
                ImGuiContext &g = *GImGui;
                g.ActiveIdNoClearOnFocusLoss = true;
                if (ImGui::Begin("Video3D", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
                {
                    g.HoveredWindow = ImGui::GetCurrentWindow();

                    ImGui::Image((ImTextureID)mVideoCol.GetID(), video_size);

                    g.HoveredWindow = nullptr;
                }
                ImGui::End();
                ImGui::PopStyleVar(2);

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            mFBO.UnBind();
        }
    }

    void Video3D::Draw()
    {
        return;
#ifdef __EMSCRIPTEN__
        if (mIsReady)
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
            menu_prog->SetMatrix4(mUniformLocations.model, glm::mat4(1.f));
            // menu_prog->SetInt(mUniformLocations.is_video, 0);
            mResultCol.Activate(0);

            gl::Pipeline::DisableBlending();
            // gl::Pipeline::EnableBlending();
            mSphere.Draw();
            // menu_prog->SetInt(mUniformLocations.is_video, 0);
        }
#else
        return;
        {
            mFBO.Bind();

            gl::Render::SetClearColor(1.f, 0.f, 0.f, 1.f);
            gl::Render::Clear(gl::BufferBit::COLOR);

            /*{
                ImVec2 video_size = ImVec2(mWidth, mHeight);

                ImGui_Impl_2d_to_3d_NewFrame(video_size, ImVec2(-1.f, -1.f));
                ImGui::NewFrame();

                ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
                ImGui::SetNextWindowSize(video_size, ImGuiCond_Always);
                if (ImGui::Begin("Video", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
                {
                    ImGui::Image((ImTextureID)mVideoCol.GetID(), video_size);
                }
                ImGui::End();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }*/

            mFBO.UnBind();
        }

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
            menu_prog->SetMatrix4(mUniformLocations.model, glm::mat4(1.f));
            menu_prog->SetInt(mUniformLocations.is_video, 1);
            mResultCol.Activate(0);

            gl::Pipeline::EnableBlending();
            mSphere.Draw();
            gl::Pipeline::DisableBlending();
            menu_prog->SetInt(mUniformLocations.is_video, 0);
        }
#endif
    }
}