#include "Node.h"
#include "Material.h"
#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace rsrc
{
	Node::Node() : mParent(nullptr),
				   mIndex(-1),
				   mLocalMatrix(1.f)
	{
	}

	Node::~Node()
	{
	}

	Node::Node(Node &&other) noexcept : mParent(other.mParent),
										mName(std::move(other.mName)),
										mIndex(other.mIndex),
										mLocalMatrix(std::move(other.mLocalMatrix)),
										mMeshes(std::move(other.mMeshes)),
										mChildrens(std::move(mChildrens))
	{
		other.mParent = nullptr;
		other.mIndex = -1;
	}
	Node &Node::operator=(Node &&other) noexcept
	{
		mParent = other.mParent;
		mName = std::move(other.mName);
		mIndex = other.mIndex;
		mLocalMatrix = std::move(other.mLocalMatrix);
		mMeshes = std::move(mMeshes);
		mChildrens = std::move(mChildrens);

		other.mParent = nullptr;
		other.mIndex = -1;

		return *this;
	}

	void Node::LoadFromTinyGLTF(Node *parent,
								const tinygltf::Node &node,
								std::vector<Material> &materials,
								std::vector<uint32_t> &indexBuffer,
								std::vector<rsrc::Vertex> &vertexBuffer,
								uint32_t nodeIndex,
								tinygltf::Model &model,
								float globalscale)
	{
		this->mIndex = nodeIndex;
		this->mParent = parent;
		this->mName = node.name;
		this->mLocalMatrix = glm::mat4(1.0f);

		// Generate local node matrix
		glm::vec3 translation = glm::vec3(0.0f);
		if (node.translation.size() == 3)
			translation = glm::make_vec3(node.translation.data());

		glm::quat rotation = glm::identity<glm::quat>();
		if (node.rotation.size() == 4)
		{
			rotation.x = static_cast<float>(node.rotation[0]);
			rotation.y = static_cast<float>(node.rotation[1]);
			rotation.z = static_cast<float>(node.rotation[2]);
			rotation.w = static_cast<float>(node.rotation[3]);
			// rotation = glm::make_quat(node.rotation.data());
		}

		glm::vec3 scale = glm::vec3(1.0f);
		if (node.scale.size() == 3)
			scale = glm::make_vec3(node.scale.data());

		glm::mat4 matrix = glm::mat4(1.f);
		if (node.matrix.size() == 16)
			matrix = glm::make_mat4x4(node.matrix.data());

		// glm::mat4 m = glm::translate(glm::mat4(1.0f), translation)* glm::mat4(rotation)* glm::scale(glm::mat4(1.0f), scale) * matrix;
		// make node local transform matrix
		this->mLocalMatrix = glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;

		this->mGlobalMatrix = GetGlobalMatrix();

		// Node with children
		if (node.children.size() > 0)
		{
			mChildrens.resize(node.children.size());
			for (size_t i = 0; i < node.children.size(); i++)
			{
				mChildrens[i].LoadFromTinyGLTF(this, model.nodes[node.children[i]], materials, indexBuffer, vertexBuffer, node.children[i], model, globalscale);
			}
		}

		// Node contains mesh data
		if (node.mesh > -1)
		{
			tinygltf::Mesh &mesh = model.meshes[node.mesh];

			mMeshes.clear();
			mMeshes.resize(mesh.primitives.size());
			for (size_t j = 0; j < mesh.primitives.size(); j++)
			{
				const tinygltf::Primitive &primitive = mesh.primitives[j];

				mMeshes[j].LoadFromTinyGLTF(model, primitive, materials, indexBuffer, vertexBuffer);
			}
		}
	}

	/*void Node::Update()
	{
		mGlobalMatrix = GetGlobalMatrix();

		for (auto& child : mChildrens)
			child.Update();
	}*/

	void Node::Draw(gl::Program *program, const glm::mat4 &model)
	{
		for (auto &mesh : mMeshes)
		{
			mesh.Draw(program, model * mGlobalMatrix);
		}

		for (auto &child : mChildrens)
			child.Draw(program, model);
	}
}