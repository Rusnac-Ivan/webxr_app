#include "VertexBuffer.h"

namespace gl
{

	VertexBuffer::VertexBuffer() {}
	VertexBuffer::~VertexBuffer() {}


	VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept :
		Buffer(std::move(other))
	{}
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
	{
		Buffer::operator=(std::move(other));
		return *this;
	}

	void VertexBuffer::Bind()
	{
		Buffer::Bind(Buffer::Type::ARRAY);
	}

	/*void VertexBuffer::AttributesPattern(std::initializer_list<AttribType> attrib_types)
	{
		assert(attrib_types.size() > 0 && "initializer list is empty");
		if (mAttributes.size() > 0)
		{
			mAttributes.clear();
			mAttributes.reserve(attrib_types.size());
		}
		mStrideSize = 0;
		for (auto type : attrib_types)
		{
			Attribute attrib = MakeAttribute(type);
			attrib.offset = mStrideSize;
			mStrideSize += attrib.numOfComponents * DataSize(attrib.dataType);
			mAttributes.push_back(attrib);
		}
	}*/

	/*void VertexBuffer::AttributesPattern(std::vector<AttribType> attrib_types)
	{
		assert(attrib_types.size() > 0 && "initializer list is empty");
		if (mAttributes.size() > 0)
		{
			mAttributes.clear();
			mAttributes.reserve(attrib_types.size());
		}
		mStrideSize = 0;
		for (auto type : attrib_types)
		{
			Attribute attrib = MakeAttribute(type);
			attrib.offset = mStrideSize;
			mStrideSize += attrib.numOfComponents * DataSize(attrib.dataType);
			mAttributes.push_back(attrib);
		}
	}*/

	void VertexBuffer::SetData(size_t bytesSize, const void* data, UsageMode accessFreq)
	{
		Buffer::Bind(Buffer::Type::ARRAY);
		Buffer::SetData(bytesSize, data, accessFreq);
		UnBind();
	}
	void VertexBuffer::SetSubData(size_t offset, size_t size, void* data)
	{
		Buffer::Bind(Buffer::Type::ARRAY);
		Buffer::SetSubData(offset, size, data);
		UnBind();
	}

	/*VertexBuffer::Attribute VertexBuffer::MakeAttribute(AttribType type)
	{
		switch (type)
		{
			case AttribType::POSITION:
				return Attribute({ DataType::FLOAT, 3, 0, 0, "aPos" });

			case AttribType::NORMAL:
				return Attribute({ DataType::FLOAT, 3, 0, 1, "aNorm" });

			case AttribType::UV_0:
				return Attribute({ DataType::FLOAT, 2, 0, 2, "aUV0" });

			case AttribType::UV_1:
				return Attribute({ DataType::FLOAT, 2, 0, 3, "aUV1" });

			case AttribType::JOINT:
				return Attribute({ DataType::FLOAT, 4, 0, 4, "aJoints" });

			case AttribType::WEIGHT:
				return Attribute({ DataType::FLOAT, 4, 0, 5, "aWeights" });

			case AttribType::COLOR:
				return Attribute({ DataType::FLOAT, 3, 0, 0, "aCol" });

			case AttribType::TANGENT:
				return Attribute({ DataType::FLOAT, 3, 0, 0, "aTan" });

			case AttribType::BITANGENT:
				return Attribute({ DataType::FLOAT, 3, 0, 0, "aBitan" });

			default:
			{
				assert(false && "Invalid attribute type!");
				return Attribute{};
			}
		}
	}*/

}