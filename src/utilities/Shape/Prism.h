#ifndef _UTIL_PRISM_H_
#define _UTIL_PRISM_H_

#include <vector>
#include "Vertex.h"

namespace util
{
	class Prism
	{
		std::vector<Vertex> mVertices;
	public:
		Prism();
		~Prism();

		void Generate(float edge_width);
		void Reset();

		const float* GetVertexData() const { return reinterpret_cast<const float*>(mVertices.size() > 0 ? mVertices.data() : nullptr); }
		uint32_t GetVertexCount() const { return mVertices.size(); }


	private:

	};

	
}

#endif