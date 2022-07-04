#ifndef _UTIL_PRISM_H_
#define _UTIL_PRISM_H_

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Shape.h"

namespace util
{
	// struct Vertex;
	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	class Prism : public Shape<Options>
	{
		float mEdgeWidth;

	public:
		Prism();
		~Prism();

		void Generate(float edge_width);

	private:
		virtual void Generate();
	};

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	Prism<Options>::Prism()
	{
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	Prism<Options>::~Prism()
	{
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	void Prism<Options>::Generate(float edge_width)
	{
		mEdgeWidth = edge_width;
		Generate();

		Shape<Options>::MakeGLVAO();
	}

	template <Attrib::underlying_type Options> // Options = ATTRIB_POS | ATTRIB_NORM | ...
	void Prism<Options>::Generate()
	{
		float a = mEdgeWidth;

		glm::vec3 P1 = glm::vec3(-a / 2.f, 0.f, -a / (2.f * sqrt(3.f)));
		glm::vec3 P2 = glm::vec3(0.f, 0.f, a / sqrt(3.f));
		glm::vec3 P3 = glm::vec3(a / 2.f, 0.f, -a / (2.f * sqrt(3.f)));

		glm::vec3 d = glm::vec3(0.f, 1.f, 0.f);

		glm::vec3 P1_, P2_, P3_;

		P1_ = P1 + d;
		P2_ = P2 + d;
		P3_ = P3 + d;

		P2 = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(P2, 1.f));
		P3 = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(P3, 1.f));
		P1 = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(P1, 1.f));

		P1_ = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(P1_, 1.f));
		P2_ = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(P2_, 1.f));
		P3_ = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(P3_, 1.f));

		Vertex<Options> v1, v2, v3, v1_, v2_, v3_;

		// triangle I
		glm::vec3 norm_1 = glm::normalize(glm::cross(P2_ - P1, P2 - P1));

		if constexpr (Options & Attrib::POS)
			v1.pos = P1;
		if constexpr (Options & Attrib::NORM)
			v1.norm = norm_1;
		if constexpr (Options & Attrib::UV)
			v1.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v2.pos = P2;
		if constexpr (Options & Attrib::NORM)
			v2.norm = norm_1;
		if constexpr (Options & Attrib::UV)
			v2.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v2_.pos = P2_;
		if constexpr (Options & Attrib::NORM)
			v2_.norm = norm_1;
		if constexpr (Options & Attrib::UV)
			v2_.uv = glm::vec2(0.f);

		Shape<Options>::mVertices.push_back(v1);
		Shape<Options>::mVertices.push_back(v2);
		Shape<Options>::mVertices.push_back(v2_);

		// triangle II
		glm::vec3 norm_2 = glm::normalize(glm::cross(P1_ - P1, P2_ - P1));

		if constexpr (Options & Attrib::POS)
			v1.pos = P1;
		if constexpr (Options & Attrib::NORM)
			v1.norm = norm_2;
		if constexpr (Options & Attrib::UV)
			v1.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v2_.pos = P2_;
		if constexpr (Options & Attrib::NORM)
			v2_.norm = norm_1;
		if constexpr (Options & Attrib::UV)
			v2_.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v1_.pos = P1_;
		if constexpr (Options & Attrib::NORM)
			v1_.norm = norm_1;
		if constexpr (Options & Attrib::UV)
			v1_.uv = glm::vec2(0.f);

		Shape<Options>::mVertices.push_back(v1);
		Shape<Options>::mVertices.push_back(v2_);
		Shape<Options>::mVertices.push_back(v1_);

		// triangle III
		glm::vec3 norm_3 = glm::normalize(glm::cross(P2_ - P2, P3_ - P2));

		if constexpr (Options & Attrib::POS)
			v2.pos = P2;
		if constexpr (Options & Attrib::NORM)
			v2.norm = norm_3;
		if constexpr (Options & Attrib::UV)
			v2.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v3_.pos = P3_;
		if constexpr (Options & Attrib::NORM)
			v3_.norm = norm_3;
		if constexpr (Options & Attrib::UV)
			v3_.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v2_.pos = P2_;
		if constexpr (Options & Attrib::NORM)
			v2_.norm = norm_3;
		if constexpr (Options & Attrib::UV)
			v2_.uv = glm::vec2(0.f);

		Shape<Options>::mVertices.push_back(v2);
		Shape<Options>::mVertices.push_back(v3_);
		Shape<Options>::mVertices.push_back(v2_);

		// triangle IV
		glm::vec3 norm_4 = glm::normalize(glm::cross(P3_ - P2, P3 - P2));

		if constexpr (Options & Attrib::POS)
			v2.pos = P2;
		if constexpr (Options & Attrib::NORM)
			v2.norm = norm_4;
		if constexpr (Options & Attrib::UV)
			v2.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v3.pos = P3;
		if constexpr (Options & Attrib::NORM)
			v3.norm = norm_4;
		if constexpr (Options & Attrib::UV)
			v3.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v3_.pos = P3_;
		if constexpr (Options & Attrib::NORM)
			v3_.norm = norm_4;
		if constexpr (Options & Attrib::UV)
			v3_.uv = glm::vec2(0.f);

		Shape<Options>::mVertices.push_back(v2);
		Shape<Options>::mVertices.push_back(v3);
		Shape<Options>::mVertices.push_back(v3_);

		// triangle V
		glm::vec3 norm_5 = glm::normalize(glm::cross(P3_ - P3, P1_ - P3));

		if constexpr (Options & Attrib::POS)
			v3.pos = P3;
		if constexpr (Options & Attrib::NORM)
			v3.norm = norm_5;
		if constexpr (Options & Attrib::UV)
			v3.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v1_.pos = P1_;
		if constexpr (Options & Attrib::NORM)
			v1_.norm = norm_5;
		if constexpr (Options & Attrib::UV)
			v1_.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v3_.pos = P3_;
		if constexpr (Options & Attrib::NORM)
			v3_.norm = norm_5;
		if constexpr (Options & Attrib::UV)
			v3_.uv = glm::vec2(0.f);

		Shape<Options>::mVertices.push_back(v3);
		Shape<Options>::mVertices.push_back(v1_);
		Shape<Options>::mVertices.push_back(v3_);

		// triangle VI
		glm::vec3 norm_6 = glm::normalize(glm::cross(P1_ - P3, P1 - P3));

		if constexpr (Options & Attrib::POS)
			v3.pos = P3;
		if constexpr (Options & Attrib::NORM)
			v3.norm = norm_6;
		if constexpr (Options & Attrib::UV)
			v3.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v1.pos = P1;
		if constexpr (Options & Attrib::NORM)
			v1.norm = norm_6;
		if constexpr (Options & Attrib::UV)
			v1.uv = glm::vec2(0.f);

		if constexpr (Options & Attrib::POS)
			v1_.pos = P1_;
		if constexpr (Options & Attrib::NORM)
			v1_.norm = norm_6;
		if constexpr (Options & Attrib::UV)
			v1_.uv = glm::vec2(0.f);

		Shape<Options>::mVertices.push_back(v3);
		Shape<Options>::mVertices.push_back(v1);
		Shape<Options>::mVertices.push_back(v1_);
	}
}

#endif