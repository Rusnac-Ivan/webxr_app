#include "Texture.h"

namespace gl
{
	Texture::Texture() : mID(0)
	{
		Create();
	}

	Texture::~Texture()
	{
		Destroy();
	}

	Texture::Texture(Texture &&other) noexcept
	{
		this->mID = other.mID;
	}
	Texture &Texture::operator=(Texture &&other) noexcept
	{
		this->mID = other.mID;

		return *this;
	}

	void Texture::Create()
	{
		GL(GenTextures(1, &mID));
		assert(mID != 0 && "Failed to generate texture!");
	}
	void Texture::Destroy()
	{
		if (mID != 0)
		{
			GL(DeleteTextures(1, &mID));
			mID = 0;
		}
	}

	void Texture::Bind(Type type) const
	{
		GL(BindTexture(static_cast<GLenum>(type), mID));
	}

	void Texture::UnBind(Type type)
	{
		GLenum target = static_cast<GLenum>(type);
		GL(BindTexture(target, NULL));
	}

	void Texture::Activate(Type type, const unsigned short unit) const
	{
		GL(ActiveTexture(GL_TEXTURE0 + unit));
		GL(BindTexture(static_cast<GLenum>(type), mID));
	}

	void Texture::Deactivate(Type type, const unsigned short unit) const
	{
		GL(ActiveTexture(GL_TEXTURE0 + unit));
		GL(BindTexture(static_cast<GLenum>(type), NULL));
	}

	void Texture::SetIntParameter(Type type, Parameter param, ParamValue val)
	{
		Bind(type);
		GL(TexParameteri(static_cast<GLenum>(type), static_cast<GLenum>(param), val.GetValue()));
		UnBind(type);
	}
	void Texture::SetFloatParameter(Type type, Parameter param, float val)
	{
		Bind(type);
		GL(TexParameterf(static_cast<GLenum>(type), static_cast<GLenum>(param), val));
		UnBind(type);
	}
	void Texture::SetFloatArrayParameter(Type type, Parameter param, float *values)
	{
		Bind(type);
		GL(TexParameterfv(static_cast<GLenum>(type), static_cast<GLenum>(param), values));
		UnBind(type);
	}

	/*Texture2D::Texture2D() : Texture(),
		mWidth(NULL),
		mHeight(NULL),
		mFormat(Format::UNKNOWN),
		mInternalFormat(Format::UNKNOWN),
		mPixelDataFormat(DataType::UNSIGNED_BYTE),
		mRawData(nullptr),
		mRawDataSize(NULL),
		mGenerateMipmap(false)
	{
	}

	Texture2D::~Texture2D()
	{
	}

	void Texture2D::SetTexture2D(int32_t level, Format internalformat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, void* pixels)
	{
		mWidth = width;
		mHeight = height;
		mInternalFormat = internalformat;
		mFormat = format;
		mPixelDataFormat = type;

		//GLenum target = static_cast<GLenum>(mTarget);
		GLenum in_form = static_cast<GLenum>(internalformat);
		GLenum form = static_cast<GLenum>(format);
		GLenum data_type = static_cast<GLenum>(type);
		Bind();
		GL(TexImage2D(mTarget, level, in_form, width, height, border, form, data_type, pixels));

		UnBind();
	}

	void Texture2D::Bind()
	{
		Texture::Bind(mTarget);
	}
	void Texture2D::UnBind()
	{
		Texture::UnBind(mTarget);
	}



	void Texture2D::SetSampler(const Sampler& sampler)
	{
		mSampler = sampler;
		Bind();
		GLenum min_fil = static_cast<GLenum>(sampler.minFilter);
		GLenum mag_fil = static_cast<GLenum>(sampler.magFilter);
		GLenum wrap_s = static_cast<GLenum>(sampler.wrapModeS);
		GLenum wrap_t = static_cast<GLenum>(sampler.wrapModeT);
		GL(TexParameteri(mTarget, SamplerParam::MIN_FILTER, min_fil));
		GL(TexParameteri(mTarget, SamplerParam::MAG_FILTER, mag_fil));
		GL(TexParameteri(mTarget, SamplerParam::WRAP_S, wrap_s));
		GL(TexParameteri(mTarget, SamplerParam::WRAP_T, wrap_t));
		if (sampler.generateMipmaps)
		{
			GL(GenerateMipmap(mTarget));
		}
		UnBind();
	}

	void Texture2D::Update(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* buffer)
	{
		//GLenum target = static_cast<GLenum>(mTarget);
		GLenum internal_form = static_cast<GLenum>(mInternalFormat);
		GLenum data_type = static_cast<GLenum>(mPixelDataFormat);
		Bind();
		GL(TexSubImage2D(mTarget, 0, x, y, width, height, internal_form, data_type, buffer));
		UnBind();
	}*/
}