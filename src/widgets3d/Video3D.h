#ifndef _W3D_VIDEO_3D_H_
#define _W3D_VIDEO_3D_H_

#include <core/Platform.h>
#include <opengl/VertexBuffer.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/VertexArray.h>
#include <opengl/FrameBuffer.h>
#include <opengl/Texture2D.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <utilities/Shape/Sphere.h>
#include <utilities/Emsc/Multimedia.h>
#include <events/IEventListener.h>
#include <utilities/Emsc/webxr.h>

namespace w3d
{

    class Video3D : public IEventListener
    {
    private:
        struct UniformLocations
        {
            int32_t model = -1;
            int32_t is_video = -1;
        };
        UniformLocations mUniformLocations;

        static constexpr util::Attrib::underlying_type OPTIONS = util::Attrib::POS | util::Attrib::UV;

        gl::Program *mProgram;

        gl::FrameBuffer mFBO;
        gl::Texture2D mResultCol;
        gl::Texture2D mVideoCol;

        util::Sphere<OPTIONS> mSphere;

#ifdef __EMSCRIPTEN__
        em::Video mEMVideo;
#endif
        float mWidth;
        float mHeight;

        bool mIsReady;

    public:
        Video3D(/* args */);
        ~Video3D();

        void Play();
        void Pause();
#ifdef __EMSCRIPTEN__
        bool IsPlaying() { return mEMVideo.GetMediaState() == em::MediaState::PLAYING; }
#endif

        void Create(const char *url);

        void Compose(WebXRInputSource *inputSource, const glm::mat4 &model);

        void Draw();

        virtual void operator()(IEvent &event);
    };

}

#endif