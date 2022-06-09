#include "Sphere.h"
#define M_PI       3.14159265358979323846   // pi

namespace util
{
    Sphere::Sphere()
    {
    }

    Sphere::~Sphere()
    {
    }

    void Sphere::Generate(float radius)
    {
        //std::vector<Vertex> vertices;

        int sectorCount = 20;
        int stackCount = 20;

        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.f / radius;

        float sectorStep = 2.f * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = M_PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;
                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;

                // vertex tex coord (u, v) range between [0, 1]
                float u = (float)j / sectorCount;
                float v = (float)i / stackCount;

                Vertex vertex;
                vertex.pos = glm::vec3(x, y, z);
                vertex.normal = glm::vec3(nx, ny, nz);
                vertex.uv0 = glm::vec2(u, v);

                mVertices.push_back(vertex);
            }
        }
        /*for (int i = 0, k1, k2; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;
            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    mVertices.push_back(vertices[k1]);
                    mVertices.push_back(vertices[k2]);
                    mVertices.push_back(vertices[k1 + 1]);
                }
                if (i != (stackCount - 1))
                {
                    mVertices.push_back(vertices[k1 + 1]);
                    mVertices.push_back(vertices[k2]);
                    mVertices.push_back(vertices[k2 + 1]);
                }
            }
        }*/

        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0)
                {
                    mIndices.push_back(k1);
                    mIndices.push_back(k2);
                    mIndices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if (i != (stackCount - 1))
                {
                    mIndices.push_back(k1 + 1);
                    mIndices.push_back(k2);
                    mIndices.push_back(k2 + 1);
                }
            }
        }
    }

    void Sphere::Reset()
    {
        mVertices.clear();
        mIndices.clear();
    }
}