#include "Model.h"
#include <fstream>
#include <vector>
#include <utilities/Resources/ResourceManager.h>
//#include <utilities/Shape/Vertex.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

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
	Model::Model() : mProgress(0.f), mIsReady(false), mIsBinary(false), mIsEditable(false)
	{
		mTransform = glm::mat4(1.f);
		mCenter = glm::vec3(0.f);
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

	void Model::Load(const char *file, bool is_editable)
	{
		mIsEditable = is_editable;
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

		GetSceneDimensions();
	}

	void Model::CalculateBoundingBox(Node *node, Node *parent)
	{
		util::AABB parentBvh = parent ? parent->GetBoundingVolumeHierarchy() : util::AABB();

		if (node->HaveMesh())
		{
			if (node->GetBoundingBox().IsValid())
			{
				node->GetBoundingBox().SetState(node->GetLocalMatrix());
				if (node->GetChildrens().size() == 0)
					node->GetBoundingVolumeHierarchy().Align(node->GetBoundingBox().GetMin(), node->GetBoundingBox().GetMax());
			}
		}

		if (node->GetBoundingVolumeHierarchy().IsValid())
		{
			dimensions.min = glm::min(dimensions.min, node->GetBoundingVolumeHierarchy().GetMin());
			dimensions.max = glm::max(dimensions.max, node->GetBoundingVolumeHierarchy().GetMax());
		}

		for (auto &child : node->GetChildrens())
			CalculateBoundingBox(&child, node);
	}

	void Model::GetSceneDimensions()
	{
		dimensions.min = glm::vec3(FLT_MAX);
		dimensions.max = glm::vec3(-FLT_MAX);

		// Calculate binary volume hierarchy for all nodes in the scene
		for (auto &node : mNodes)
		{
			CalculateBoundingBox(&node, nullptr);
		}

		// Calculate scene aabb
		mAABBScale = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.max[0] - dimensions.min[0], dimensions.max[1] - dimensions.min[1], dimensions.max[2] - dimensions.min[2]));
		mAABBScale[3][0] = dimensions.min[0];
		mAABBScale[3][1] = dimensions.min[1];
		mAABBScale[3][2] = dimensions.min[2];

		mCenter = (dimensions.min + dimensions.max) / 2.f;

		printf("model min: [%.3f, %.3f, %.3f]\n", dimensions.min.x, dimensions.min.y, dimensions.min.z);
		printf("model max: [%.3f, %.3f, %.3f]\n", dimensions.max.x, dimensions.max.y, dimensions.max.z);
		printf("model_center: [%.3f, %.3f, %.3f]\n", mCenter.x, mCenter.y, mCenter.z);

		// mTransform = glm::translate(glm::mat4(1.f), glm::vec3(-model_center.x, 0.f, -model_center.z));
	}

	void Model::DrawNode(Node *node)
	{
	}

	void Model::Draw(const glm::mat4 &model)
	{
		return;
		mModel = model;
		if (mIsReady)
		{
			gl::Program *pbr_prog = util::ResourceManager::GetShaders()->GetLightMapProg();
			pbr_prog->Use();

			mVAO.Bind();
			for (auto &node : mNodes)
			{
				node.Draw(pbr_prog, mTransform * mModel);
			}
		}
	}

	void Model::DrawEditor(const glm::mat4 &view, const glm::mat4 &proj)
	{
		return;
		if (mIsReady && mIsEditable)
		{
			{
				// ImGuizmo::Enable(true);

				ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y | ImGuizmo::TRANSLATE_Z;
				glm::vec3 snap = glm::vec3(0.1f, 0.1f, 0.1f);

				glm::mat4 matrix = mTransform * mModel;

				if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), operation, ImGuizmo::LOCAL, (float *)glm::value_ptr(matrix), NULL, (float *)glm::value_ptr(snap), NULL, NULL))
				{
					mTransform = matrix * glm::inverse(mModel);
				}

				// ImGuizmo::DrawCube(glm::value_ptr(view), glm::value_ptr(proj), mModel);

				/*float matrixTranslation[3], matrixRotation[3], matrixScale[3];
				ImGuizmo::DecomposeMatrixToComponents(gizmoMatrix.m16, matrixTranslation, matrixRotation, matrixScale);
				ImGui::InputFloat3("Tr", matrixTranslation, 3);
				ImGui::InputFloat3("Rt", matrixRotation, 3);
				ImGui::InputFloat3("Sc", matrixScale, 3);
				ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, gizmoMatrix.m16);*/
			}
		}
	}
}