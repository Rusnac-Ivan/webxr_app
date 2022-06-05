#include "Buffer.h"
#include <utility>
#include <cassert>

namespace gl
{

	Buffer::Buffer() : mID(0), mByteSize(0),
		mCurrentTarget(Type::UNKNOWN), 
		mAccessFreq(UsageMode::STATIC_DRAW)
	{
		Create();
	}

	Buffer::~Buffer()
	{
		Destroy();
	}

	Buffer::Buffer(Buffer&& other) noexcept
	{
		this->mID = other.mID;
		this->mByteSize = other.mByteSize;
		this->mCurrentTarget = other.mCurrentTarget;
		this->mAccessFreq = other.mAccessFreq;

		other.mID = 0;
		other.mByteSize = 0;
		other.mCurrentTarget = Type::UNKNOWN;
		other.mAccessFreq = UsageMode::STATIC_DRAW;
		other.Destroy();
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		this->Destroy();

		this->mID = other.mID;
		this->mByteSize = other.mByteSize;
		this->mCurrentTarget = other.mCurrentTarget;
		this->mAccessFreq = other.mAccessFreq;

		other.mID = 0;
		other.mByteSize = 0;
		other.mCurrentTarget = Type::UNKNOWN;
		other.mAccessFreq = UsageMode::STATIC_DRAW;
		other.Destroy();

		return *this;
	}

	void Buffer::Create()
	{
		if (mID != 0)
			Destroy();
		GL(GenBuffers(1, &mID));
	}
	void Buffer::Destroy()
	{
		if (!mID)
		{
			GL(DeleteBuffers(1, &mID));
			mID = 0;
		}
	}

	void Buffer::Bind(Type target)
	{
		mCurrentTarget = target;
		assert(mCurrentTarget != Type::UNKNOWN && "Use undefined binding target!");
		GL(BindBuffer(static_cast<GLenum>(mCurrentTarget), mID));
	}

	void Buffer::UnBind()
	{
		GL(BindBuffer(static_cast<GLenum>(mCurrentTarget), 0));
		mCurrentTarget = Type::UNKNOWN;
	}

	void Buffer::BindBase(Type target, size_t index)
	{
		GL(BindBufferBase(static_cast<GLenum>(mCurrentTarget), index, mID));
	}

	void Buffer::UnBindBase(size_t index)
	{
		GL(BindBufferBase(static_cast<GLenum>(mCurrentTarget), index, 0));
	}

	void Buffer::SetData(const size_t size, const void* data, UsageMode accessFreq)
	{
		mAccessFreq = accessFreq;
		mByteSize = size;
#ifndef NDEBUG
		int currentBinding = 0;
		switch (mCurrentTarget)
		{
			case Type::ARRAY:
			{
				GL(GetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBinding));
			}
			break;
			case Type::ELEMENT_ARRAY:
			{
				GL(GetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentBinding));
			}
			break;
			default:
				break;
		}
		assert(currentBinding == mID && "Not matching Buffer Object!");
		assert(mCurrentTarget != Type::UNKNOWN && "Use undefined binding target!");
#endif

		GL(BufferData(static_cast<GLenum>(mCurrentTarget), size, data, static_cast<GLenum>(mAccessFreq)));
	}

	void Buffer::SetSubData(const size_t offset, const size_t size, const void *data)
	{
#ifndef NDEBUG
		int currentBinding;
		switch (mCurrentTarget)
		{
			case Type::ARRAY:
			{
				GL(GetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBinding));
			}
			break;
			case Type::ELEMENT_ARRAY:
			{
				GL(GetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentBinding));
			}
			break;
			default:
				break;
		}
		assert(currentBinding == mID && "Not matching Buffer Object!");
		assert(mCurrentTarget != Type::UNKNOWN && "Use undefined binding target!");
#endif

		GL(BufferSubData(static_cast<GLenum>(mCurrentTarget), offset, size, data));
	}
	
	void Buffer::SetDataWithResize(size_t byteSize, const void* data)
	{
		if (this->mByteSize < byteSize)
            this->SetData(byteSize, data, this->mAccessFreq);
        else if(this->mByteSize == byteSize)
            this->SetSubData(0, byteSize, data);
		else
			assert(0 && "Not matching Buffer size!");
	}
}

