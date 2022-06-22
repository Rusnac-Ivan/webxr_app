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
		for (uint32_t i = 0; i < ATTACH_COUNT; i++)
		{
			mAttachedTextures[i] = nullptr;
			mAttachedCubeMaps[i] = nullptr;
		}
		if (mID)
		{
			GL(DeleteFramebuffers(1, &mID));
			mID = 0;
		}
		GL(GenFramebuffers(1, &mID));
	}
	void FrameBuffer::Destroy()
	{
		for (uint32_t i = 0; i < ATTACH_COUNT; i++)
		{
			mAttachedTextures[i] = nullptr;
			mAttachedCubeMaps[i] = nullptr;
		}
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
		if (mWidth != width || mHeight != height)
		{
			mWidth = width;
			mHeight = height;

			for (uint32_t i = 0; i < ATTACH_COUNT; i++)
			{
				gl::Texture2D* tex = mAttachedTextures[i];
				if (tex)
					tex->LoadData(0, tex->GetFormat(), mWidth, mHeight, 0, tex->GetFormat(), tex->GetPixelDataType(), nullptr);
				gl::CubeMap* cub_map = mAttachedCubeMaps[i];
				if (cub_map)
					cub_map->SetTargets(0, cub_map->GetFormat(), mWidth, mHeight, 0, cub_map->GetFormat(), cub_map->GetPixelDataType(), true);
			}
		}
	}

	void FrameBuffer::AttachTexture2D(Attachment attachment, Texture2D* texture)
	{
		mAttachedTextures[attachment] = texture;
		if (attachment == Attachment::DEPTH)
		{
			mAttachedTextures[attachment]->LoadData(0, Texture::Format::DEPTH24, mWidth, mHeight, 0, Texture::Format::DEPTH, DataType::UNSIGNED_BYTE, nullptr);
		}
		else if (attachment == Attachment::DEPTH_STENCIL)
		{
			mAttachedTextures[attachment]->LoadData(0, Texture::Format::DEPTH24_STENCIL8, mWidth, mHeight, 0, Texture::Format::DEPTH_STENCIL, DataType::UNSIGNED_BYTE, nullptr);
		}
		else // Color Attachment
		{
			mAttachedTextures[attachment]->LoadData(0, Texture::Format::RGBA, mWidth, mHeight, 0, Texture::Format::RGBA, DataType::UNSIGNED_BYTE, nullptr);
		}

		Bind();
		GL(FramebufferTexture2D(static_cast<GLenum>(mBinding), mAttachments[attachment], static_cast<GLenum>(Texture::Type::TARGET_2D), mAttachedTextures[attachment]->GetID(), 0));
		UnBind();
	}
	void FrameBuffer::AttachCubeMap(Attachment attachment, CubeMap* cube_map)
	{
		mAttachedCubeMaps[attachment] = cube_map;
		if (attachment == Attachment::DEPTH)
		{
			mAttachedCubeMaps[attachment]->SetTargets(0, Texture::Format::DEPTH24, mWidth, mHeight, 0, Texture::Format::DEPTH, DataType::UNSIGNED_BYTE, true);
		}
		else if (attachment == Attachment::DEPTH_STENCIL)
		{
			mAttachedCubeMaps[attachment]->SetTargets(0, Texture::Format::DEPTH24_STENCIL8, mWidth, mHeight, 0, Texture::Format::DEPTH_STENCIL, DataType::UNSIGNED_BYTE, true);
		}
		else // Color Attachment
		{
			mAttachedCubeMaps[attachment]->SetTargets(0, Texture::Format::RGB8, mWidth, mHeight, 0, Texture::Format::RGB, DataType::UNSIGNED_BYTE, true);
		}

		Bind();
		GL(FramebufferTexture(static_cast<GLenum>(mBinding), mAttachments[attachment], mAttachedCubeMaps[attachment]->GetID(), 0));
		UnBind();
	}

	void FrameBuffer::DetachTexture2D(Attachment attachment)
	{
		mAttachedTextures[attachment] = nullptr;

		Bind();
		GL(FramebufferTexture2D(static_cast<GLenum>(mBinding), mAttachments[attachment], static_cast<GLenum>(Texture::Type::TARGET_2D), 0, 0));
		UnBind();
	}

	void FrameBuffer::DetachCubeMap(Attachment attachment)
	{
		mAttachedCubeMaps[attachment] = nullptr;

		Bind();
		GL(FramebufferTexture(static_cast<GLenum>(mBinding), static_cast<GLenum>(attachment), 0, 0));
		UnBind();
	}

	bool FrameBuffer::IsComplete() const
	{
		Bind();
		GLenum status = GL(CheckFramebufferStatus(GL_FRAMEBUFFER));
		UnBind();
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		{
			assert("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" && NULL);
		}
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		{
			assert("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" && NULL);
		}
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		{
			assert("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" && NULL);
		}
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		{
			assert("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" && NULL);
		}
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED)
		{
			assert("GL_FRAMEBUFFER_UNSUPPORTED" && NULL);
		}

		if (status == GL_FRAMEBUFFER_COMPLETE)
			return true;
		else
			return false;
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
		std::vector<GLenum> vattachments;
		for (Attachment attach : attachments)
			vattachments.push_back(mAttachments[attach]);

		Bind();
		GL(DrawBuffers(vattachments.size(), vattachments.data()));
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