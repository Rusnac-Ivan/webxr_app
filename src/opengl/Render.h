#ifndef _GL_RENDER_CONTEXT_H_
#define _GL_RENDER_CONTEXT_H_

#include "GLUtilities.h"
#include <glm/glm.hpp>

namespace gl
{
	enum  BufferBit
	{
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT
	};

	enum struct Primitive
	{
		POINTS = GL_POINTS,
		LINE_STRIP = GL_LINE_STRIP,
		LINE_LOOP = GL_LINE_LOOP,
		LINES = GL_LINES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		TRIANGLE_FAN = GL_TRIANGLE_FAN,
		TRIANGLES = GL_TRIANGLES
	};
	

	class Render
	{
		Render() = delete;
		~Render() = delete;
		Render(Render&) = delete;

	public:
		static void Clear(const BufferBit buffBit);
		static void Clear(const BufferBit buffBit_0, const BufferBit buffBit_1);
		static void Clear(const BufferBit buffBit_0, const BufferBit buffBit_1, const BufferBit buffBit_2);

		static void SetViewport(unsigned int width, unsigned int height);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		static void SetClearColor(const float red, const float green, const float blue, const float alpha);
		static void SetClearColor(const glm::vec4 &color);
		static void SetClearDepth(const float val);
		static void SetClearStencil(const int val);

		static void DrawVertices(Primitive primitive, size_t vertexCount, size_t vertexOffset);
		static void DrawIndices(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset);
		static void DrawVerticesInstanced(Primitive primitive, size_t vertexCount, size_t vertexOffset, size_t instanceCount, size_t baseInstance);
		static void DrawIndicesInstanced(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset, size_t instanceCount, size_t baseInstance);
		static void DrawIndicesBaseVertex(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset, size_t baseVertex);
		static void DrawIndicesBaseVertexInstanced(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset, size_t baseVertex, size_t instanceCount, size_t baseInstance);

		static void Flush();
		static void Finish();

	private:
	};

	inline void Render::Clear(const BufferBit buffBit)
	{
		GL(Clear(buffBit));
	}

	inline void Render::Clear(const BufferBit buffBit_0, const BufferBit buffBit_1)
	{
		GL(Clear(buffBit_0 | buffBit_1));
	}
	inline void Render::Clear(const BufferBit buffBit_0, const BufferBit buffBit_1, const BufferBit buffBit_2)
	{
		GL(Clear(buffBit_0 | buffBit_1 | buffBit_2));
	}

	inline void Render::SetViewport(unsigned int width, unsigned int height)
	{
		GL(Viewport(0, 0, width, height));
	}

	inline void Render::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		GL(Viewport(x, y, width, height));
	}

	inline void Render::SetClearColor(const float red, const float green, const float blue, const float alpha)
	{
		GL(ClearColor(red, green, blue, alpha));
	}

	inline void Render::SetClearColor(const glm::vec4 &color)
	{
		GL(ClearColor(color.r, color.g, color.b, color.a));
	}

	inline void Render::SetClearDepth(const float val)
	{
		GL(ClearDepth(val));
	}

	inline void Render::SetClearStencil(const int val)
	{
		GL(ClearStencil(val));
	}

	inline void Render::DrawVertices(Primitive primitive, size_t vertexCount, size_t vertexOffset)
	{
		GL(DrawArrays(static_cast<GLenum>(primitive), vertexOffset, vertexCount));
	}

	inline void Render::DrawIndices(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset)
	{
		GL(DrawElements(
			static_cast<GLenum>(primitive),
			indexCount,
			static_cast<GLenum>(type),
			BUFFER_OFFSET(indexOffset * DataSize(type))
		));
	}

	inline void Render::DrawVerticesInstanced(Primitive primitive, size_t vertexCount, size_t vertexOffset, size_t instanceCount, size_t baseInstance)
	{
		GL(DrawArraysInstancedBaseInstance(
			static_cast<GLenum>(primitive),
			vertexOffset,
			vertexCount,
			instanceCount,
			baseInstance
		));
	}

	inline void Render::DrawIndicesInstanced(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset, size_t instanceCount, size_t baseInstance)
	{
		GL(DrawElementsInstancedBaseInstance(
			static_cast<GLenum>(primitive),
			indexCount,
			static_cast<GLenum>(type),
			(const void*)(indexOffset * DataSize(type)),
			instanceCount,
			baseInstance
		));
	}

	inline void Render::DrawIndicesBaseVertex(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset, size_t baseVertex)
	{
		GL(DrawElementsBaseVertex(
			static_cast<GLenum>(primitive),
			indexCount,
			static_cast<GLenum>(type),
			(const void*)(indexOffset * DataSize(type)),
			baseVertex
		));
	}


	inline void Render::DrawIndicesBaseVertexInstanced(Primitive primitive, size_t indexCount, DataType type, size_t indexOffset, size_t baseVertex, size_t instanceCount, size_t baseInstance)
	{
		GL(DrawElementsInstancedBaseVertexBaseInstance(
			static_cast<GLenum>(primitive),
			indexCount,
			static_cast<GLenum>(type),
			(const void*)(indexOffset * DataSize(type)),
			instanceCount,
			baseVertex,
			baseInstance
		));
	}

	inline void Render::Flush()
	{
		GL(Flush());
	}

	inline void Render::Finish()
	{
		GL(Finish());
	}
}

#endif