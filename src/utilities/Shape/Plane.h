#ifndef _UTIL_PLANE_H_
#define _UTIL_PLANE_H_

#include <vector>
#include "Vertex.h"

namespace util
{
	

	class Plane
	{
	public:
		enum Direction
		{
			OX_POS,
			OX_NEG,
			OY_POS,
			OY_NEG,
			OZ_POS,
			OZ_NEG
		};
	private:
		float mWidth;
		float mHeight;
		
		float mHSD;
		float mVSD;

		Direction mDir;

		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;
	public:
		Plane();
		~Plane();

		void Generate(float width, float height, float hsd, float vsd, Direction dir = Direction::OZ_POS);
		void Reset();

		const float* GetVertexData() const { return reinterpret_cast<const float*>(mVertices.size() > 0 ? mVertices.data() : nullptr); }
		uint32_t GetVertexCount() const { return mVertices.size(); }

		const uint32_t* GetIndicesData() const { return reinterpret_cast<const uint32_t*>(mIndices.size() > 0 ? mIndices.data() : nullptr); }
		uint32_t GetIndicesCount() const { return mIndices.size(); }

	private:
		void Generate(glm::vec3 p0, glm::vec3 norm, float h_step, float v_step);
		glm::vec3 MakeStep(Direction dir, float h_step, float v_step);
	};

	
}

#endif