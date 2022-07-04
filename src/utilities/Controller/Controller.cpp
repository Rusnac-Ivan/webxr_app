#include "Controller.h"

#include <utilities/Shape/Vertex.h>
#include <opengl/VertexAttribute.h>
#include <opengl/Render.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <utilities/Resources/ResourceManager.h>
#include <opengl/Pipeline.h>

namespace util
{
    Controller::Controller(/* args */) : mRayProgram(nullptr) {}

    Controller::~Controller() {}

    void Controller::Create(const char *path, float ray_length, float ray_width)
    {
        mModel.Load(std::string(std::string(path) + "/models/generic_controller.glb").c_str());

        mRayLength = ray_length;
        mPrism.Generate(ray_width);

    }

    void Controller::Draw(const glm::vec3 &pos, const glm::quat &orient)
    {
        gl::Program *pbr_prog = util::ResourceManager::GetShaders()->GetPBRProg();
        pbr_prog->Use();

        glm::mat4 model = glm::translate(glm::mat4(1.f), pos) * glm::toMat4(orient);
        mModel.Draw(model);

        gl::Program *ray_prog = util::ResourceManager::GetShaders()->GetRayProg();
        ray_prog->Use();
        if (mRayProgram != ray_prog)
        {
            mUniformLocations.ray_model = ray_prog->Uniform("model");
            mRayProgram = ray_prog;
        }

        glm::mat4 ray_model = model * glm::scale(glm::mat4(1.f), glm::vec3(1.f, mRayLength, 1.f));
        ray_prog->SetMatrix4(mUniformLocations.ray_model, ray_model);

        gl::Pipeline::EnableBlending();

        mPrism.Draw();
        // gl::Pipeline::DisableBlending();
    }
}