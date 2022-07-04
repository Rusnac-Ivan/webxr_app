#ifndef _UTIL_PLANE_H_
#define _UTIL_PLANE_H_

#include <vector>
#include "Shape.h"
#include <glm/glm.hpp>

namespace util
{
	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	class Plane : public Shape<Options>
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

		glm::vec3 mNormal;
		glm::vec3 mOrigin;
		glm::vec3 mUp;
		glm::vec3 mRight;

	public:
		Plane();
		~Plane();

		float GetWidth() { return mWidth; }
		float GetHeight() { return mHeight; }

		glm::vec3 &GetNormal() { return mNormal; }
		glm::vec3 &GetOrigin() { return mOrigin; }
		glm::vec3 &GetUp() { return mUp; }
		glm::vec3 &GetRight() { return mRight; }

		void Generate(float width, float height, float hsd, float vsd, Direction dir = Direction::OZ_POS);

	private:
		void Generate(glm::vec3 p0, glm::vec3 norm, float h_step, float v_step);
		glm::vec3 MakeStep(Direction dir, float h_step, float v_step);

		virtual void Generate();
	};

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	Plane<Options>::Plane() : mWidth(0.f),
							  mHeight(0.f),
							  mHSD(0.f),
							  mVSD(0.f),
							  mDir(Direction::OZ_POS),
							  mNormal(0.f),
							  mUp(0.f),
							  mOrigin(0.f),
							  mRight(0.f)

	{
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	Plane<Options>::~Plane()
	{
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	void Plane<Options>::Generate(float width, float height, float hsd, float vsd, Direction dir)
	{
		mWidth = width;
		mHeight = height;

		mDir = dir;
		mHSD = hsd;
		mVSD = vsd;

		Generate();

		Shape<Options>::MakeGLVAO();
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	void Plane<Options>::Generate()
	{
		Shape<Options>::mVertices.clear();
		Shape<Options>::mIndices.clear();

		float h_step = mWidth / mHSD;
		float v_step = mHeight / mVSD;

		switch (mDir)
		{
		case OX_POS:
		{
			mNormal = glm::vec3(1.f, 0.f, 0.f);
			mOrigin = glm::vec3(0.f, -mHeight / 2.f, mWidth / 2);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(0.f, 0.f, -1.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case OX_NEG:
		{
			mNormal = glm::vec3(-1.f, 0.f, 0.f);
			mOrigin = glm::vec3(0.f, -mHeight / 2.f, -mWidth / 2);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(0.f, 0.f, 1.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case OY_POS:
		{
			mNormal = glm::vec3(0.f, 1.f, 0.f);
			mOrigin = glm::vec3(-mWidth / 2, 0.f, mHeight / 2.f);
			mUp = glm::vec3(0.f, 0.f, 1.f);
			mRight = glm::vec3(-1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case OY_NEG:
		{
			mNormal = glm::vec3(0.f, -1.f, 0.f);
			mOrigin = glm::vec3(-mWidth / 2, 0.f, -mHeight / 2.f);
			mUp = glm::vec3(0.f, 0.f, 1.f);
			mRight = glm::vec3(1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case OZ_POS:
		{
			mNormal = glm::vec3(0.f, 0.f, 1.f);
			mOrigin = glm::vec3(-mWidth / 2, -mHeight / 2.f, 0.f);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case OZ_NEG:
		{
			mNormal = glm::vec3(0.f, 0.f, -1.f);
			mOrigin = glm::vec3(mWidth / 2, -mHeight / 2.f, 0.f);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(-1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		default:
			break;
		}
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	glm::vec3 Plane<Options>::MakeStep(Direction dir, float h_step, float v_step)
	{
		switch (dir)
		{
		case OX_POS:
			return glm::vec3(0.f, v_step, -h_step);
		case OX_NEG:
			return glm::vec3(0.f, v_step, h_step);
		case OY_POS:
			return glm::vec3(h_step, 0.f, -v_step);
		case OY_NEG:
			return glm::vec3(h_step, 0.f, v_step);
		case OZ_POS:
			return glm::vec3(h_step, v_step, 0.f);
		case OZ_NEG:
			return glm::vec3(-h_step, v_step, 0.f);
		default:
			return glm::vec3(0.f, 0.f, 0.f);
		}
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	void Plane<Options>::Generate(glm::vec3 p0, glm::vec3 norm, float h_step, float v_step)
	{
		float tex_u_step = 1.f / mHSD;
		float tex_v_step = 1.f / mVSD;

		// generate vertices
		for (int ver = 0; ver < mVSD + 1; ver++)
		{
			for (int hor = 0; hor < mHSD + 1; hor++)
			{
				Vertex<Options> vertex;
				if constexpr (Options & Attrib::POS)
					vertex.pos = p0 + MakeStep(mDir, h_step * hor, v_step * ver);
				if constexpr (Options & Attrib::NORM)
					vertex.norm = norm;
				if constexpr (Options & Attrib::UV)
					vertex.uv = glm::vec2(tex_u_step * hor, tex_v_step * ver);

				Shape<Options>::mVertices.push_back(vertex);
			}
		}

		// generate indices
		for (int ver = 0; ver < mVSD; ver++)
		{
			for (int hor = 0; hor < mHSD; hor++)
			{
				uint32_t idx0 = hor + (mHSD + 1) * ver;
				uint32_t idx1 = hor + (mHSD + 1) * (ver + 1);
				uint32_t idx2 = (hor + 1) + (mHSD + 1) * (ver + 1);
				uint32_t idx3 = hor + (mHSD + 1) * ver;
				uint32_t idx4 = (hor + 1) + (mHSD + 1) * (ver + 1);
				uint32_t idx5 = (hor + 1) + (mHSD + 1) * ver;

				Shape<Options>::mIndices.push_back(idx0);
				Shape<Options>::mIndices.push_back(idx1);
				Shape<Options>::mIndices.push_back(idx2);
				Shape<Options>::mIndices.push_back(idx3);
				Shape<Options>::mIndices.push_back(idx4);
				Shape<Options>::mIndices.push_back(idx5);
			}
		}
	}
}

#endif