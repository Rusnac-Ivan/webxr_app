#include "Node.h"
#include "Material.h"
#include <tiny_gltf.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace rsrc
{
	Node::Node() :
		mParent(nullptr),
		mIndex(-1),
		mTransform(1.f)
	{
	}

	Node::~Node()
	{
	}

	Node::Node(Node&& other) noexcept :
		mParent(other.mParent),
		mName(std::move(other.mName)),
		mIndex(other.mIndex),
		mTransform(std::move(other.mTransform)),
		mMeshes(std::move(other.mMeshes)),
		mChildrens(std::move(mChildrens))
	{
		other.mParent = nullptr;
		other.mIndex = -1;
	}
	Node& Node::operator=(Node&& other) noexcept
	{
		mParent = other.mParent;
		mName = std::move(other.mName);
		mIndex = other.mIndex;
		mTransform = std::move(other.mTransform);
		mMeshes = std::move(mMeshes);
		mChildrens = std::move(mChildrens);

		other.mParent = nullptr;
		other.mIndex = -1;

		return *this;
	}

	void Node::LoadFromTinyGLTF(Node* parent, const tinygltf::Node& node, std::vector<Material>& materials, uint32_t nodeIndex, tinygltf::Model& model, float globalscale)
	{
		this->mIndex = nodeIndex;
		this->mParent = parent;
		this->mName = node.name;
		this->mTransform = glm::mat4(1.0f);

		// Generate local node matrix
		glm::vec3 translation = glm::vec3(0.0f);
		if (node.translation.size() == 3)
			translation = glm::make_vec3(node.translation.data());
		
		glm::quat q = glm::quat(1.f, 0.f, 0.f, 0.0f);
		if (node.rotation.size() == 4)
			q = glm::make_quat(node.rotation.data());

		glm::vec3 scale = glm::vec3(1.0f);
		if (node.scale.size() == 3)
			scale = glm::make_vec3(node.scale.data());

		glm::mat4 matrix = glm::mat4(1.f);
		if (node.matrix.size() == 16)
			matrix = glm::make_mat4x4(node.matrix.data());

		this->mTransform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(q) * glm::scale(glm::mat4(1.0f), scale) * matrix;

		// Node with children
		if (node.children.size() > 0)
		{
			mChildrens.resize(node.children.size());
			for (size_t i = 0; i < node.children.size(); i++)
			{
				mChildrens[i].LoadFromTinyGLTF(this, model.nodes[node.children[i]], materials, node.children[i], model, globalscale);
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
				const tinygltf::Primitive& primitive = mesh.primitives[j];

				mMeshes[j].LoadFromTinyGLTF(model, primitive, materials, GetGlobalMatrix());
			}
		}
	}

	void Node::Draw(gl::Program* program, const glm::mat4& model)
	{
		for (auto& mesh : mMeshes)
		{
			mesh.Draw(program, model);
		}

		for (auto& node : mChildrens)
		{
			node.Draw(program, model);
		}
	}
}