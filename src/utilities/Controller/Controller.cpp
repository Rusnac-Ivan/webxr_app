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
    Controller::Controller(/* args */) {}

    Controller::~Controller() {}

    void Controller::Create(const char *path, float ray_length, float ray_width)
    {
        mModel.Load(std::string(std::string(path) + "/models/generic_controller.glb").c_str());

        mRayLength = ray_length;
        mPrism.Generate(ray_width);

        mVertexCount = mPrism.GetVertexCount();

        mVBO.SetData(mPrism.GetVertexCount() * sizeof(Vertex), mPrism.GetVertexData());
        mVAO.AddVertexLayout(
            mVBO,
            {
                gl::VertexAttribute::Entry<glm::vec3>(), // position
                gl::VertexAttribute::Entry<glm::vec3>(), // normal
                gl::VertexAttribute::Entry<glm::vec2>(), // uv0 coords
                gl::VertexAttribute::Entry<glm::vec2>()  // uv1 coords
            },
            gl::VertexAttributeRate::PER_VERTEX);
    }

    void Controller::Draw(const glm::mat4 &view, const glm::mat4 &proj, const glm::vec3 &pos, const glm::quat &orient)
    {
        gl::Program *pbr_prog = util::ResourceManager::GetShaders()->GetPBRProg();
        pbr_prog->Use();
        glm::mat4 model = glm::translate(glm::mat4(1.f), pos) * glm::toMat4(orient);
        mModel.Draw(pbr_prog, model);

        gl::Program *ray_prog = util::ResourceManager::GetShaders()->GetRayProg();
        ray_prog->Use();
        ray_prog->SetMatrix4(ray_prog->Uniform("view"), view);
        ray_prog->SetMatrix4(ray_prog->Uniform("projection"), proj);

        glm::mat4 ray_model = model * glm::scale(glm::mat4(1.f), glm::vec3(1.f, mRayLength, 1.f));
        ray_prog->SetMatrix4(ray_prog->Uniform("model"), ray_model);

        gl::Pipeline::EnableBlending();

        mVAO.Bind();
        gl::Render::DrawVertices(gl::Primitive::TRIANGLES, mVertexCount, 0);
        gl::Pipeline::DisableBlending();

        /*gl::Program *program = util::ResourceManager::GetShaders()->GetRayProg();
        program->Use();

        program->SetMatrix4(program->Uniform("view"), view);
        program->SetMatrix4(program->Uniform("projection"), proj);

        glm::mat4 scale_mat(1.f);
        scale_mat = glm::scale(scale_mat, glm::vec3(1.f, mRayLength, 1.f));

        float angle = acos(glm::dot(glm::vec3(0.f, 1.f, 0.f), dir));

        angle = glm::radians(angle);

        glm::vec3 rot_axis = glm::cross(dir, glm::vec3(0.f, 1.f, 0.f));

        glm::mat4 rotation_mat;

        glm::vec3 v = rot_axis;
        v = glm::normalize(v);
        float x, y, z;

        x = v.x;
        y = v.y;
        z = v.z;

        angle = glm::degrees(angle);

        float cos_a = cos(angle);
        float one_minus_cos_a = 1.0f - cos_a;
        float sin_a = sin(angle);

        rotation_mat[0][0] = one_minus_cos_a * x * x + cos_a;
        rotation_mat[0][1] = one_minus_cos_a * x * y - sin_a * z;
        rotation_mat[0][2] = one_minus_cos_a * x * z + sin_a * y;
        rotation_mat[0][3] = 0.0f;
        rotation_mat[1][0] = one_minus_cos_a * y * x + sin_a * z;
        rotation_mat[1][1] = one_minus_cos_a * y * y + cos_a;
        rotation_mat[1][2] = one_minus_cos_a * y * z - sin_a * x;
        rotation_mat[1][3] = 0.0f;
        rotation_mat[2][0] = one_minus_cos_a * z * x - sin_a * y;
        rotation_mat[2][1] = one_minus_cos_a * z * y + sin_a * x;
        rotation_mat[2][2] = one_minus_cos_a * z * z + cos_a;
        rotation_mat[2][3] = 0.0f;
        rotation_mat[3][0] = 0.0f;
        rotation_mat[3][1] = 0.0f;
        rotation_mat[3][2] = 0.0f;
        rotation_mat[3][3] = 1.f;

        glm::mat4 rot_scale_mat;

        rot_scale_mat = rotation_mat * scale_mat;
        glm::mat4 translate = glm::translate(glm::mat4(1.f), pos);

        glm::mat4 transform = translate * rot_scale_mat;

        program->SetMatrix4(program->Uniform("model"), transform);

        mVAO.Bind();
        gl::Render::DrawVertices(gl::Primitive::TRIANGLES, mVertexCount, 0);*/
    }
}