#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_

#include "GLUtilities.h"
#include "Texture2D.h"
#include "CubeMap.h"
#include <initializer_list>


namespace gl
{
	class Texture2D;
	class CubeMap;

	enum struct BindType
	{
		ReadAndDraw = GL_FRAMEBUFFER,
		Draw = GL_DRAW_FRAMEBUFFER,
		Read = GL_READ_FRAMEBUFFER,
	};

	enum struct Attachment
	{
		COLOR0 = GL_COLOR_ATTACHMENT0,
		COLOR1 = GL_COLOR_ATTACHMENT1,
		COLOR2 = GL_COLOR_ATTACHMENT2,
		COLOR3 = GL_COLOR_ATTACHMENT3,
		COLOR4 = GL_COLOR_ATTACHMENT4,
		COLOR5 = GL_COLOR_ATTACHMENT5,
		COLOR6 = GL_COLOR_ATTACHMENT6,
		COLOR7 = GL_COLOR_ATTACHMENT7,
		COLOR8 = GL_COLOR_ATTACHMENT8,
		COLOR9 = GL_COLOR_ATTACHMENT9,
		COLOR10 = GL_COLOR_ATTACHMENT10,
		COLOR11 = GL_COLOR_ATTACHMENT11,
		COLOR12 = GL_COLOR_ATTACHMENT12,
		COLOR13 = GL_COLOR_ATTACHMENT13,
		COLOR14 = GL_COLOR_ATTACHMENT14,
		COLOR15 = GL_COLOR_ATTACHMENT15,
		COLOR16 = GL_COLOR_ATTACHMENT16,
		DEPTH = GL_DEPTH_ATTACHMENT,
		STENCIL = GL_STENCIL_ATTACHMENT,
		DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT
	};

	class FrameBuffer
	{
		static constexpr BindType mBinding = BindType::ReadAndDraw;

		unsigned int mID;

		uint32_t mWidth;
		uint32_t mHeight;

		//std::map<Attachment, Texture2D> mAttachedTextures;
		//std::map<Attachment, CubeMap> mAttachedCubeMaps;

		//Texture2D* mAttachedTextures[]
	public:
		FrameBuffer();
		~FrameBuffer();
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) noexcept;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) noexcept;

		void SetSize(uint32_t width, uint32_t height);

		void Bind() const;
		void UnBind() const;

		void AttachTexture2D(Attachment attachment);
		void AttachCubeMap(Attachment attachment);
		void DetachTexture2D(Attachment attachment);
		void DetachCubeMap(Attachment attachment);

		void UseDrawBuffers(const std::initializer_list<Attachment>& attachments);
		void UseDrawBuffers(const std::vector<Attachment>& attachments);
		void UseOnlyDepth();

		//bool HasTextureAttached() const { return mAttachedTextures.size() > 0; }
		//bool HasCubeMapAttached() const { return mAttachedCubeMaps.size() > 0; }

		bool IsComplete() const;

	private:
		void DetachRenderTarget();

		void Create();
		void Destroy();
		
	};


	
}


#endif