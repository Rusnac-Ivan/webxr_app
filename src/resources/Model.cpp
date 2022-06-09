#include "Model.h"

#ifdef __GNUC__
	#define errno_t int
	errno_t fopen_s(FILE** f, const char* name, const char* mode) 
	{
		errno_t ret = 0;
		assert(f);
		*f = fopen(name, mode);
		/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
		if (!*f)
			ret = errno;
		return ret;
	}
	errno_t _wfopen_s(FILE** f, const wchar_t* name, const wchar_t* mode)
	{
		errno_t ret = 0;
		assert(f);
		*f = _wfopen(name, mode);
		/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
		if (!*f)
			ret = errno;
		return ret;
	}
#endif

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include "Image.h"
#include "Mesh.h"
#include "Material.h"
#include "Node.h"

namespace rsrc
{
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
				mNodes[i].LoadFromTinyGLTF(nullptr, node, mMaterials, scene.nodes[i], gltfModel, scale);
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