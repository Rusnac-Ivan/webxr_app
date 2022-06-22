#ifndef _W3D_MENU_H_
#define _W3D_MENU_H_

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
#include <utilities/Emsc/webxr.h>

#ifndef __EMSCRIPTEN__
typedef void* WebXRInputSource;
#endif

namespace w3d
{
    class Menu
    {
        struct UniformLocations
        {
            int32_t model = -1;
        };
        UniformLocations mUniformLocations;
    public:
        using ComposeFun = void (*)();

    private:
        gl::VertexBuffer mVBO;
        gl::VertexArray mVAO;
        gl::IndexBuffer mEBO;

        gl::Program *mProgram;

        gl::FrameBuffer mFBO;
        gl::Texture2D mBaseColor;

        float mWidth;
        float mHeight;

        bool mIsOpen;

        util::Plane mPlane;

    public:
        Menu();
        ~Menu();

        void Create(float width, float height);

        void Compose(WebXRInputSource *inputSource, const char *name, ComposeFun gui_fun);
        void Draw(const glm::mat4 &model);
    };

}

#endif