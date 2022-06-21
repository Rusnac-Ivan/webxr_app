#ifndef _RSRC_CUBEMAP_H_
#define _RSRC_CUBEMAP_H_

#include <core/Platform.h>
#include <opengl/CubeMap.h>
#include <opengl/VertexArray.h>
#include <opengl/VertexBuffer.h>

namespace rsrc
{
    class CubeMap
    {
    private:
        gl::CubeMap mGLCubeMap;

        static constexpr uint32_t TEXT_COUNT = 6;
        float mProgress[TEXT_COUNT] = {};

        bool mIsLoadedText[TEXT_COUNT] = {};
        bool mIsLoaded;
        bool mIsReady;

        gl::VertexBuffer mVBO;
        gl::VertexArray mVAO;

    public:
        CubeMap();
        ~CubeMap();

        void Load(const char *path);

        float GetProgress();
        void Draw(gl::Program *prog, const glm::mat4 &view, const glm::mat4 &proj);

    private:
        void Loaded(int i);
    };

}

#endif