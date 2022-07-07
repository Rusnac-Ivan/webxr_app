#ifndef _RSRC_MODEL_H_
#define _RSRC_MODEL_H_

#include <vector>
#include <utilities/BoundingObjects/AABB.h>
#include <opengl/VertexBuffer.h>
#include <opengl/VertexArray.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/Texture2D.h>
#include <opengl/Program.h>
#include <string>
#include <memory>
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
		struct Dimensions {
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
		} dimensions;


		std::vector<Node> mNodes;
		util::AABB mAABB;

		std::vector<gl::Texture2D::Sampler> mSamplers;
		std::vector<Mesh> mMeshes;
		std::vector<Image> mTextures;
		std::vector<Material> mMaterials;

		gl::VertexBuffer mVBO;
		std::unique_ptr<gl::IndexBuffer> mIBO;
		gl::VertexArray mVAO;

		glm::mat4 mAABBScale;

		bool mIsBinary;

		float mProgress;
		bool mIsReady;

	public:
		Model();
		~Model();

		void Load(const char *file);

		float GetProgress() { return mProgress; }

		void Draw(const glm::mat4 &model);
		void CalculateBoundingBox(Node *node, Node *parent);
		void GetSceneDimensions();

	private:
		void DrawNode(Node *node);

		void LoadTextures(tinygltf::Model &model);
		void LoadMaterials(tinygltf::Model &model);

		void LoadFromFile(std::string filename, float scale = 1.0f);
		void LoadFromMemoryTinyGLTF(const uint8_t *data, size_t dataSize, bool is_binary, float scale = 1.0f);

		void LoadGltfModel(tinygltf::Model& gltfModel, float scale);
	};

}

#endif
