#ifndef _RSRC_MESH_H_
#define _RSRC_MESH_H_

#include <glm/glm.hpp>
#include <opengl/VertexBuffer.h>
#include <opengl/VertexArray.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/Render.h>
#include <utilities/BoundingObjects/AABB.h>
#include <memory>

namespace tinygltf
{
	class Model;
	struct Material;
	struct Primitive;
}

namespace rsrc
{
	class Material;

	struct Vertex 
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv0;
		//glm::vec2 uv1;
		//glm::vec4 joint;
		//glm::vec4 weight;
	};

	class Mesh
	{
		uint32_t mFirstIndex;
		uint32_t mIndexCount;
		uint32_t mVertexCount;

		gl::Primitive mPrimitiveMode;
		gl::VertexBuffer mVBO;
		std::unique_ptr<gl::IndexBuffer> mIBO;
		gl::VertexArray mVAO;

		const Material* mMaterial;

		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;

		util::AABB mAABB;

		glm::mat4 mTransform;
	public:
		Mesh();
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept;
		Mesh& operator=(Mesh&&) noexcept;

		uint32_t GetFirstIndex() const { return mFirstIndex; }
		uint32_t GetIndexCount() const { return mIndexCount; }
		uint32_t GetVertexCount() const { return mVertexCount; }
		bool HasIndices() { return mIndices.size() > 0; }
		const util::AABB& GetBoundingBox() { return mAABB; }

		void LoadFromTinyGLTF(tinygltf::Model& model, const tinygltf::Primitive& primitive, const std::vector<Material>& materials, const glm::mat4& transform);

		void Draw();
	};

	
}

#endif // !_RSRC_MESH_H_
