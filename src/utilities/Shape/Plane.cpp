#include "Plane.h"

namespace util
{
	Plane::Plane() : mWidth(0.f),
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

	Plane::~Plane()
	{
	}

	void Plane::Generate(float width, float height, float hsd, float vsd, Direction dir)
	{
		mWidth = width;
		mHeight = height;

		mVertices.clear();
		mIndices.clear();

		mDir = dir;
		mHSD = hsd;
		mVSD = vsd;

		float h_step = width / hsd;
		float v_step = height / vsd;

		switch (dir)
		{
		case util::Plane::OX_POS:
		{
			mNormal = glm::vec3(1.f, 0.f, 0.f);
			mOrigin = glm::vec3(0.f, -height / 2.f, width / 2);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(0.f, 0.f, -1.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case util::Plane::OX_NEG:
		{
			mNormal = glm::vec3(-1.f, 0.f, 0.f);
			mOrigin = glm::vec3(0.f, -height / 2.f, -width / 2);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(0.f, 0.f, 1.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case util::Plane::OY_POS:
		{
			mNormal = glm::vec3(0.f, 1.f, 0.f);
			mOrigin = glm::vec3(-width / 2, 0.f, height / 2.f);
			mUp = glm::vec3(0.f, 0.f, 1.f);
			mRight = glm::vec3(-1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case util::Plane::OY_NEG:
		{
			mNormal = glm::vec3(0.f, -1.f, 0.f);
			mOrigin = glm::vec3(-width / 2, 0.f, -height / 2.f);
			mUp = glm::vec3(0.f, 0.f, 1.f);
			mRight = glm::vec3(1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case util::Plane::OZ_POS:
		{
			mNormal = glm::vec3(0.f, 0.f, 1.f);
			mOrigin = glm::vec3(-width / 2, -height / 2.f, 0.f);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		case util::Plane::OZ_NEG:
		{
			mNormal = glm::vec3(0.f, 0.f, -1.f);
			mOrigin = glm::vec3(width / 2, -height / 2.f, 0.f);
			mUp = glm::vec3(0.f, 1.f, 0.f);
			mRight = glm::vec3(-1.f, 0.f, 0.f);
			Generate(mOrigin, mNormal, h_step, v_step);
		}
		break;
		default:
			break;
		}
	}

	glm::vec3 Plane::MakeStep(Direction dir, float h_step, float v_step)
	{
		switch (dir)
		{
		case util::Plane::OX_POS:
			return glm::vec3(0.f, v_step, -h_step);
		case util::Plane::OX_NEG:
			return glm::vec3(0.f, v_step, h_step);
		case util::Plane::OY_POS:
			return glm::vec3(h_step, 0.f, -v_step);
		case util::Plane::OY_NEG:
			return glm::vec3(h_step, 0.f, v_step);
		case util::Plane::OZ_POS:
			return glm::vec3(h_step, v_step, 0.f);
		case util::Plane::OZ_NEG:
			return glm::vec3(-h_step, v_step, 0.f);
		default:
			return glm::vec3(0.f, 0.f, 0.f);
		}
	}

	void Plane::Generate(glm::vec3 p0, glm::vec3 norm, float h_step, float v_step)
	{
		float tex_u_step = 1.f / mHSD;
		float tex_v_step = 1.f / mVSD;

		// generate vertices
		for (int ver = 0; ver < mVSD + 1; ver++)
		{
			for (int hor = 0; hor < mHSD + 1; hor++)
			{
				Vertex v;
				v.pos = p0 + MakeStep(mDir, h_step * hor, v_step * ver);
				// v.normal = norm;
				v.uv = glm::vec2(tex_u_step * hor, tex_v_step * ver);

				mVertices.push_back(v);
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

				mIndices.push_back(idx0);
				mIndices.push_back(idx1);
				mIndices.push_back(idx2);
				mIndices.push_back(idx3);
				mIndices.push_back(idx4);
				mIndices.push_back(idx5);
			}
		}
	}

	void Plane::Reset()
	{
		mVertices.clear();
		mIndices.clear();

		mWidth = 0.f;
		mHeight = 0.f;
		mHSD = 0.f;
		mVSD = 0.f;
		mDir = Direction::OZ_POS;
	}
}