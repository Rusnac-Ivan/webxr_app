#include "Image.h"
#include <tiny_gltf.h>

namespace rsrc
{
	Image::Image() : mWidth(0),
					 mHeight(0),
					 mChannels(0),
					 mIsFloatingPoint(false)
	{
	}
	Image::Image(uint8_t *data, size_t width, size_t height, size_t channels, bool isFloatingPoint)
	{
		SetData(data, width, height, channels, isFloatingPoint);
	}

	Image::Image(Image &&other) noexcept : mData(std::move(other.mData)),
										   mWidth(other.mWidth),
										   mHeight(other.mHeight),
										   mChannels(other.mChannels),
										   mIsFloatingPoint(other.mIsFloatingPoint),
										   mTexture(std::move(other.mTexture))
	{
		other.mData.clear();
	}
	Image &Image::operator=(Image &&other) noexcept
	{
		mData = std::move(other.mData);
		mWidth = other.mWidth;
		mHeight = other.mHeight;
		mChannels = other.mChannels;
		mIsFloatingPoint = other.mIsFloatingPoint;
		mTexture = std::move(other.mTexture);
		other.mData.clear();

		return *this;
	}

	Image::~Image() {}

	void Image::SetData(const unsigned char *data, size_t width, size_t height, size_t channels, bool isFloatingPoint)
	{
		mWidth = width;
		mHeight = height;
		mChannels = channels;
		mIsFloatingPoint = isFloatingPoint;

		const size_t byte_size = GetTotalByteSize();

		mData.clear();
		mData.reserve(byte_size);

		mData.insert(mData.begin(), data, data + byte_size);

		gl::Texture::Format format = gl::Texture::Format::UNKNOWN;
		switch (mChannels)
		{
		case 1:
		{
			format = gl::Texture::Format::RED;
		}
		break;
		case 2:
		{
			format = gl::Texture::Format::RG;
		}
		break;
		case 3:
		{
			format = gl::Texture::Format::RGB;
		}
		break;
		case 4:
		{
			format = gl::Texture::Format::RGBA;
		}
		break;
		default:
			break;
		}

		mTexture.LoadData(0, format, mWidth, mHeight, 0, format, gl::DataType::UNSIGNED_BYTE, mData.data());

		mData.clear();
	}

	void Image::SetPixelByte(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
	{
		if (this->mIsFloatingPoint)
		{
			this->SetPixelFloat(x, y, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
			return;
		}

		assert(x < this->mWidth && y < this->mHeight);

		switch (this->mChannels)
		{
		case 1:
			this->mData[x * this->mHeight + y] = r;
			break;
		case 2:
			this->mData[(x * this->mHeight + y) * 2 + 0] = r;
			this->mData[(x * this->mHeight + y) * 2 + 1] = g;
			break;
		case 3:
			this->mData[(x * this->mHeight + y) * 3 + 0] = r;
			this->mData[(x * this->mHeight + y) * 3 + 1] = g;
			this->mData[(x * this->mHeight + y) * 3 + 2] = b;
			break;
		case 4:
			this->mData[(x * this->mHeight + y) * 4 + 0] = r;
			this->mData[(x * this->mHeight + y) * 4 + 1] = g;
			this->mData[(x * this->mHeight + y) * 4 + 2] = b;
			this->mData[(x * this->mHeight + y) * 4 + 3] = a;
			break;
		default:
			assert(false && "Unknown");
			break;
		}
	}

	template <typename T>
	T Clamp(T v, T minv, T maxv)
	{
		return (v < minv) ? minv : (v > maxv ? maxv : v);
	}

	void Image::SetPixelFloat(size_t x, size_t y, float r, float g, float b, float a = 1.0f)
	{
		if (!this->mIsFloatingPoint)
		{
			this->SetPixelByte(x, y,
							   (uint8_t)Clamp(r * 255.0f, 0.0f, 255.0f),
							   (uint8_t)Clamp(g * 255.0f, 0.0f, 255.0f),
							   (uint8_t)Clamp(b * 255.0f, 0.0f, 255.0f),
							   (uint8_t)Clamp(a * 255.0f, 0.0f, 255.0f));
			return;
		}

		assert(x < this->mWidth && y < this->mHeight);
		switch (this->mChannels)
		{
		case 1:
			((float *)this->mData.data())[x * this->mHeight + y] = r;
			break;
		case 2:
			((float *)this->mData.data())[(x * this->mHeight + y) * 2 + 0] = r;
			((float *)this->mData.data())[(x * this->mHeight + y) * 2 + 1] = g;
			break;
		case 3:
			((float *)this->mData.data())[(x * this->mHeight + y) * 3 + 0] = r;
			((float *)this->mData.data())[(x * this->mHeight + y) * 3 + 1] = g;
			((float *)this->mData.data())[(x * this->mHeight + y) * 3 + 2] = b;
			break;
		case 4:
			((float *)this->mData.data())[(x * this->mHeight + y) * 4 + 0] = r;
			((float *)this->mData.data())[(x * this->mHeight + y) * 4 + 1] = g;
			((float *)this->mData.data())[(x * this->mHeight + y) * 4 + 2] = b;
			((float *)this->mData.data())[(x * this->mHeight + y) * 4 + 3] = a;
			break;
		default:
			assert(false && "Unknown"); // invalid channel count
			break;
		}
	}

	void Image::LoadFromGLTF(const tinygltf::Image &gltfimage, std::vector<tinygltf::Sampler> &samplers, int sampler_idx)
	{

		SetData(&gltfimage.image[0], gltfimage.width, gltfimage.height, gltfimage.component, false);

		if (sampler_idx == -1)
		{
			gl::Texture2D::Sampler sam;
			sam.wrapS = gl::Texture::WrapMode::CLAMP_TO_EDGE;
			sam.wrapT = gl::Texture::WrapMode::CLAMP_TO_EDGE;
			sam.minFilter = gl::Texture::FilterMode::LINEAR;
			sam.magFilter = gl::Texture::FilterMode::LINEAR;
			mTexture.SetSampler(sam);
		}
		else
		{
			tinygltf::Sampler &sampler = samplers[sampler_idx];

			gl::Texture2D::Sampler sam;
			sam.wrapS = static_cast<gl::Texture::WrapMode>(sampler.wrapS);
			sam.wrapT = static_cast<gl::Texture::WrapMode>(sampler.wrapT);
			sam.minFilter = static_cast<gl::Texture::FilterMode>(sampler.minFilter > -1 ? sampler.minFilter : GL_LINEAR);
			sam.magFilter = static_cast<gl::Texture::FilterMode>(sampler.magFilter > -1 ? sampler.magFilter : GL_LINEAR);
			mTexture.SetSampler(sam);
		}

		if (mTexture.GetMinFilterMode() == gl::Texture::FilterMode::LINEAR_MIPMAP_LINEAR ||
			mTexture.GetMinFilterMode() == gl::Texture::FilterMode::LINEAR_MIPMAP_NEAREST ||
			mTexture.GetMinFilterMode() == gl::Texture::FilterMode::NEAREST_MIPMAP_LINEAR ||
			mTexture.GetMinFilterMode() == gl::Texture::FilterMode::NEAREST_MIPMAP_NEAREST)
		{
			mTexture.GenerateMipmaps();
		}
			
	}
}