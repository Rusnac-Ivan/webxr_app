#ifndef _GL_INDEX_BUFFER_H_
#define _GL_INDEX_BUFFER_H_

#include "GLUtilities.h"
#include "Buffer.h"

namespace gl
{
	class IndexBuffer : public Buffer
	{
			uint32_t mIndexCount;
			DataType mDataType;
		public:
			IndexBuffer();
			~IndexBuffer();

			IndexBuffer(const IndexBuffer&) = delete;
			IndexBuffer& operator=(const IndexBuffer&) = delete;
			IndexBuffer(IndexBuffer&&) noexcept;
			IndexBuffer& operator=(IndexBuffer&&) noexcept;

			void SetIndexCount(uint32_t count) { mIndexCount = count; }
			uint32_t GetIndexCount() const { return  mIndexCount; }
			DataType GetDataType() const { return mDataType; }

			void Bind();

			void Data(const unsigned int size, const void* data, DataType type, UsageMode accessFreq = Buffer::UsageMode::STATIC_DRAW);
			void SubData(const unsigned short offset, const unsigned short& size, const void* data);
		private:

	};

}

#endif