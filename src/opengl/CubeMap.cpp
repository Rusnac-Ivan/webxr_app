#include "CubeMap.h"

namespace gl
{
	CubeMap::CubeMap() : mWidth(0),
						 mHeight(0),
						 mFormat(Format::UNKNOWN),
						 mMipLevel(0),
						 mMinFilterMode(FilterMode::LINEAR),
						 mMagFilterMode(FilterMode::LINEAR),
						 mWrapModeS(WrapMode::CLAMP_TO_EDGE),
						 mWrapModeT(WrapMode::CLAMP_TO_EDGE),
						 mWrapModeR(WrapMode::CLAMP_TO_EDGE)
	{
	}

	CubeMap::~CubeMap() {}

	CubeMap::CubeMap(CubeMap &&other) noexcept
	{
		this->mWidth = other.mWidth;
		this->mHeight = other.mHeight;
		this->mMipLevel = other.mMipLevel;
		this->mFormat = other.mFormat;
		this->mMinFilterMode = other.mMinFilterMode;
		this->mMagFilterMode = other.mMagFilterMode;
		this->mWrapModeS = other.mWrapModeS;
		this->mWrapModeT = other.mWrapModeT;
		this->mWrapModeR = other.mWrapModeR;

		other.mWidth = 0;
		other.mHeight = 0;
		other.mMipLevel = 0;
		other.mFormat = Format::UNKNOWN;
		other.mMinFilterMode = FilterMode::LINEAR;
		other.mMagFilterMode = FilterMode::LINEAR;
		other.mWrapModeS = WrapMode::CLAMP_TO_EDGE;
		other.mWrapModeT = WrapMode::CLAMP_TO_EDGE;
		other.mWrapModeR = WrapMode::CLAMP_TO_EDGE;

		other.Destroy();
	}
	CubeMap &CubeMap::operator=(CubeMap &&other) noexcept
	{
		this->mWidth = other.mWidth;
		this->mHeight = other.mHeight;
		this->mMipLevel = other.mMipLevel;
		this->mFormat = other.mFormat;
		this->mMinFilterMode = other.mMinFilterMode;
		this->mMagFilterMode = other.mMagFilterMode;
		this->mWrapModeS = other.mWrapModeS;
		this->mWrapModeT = other.mWrapModeT;
		this->mWrapModeR = other.mWrapModeR;

		other.mWidth = 0;
		other.mHeight = 0;
		other.mMipLevel = 0;
		other.mFormat = Format::UNKNOWN;
		other.mMinFilterMode = FilterMode::LINEAR;
		other.mMagFilterMode = FilterMode::LINEAR;
		other.mWrapModeS = WrapMode::CLAMP_TO_EDGE;
		other.mWrapModeT = WrapMode::CLAMP_TO_EDGE;
		other.mWrapModeR = WrapMode::CLAMP_TO_EDGE;

		other.Destroy();

		return *this;
	}

	void CubeMap::Create()
	{
		Texture::Destroy();
		Texture::Create();
	}

	void CubeMap::Destroy()
	{
		Texture::Destroy();
	}

	void CubeMap::Bind() const
	{
		Texture::Bind(mTarget);
	}
	void CubeMap::UnBind()
	{
		Texture::UnBind(mTarget);
	}

	void CubeMap::Activate(const unsigned short unit) const
	{
		Texture::Activate(mTarget, unit);
	}
	void CubeMap::Deactivate(const unsigned short unit) const
	{
		Texture::Deactivate(mTarget, unit);
	}

	void CubeMap::SetWrapModeS(WrapMode mode)
	{
		mWrapModeS = mode;
		Texture::SetIntParameter(mTarget, Parameter::WRAP_S, ParamValue(mWrapModeS));
	}
	void CubeMap::SetWrapModeT(WrapMode mode)
	{
		mWrapModeT = mode;
		Texture::SetIntParameter(mTarget, Parameter::WRAP_T, ParamValue(mWrapModeT));
	}
	void CubeMap::SetWrapModeR(WrapMode mode)
	{
		mWrapModeR = mode;
		Texture::SetIntParameter(mTarget, Parameter::WRAP_R, ParamValue(mWrapModeR));
	}
	void CubeMap::SetMinFilterMode(FilterMode mode)
	{
		mMinFilterMode = mode;
		Texture::SetIntParameter(mTarget, Parameter::MIN_FILTER, ParamValue(mMinFilterMode));
	}
	void CubeMap::SetMagFilterMode(FilterMode mode)
	{
		mMagFilterMode = mode;
		Texture::SetIntParameter(mTarget, Parameter::MAG_FILTER, ParamValue(mMagFilterMode));
	}

