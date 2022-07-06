#include "Material.h"
#include <tiny_gltf.h>
#include "Image.h"
#include <opengl/Texture2D.h>
#include <glm/gtc/type_ptr.hpp>

namespace rsrc
{
	Material::Material() : mTransparency(1.f),
						   mMetallicFactor(0.1f),
						   mRoughnessFactor(0.5f),
						   mAlphaCutoff(0.5f),
						   mBaseColorFactor(0.5),
						   mEmissiveFactor(0.f)
	{
		for (uint32_t i = 0; i < MapType::COUNT; i++)
		{
			mTexCoordSets[i] = -1;
		}
	}
	Material::~Material() {}

	Material::Material(Material &&other) noexcept : mAlphaMode(other.mAlphaMode),
													mTransparency(other.mTransparency),
													mMetallicFactor(other.mMetallicFactor),
													mRoughnessFactor(other.mRoughnessFactor),
													mAlphaCutoff(other.mAlphaCutoff),
													mBaseColorFactor(other.mBaseColorFactor),
													mEmissiveFactor(other.mEmissiveFactor),
													mExtension(other.mExtension),
													mPbrWorkflows(other.mPbrWorkflows)
	{
		for (uint32_t i = 0; i < MapType::COUNT; i++)
		{
			mAttachedMaps[i] = other.mAttachedMaps[i];
			other.mAttachedMaps[i] = nullptr;
		}
		for (uint32_t i = 0; i < MapType::COUNT; i++)
		{
			mTexCoordSets[i] = other.mTexCoordSets[i];
			other.mTexCoordSets[i] = -1;
		}
		other.mAlphaMode = AlphaMode::OPAQUE;
		other.mTransparency = 1.f;
		other.mMetallicFactor = 0.5f;
		other.mRoughnessFactor = 0.5f;
		other.mAlphaCutoff = 0.5f;
		other.mBaseColorFactor = glm::vec4(0.5f);
		other.mEmissiveFactor = glm::vec4(0.f);
		;
		other.mExtension.diffuseFactor = glm::vec4(0.5f);
		other.mExtension.glossinessFactor = 0.5f;
		other.mExtension.specularFactor = glm::vec3(0.5f);
		other.mPbrWorkflows.metallicRoughness = true;
		other.mPbrWorkflows.specularGlossiness = false;
	}

	Material &Material::operator=(Material &&other) noexcept
	{
		mAlphaMode = other.mAlphaMode;
		mTransparency = other.mTransparency;
		mMetallicFactor = other.mMetallicFactor;
		mRoughnessFactor = other.mRoughnessFactor;
		mAlphaCutoff = other.mAlphaCutoff;
		mBaseColorFactor = other.mBaseColorFactor;
		mEmissiveFactor = other.mEmissiveFactor;
		for (uint32_t i = 0; i < MapType::COUNT; i++)
		{
			mAttachedMaps[i] = other.mAttachedMaps[i];
			other.mAttachedMaps[i] = nullptr;
		}
		for (uint32_t i = 0; i < MapType::COUNT; i++)
		{
			mTexCoordSets[i] = other.mTexCoordSets[i];
			other.mTexCoordSets[i] = -1;
		}
		mExtension = other.mExtension;
		mPbrWorkflows = other.mPbrWorkflows;

		other.mAlphaMode = AlphaMode::OPAQUE;
		other.mTransparency = 1.f;
		other.mMetallicFactor = 0.5f;
		other.mRoughnessFactor = 0.5f;
		other.mBaseColorFactor = glm::vec4(0.5f);
		other.mEmissiveFactor = glm::vec4(0.f);
		;
		other.mExtension.diffuseFactor = glm::vec4(0.5f);
		other.mExtension.glossinessFactor = 0.5f;
		other.mExtension.specularFactor = glm::vec3(0.5f);
		other.mPbrWorkflows.metallicRoughness = true;
		other.mPbrWorkflows.specularGlossiness = false;

		return *this;
	}

