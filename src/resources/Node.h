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
			Node* mParent;
			int32_t mIndex;
			std::vector<Node> mChildrens;
			
			std::string mName;
			std::vector<Mesh> mMeshes;

			glm::vec3 mTranslation;
			glm::vec3 mScale;
			glm::quat mRotation;
			glm::mat4 mTransform;

		public:
			Node();
			~Node();

			Node(const Node&) = delete;
			Node& operator=(const Node&) = delete;
			Node(Node&&) noexcept;
			Node& operator=(Node&&) noexcept;

			void LoadFromTinyGLTF(Node* parent, const tinygltf::Node& node, const std::vector<Material>& materials, uint32_t nodeIndex, tinygltf::Model& model, float globalscale);

			void Draw();

		private:

	};

	
}

#endif