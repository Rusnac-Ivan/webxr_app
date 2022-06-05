#ifndef _RSRC_MODEL_H_
#define _RSRC_MODEL_H_

#include <vector>

#include <utilities/BoundingObjects/AABB.h>
#include "Node.h"
#include <opengl/Texture.h>

namespace rsrc
{
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

	Model::Model()
	{
	}

	Model::~Model()
	{
	}



	


	void Model::LoadTextures(tinygltf::Model& model)
	{
		mTextures.clear();
		mTextures.resize(model.textures.size());
		for (uint32_t i = 0; i < model.textures.size(); i++)
		{
			tinygltf::Texture& tex = model.textures[i];
			tinygltf::Image& image = model.images[tex.source];
			int sampler_idx = tex.sampler;

			mTextures[i].LoadFromGLTF(image, model.samplers, tex.sampler);
		}
	}

	void Model::LoadMaterials(tinygltf::Model& model)
	{
		mMaterials.clear();
		mMaterials.resize(model.materials.size());
		for (uint32_t i = 0; i < model.materials.size(); i++)
		{
			tinygltf::Material& mat = model.materials[i];
			mMaterials[i].LoadFromTinyGLTF(mat, mTextures);
		}
	}


	void Model::LoadFromFile(std::string filename, float scale)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;
		std::string error;
		std::string warning;

		bool binary = false;
		size_t extpos = filename.rfind('.', filename.length());
		if (extpos != std::string::npos) {
			binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
		}
	}
	void Model::LoadFromMemoryTinyGLTF(const uint8_t* data, size_t dataSize, bool is_binary, float scale)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;
		std::string error;
		std::string warning;

		bool fileLoaded = false;

		if (is_binary)
			fileLoaded = gltfContext.LoadBinaryFromMemory(&gltfModel, &error, &warning, (const unsigned char*)data, dataSize);
		else
			fileLoaded = gltfContext.LoadASCIIFromString(&gltfModel, &error, &warning, (const char*)data, dataSize, "");

		if (fileLoaded)
		{
			const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

			mNodes.clear();
			mNodes.resize(scene.nodes.size());
			for (size_t i = 0; i < scene.nodes.size(); i++)
			{
				const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
				mNodes[i].LoadFromTinyGLTF(nullptr, node, scene.nodes[i], gltfModel, scale);
			}

			
		}
	}


	void Model::CalculateBoundingBox(Node* node, Node* parent)
	{

	}
	void Model::GetSceneDimensions()
	{

	}

	void Model::Draw()
	{
		for (auto& node : mNodes)
		{
			node.Draw();
		}
	}


}

#endif
