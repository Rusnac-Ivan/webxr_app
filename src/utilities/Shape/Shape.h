#ifndef _UTIL_SHAPE_H_
#define _UTIL_SHAPE_H_

#include <vector>
#include <opengl/VertexBuffer.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/VertexArray.h>
#include <glm/glm.hpp>
#include <opengl/Render.h>
#include <opengl/VertexAttribute.h>

namespace util
{
    namespace Attrib
    {
        using underlying_type = uint8_t;
        enum : underlying_type
        {
            POS = 1 << 0,
            NORM = 1 << 1,
            UV = 1 << 2,
            TAN = 1 << 3
        };
    }

    /*enum Attrib : uint8_t
    {
        ATTRIB_NONE = 0X00,
        ATTRIB_POS = 1 << 0,
        ATTRIB_NORM = 1 << 1,
        ATTRIB_UV = 1 << 2,
        ATTRIB_TAN = 1 << 3
    };*/

    template <Attrib::underlying_type Options>
    struct Vertex;

    template <>
    struct Vertex<Attrib::POS>
    {
        glm::vec3 pos;
    };

    template <>
    struct Vertex<Attrib::POS | Attrib::NORM>
    {
        glm::vec3 pos;
        glm::vec3 norm;
    };

    template <>
    struct Vertex<Attrib::POS | Attrib::UV>
    {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    template <>
    struct Vertex<Attrib::POS | Attrib::NORM | Attrib::UV>
    {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec2 uv;
    };

    template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
    class Shape
    {
    public:
        using vertex_t = Vertex<Options>;

    protected:
        std::vector<vertex_t> mVertices;
        std::vector<uint32_t> mIndices;

        uint32_t mVerticesCount;
        uint32_t mIndicesCount;

        gl::VertexBuffer mVBO;
        gl::VertexArray mVAO;
        gl::IndexBuffer mEBO;

    public:
        Shape(/* args */);
        virtual ~Shape();

        const float *GetVertexData() const { return reinterpret_cast<const float *>(mVertices.size() > 0 ? mVertices.data() : nullptr); }
        uint32_t GetVertexCount() const { return mVerticesCount; }
        uint32_t GetVertexDataSize() { return GetVertexCount() * sizeof(vertex_t); }

        const uint32_t *GetIndicesData() const { return reinterpret_cast<const uint32_t *>(mIndices.size() > 0 ? mIndices.data() : nullptr); }
        uint32_t GetIndicesCount() const { return mIndicesCount; }

        void Draw();

    protected:
        virtual void Generate() = 0;

    protected:
        void Reset();

        void MakeGLVAO();
    };

    template <Attrib::underlying_type Options>
    Shape<Options>::Shape(/* args */)
    {
        mVerticesCount = 0;
        mIndicesCount = 0;
    }

    template <Attrib::underlying_type Options>
    Shape<Options>::~Shape()
    {
    }

    template <Attrib::underlying_type Options>
    void Shape<Options>::Reset()
    {
        mVertices.clear();
        mIndices.clear();
    }

    template <Attrib::underlying_type Options>
    void Shape<Options>::MakeGLVAO()
    {
        mVerticesCount = mVertices.size();
        mIndicesCount = mIndices.size();

        mVBO.SetData(mVerticesCount * sizeof(vertex_t), reinterpret_cast<const void *>(mVertices.size() > 0 ? mVertices.data() : nullptr));
        mEBO.Data(sizeof(uint32_t) * mIndicesCount, reinterpret_cast<const uint32_t *>(mIndices.size() > 0 ? mIndices.data() : nullptr), gl::DataType::UNSIGNED_INT);
        switch (Options)
        {
        case Attrib::POS:
        {
            mVAO.AddVertexLayout(mVBO, {gl::VertexAttribute::Entry<glm::vec3>()});
        }
        break;
        case Attrib::POS | Attrib::NORM:
        {
            mVAO.AddVertexLayout(mVBO, {gl::VertexAttribute::Entry<glm::vec3>(),
                                        gl::VertexAttribute::Entry<glm::vec3>()});
        }
        break;
        case Attrib::POS | Attrib::UV:
        {
            mVAO.AddVertexLayout(mVBO, {gl::VertexAttribute::Entry<glm::vec3>(),
                                        gl::VertexAttribute::Entry<glm::vec2>()});
        }
        break;
        case Attrib::POS | Attrib::NORM | Attrib::UV:
        {
            mVAO.AddVertexLayout(mVBO, {gl::VertexAttribute::Entry<glm::vec3>(),
                                        gl::VertexAttribute::Entry<glm::vec3>(),
                                        gl::VertexAttribute::Entry<glm::vec2>()});
        }
        break;
        default:
            break;
        }
        if (mIndicesCount > 0)
            mVAO.LinkIndexBuffer(mEBO);

        Reset();
    }

    template <Attrib::underlying_type Options>
    void Shape<Options>::Draw()
    {
        mVAO.Bind();
        if (mIndicesCount > 0)
            gl::Render::DrawIndices(gl::Primitive::TRIANGLES, mIndicesCount, mEBO.GetDataType(), 0);
        else if (mVerticesCount > 0)
            gl::Render::DrawVertices(gl::Primitive::TRIANGLES, mVerticesCount, 0);
    }
}

#endif