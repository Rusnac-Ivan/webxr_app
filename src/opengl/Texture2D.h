#ifndef _TEXTURE_2D_H_
#define _TEXTURE_2D_H_

#include "Texture.h"

namespace gl
{
	class Texture2D : public Texture
	{
	public:
		struct Sampler
		{
			gl::Texture::FilterMode magFilter;
			gl::Texture::FilterMode minFilter;
			gl::Texture::WrapMode wrapS;
			gl::Texture::WrapMode wrapT;
		};

	private:
		static constexpr Type mTarget = Type::TARGET_2D;

		uint32_t mWidth;
		uint32_t mHeight;
		Format mFormat;
		uint32_t mMipLevel;
		DataType mDataType;

		WrapMode mWrapModeS;
		WrapMode mWrapModeT;
		FilterMode mMinFilterMode;
		FilterMode mMagFilterMode;

	public:
		Texture2D();
		~Texture2D();

		Texture2D(const Texture2D &) = delete;
		Texture2D &operator=(const Texture2D &) = delete;
		Texture2D(Texture2D &&) noexcept;
		Texture2D &operator=(Texture2D &&) noexcept;

		// void SetWrapModeS(WrapMode mode);
		// void SetWrapModeT(WrapMode mode);
		// void SetMinFilterMode(FilterMode mode);
		// void SetMagFilterMode(FilterMode mode);

		void SetSampler(const Sampler &sampler);

		WrapMode GetWrapModeS() const { return mWrapModeS; }
		WrapMode GetWrapModeT() const { return mWrapModeT; }
		FilterMode GetMinFilterMode() const { return mMinFilterMode; }
		FilterMode GetMagFilterMode() const { return mMagFilterMode; }

		DataType GetPixelDataType() { return mDataType; }
		Format GetFormat() { return mFormat; }
		Type GetTarget() const { return mTarget; }

		void Bind() const;
		static void UnBind();

		void Activate(const unsigned short unit) const;
		void Deactivate(const unsigned short unit) const;

		void LoadData(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);
		void UpdateData(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void *pixels);

		void GenerateMipmaps();
	};
}

#endif
