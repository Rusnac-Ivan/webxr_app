#include "Prism.h"

namespace util
{
    Prism::Prism() {}

    Prism::~Prism() {}

    void Prism::Generate(float edge_width)
    {
        float a = edge_width;

        glm::vec3 P1 = glm::vec3(-a / 2.f, 0.f, -a / (2.f * sqrt(3.f)));
        glm::vec3 P2 = glm::vec3(0.f, 0.f, a / sqrt(3.f));
        glm::vec3 P3 = glm::vec3(a / 2.f, 0.f, -a / (2.f * sqrt(3.f)));

        glm::vec3 d = glm::vec3(0.f, 1.f, 0.f);

        glm::vec3 P1_, P2_, P3_;

        P1_ = P1 + d;
        P2_ = P2 + d;
        P3_ = P3 + d;

        Vertex v1, v2, v3, v1_, v2_, v3_;

        //triangle I
        glm::vec3 norm_1 = glm::normalize(glm::cross(P2_ - P1, P2 - P1));

        v1.pos = P1;
        v1.normal = norm_1;
        v1.uv0 = glm::vec2(0.f);

        v2.pos = P2;
        v2.normal = norm_1;
        v2.uv0 = glm::vec2(0.f);

        v2_.pos = P2_;
        v2_.normal = norm_1;
        v2_.uv0 = glm::vec2(0.f);


        mVertices.push_back(v1);
        mVertices.push_back(v2);
        mVertices.push_back(v2_);

        //triangle II
        glm::vec3 norm_2 = glm::normalize(glm::cross(P1_ - P1, P2_ - P1));

        v1.pos = P1;
        v1.normal = norm_2;
        v1.uv0 = glm::vec2(0.f);

        v2_.pos = P2_;
        v2_.normal = norm_1;
        v2_.uv0 = glm::vec2(0.f);

        v1_.pos = P1_;
        v1_.normal = norm_1;
        v1_.uv0 = glm::vec2(0.f);

        mVertices.push_back(v1);
        mVertices.push_back(v2_);
        mVertices.push_back(v1_);

        //triangle III
        glm::vec3 norm_3 = glm::normalize(glm::cross(P2_ - P2, P3_ - P2));

        v2.pos = P2;
        v2.normal = norm_3;
        v2.uv0 = glm::vec2(0.f);

        v3_.pos = P3_;
        v3_.normal = norm_3;
        v3_.uv0 = glm::vec2(0.f);

        v2_.pos = P2_;
        v2_.normal = norm_3;
        v2_.uv0 = glm::vec2(0.f);

        mVertices.push_back(v2);
        mVertices.push_back(v3_);
        mVertices.push_back(v2_);

        //triangle IV
        glm::vec3 norm_4 = glm::normalize(glm::cross(P3_ - P2, P3 - P2));

        v2.pos = P2;
        v2.normal = norm_4;
        v2.uv0 = glm::vec2(0.f);

        v3.pos = P3;
        v3.normal = norm_4;
        v3.uv0 = glm::vec2(0.f);

        v3_.pos = P3_;
        v3_.normal = norm_4;
        v3_.uv0 = glm::vec2(0.f);

        mVertices.push_back(v2);
        mVertices.push_back(v3);
        mVertices.push_back(v3_);

        //triangle V
        glm::vec3 norm_5 = glm::normalize(glm::cross(P3_ - P3, P1_ - P3));

        v3.pos = P3;
        v3.normal = norm_5;
        v3.uv0 = glm::vec2(0.f);

        v1_.pos = P1_;
        v1_.normal = norm_5;
        v1_.uv0 = glm::vec2(0.f);

        v3_.pos = P3_;
        v3_.normal = norm_5;
        v3_.uv0 = glm::vec2(0.f);

        mVertices.push_back(v3);
        mVertices.push_back(v1_);
        mVertices.push_back(v3_);

        //triangle VI
        glm::vec3 norm_6 = glm::normalize(glm::cross(P1_ - P3, P1 - P3));

        v3.pos = P3;
        v3.normal = norm_6;
        v3.uv0 = glm::vec2(0.f);

        v1.pos = P1;
        v1.normal = norm_6;
        v1.uv0 = glm::vec2(0.f);

        v1_.pos = P1_;
        v1_.normal = norm_6;
        v1_.uv0 = glm::vec2(0.f);

        mVertices.push_back(v3);
        mVertices.push_back(v1);
        mVertices.push_back(v1_);
    }

    void Prism::Reset()
    {
        mVertices.clear();
    }
}