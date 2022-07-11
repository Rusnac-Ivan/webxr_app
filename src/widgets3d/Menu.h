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



namespace w3d
{
    constexpr util::Attrib::underlying_type OPTIONS = util::Attrib::POS | util::Attrib::UV;

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
        gl::Program *mProgram;

        gl::FrameBuffer mFBO;
        gl::Texture2D mBaseColor;

        //ImGuiContext* mImGuiContext;

        float mWidth;
        float mHeight;

        bool mIsOpen;

        util::Plane<OPTIONS> mPlane;

        glm::mat4 mModel;

    public:
        Menu();
        ~Menu();

        void Create(float widthPX, float heightPX, float mm_per_px);

        void Compose(WebXRInputSource *inputSource, const glm::mat4 &model, const char *name, ComposeFun gui_fun, ImGuiWindowFlags window_flags = 0);
        void Draw();
    };

}

#endif