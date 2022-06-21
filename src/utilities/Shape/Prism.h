#ifndef _UTIL_PRISM_H_
#define _UTIL_PRISM_H_

#include <vector>

namespace util
{
	struct Vertex;

	class Prism
	{
		std::vector<Vertex> mVertices;

	public:
		Prism();
		~Prism();

		void Generate(float edge_width);
		void Reset();

		const float *GetVertexData() const;
		uint32_t GetVertexCount() const;

	private:
	};

}

#endif