#ifndef _VERTEX_ATTRIBUTE_H_
#define _VERTEX_ATTRIBUTE_H_

#include "GLUtilities.h"
#include <glm/glm.hpp>

namespace gl
{
	struct VertexAttribute
	{
        DataType type;
        uint16_t components;
        uint16_t entries;
        size_t byteSize;

        template<typename T>
        static VertexAttribute Entry();
	};

    template<>
    inline VertexAttribute VertexAttribute::Entry<float>()
    {
        return { DataType::FLOAT, 1, 1, sizeof(GLfloat) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::vec2>()
    {
        return { DataType::FLOAT, 2, 1, sizeof(glm::vec2) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::vec3>()
    {
        return { DataType::FLOAT, 3, 1, sizeof(glm::vec3) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::vec4>()
    {
        return { DataType::FLOAT, 4, 1, sizeof(glm::vec4) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<int32_t>()
    {
        return { DataType::INT, 1, 1, sizeof(int32_t) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::ivec2>()
    {
        return { DataType::INT, 2, 1, sizeof(glm::ivec2) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::ivec3>()
    {
        return { DataType::INT, 3, 1, sizeof(glm::ivec3) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::ivec4>()
    {
        return { DataType::INT, 4, 1, sizeof(glm::ivec4) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::mat2>()
    {
        return { DataType::FLOAT, 2, 2, sizeof(glm::mat2) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::mat3>()
    {
        return { DataType::FLOAT, 3, 3, sizeof(glm::mat3) };
    }

    template<>
    inline VertexAttribute VertexAttribute::Entry<glm::mat4>()
    {
        return { DataType::FLOAT, 4, 4, sizeof(glm::mat4) };
    }
}





#endif