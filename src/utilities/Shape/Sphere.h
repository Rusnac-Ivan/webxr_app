#ifndef _UTIL_SPHERE_H_
#define _UTIL_SPHERE_H_

#include <vector>
#include "Vertex.h"

namespace util
{
	class Sphere
	{
		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;
	public:
		Sphere();
		~Sphere();

		void Generate(float radius);
		void Reset();

		const float* GetVertexData() const { return reinterpret_cast<const float*>(mVertices.size() > 0 ? mVertices.data() : nullptr); }
		uint32_t GetVertexCount() const { return mVertices.size(); }

		const uint32_t* GetIndicesData() const { return reinterpret_cast<const uint32_t*>(mIndices.size() > 0 ? mIndices.data() : nullptr); }
		uint32_t GetIndicesCount() const { return mIndices.size(); }

	private:

	};

	
}

#endif