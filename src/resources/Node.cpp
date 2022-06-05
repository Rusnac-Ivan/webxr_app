#include "Node.h"

namespace rsrc
{
	Node::Node() :
		mParent(nullptr),
		mIndex(-1),
		mTranslation(0.f),
		mScale(1.f),
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
		mTranslation(std::move(other.mTranslation)),
		mScale(std::move(other.mScale)),
		mRotation(std::move(other.mRotation)),
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
		mTranslation = std::move(other.mTranslation);
		mScale = std::move(other.mScale);
		mRotation = std::move(other.mRotation);
		mTransform = std::move(other.mTransform);
		mMeshes = std::move(mMeshes);
		mChildrens = std::move(mChildrens);

		other.mParent = nullptr;
		other.mIndex = -1;

		return *this;
	}

	void Node::LoadFromTinyGLTF(Node* parent, const tinygltf::Node& node, const std::vector<Material>& materials, uint32_t nodeIndex, tinygltf::Model& model, float globalscale)
	{
		this->mIndex = nodeIndex;
		this->mParent = parent;
		this->mName = node.name;
		this->mTransform = glm::mat4(1.0f);

		// Generate local node matrix
		glm::vec3 translation = glm::vec3(0.0f);
		if (node.translation.size() == 3)
		{
			translation = glm::make_vec3(node.translation.data());
			this->mTranslation = translation;
		}
		glm::mat4 rotation = glm::mat4(1.0f);
		if (node.rotation.size() == 4)
		{
			glm::quat q = glm::make_quat(node.rotation.data());
			this->mRotation = glm::mat4(q);
		}
		glm::vec3 scale = glm::vec3(1.0f);
		if (node.scale.size() == 3)
		{
			scale = glm::make_vec3(node.scale.data());
			this->mScale = scale;
		}
		if (node.matrix.size() == 16)
		{
			this->mTransform = glm::make_mat4x4(node.matrix.data());
		}

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

				mMeshes[j].LoadFromTinyGLTF(model, primitive, materials, this->mTransform);
			}
		}
	}

	void Node::Draw()
	{
		for (auto& mesh : mMeshes)
		{
			mesh.Draw();
		}

		for (auto& node : mChildrens)
		{
			node.Draw();
		}
	}
}