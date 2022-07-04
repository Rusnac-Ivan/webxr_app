#ifndef _W3D_Video2D_H_
#define _W3D_Video2D_H_

#include <core/Platform.h>
#include <opengl/VertexBuffer.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/VertexArray.h>
#include <opengl/FrameBuffer.h>
#include <opengl/Texture2D.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <utilities/Shape/Plane.h>
#include <utilities/Emsc/Multimedia.h>
#include <events/IEventListener.h>

namespace w3d
{
    class Video2D : public IEventListener
    {
        struct UniformLocations
        {
            int32_t model = -1;
            int32_t is_video = -1;
        };
        UniformLocations mUniformLocations;

    private:
        static constexpr util::Attrib::underlying_type OPTIONS = util::Attrib::POS | util::Attrib::UV;

        gl::Program *mProgram;

        gl::FrameBuffer mFBO;
        gl::Texture2D mResultCol;
        gl::Texture2D mVideoCol;

        float mWidth;
        float mHeight;

        util::Plane<OPTIONS> mPlane;

        glm::mat4 mModel;

#ifdef __EMSCRIPTEN__
        em::Video mEMVideo;
#endif

    public:
        Video2D(/* args */);
        ~Video2D();

        void Create(const char *url);

        virtual void operator()(IEvent &event);

        void Draw(const glm::mat4 &model);
    };

} // namespace w3d

#endif