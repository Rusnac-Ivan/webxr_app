#ifndef _UTIL_CUBE_H_
#define _UTIL_CUBE_H_

#include <vector>
#include "Vertex.h"

namespace util
{
    class Cube
    {
    private:
        std::vector<Vertex> mVertices;

    public:
        Cube();
        ~Cube();

        void Generate(float edge_size);
        void Reset();

        const float *GetVertexData() const { return reinterpret_cast<const float *>(mVertices.size() > 0 ? mVertices.data() : nullptr); }
        uint32_t GetVertexCount() const { return mVertices.size(); }
    };

    Cube::Cube(/* args */)
    {
    }

    Cube::~Cube()
    {
    }

}

#endif