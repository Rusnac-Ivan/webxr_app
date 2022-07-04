#include "Video2D.h"

#include <opengl/Render.h>
#include <opengl/Pipeline.h>
#include <backends/imgui_impl_opengl3.h>
#include <utilities/Resources/ResourceManager.h>
#include "ImGui_Impl_2d_to_3d.h"

namespace w3d
{
    Video2D::Video2D(/* args */)
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
        mEMVideo.SetAutoplay(true);
        mEMVideo.SetMute(true);
        mEMVideo.SetLoop(false);
        mEMVideo.Play();

#endif
    }

    void Video2D::operator()(IEvent &event)
    {
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
        }
    }

    void Video2D::Draw(const glm::mat4 &model)
    {
#ifdef __EMSCRIPTEN__
        if (mEMVideo.GetState() != MediaState::NONE)
        {
            {
                mEMVideo.UpdateFrame(mVideoCol);

                mFBO.Bind();

                gl::Render::SetClearColor(0.f, 0.f, 0.f, 0.f);
                gl::Render::Clear(gl::BufferBit::COLOR);

                {
                    // ImGui_Impl_2d_to_3d_NewFrame(ImVec2(mWidth, mHeight), ImVec2(-1.f, -1.f));
                    ImGui::NewFrame();

                    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
                    ImGui::SetNextWindowSize(ImVec2(mWidth, mHeight), ImGuiCond_Always);
                    if (ImGui::Begin("Video", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
                    {
                        ImGui::Image((ImTextureID)mVideoCol.GetID(), ImVec2(mWidth, mHeight));
                    }
                    ImGui::End();

                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                }

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
                menu_prog->SetMatrix4(mUniformLocations.model, model);
                menu_prog->SetInt(mUniformLocations.is_video, 1);
                mResultCol.Activate(0);
                mPlane.Draw();
                menu_prog->SetInt(mUniformLocations.is_video, 0);
            }
        }
#endif
    }

}