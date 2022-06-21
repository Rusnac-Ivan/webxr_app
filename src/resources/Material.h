#ifndef _RSRC_MATERIAL_H_
#define _RSRC_MATERIAL_H_

#undef OPAQUE

#include <glm/glm.hpp>
#include <map>
#include <vector>


namespace tinygltf
{
	struct Material;
}

namespace gl
{
	class Texture2D;
}


namespace rsrc
{
	class Image;

	class Material
	{
		public:
			enum struct AlphaMode
			{
				OPAQUE,
				MASK,
				BLEND //TRANSPARENT
			};

			enum MapType
			{
				BASE_COLOR,
				NORMAL,
				ROUGHNESS,
				METALLIC,
				METALLIC_ROUGHNESS,
				SPECULAR,
				GLOSSINESS,
				AMBIENT_OCCLUSION,
				EMISSIVE,
				HEIGHT,
				COUNT
			};
		private:
			AlphaMode mAlphaMode = AlphaMode::OPAQUE;

			float mTransparency;
			float mMetallicFactor;
			float mRoughnessFactor;
			float mAlphaCutoff;
			glm::vec4 mBaseColorFactor;
			glm::vec4 mEmissiveFactor;

			Image* mAttachedMaps[MapType::COUNT] = {};
			int mTexCoordSets[MapType::COUNT] = {};

			struct Extension {
				float glossinessFactor = 0.5f;
				glm::vec4 diffuseFactor = glm::vec4(1.0f);
				glm::vec3 specularFactor = glm::vec3(0.0f);
			} mExtension;

			struct PbrWorkflows {
				bool metallicRoughness = true;
				bool specularGlossiness = false;
			} mPbrWorkflows;

		public:
			Material();
			~Material();

			Material(const Material&) = delete;
			Material& operator=(const Material&) = delete;
			Material(Material&&) noexcept;
			Material& operator=(Material&&) noexcept;

			gl::Texture2D* GetTextureByMap(MapType type);
			AlphaMode GetAlphaMode() { return mAlphaMode; }
			int GetTextureCoordSets(MapType type) { return mTexCoordSets[type]; }

			float GetTransparency() const { return mTransparency; }
			float GetMetallicFactor() const { return mMetallicFactor; }
			float GetRoughnessFactor() const { return mRoughnessFactor; }
			glm::vec4 GetBaseColorFactor() const { return mBaseColorFactor; }
			glm::vec4 GetEmissiveFactor() const { return mEmissiveFactor; }

			
			
			float GetGlossinessFactor() const { return mExtension.glossinessFactor; }
			glm::vec4 GetDiffuseFactor() const { return mExtension.diffuseFactor; }
			glm::vec3 GetSpecularFactor() const { return mExtension.specularFactor; }

			void SetTransparency(float transparency) { mTransparency = transparency; }
			void SetMetallicFactor(float metallic) { mMetallicFactor = metallic; }
			void SetRoughnessFactor(float roughness) { mRoughnessFactor = roughness; }
			void SeBaseColorFactor(glm::vec4 baseColor) { mBaseColorFactor = baseColor; }
			void SetEmissiveFactor(glm::vec4 emissive) { mEmissiveFactor = emissive; }

			void SetGlossinessFactor(float glossiness) { mExtension.glossinessFactor = glossiness; }
			void SetDiffuseFactor(glm::vec4 diffuse) { mExtension.diffuseFactor = diffuse; } 
			void SetSpecularFactor(glm::vec3 specular) { mExtension.specularFactor = specular; }

			void LoadFromTinyGLTF(tinygltf::Material& material, std::vector<Image>& textures);

			//void AttachMap(const MapType type, uint8_t* data, size_t width, size_t height, size_t channels, bool isFloatingPoint);
			void AttachMap(const MapType type, Image* image);
			void DetachMap(const MapType type);

		private:
	};

}

#endif // !1
