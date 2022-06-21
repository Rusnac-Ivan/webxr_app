#ifndef _UTIL_CONTROLLER_H_
#define _UTIL_CONTROLLER_H_

#include <core/Platform.h>
#include <utilities/Shape/Prism.h>
#include <resources/Model.h>
#include <opengl/VertexArray.h>
#include <opengl/VertexBuffer.h>
#include <glm/glm.hpp>

namespace util
{
    class Controller
    {
    private:
        rsrc::Model mModel;
        util::Prism mPrism;

        float mRayLength;

        gl::VertexBuffer mVBO;
        gl::VertexArray mVAO;

        uint32_t mVertexCount;

    public:
        Controller(/* args */);
        ~Controller();

        void Create(const char* path, float ray_length, float ray_width);
        float GetProgress() { return mModel.GetProgress(); }

        void Draw(const glm::mat4 &view, const glm::mat4 &proj, const glm::vec3 &pos, const glm::quat &orient);
    };

}

#endif