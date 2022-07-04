#include "Model.h"
#include <fstream>
#include <vector>
#include <utilities/Resources/ResourceManager.h>
//#include <utilities/Shape/Vertex.h>

#ifndef __EMSCRIPTEN__
#ifdef __GNUC__
#define errno_t int
errno_t fopen_s(FILE **f, const char *name, const char *mode)
{
	errno_t ret = 0;
	assert(f);
	*f = fopen(name, mode);
	/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
	if (!*f)
		ret = errno;
	return ret;
}
errno_t _wfopen_s(FILE **f, const wchar_t *name, const wchar_t *mode)
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
	Model::Model() : mProgress(0.f), mIsReady(false), mIsBinary(false)
	{
	}

	Model::~Model()
	{
	}

	void Model::LoadTextures(tinygltf::Model &model)
	{
		mTextures.clear();
		mTextures.resize(model.textures.size());
		for (uint32_t i = 0; i < model.textures.size(); i++)
		{
			tinygltf::Texture &tex = model.textures[i];
			tinygltf::Image &image = model.images[tex.source];
			int sampler_idx = tex.sampler;

			mTextures[i].LoadFromGLTF(image, model.samplers, tex.sampler);
		}
	}

	void Model::LoadMaterials(tinygltf::Model &model)
	{
		mMaterials.clear();
		mMaterials.resize(model.materials.size());
		for (uint32_t i = 0; i < model.materials.size(); i++)
		{
			tinygltf::Material &mat = model.materials[i];
			mMaterials[i].LoadFromTinyGLTF(mat, mTextures);
		}
		// Push a default material at the end of the list for meshes with no material assigned
		mMaterials.push_back(std::move(Material()));
	}

	void Model::LoadFromFile(std::string filename, float scale)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;
		std::string error;
		std::string warning;

		bool binary = false;
		size_t extpos = filename.rfind('.', filename.length());
		if (extpos != std::string::npos)
		{
			binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
		}

		bool fileLoaded = false;
		if (binary)
			fileLoaded = gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filename);
		else
			fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);

		if (fileLoaded)
		{
			LoadGltfModel(gltfModel, scale);
		}

		mIsReady = true;
	}

	void Model::Load(const char *file)
	{
		std::string fileName = std::string(file);
		mIsBinary = false;
		size_t extpos = fileName.rfind('.', fileName.length());
		if (extpos != std::string::npos)
		{
			mIsBinary = (fileName.substr(extpos + 1, fileName.length() - extpos) == "glb");
		}
#ifdef __EMSCRIPTEN__
		emscripten_async_wget2_data(
			fileName.c_str(), "GET", NULL, this, true,
			[](unsigned handle, void *arg, void *data, unsigned size)
			{
				Model *model = (Model *)arg;
				model->LoadFromMemoryTinyGLTF((const uint8_t *)data, size, model->mIsBinary);
			},
			[](unsigned handle, void *arg, int error_code, const char *status)
			{
				fprintf(stderr, "Failed to load model error_code: %d, status: %s\n", error_code, status);
			},
			[](unsigned handle, void *arg, int bytes_loaded, int total_size)
			{
				Model *model = (Model *)arg;
				if (total_size)
					model->mProgress = ((float)bytes_loaded / total_size) * 100.f;
			});
#else

		LoadFromFile(file);
		mProgress = 100.f;
		mIsReady = true;
#endif
	}

	void Model::LoadFromMemoryTinyGLTF(const uint8_t *data, size_t dataSize, bool is_binary, float scale)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;
		std::string error;
		std::string warning;

		bool fileLoaded = false;

		if (is_binary)
			fileLoaded = gltfContext.LoadBinaryFromMemory(&gltfModel, &error, &warning, (const unsigned char *)data, dataSize);
		else
			fileLoaded = gltfContext.LoadASCIIFromString(&gltfModel, &error, &warning, (const char *)data, dataSize, "");

		if (fileLoaded)
		{
			LoadGltfModel(gltfModel, scale);
		}

		mIsReady = true;
	}

	void Model::LoadGltfModel(tinygltf::Model &gltfModel, float scale)
	{
		const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

		LoadTextures(gltfModel);
		LoadMaterials(gltfModel);

		std::vector<uint32_t> indexBuffer;
		std::vector<rsrc::Vertex> vertexBuffer;

		mNodes.clear();
		mNodes.resize(scene.nodes.size());
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			mNodes[i].LoadFromTinyGLTF(nullptr, node, mMaterials, indexBuffer, vertexBuffer, scene.nodes[i], gltfModel, scale);
		}

		mVBO.SetData(sizeof(rsrc::Vertex) * vertexBuffer.size(), vertexBuffer.data());

		mVAO.AddVertexLayout(
			mVBO,
			{
				gl::VertexAttribute::Entry<glm::vec3>(), // position
				gl::VertexAttribute::Entry<glm::vec3>(), // normal
				gl::VertexAttribute::Entry<glm::vec2>(), // uv0 coords
				gl::VertexAttribute::Entry<glm::vec2>()	 // uv1 coords
			},
			gl::VertexAttributeRate::PER_VERTEX);

		if (indexBuffer.size() > 0)
		{
			mIBO = std::make_unique<gl::IndexBuffer>();
			mIBO->Data(sizeof(uint32_t) * indexBuffer.size(), indexBuffer.data(), gl::DataType::UNSIGNED_INT);
			mVAO.LinkIndexBuffer(*mIBO);
		}
	}

	void Model::CalculateBoundingBox(Node *node, Node *parent)
	{
	}
	void Model::GetSceneDimensions()
	{
	}

	void Model::DrawNode(Node *node)
	{
	}

	void Model::Draw(const glm::mat4 &model)
	{
		if (mIsReady)
		{
			gl::Program *pbr_prog = util::ResourceManager::GetShaders()->GetPBRProg();
			pbr_prog->Use();

			mVAO.Bind();
			for (auto &node : mNodes)
			{
				node.Draw(pbr_prog, model);
			}
		}
	}
}