	void CubeMap::GenerateMipmaps()
	{
		mMinFilterMode = FilterMode::LINEAR_MIPMAP_LINEAR;
		mMagFilterMode = FilterMode::LINEAR;

		Bind();
		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::MIN_FILTER),
			static_cast<GLint>(mMinFilterMode)));
		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::MAG_FILTER),
			static_cast<GLenum>(mMagFilterMode)));
		GL(GenerateMipmap(static_cast<GLenum>(mTarget)));
		UnBind();
	}

	void CubeMap::SetTarget(Target target, int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		mMipLevel = mipLevel;
		mWidth = width;
		mHeight = height;
		mFormat = internFormat;
		mDataType = type;

		Bind();
		GL(TexImage2D(
			static_cast<GLenum>(target),
			mMipLevel,
			static_cast<GLenum>(mFormat),
			mWidth, mHeight,
			border,
			static_cast<GLenum>(format),
			static_cast<GLenum>(mDataType),
			pixels));
		UnBind();
	}

	void CubeMap::SetTargets(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, bool genMipMap = false)
	{
		mMipLevel = mipLevel;
		mWidth = width;
		mHeight = height;
		mFormat = internFormat;
		mDataType = type;

		mWrapModeS = WrapMode::CLAMP_TO_BORDER;
		mWrapModeR = WrapMode::CLAMP_TO_BORDER;
		mWrapModeT = WrapMode::CLAMP_TO_BORDER;

		Bind();
		for (size_t i = 0; i < 6; i++)
		{
			GL(TexImage2D(
				static_cast<GLenum>(Target::POSITIVE_X) + (GLenum)i,
				mMipLevel,
				static_cast<GLenum>(mFormat),
				mWidth, mHeight, 0,
				static_cast<GLenum>(format),
				static_cast<GLenum>(mDataType),
				nullptr));
		}

		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::WRAP_S),
			static_cast<GLint>(mWrapModeS)));
		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::WRAP_T),
			static_cast<GLint>(mWrapModeR)));
		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::WRAP_R),
			static_cast<GLint>(mWrapModeT)));

		float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
		GL(TexParameterfv(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::BORDER_COLOR), border_color));
		UnBind();

		if (genMipMap)
			GenerateMipmaps();
		else
		{
			mMinFilterMode = FilterMode::LINEAR;
			mMagFilterMode = FilterMode::LINEAR;

			Bind();
			GL(TexParameteri(
				static_cast<GLenum>(mTarget),
				static_cast<GLenum>(Parameter::MIN_FILTER),
				static_cast<GLint>(mMinFilterMode)));
			GL(TexParameteri(
				static_cast<GLenum>(mTarget),
				static_cast<GLenum>(Parameter::MAG_FILTER),
				static_cast<GLenum>(mMagFilterMode)));
			UnBind();
		}
	}

	/*void CubeMap::SetTargetAsDepth(uint32_t width, uint32_t height)
	{
		mMipLevel = 0;
		mWidth = width;
		mHeight = height;
		mFormat = Format::DEPTH;
		mDataType = DataType::FLOAT;

		mWrapModeS = WrapMode::CLAMP_TO_BORDER;
		mWrapModeR = WrapMode::CLAMP_TO_BORDER;
		mWrapModeT = WrapMode::CLAMP_TO_BORDER;

		Bind();
		for (size_t i = 0; i < 6; i++)
		{
			GL(TexImage2D(
				static_cast<GLenum>(Target::POSITIVE_X) + (GLenum)i, mMipLevel,
				static_cast<GLenum>(mFormat),
				mWidth, mHeight, 0,
				static_cast<GLenum>(mFormat),
				static_cast<GLenum>(mDataType),
				nullptr
			));
		}

		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::WRAP_S),
			static_cast<GLint>(mWrapModeS)
		));
		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::WRAP_T),
			static_cast<GLint>(mWrapModeR)
		));
		GL(TexParameteri(
			static_cast<GLenum>(mTarget),
			static_cast<GLenum>(Parameter::WRAP_R),
			static_cast<GLint>(mWrapModeT)
		));

		float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GL(TexParameterfv(static_cast<GLenum>(mTarget), static_cast<GLenum>(Parameter::BORDER_COLOR), border));
		UnBind();

		GenerateMipmaps();
	}*/

	void CubeMap::SetPositiveX(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		SetTarget(Target::POSITIVE_X, mipLevel, internFormat, width, height, border, format, type, pixels);
	}
	void CubeMap::SetNegativeX(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		SetTarget(Target::NEGATIVE_X, mipLevel, internFormat, width, height, border, format, type, pixels);
	}
	void CubeMap::SetPositiveY(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		SetTarget(Target::POSITIVE_Y, mipLevel, internFormat, width, height, border, format, type, pixels);
	}
	void CubeMap::SetNegativeY(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		SetTarget(Target::NEGATIVE_Y, mipLevel, internFormat, width, height, border, format, type, pixels);
	}
	void CubeMap::SetPositiveZ(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		SetTarget(Target::POSITIVE_Z, mipLevel, internFormat, width, height, border, format, type, pixels);
	}
	void CubeMap::SetNegativeZ(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels)
	{
		SetTarget(Target::NEGATIVE_Z, mipLevel, internFormat, width, height, border, format, type, pixels);
	}

}