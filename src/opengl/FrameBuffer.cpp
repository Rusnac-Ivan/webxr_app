#include "Framebuffer.h"

namespace gl
{
	FrameBuffer::FrameBuffer() : mID(0), mWidth(0), mHeight(0)
	{
		Create();
	}
	FrameBuffer::~FrameBuffer()
	{
		Destroy();
	}

	void FrameBuffer::Create()
	{
		if (mID)
		{
			GL(DeleteFramebuffers(1, &mID));
			mID = 0;
		}
		GL(GenFramebuffers(1, &mID));
	}
	void FrameBuffer::Destroy()
	{
		if (mID)
			GL(DeleteFramebuffers(1, &mID));
	}

	void FrameBuffer::Bind() const
	{
		GL(BindFramebuffer(static_cast<GLenum>(mBinding), mID));
	}
	void FrameBuffer::UnBind() const
	{
		GL(BindFramebuffer(static_cast<GLenum>(mBinding), 0));
	}

	void FrameBuffer::SetSize(uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;

		if (mAttachedTextures.size() > 0)
		{
			for (auto& attached_texture : mAttachedTextures)
			{
				Texture2D& texture = attached_texture.second;
				texture.LoadData(0, texture.GetFormat(), mWidth, mHeight, 0, texture.GetFormat(), texture.GetPixelDataType(), nullptr);
			}
		}
		if (mAttachedCubeMaps.size() > 0)
		{
			for (auto& attached_cubemap : mAttachedCubeMaps)
			{
				CubeMap& cubemap = attached_cubemap.second;
				cubemap.SetTargets(0, cubemap.GetFormat(), mWidth, mHeight, 0, cubemap.GetFormat(), cubemap.GetPixelDataType(), true);
			}
		}
	}

	void FrameBuffer::AttachTexture2D(Attachment attachment = Attachment::COLOR0)
	{
		if (attachment == Attachment::DEPTH)
		{
			mAttachedTextures[attachment].LoadData(0, Texture::Format::DEPTH24, mWidth, mHeight, 0, Texture::Format::DEPTH, DataType::UNSIGNED_BYTE, nullptr);
		}
		else if (attachment == Attachment::DEPTH_STENCIL)
		{
			mAttachedTextures[attachment].LoadData(0, Texture::Format::DEPTH24_STENCIL8, mWidth, mHeight, 0, Texture::Format::DEPTH_STENCIL, DataType::UNSIGNED_BYTE, nullptr);
		}
		else // Color Attachment
		{
			mAttachedTextures[attachment].LoadData(0, Texture::Format::RGB8, mWidth, mHeight, 0, Texture::Format::RGB, DataType::UNSIGNED_BYTE, nullptr);
		}

		Bind();
		GL(FramebufferTexture2D(static_cast<GLenum>(mBinding), static_cast<GLenum>(attachment), static_cast<GLenum>(Texture::Type::TARGET_2D), mAttachedTextures[attachment].GetID(), 0));
		UnBind();
	}
	void FrameBuffer::AttachCubeMap(Attachment attachment = Attachment::COLOR0)
	{
		if (attachment == Attachment::DEPTH)
		{
			mAttachedCubeMaps[attachment].SetTargets(0, Texture::Format::DEPTH24, mWidth, mHeight, 0, Texture::Format::DEPTH, DataType::UNSIGNED_BYTE, true);
		}
		else if (attachment == Attachment::DEPTH_STENCIL)
		{
			mAttachedCubeMaps[attachment].SetTargets(0, Texture::Format::DEPTH24_STENCIL8, mWidth, mHeight, 0, Texture::Format::DEPTH_STENCIL, DataType::UNSIGNED_BYTE, true);
		}
		else // Color Attachment
		{
			mAttachedCubeMaps[attachment].SetTargets(0, Texture::Format::RGB8, mWidth, mHeight, 0, Texture::Format::RGB, DataType::UNSIGNED_BYTE, true);
		}

		Bind();
		GL(FramebufferTexture(static_cast<GLenum>(mBinding), static_cast<GLenum>(attachment), mAttachedCubeMaps[attachment].GetID(), 0));
		UnBind();
	}

	void FrameBuffer::DetachTexture2D(Attachment attachment)
	{
		mAttachedTextures.erase(attachment);

		Bind();
		GL(FramebufferTexture2D(static_cast<GLenum>(mBinding), static_cast<GLenum>(attachment), static_cast<GLenum>(Texture::Type::TARGET_2D), 0, 0));
		UnBind();
	}

	void FrameBuffer::DetachCubeMap(Attachment attachment)
	{
		mAttachedCubeMaps.erase(attachment);

		Bind();
		GL(FramebufferTexture(static_cast<GLenum>(mBinding), static_cast<GLenum>(attachment), 0, 0));
		UnBind();
	}

	bool FrameBuffer::IsComplete() const
	{
		bool is_complete = true;
		Bind();
		if (glCheckFramebufferStatus(static_cast<GLenum>(mBinding)) != GL_FRAMEBUFFER_COMPLETE)
			is_complete = false;
		UnBind();
		return is_complete;
	}

	/*template<typename T, typename... Args>
	T vmin(T arg1, Args&&... args)
	{
		T* p[] = { &arg1, &args... };

		return **std::min_element(begin(p), end(p),
			[](T* a, T* b) { return *a < *b; });
	}*/

	void FrameBuffer::UseDrawBuffers(const std::vector<Attachment>& attachments)
	{
		Bind();
		GL(DrawBuffers((GLsizei)attachments.size(), reinterpret_cast<const GLenum*>(attachments.data())));
		UnBind();
	}

	void FrameBuffer::UseDrawBuffers(const std::initializer_list<Attachment>& attachments)
	{
		std::vector<Attachment> vattachments;
		vattachments.insert(vattachments.end(), attachments.begin(), attachments.end());
		UseDrawBuffers(vattachments);
	}
	void FrameBuffer::UseOnlyDepth()
	{
		Bind();
		GL(DrawBuffer(GL_NONE));
		UnBind();
	}
}