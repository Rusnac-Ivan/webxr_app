#ifndef _RSRC_IMAGE_H_
#define _RSRC_IMAGE_H_

#include <cstdint>
#include <vector>
#include <cassert>
#include <opengl/Texture2D.h>

namespace tinygltf
{
	class Image;
	class Sampler;
}

namespace rsrc
{
	class Image
	{
		private:
			std::vector<uint8_t> mData;
			size_t mWidth;
			size_t mHeight;
			uint8_t mChannels;
			bool mIsFloatingPoint;
			gl::Texture2D mTexture;
		public:

			Image();
			Image(uint8_t* data, size_t width, size_t height, size_t channels, bool isFloatingPoint);
			~Image();
			Image(const Image&) = delete;
			Image& operator=(const Image&) = delete;
			Image(Image&&) noexcept;
			Image& operator=(Image&&) noexcept;

			inline const uint8_t* GetRawData() const { return mData.data(); }
			inline size_t GetWidth() const { return mWidth; }
			inline size_t GetHeight() const { return mHeight; }
			inline size_t GetChannelCount() const { return mChannels; }
			inline size_t GetChannelSize() const { return this->mIsFloatingPoint ? sizeof(float) : sizeof(uint8_t); }
			inline size_t GetPixelSize() const { return this->GetChannelCount() * this->GetChannelSize(); }
			inline size_t GetTotalByteSize() const { return this->GetWidth() * this->GetHeight() * this->GetPixelSize(); }
			inline bool IsFloatingPoint() const { return mIsFloatingPoint; };

			void SetData(const unsigned char* data, size_t width, size_t height, size_t channels, bool isFloatingPoint);

			void SetPixelByte(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
			void SetPixelFloat(size_t x, size_t y, float r, float g, float b, float a);

			void LoadFromGLTF(const tinygltf::Image& gltfimage, std::vector<tinygltf::Sampler>& samplers, int sampler_idx);
			//std::array<uint8_t, 4> GetPixelByte(size_t x, size_t y) const;
			//std::array<float, 4> GetPixelFloat(size_t x, size_t y) const;
		private:

	};

	
}

#endif // !_RSRC_TEXTURE_H_
