#ifndef _UTIL_SPHERE_H_
#define _UTIL_SPHERE_H_

#include <vector>
#include "Shape.h"

#define M_PI 3.14159265358979323846 // pi

namespace util
{

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	class Sphere : public Shape<Options>
	{
		float mRadius;
		uint32_t mSectorCount;
		uint32_t mStackCount;

	public:
		Sphere();
		~Sphere();

		void Generate(float radius, uint32_t sectorCount, uint32_t stackCount);

	protected:
		virtual void Generate();

	private:
	};

	template <Attrib::underlying_type Options>
	Sphere<Options>::Sphere()
	{
	}

	template <Attrib::underlying_type Options>
	Sphere<Options>::~Sphere()
	{
	}

	template <Attrib::underlying_type Options>
	void Sphere<Options>::Generate(float radius, uint32_t sectorCount, uint32_t stackCount)
	{
		mRadius = radius;
		mSectorCount = sectorCount;
		mStackCount = stackCount;

		Generate();

		Shape<Options>::MakeGLVAO();
	}

	template <Attrib::underlying_type Options>
	void Sphere<Options>::Generate()
	{
		float x, y, z, xz;
		float nx, ny, nz, lengthInv = 1.f / mRadius;

		const float PI = 3.14159265358979323846f;

		float sectorStep = 2.f * PI / mSectorCount;
		float stackStep = PI / mStackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= mStackCount; ++i)
		{
			stackAngle = PI / 2.f - i * stackStep;
			xz = mRadius * cosf(stackAngle);
			y = mRadius * sinf(stackAngle);
			for (int j = 0; j <= mSectorCount; ++j)
			{
				sectorAngle = j * sectorStep;

				x = xz * cosf(sectorAngle);
				z = xz * sinf(sectorAngle);

				nx = x * lengthInv;
				nz = z * lengthInv;
				ny = y * lengthInv;

				// vertex tex coord (u, v) range between [0, 1]
				float u = (float)j / mSectorCount;
				float v = 1.f - (float)i / mStackCount;

				Vertex<Options> vertex;
				if constexpr (Options & Attrib::POS)
					vertex.pos = glm::vec3(x, y, z);
				if constexpr (Options & Attrib::NORM)
					vertex.norm = glm::vec3(nx, ny, nz);
				if constexpr (Options & Attrib::UV)
					vertex.uv = glm::vec2(u, v);

				Shape<Options>::mVertices.push_back(vertex);
			}
		}

		int k1, k2;
		for (int i = 0; i < mStackCount; ++i)
		{
			k1 = i * (mSectorCount + 1); // beginning of current stack
			k2 = k1 + mSectorCount + 1;	 // beginning of next stack

			for (int j = 0; j < mSectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					Shape<Options>::mIndices.push_back(k1);
					Shape<Options>::mIndices.push_back(k2);
					Shape<Options>::mIndices.push_back(k1 + 1);

					// Shape<Options>::mIndices.push_back(k1 + 1);
					// Shape<Options>::mIndices.push_back(k2);
					// Shape<Options>::mIndices.push_back(k1);
				}

				// k1+1 => k2 => k2+1
				if (i != (mStackCount - 1))
				{
					Shape<Options>::mIndices.push_back(k1 + 1);
					Shape<Options>::mIndices.push_back(k2);
					Shape<Options>::mIndices.push_back(k2 + 1);

					// Shape<Options>::mIndices.push_back(k2 + 1);
					// Shape<Options>::mIndices.push_back(k2);
					// Shape<Options>::mIndices.push_back(k1 + 1);
				}
			}
		}
	}
}

#endif