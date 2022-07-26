#ifndef _GL_TEXTURE_
#define _GL_TEXTURE_

#include "GLUtilities.h"
#include <map>

namespace gl
{
	class Texture
	{
	public:
		enum struct Type
		{
			TARGET_1D = GL_TEXTURE_1D,
			TARGET_2D = GL_TEXTURE_2D,
			TARGET_3D = GL_TEXTURE_3D,
			TARGET_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
			TARGET_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,

			UNKNOWN = GL_FALSE
		};

		enum struct Format
		{
			// Base
			RED = GL_RED,
			RG = GL_RG,
			RGB = GL_RGB,
			RGBA = GL_RGBA,
			DEPTH = GL_DEPTH_COMPONENT,
			DEPTH_STENCIL = GL_DEPTH_STENCIL,

			// Sized
			R8 = GL_R8,
			R16 = GL_R16,
			RG8 = GL_RG8,
			RG16 = GL_RG16,
			R16F = GL_R16F,
			R32F = GL_R32F,
			RG16F = GL_RG16F,
			RG32F = GL_RG32F,
			RGB8 = GL_RGB8,
			RGB16 = GL_RGB16,
			RGBA8 = GL_RGBA8,
			RGB16F = GL_RGB16F,
			RGBA16 = GL_RGBA16,
			RGBA16F = GL_RGBA16F,
			RGB32F = GL_RGB32F,
			RGBA32F = GL_RGBA32F,

			DEPTH16 = GL_DEPTH_COMPONENT16,
			DEPTH24 = GL_DEPTH_COMPONENT24,
			DEPTH32F = GL_DEPTH_COMPONENT32F,

			DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
			DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8,

			UNKNOWN = -1
		};

		enum struct Parameter
		{
			MIN_FILTER = GL_TEXTURE_MIN_FILTER,
			MAG_FILTER = GL_TEXTURE_MAG_FILTER,
			WRAP_S = GL_TEXTURE_WRAP_S,
			WRAP_T = GL_TEXTURE_WRAP_T,
			WRAP_R = GL_TEXTURE_WRAP_R,
			BORDER_COLOR = GL_TEXTURE_BORDER_COLOR
		};

		enum struct WrapMode
		{
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
			REPEAT = GL_REPEAT,
			MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE,
			MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		};
		enum struct FilterMode
		{
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR,
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
		};

		union ParamValue
		{
			WrapMode wrap_mode;
			FilterMode filter_mode;
			ParamValue(WrapMode mode) : wrap_mode(mode){};
			ParamValue(FilterMode mode) : filter_mode(mode){};
			GLint GetValue() { return value; }

		private:
			GLint value;
		};

		unsigned int GetID() const { return mID; }

	private:
		unsigned int mID;

	protected:
		Texture();
		virtual ~Texture();

		Texture(const Texture &) = delete;
		Texture &operator=(const Texture &) = delete;
		Texture(Texture &&) noexcept;
		Texture &operator=(Texture &&) noexcept;

		void Bind(Type type) const;
		static void UnBind(Type type);

		void Activate(Type type, const unsigned short unit) const;
		void Deactivate(Type type, const unsigned short unit) const;

		void SetIntParameter(Type type, Parameter param, ParamValue val);
		void SetFloatParameter(Type type, Parameter param, float val);
		void SetFloatArrayParameter(Type type, Parameter param, float *values);

		unsigned int GetId() const { return mID; }

	protected:
		void Create();
		void Destroy();
	};

	/*class Texture2D : public Texture
	{
		public:

			struct Sampler
			{
				FilterMode magFilter;
				FilterMode minFilter;
				WrapMode wrapModeS;
				WrapMode wrapModeT;
				bool generateMipmaps;
			};
		private:

			static constexpr Type mTarget = Type::TARGET_2D;

			uint32_t	mWidth;
			uint32_t	mHeight;
			Format		mFormat;
			Format		mInternalFormat;
			DataType	mPixelDataFormat;
			uint8_t*	mRawData;
			uint32_t	mRawDataSize;
			bool		mGenerateMipmap;



			Sampler		mSampler;

			//std::map<ParamName, ParamValue> mParameters;

		public:
			Texture2D();
			virtual ~Texture2D();

			Texture2D(const Texture2D&) = delete;
			Texture2D& operator=(const Texture2D&) = delete;

			uint32_t GetWidth() { return mWidth; };
			uint32_t GetHeight() { return mHeight; };
			Format GetFormat() { return mFormat; };
			Format GetInternalFormat() { return mInternalFormat; };

			void SetSampler(const Sampler& sampler);

			void Bind();
			void UnBind();

			void SetTexture2D(int32_t level, Format internalformat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, void* pixels);

			void Update(uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* buffer);

		private:

	};*/

}

#endif // !_GL_TEXTURE_