	/*void Material::AttachMap(const MapType type, uint8_t* data, size_t width, size_t height, size_t channels, bool isFloatingPoint)
	{
		mAttachedMaps[type].SetData(data, width, height, channels, isFloatingPoint);
	}*/
	void Material::AttachMap(const MapType type, Image *image)
	{
		mAttachedMaps[type] = image;
	}
	void Material::DetachMap(const MapType type)
	{
		mAttachedMaps[type] = nullptr;
	}

	gl::Texture2D *Material::GetTextureByMap(MapType type)
	{
		Image *img = mAttachedMaps[type];
		gl::Texture2D *tex = nullptr;

		if (img)
			tex = img->GetTexture();

		return tex;
	}

	void Material::LoadFromTinyGLTF(tinygltf::Material &mat, std::vector<Image> &textures)
	{

		if (mat.values.find("baseColorTexture") != mat.values.end())
		{
			uint32_t texture_idx = mat.values["baseColorTexture"].TextureIndex();
			mTexCoordSets[MapType::BASE_COLOR] = mat.values["baseColorTexture"].TextureTexCoord();

			AttachMap(MapType::BASE_COLOR, &(textures[texture_idx]));
		}
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
		{
			uint32_t texture_idx = mat.values["metallicRoughnessTexture"].TextureIndex();
			mTexCoordSets[MapType::METALLIC_ROUGHNESS] = mat.values["metallicRoughnessTexture"].TextureTexCoord();

			AttachMap(MapType::METALLIC_ROUGHNESS, &(textures[texture_idx]));
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
		{
			uint32_t texture_idx = mat.additionalValues["normalTexture"].TextureIndex();
			mTexCoordSets[MapType::NORMAL] = mat.values["normalTexture"].TextureTexCoord();

			AttachMap(MapType::NORMAL, &(textures[texture_idx]));
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
		{
			uint32_t texture_idx = mat.additionalValues["emissiveTexture"].TextureIndex();
			mTexCoordSets[MapType::EMISSIVE] = mat.values["emissiveTexture"].TextureTexCoord();

			AttachMap(MapType::EMISSIVE, &(textures[texture_idx]));
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
		{
			uint32_t texture_idx = mat.additionalValues["occlusionTexture"].TextureIndex();
			mTexCoordSets[MapType::AMBIENT_OCCLUSION] = mat.values["occlusionTexture"].TextureTexCoord();

			AttachMap(MapType::AMBIENT_OCCLUSION, &(textures[texture_idx]));
		}

		if (mat.values.find("roughnessFactor") != mat.values.end())
		{
			mRoughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end())
		{
			mMetallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end())
		{
			mBaseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}

		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
		{
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND")
			{
				mAlphaMode = AlphaMode::BLEND;
			}
			if (param.string_value == "MASK")
			{
				mAlphaCutoff = 0.5f;
				mAlphaMode = AlphaMode::MASK;
			}
		}

		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
		{
			mAlphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}
		if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
		{
			mEmissiveFactor = glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
		}

		// Extensions
		// @TODO: Find out if there is a nicer way of reading these properties with recent tinygltf headers
		/*if (mat.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
			auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
			if (ext->second.Has("specularGlossinessTexture")) {
				auto index = ext->second.Get("specularGlossinessTexture").Get("index");
				material.extension.specularGlossinessTexture = &textures[index.Get<int>()];
				auto texCoordSet = ext->second.Get("specularGlossinessTexture").Get("texCoord");
				material.texCoordSets.specularGlossiness = texCoordSet.Get<int>();
				material.pbrWorkflows.specularGlossiness = true;
			}
			if (ext->second.Has("diffuseTexture")) {
				auto index = ext->second.Get("diffuseTexture").Get("index");
				material.extension.diffuseTexture = &textures[index.Get<int>()];
			}
			if (ext->second.Has("diffuseFactor")) {
				auto factor = ext->second.Get("diffuseFactor");
				for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
					auto val = factor.Get(i);
					material.extension.diffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
				}
			}
			if (ext->second.Has("specularFactor")) {
				auto factor = ext->second.Get("specularFactor");
				for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
					auto val = factor.Get(i);
					material.extension.specularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
				}
			}
		}*/
	}
}