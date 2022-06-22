#ifndef _RSRC_MESH_H_
#define _RSRC_MESH_H_

#include <glm/glm.hpp>
#include <opengl/VertexBuffer.h>
#include <opengl/VertexArray.h>
#include <opengl/IndexBuffer.h>
#include <opengl/VertexAttribute.h>
#include <opengl/Render.h>
#include <utilities/BoundingObjects/AABB.h>
#include <utilities/Shape/Vertex.h>
#include <memory>

namespace tinygltf
{
	class Model;
	struct Material;
	struct Primitive;
}

namespace gl
{
	class Program;
}

namespace rsrc
{
	class Material;

	

	class Mesh
	{
		struct UniformLocations
		{
			int32_t model = -1;
			int32_t uMaterState = -1;
			int32_t uTexMapSets = -1;
		};

		UniformLocations mUniformLocations;

		uint32_t mFirstIndex;
		uint32_t mIndexCount;
		uint32_t mVertexCount;

		gl::Primitive mPrimitiveMode;
		gl::VertexBuffer mVBO;
		std::unique_ptr<gl::IndexBuffer> mIBO;
		gl::VertexArray mVAO;

		Material* mMaterial;

		std::vector<util::Vertex> mVertices;
		std::vector<uint32_t> mIndices;

		util::AABB mAABB;

		glm::mat4 mTransform;

		gl::Program* mProgram;
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

		void LoadFromTinyGLTF(tinygltf::Model& model, const tinygltf::Primitive& primitive, std::vector<Material>& materials, const glm::mat4& transform);

		void Draw(const glm::mat4& model);
	};

	
}

#endif // !_RSRC_MESH_H_
