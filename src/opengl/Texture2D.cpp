#include "Texture2D.h"

namespace gl
{
	Texture2D::Texture2D() : mWidth(0),
							 mHeight(0),
							 mFormat(Format::UNKNOWN),
							 mMipLevel(0),
							 mWrapModeS(WrapMode::CLAMP_TO_EDGE),
							 mWrapModeT(WrapMode::CLAMP_TO_EDGE),
							 mMinFilterMode(FilterMode::LINEAR),
							 mMagFilterMode(FilterMode::LINEAR)
	{
	}

	Texture2D::~Texture2D() {}

	Texture2D::Texture2D(Texture2D &&other) noexcept : mWidth(other.mWidth),
													   mHeight(other.mHeight),
													   mFormat(other.mFormat),
													   mDataType(other.mDataType),
													   mMipLevel(other.mMipLevel),
													   mWrapModeS(other.mWrapModeS),
													   mWrapModeT(other.mWrapModeT),
													   mMinFilterMode(other.mMinFilterMode),
													   mMagFilterMode(other.mMagFilterMode)
	{
	}

	Texture2D &Texture2D::operator=(Texture2D &&other) noexcept
	{
		mWidth = other.mWidth;
		mHeight = other.mHeight;
		mFormat = other.mFormat;
		mDataType = other.mDataType;
		mMipLevel = other.mMipLevel;
		mWrapModeS = other.mWrapModeS;
		mWrapModeT = other.mWrapModeT;
		mMinFilterMode = other.mMinFilterMode;
		mMagFilterMode = other.mMagFilterMode;

		return *this;
	}

	void Texture2D::Bind() const
	{
		Texture::Bind(mTarget);
	}
	void Texture2D::UnBind()
	{
		Texture::UnBind(mTarget);
	}

	void Texture2D::Activate(const unsigned short unit) const
	{
		Texture::Activate(mTarget, unit);
	}
	void Texture2D::Deactivate(const unsigned short unit) const
	{
		Texture::Deactivate(mTarget, unit);
	}

	void Texture2D::LoadData(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType data_type, const void *pixels)
	{
		mMipLevel = mipLevel;
		mWidth = width;
		mHeight = height;
		mFormat = internFormat;
		mDataType = data_type;

		Bind();
		GL(TexImage2D(
			static_cast<GLenum>(mTarget),
			mMipLevel,
			static_cast<GLenum>(mFormat),
			mWidth, mHeight,
			border,
			static_cast<GLenum>(format),
			static_cast<GLenum>(mDataType),
			pixels));
		UnBind();
	}

	void Texture2D::UpdateData(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void *pixels)
	{
		Bind();
		GL(TexSubImage2D(
			static_cast<GLenum>(mTarget),
			mMipLevel,
			x, y,
			width, height,
			static_cast<GLenum>(mFormat),
			static_cast<GLenum>(mDataType),
			pixels));
		UnBind();
	}

	void Texture2D::GenerateMipmaps()
	{
		mMinFilterMode = FilterMode::LINEAR_MIPMAP_LINEAR;
		mMagFilterMode = FilterMode::LINEAR;

		Bind();
		GL(TexParameteri(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::MIN_FILTER), static_cast<GLenum>(mMinFilterMode)));
		GL(TexParameteri(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::MAG_FILTER), static_cast<GLenum>(mMagFilterMode)));
		GL(GenerateMipmap(static_cast<GLenum>(mTarget)));
		UnBind();
	}

	void Texture2D::SetSampler(const Sampler &sampler)
	{
		mWrapModeS = sampler.wrapS;
		mWrapModeT = sampler.wrapT;
		mMinFilterMode = sampler.minFilter;
		mMagFilterMode = sampler.magFilter;

		Bind();
		GL(TexParameteri(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::WRAP_S), static_cast<GLint>(mWrapModeS)));
		GL(TexParameteri(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::WRAP_T), static_cast<GLint>(mWrapModeT)));
		GL(TexParameteri(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::MIN_FILTER), static_cast<GLint>(mMinFilterMode)));
		GL(TexParameteri(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::MAG_FILTER), static_cast<GLint>(mMagFilterMode)));
	}
}