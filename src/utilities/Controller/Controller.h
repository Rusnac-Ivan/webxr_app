#ifndef _UTIL_CONTROLLER_H_
#define _UTIL_CONTROLLER_H_

#include <core/Platform.h>
#include <utilities/Shape/Prism.h>
#include <resources/Model.h>
#include <opengl/VertexArray.h>
#include <opengl/VertexBuffer.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace util
{
    class Controller
    {
        struct UniformLocations
        {
            int32_t ray_model = -1;
        };
        UniformLocations mUniformLocations;

        gl::Program *mRayProgram;

        rsrc::Model mModel;
        util::Prism<util::Attrib::POS> mPrism;

        float mRayLength;

        glm::vec3 mPos;
        glm::quat mRot;

    public:
        Controller(/* args */);
        ~Controller();

        const glm::vec3 &GetPos() { return mPos; }
        const glm::quat &GetRot() { return mRot; }

        void Create(const char *path, float ray_length, float ray_width);
        float GetProgress() { return mModel.GetProgress(); }

        void Draw(const glm::vec3 &pos, const glm::quat &orient);
    };

}

#endif