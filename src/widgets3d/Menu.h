#ifndef _W3D_MENU_H_
#define _W3D_MENU_H_

#include <core/Platform.h>
#include <opengl/VertexBuffer.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/VertexArray.h>
#include <opengl/FrameBuffer.h>
#include <opengl/Texture2D.h>

namespace w3d
{
    class Menu
    {
    public:
        using ComposeFun = void (*)();

    private:
        gl::VertexBuffer mVBO;
        gl::VertexArray mVAO;
        gl::IndexBuffer mEBO;

        gl::FrameBuffer mFBO;
        gl::Texture2D *mBaseColor;

        float mWidth;
        float mHeight;

        bool mIsOpen;

    public:
        Menu(/* args */);
        ~Menu();

        void Compose(const char* name, ImGuiWindowFlags flags, ComposeFun fun);
        void Draw(const glm::mat4 &view, const glm::mat4 &proj);
    };

}

#endif