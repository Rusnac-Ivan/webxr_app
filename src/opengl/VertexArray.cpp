#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexAttribute.h"

namespace gl
{
	VertexArray::VertexArray() : mID(0), mAttributeIndex(0)
	{
		Create();
	}

	VertexArray::~VertexArray()
	{
		if (!mID)
		{
			Destroy();
			mID = 0;
		}
	}

	void VertexArray::Create()
	{
		GL(GenVertexArrays(1, &mID));
	}

	void VertexArray::Destroy()
	{
		GL(DeleteVertexArrays(1, &mID));
	}

	VertexArray::VertexArray(VertexArray&& array) noexcept
	{
		if (!mID)
		{
			Destroy();
			mID = 0;
		}

		this->mID = array.mID;
		this->mAttributeIndex = array.mAttributeIndex;
		this->mVertexCount = array.mVertexCount;

		array.mID = 0;
		array.mAttributeIndex = 0;
		array.mVertexCount = 0;
	}

	VertexArray& VertexArray::operator=(VertexArray&& array) noexcept
	{
		if (!mID)
		{
			Destroy();
			mID = 0;
		}

		this->mID = array.mID;
		this->mAttributeIndex = array.mAttributeIndex;
		this->mVertexCount = array.mVertexCount;

		array.mID = 0;
		array.mAttributeIndex = 0;
		array.mVertexCount = 0;

		return *this;
	}

	void VertexArray::Bind()
	{
		GL(BindVertexArray(mID));
	}

	void VertexArray::UnBind()
	{
		GL(BindVertexArray(0));
	}

	void VertexArray::AddVertexLayout(VertexBuffer& buffer, std::vector<VertexAttribute> layout, VertexAttributeRate inputRate)
	{
		this->Bind();
		buffer.Bind();

		size_t offset = 0;
		size_t stride = 0;

		for (const auto& element : layout)
			stride += element.byteSize;

		for (const auto& element : layout)
		{
			for (size_t i = 0; i < element.entries; i++)
			{
				// TODO: handle integer case with glVertexAttribIPointer
				GL(EnableVertexAttribArray(this->mAttributeIndex));
				GL(VertexAttribPointer(this->mAttributeIndex, element.components, (GLenum)element.type, GL_FALSE, stride, (void*)offset));
				if (inputRate == VertexAttributeRate::PER_INSTANCE)
				{
					GL(VertexAttribDivisor(this->mAttributeIndex, 1));
				}

				offset += element.byteSize / element.entries;
				this->mAttributeIndex++;
			}
		}
		this->UnBind();
	}

	void VertexArray::RemoveVertexLayout(std::vector<VertexAttribute> layout)
	{
		assert(this->mAttributeIndex > layout.size());

		this->Bind();
		for (const auto& element : layout)
		{
			for (size_t i = 0; i < element.entries; i++)
			{
				this->mAttributeIndex--;
				GL(DisableVertexAttribArray(this->mAttributeIndex));
			}
		}
		this->UnBind();
	}

	void VertexArray::LinkIndexBuffer(IndexBuffer& buffer)
	{
		this->Bind();
		buffer.Bind();
		this->UnBind();
	}
}
