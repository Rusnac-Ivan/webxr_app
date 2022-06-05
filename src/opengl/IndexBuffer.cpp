#include "IndexBuffer.h"
#include <type_traits>
#include <utility>

namespace gl
{
	IndexBuffer::IndexBuffer()
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	void IndexBuffer::Bind()
	{
		Buffer::Bind(Buffer::Type::ELEMENT_ARRAY);
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept :
		Buffer(std::move(other)),
		mDataType(other.mDataType),
		mIndexCount(other.mIndexCount)
	{
		other.mDataType = DataType::UNSIGNED_INT;
		other.mIndexCount = 0;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
	{
		Buffer::operator=(std::move(other));

		mDataType = other.mDataType;
		mIndexCount = other.mIndexCount;

		other.mDataType = DataType::UNSIGNED_INT;
		other.mIndexCount = 0;

		return *this;
	}


	void IndexBuffer::Data(const unsigned int size, const void* data, DataType type, UsageMode accessFreq)
	{
		mDataType = type;
		this->Bind();
		Buffer::SetData(size, data, accessFreq);
		this->UnBind();
	}
	void IndexBuffer::SubData(const unsigned short offset, const unsigned short& size, const void* data)
	{
		this->Bind();
		Buffer::SetSubData(offset, size, data);
		this->UnBind();
	}
}