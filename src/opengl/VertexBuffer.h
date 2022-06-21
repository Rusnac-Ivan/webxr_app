#ifndef _GL_VERTEX_BUFFER_H_
#define _GL_VERTEX_BUFFER_H_

#include "Buffer.h"
#include <initializer_list>
#include <string>
#include <vector>
#include <cassert>

namespace gl
{

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		VertexBuffer(const VertexBuffer &) = delete;
		VertexBuffer &operator=(const VertexBuffer &) = delete;
		VertexBuffer(VertexBuffer &&) noexcept;
		VertexBuffer &operator=(VertexBuffer &&) noexcept;

		void Bind();

		void SetData(size_t size, const void *data, UsageMode accessFreq = UsageMode::STATIC_DRAW);
		void SetSubData(size_t offset, size_t size, void *data);
	};
}
#endif
