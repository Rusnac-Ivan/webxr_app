#ifndef _GL_VERTEX_ARRAY_H_
#define _GL_VERTEX_ARRAY_H_

#include "GLUtilities.h"
#include <vector>

namespace gl
{
	class VertexBuffer;
	class IndexBuffer;
	struct VertexAttribute;

	enum class VertexAttributeRate
	{
		PER_VERTEX,
		PER_INSTANCE
	};

	class VertexArray
	{
	private:
		unsigned int mID;
		unsigned int mAttributeIndex;
		size_t mVertexCount;

	public:
		VertexArray();
		~VertexArray();

		void Create();
		void Destroy();

		VertexArray(const VertexArray &) = delete;
		VertexArray(VertexArray &&array) noexcept;
		VertexArray &operator=(const VertexArray &) = delete;
		VertexArray &operator=(VertexArray &&array) noexcept;

		void Bind();
		void UnBind();

		void AddVertexLayout(VertexBuffer &buffer, std::vector<VertexAttribute> layout, VertexAttributeRate inputRate = VertexAttributeRate::PER_VERTEX);
		void RemoveVertexLayout(std::vector<VertexAttribute> layout);
		void LinkIndexBuffer(IndexBuffer &buffer);

		int GetAttributeCount() const { return mAttributeIndex; }
	};
}

#endif