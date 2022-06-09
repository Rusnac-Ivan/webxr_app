#include "Mesh.h"
#include <utilities/BoundingObjects/AABB.h>
#include <tiny_gltf.h>
#include "Material.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace rsrc
{
	Mesh::Mesh() :
		mPrimitiveMode(gl::Primitive::TRIANGLES),
		mFirstIndex(0),
		mIndexCount(0),
		mVertexCount(0),
		mTransform(glm::mat4(1.f))
	{}

	Mesh::~Mesh() {}

	Mesh::Mesh(Mesh&& other) noexcept :
		mPrimitiveMode(other.mPrimitiveMode),
		mFirstIndex(other.mFirstIndex),
		mIndexCount(other.mIndexCount),
		mVertexCount(other.mVertexCount),
		mMaterial(std::move(other.mMaterial)),
		mVertices(std::move(other.mVertices)),
		mIndices(std::move(other.mIndices)),
		mVBO(std::move(other.mVBO)),
		mIBO(std::move(other.mIBO)),
		mVAO(std::move(other.mVAO)),
		mTransform(std::move(other.mTransform)),
		mAABB(std::move(other.mAABB))
	{}
	Mesh& Mesh::operator=(Mesh&& other) noexcept
	{
		mPrimitiveMode = other.mPrimitiveMode;
		mFirstIndex = other.mFirstIndex;
		mIndexCount = other.mIndexCount;
		mVertexCount = other.mVertexCount;
		mMaterial = std::move(other.mMaterial);
		mVertices = std::move(other.mVertices);
		mIndices = std::move(other.mIndices);
		mVBO = std::move(other.mVBO);
		mIBO = std::move(other.mIBO);
		mVAO = std::move(other.mVAO);
		mTransform = std::move(other.mTransform);
		mAABB = std::move(other.mAABB);

		return *this;
	}

	static inline int32_t GetTypeSizeInBytes(uint32_t ty)
	{
		if (ty == TINYGLTF_TYPE_SCALAR)
		{
			return 1;
		}
		else if (ty == TINYGLTF_TYPE_VEC2)
		{
			return 2;
		}
		else if (ty == TINYGLTF_TYPE_VEC3)
		{
			return 3;
		}
		else if (ty == TINYGLTF_TYPE_VEC4)
		{
			return 4;
		}
		else if (ty == TINYGLTF_TYPE_MAT2)
		{
			return 4;
		}
		else if (ty == TINYGLTF_TYPE_MAT3)
		{
			return 9;
		}
		else if (ty == TINYGLTF_TYPE_MAT4)
		{
			return 16;
		}
		else
		{
			// Unknown componenty type
			return -1;
		}
	}


	void Mesh::LoadFromTinyGLTF(tinygltf::Model& model, const tinygltf::Primitive& primitive, const std::vector<Material>& materials, const glm::mat4& transform)
	{
		mTransform = transform;

		gl::Primitive mode = static_cast<gl::Primitive>(primitive.mode);
		uint32_t indexStart = static_cast<uint32_t>(mIndices.size());
		uint32_t vertexStart = static_cast<uint32_t>(mVertices.size());
		uint32_t indexCount = 0;
		uint32_t vertexCount = 0;
		uint32_t byteOffset;

		assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
		assert(primitive.attributes.find("NORMAL") != primitive.attributes.end());
		assert(primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end());

		const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
		const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
		const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

		vertexCount = static_cast<uint32_t>(posAccessor.count);
		const float* bufferPos = reinterpret_cast<const float*>(&posBuffer.data[posView.byteOffset + posAccessor.byteOffset]);
		const int posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : GetTypeSizeInBytes(TINYGLTF_TYPE_VEC3);

		glm::vec3 posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
		glm::vec3 posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

		mAABB.Align(posMin, posMax);

		const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
		const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
		const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];

		const float* bufferNorm = reinterpret_cast<const float*>(&normBuffer.data[normView.byteOffset + normAccessor.byteOffset]);
		const int normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : GetTypeSizeInBytes(TINYGLTF_TYPE_VEC3);

		const tinygltf::Accessor& texCoordAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
		const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAccessor.bufferView];
		const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];

		const float* bufferTexCoord = reinterpret_cast<const float*>(&texCoordBuffer.data[texCoordView.byteOffset + texCoordAccessor.byteOffset]);
		const int texCoordByteStride = normAccessor.ByteStride(texCoordView) ? (texCoordAccessor.ByteStride(texCoordView) / sizeof(float)) : GetTypeSizeInBytes(TINYGLTF_TYPE_VEC2);

		for (size_t v = 0; v < posAccessor.count; v++)
		{
			util::Vertex vert{};
			vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * posByteStride]), 1.0f);
			vert.normal = glm::normalize(glm::vec3(bufferNorm ? glm::make_vec3(&bufferNorm[v * normByteStride]) : glm::vec3(0.0f)));
			vert.uv0 = bufferTexCoord ? glm::make_vec2(&bufferTexCoord[v * texCoordByteStride]) : glm::vec3(0.0f);

			mVertices.push_back(vert);
		}

		if (primitive.indices > -1)
		{
			const tinygltf::Accessor& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			indexCount = static_cast<uint32_t>(accessor.count);
			const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);


			switch (accessor.componentType)
			{
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
				{
					const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						mIndices.push_back((uint32_t)(buf[index] + vertexStart));
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
				{
					const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						mIndices.push_back((uint16_t)(buf[index] + vertexStart));
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
				{
					const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						mIndices.push_back((uint8_t)(buf[index] + vertexStart));
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
			}
		}
		

		mVBO.SetData(sizeof(util::Vertex) * mVertices.size(), mVertices.data());

		mVAO.AddVertexLayout(
			mVBO,
			{
				gl::VertexAttribute::Entry<glm::vec3>(), //position
				gl::VertexAttribute::Entry<glm::vec3>(), //normal
				gl::VertexAttribute::Entry<glm::vec2>() //uv coords
			},
			gl::VertexAttributeRate::PER_VERTEX
		);

		if (mIndices.size() > 0)
		{
			mIBO = std::make_unique<gl::IndexBuffer>();
			mIBO->Data(sizeof(uint32_t) * mIndices.size(), mIndices.data(), gl::DataType::UNSIGNED_INT);
			mVAO.LinkIndexBuffer(*mIBO);
		}

		tinygltf::Material& mat = primitive.material > -1 ? model.materials[primitive.material] : model.materials.back();
		mMaterial = primitive.material > -1 ? &(materials[primitive.material]) : &(materials.back());
	}

	void Mesh::Draw()
	{
		mVAO.Bind();
		if (mIBO.get() != nullptr)
			gl::Render::DrawIndices(mPrimitiveMode, mIndexCount, gl::DataType::UNSIGNED_INT, 0);
		else
			gl::Render::DrawVertices(mPrimitiveMode, mVertexCount, 0);
		mVAO.UnBind();
	}
}