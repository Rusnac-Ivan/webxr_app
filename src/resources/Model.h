#ifndef _RSRC_MODEL_H_
#define _RSRC_MODEL_H_

#include <vector>
#include <utilities/BoundingObjects/AABB.h>
#include <opengl/Texture.h>
#include <string>
#include <cstdint>



namespace tinygltf
{
	class Model;
}

namespace rsrc
{
	class Image;
	class Material;
	class Mesh;
	class Node;

	class Model
	{
		struct TextureSampler 
		{
			gl::Texture::FilterMode magFilter;
			gl::Texture::FilterMode minFilter;
			gl::Texture::WrapMode wrapS;
			gl::Texture::WrapMode wrapT;
		};

		std::vector<Node> mNodes;
		util::AABB mAABB;

		std::vector<TextureSampler> mSamplers;
		std::vector<Mesh> mMeshes;
		std::vector<Image> mTextures;
		std::vector<Material> mMaterials;
	public:
		Model();
		~Model();

		void LoadTextures(tinygltf::Model& model);
		void LoadMaterials(tinygltf::Model& model);

		void LoadFromFile(std::string filename, float scale = 1.0f);
		void LoadFromMemoryTinyGLTF(const uint8_t* data, size_t dataSize, bool is_binary, float scale = 1.0f);

		void Draw();
		void CalculateBoundingBox(Node* node, Node* parent);
		void GetSceneDimensions();

	private:

	};


}

#endif
