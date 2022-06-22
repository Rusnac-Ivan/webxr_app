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

	

	class FrameBuffer
	{
	public:
		enum Attachment
		{
			COLOR0,
			COLOR1,
			COLOR2,
			COLOR3,
			COLOR4,
			COLOR5,
			COLOR6,
			COLOR7,
			COLOR8,
			COLOR9,
			COLOR10,
			COLOR11,
			COLOR12,
			COLOR13,
			COLOR14,
			COLOR15,
			COLOR16,
			DEPTH,
			STENCIL,
			DEPTH_STENCIL,
			ATTACH_COUNT
		};
	private:
		static constexpr BindType mBinding = BindType::ReadAndDraw;

		static constexpr GLenum mAttachments[ATTACH_COUNT] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4,
				GL_COLOR_ATTACHMENT5,
				GL_COLOR_ATTACHMENT6,
				GL_COLOR_ATTACHMENT7,
				GL_COLOR_ATTACHMENT8,
				GL_COLOR_ATTACHMENT9,
				GL_COLOR_ATTACHMENT10,
				GL_COLOR_ATTACHMENT11,
				GL_COLOR_ATTACHMENT12,
				GL_COLOR_ATTACHMENT13,
				GL_COLOR_ATTACHMENT14,
				GL_COLOR_ATTACHMENT15,
				GL_COLOR_ATTACHMENT16,
				GL_DEPTH_ATTACHMENT,
				GL_STENCIL_ATTACHMENT,
				GL_DEPTH_STENCIL_ATTACHMENT
			};

		

		unsigned int mID;

		uint32_t mWidth;
		uint32_t mHeight;

		//std::map<Attachment, Texture2D> mAttachedTextures;
		//std::map<Attachment, CubeMap> mAttachedCubeMaps;

		

		Texture2D* mAttachedTextures[ATTACH_COUNT];
		CubeMap* mAttachedCubeMaps[ATTACH_COUNT];
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

		void AttachTexture2D(Attachment attachment, Texture2D* texture);
		void AttachCubeMap(Attachment attachment, CubeMap* cube_map);
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