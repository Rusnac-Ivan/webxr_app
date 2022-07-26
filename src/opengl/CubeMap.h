#ifndef _CUBE_MAP_H_
#define _CUBE_MAP_H_

#include "GLUtilities.h"
#include "Texture.h"
#include <string>
#include "Program.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

namespace gl
{
	class CubeMap : public Texture
	{
	public:
		enum struct Target
		{
			POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};

	private:
		static constexpr Type mTarget = Type::TARGET_CUBE_MAP;

		uint32_t mWidth;
		uint32_t mHeight;
		Format mFormat;
		uint32_t mMipLevel;
		DataType mDataType;

		FilterMode mMinFilterMode;
		FilterMode mMagFilterMode;
		WrapMode mWrapModeS;
		WrapMode mWrapModeT;
		WrapMode mWrapModeR;

	public:
		CubeMap();
		CubeMap(const CubeMap &) = delete;
		CubeMap &operator=(const CubeMap &) = delete;
		CubeMap(CubeMap &&) noexcept;
		CubeMap &operator=(CubeMap &&) noexcept;
		~CubeMap();

		void Bind() const;
		static void UnBind();

		void Activate(const unsigned short unit) const;
		void Deactivate(const unsigned short unit) const;

		void SetWrapModeS(WrapMode mode);
		void SetWrapModeT(WrapMode mode);
		void SetWrapModeR(WrapMode mode);
		void SetMinFilterMode(FilterMode mode);
		void SetMagFilterMode(FilterMode mode);

		WrapMode GetWrapModeS() const { return mWrapModeS; }
		WrapMode GetWrapModeT() const { return mWrapModeT; }
		WrapMode GetWrapModeR() const { return mWrapModeR; }
		FilterMode GetMinFilterMode() const { return mMinFilterMode; }
		FilterMode GetMagFilterMode() const { return mMagFilterMode; }

		Format GetFormat() const { return mFormat; }
		DataType GetPixelDataType() const { return mDataType; }

		void SetPositiveX(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);
		void SetNegativeX(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);
		void SetPositiveY(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);
		void SetNegativeY(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);
		void SetPositiveZ(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);
		void SetNegativeZ(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);

		// void SetTargetAsDepth(uint32_t width, uint32_t height);
		void SetTargets(int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, bool genMipMap);

		void GenerateMipmaps();

		void SetTarget(Target target, int32_t mipLevel, Format internFormat, uint32_t width, uint32_t height, uint32_t border, Format format, DataType type, const void *pixels);

	private:
		void Create();
		void Destroy();
	};

}

#endif