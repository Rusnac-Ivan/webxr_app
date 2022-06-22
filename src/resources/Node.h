#ifndef _RSRC_NODE_H_
#define _RSRC_NODE_H_

#include "Mesh.h"
#include <glm/gtc/quaternion.hpp>

namespace tinygltf
{
	class Model;
	class Node;
}

namespace rsrc
{
	class Material;

	class Node
	{
		Node *mParent;
		int32_t mIndex;
		std::vector<Node> mChildrens;

		std::string mName;
		std::vector<Mesh> mMeshes;

		glm::mat4 mTransform;

	public:
		Node();
		~Node();

		Node(const Node &) = delete;
		Node &operator=(const Node &) = delete;
		Node(Node &&) noexcept;
		Node &operator=(Node &&) noexcept;

		const glm::mat4 &GetLocalMatrix() { return mTransform; }

		void LoadFromTinyGLTF(Node *parent, const tinygltf::Node &node, std::vector<Material> &materials, uint32_t nodeIndex, tinygltf::Model &model, float globalscale);

		void Draw(const glm::mat4& model);

		glm::mat4 GetGlobalMatrix() 
		{
			glm::mat4 m = GetLocalMatrix();
			Node* p = mParent;
			while (p) {
				m = p->GetLocalMatrix() * m;
				p = p->mParent;
			}
			return m;
		}

	private:
	};

}

#endif