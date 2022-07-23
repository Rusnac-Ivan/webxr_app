#ifndef _GL_PIPELINE_H_
#define _GL_PIPELINE_H_

#include "GLUtilities.h"

namespace gl
{
	enum struct CompareFunc
	{
		ALWAYS = GL_ALWAYS,
		NEVER = GL_NEVER,
		LESS = GL_LESS,
		EQUAL = GL_EQUAL,
		LEQUAL = GL_LEQUAL,
		GREATER = GL_GREATER,
		NOTEQUAL = GL_NOTEQUAL,
		GEQUAL = GL_GEQUAL
	};

	enum struct Action
	{
		KEEP = GL_KEEP,
		ZERO = GL_ZERO,
		REPLACE = GL_REPLACE,
		INCR = GL_INCR,
		INCR_WRAP = GL_INCR_WRAP,
		DECR = GL_DECR,
		DECR_WRAP = GL_DECR_WRAP,
		INVERT = GL_INVERT
	};

	enum struct ComputOption
	{
		ZERO = GL_ZERO,
		ONE = GL_ONE,
		SRC_COLOR = GL_SRC_COLOR,
		ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
		DST_COLOR = GL_DST_COLOR,
		ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
		SRC_ALPHA = GL_SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
		DST_ALPHA = GL_DST_ALPHA,
		ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR = GL_CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA
	};

	enum struct CombineMode
	{
		FUNC_ADD = GL_FUNC_ADD,
		FUNC_SUBTRACT = GL_FUNC_SUBTRACT,
		FUNC_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
		MIN = GL_MIN,
		MAX = GL_MAX
	};

	enum struct Face
	{
		BACK = GL_BACK,
		FRONT = GL_FRONT,
		FRONT_AND_BACK = GL_FRONT_AND_BACK
	};

	enum struct Orientation
	{
		COUNTERCLOCKWISE = GL_CCW,
		CLOCKWISE = GL_CW
	};

	class Pipeline
	{
	private:
		/*//state of Depth buffer
		static GLboolean	mIsActiveDepthTest;
		static GLboolean	mIsActiveDepthMask;
		static CompareFunc	mDepthCompFun;

		//state of Stencil buffer
		static GLboolean	mIsActiveStencilTest;
		static GLuint		mStencilMask;
		static CompareFunc	mStencilCompFun;
		static GLint		mRefValue;
		static GLuint		m;*/

		Pipeline() = delete;
		~Pipeline() = delete;
		Pipeline(Pipeline &) = delete;

	public:
		// work with Depth buffer
		static void EnableDepthTest();
		static void DisableDepthTest();
		static void SetDepthMask(const bool &flag);
		static void SetDepthFunc(const CompareFunc &depthFun);

		// work with Stencil buffer
		static void EnableStencilTest();
		static void DisableStencilTest();
		static void SetStencilMask(const int &mask);
		static void SetStencilFunc(const CompareFunc &stencilFun, const int &refVal, const int &mask);
		static void SetStencilOperations(const Action &stencilFail, const Action &depthFail, const Action &depthPass);

		// work with Blending
		static void EnableBlending();
		static void DisableBlending();
		static void SetBlendFunc(const ComputOption &sourceFactor, const ComputOption &destinationFactor);
		static void SetBlendFuncSeparate(const ComputOption &sourceFactorRGB, const ComputOption &destinationFactorRGB, const ComputOption &sourceFactorAlpha, const ComputOption &destinationFactorAlpha);
		static void SetBlendEquation(const CombineMode &mode);

		// work with CullFace
		static void EnableCullFace();
		static void DisableCullFace();
		static void SetCullFace(const Face &faceForCulling);
		static void SetFrontFace(const Orientation &orientOfFront);

		static void EnableSeamlessCubeMaps();
		static void DisableSeamlessCubeMaps();

		static void EnableClipDistance(size_t count);
		static void DisableClipDistance(size_t count);

		static void EnableMultisampling();
		static void DisableMultisampling();
	};

	inline void Pipeline::EnableDepthTest()
	{
		GL(Enable(GL_DEPTH_TEST));
	}
	inline void Pipeline::DisableDepthTest()
	{
		GL(Disable(GL_DEPTH_TEST));
	}
	inline void Pipeline::SetDepthMask(const bool &flag)
	{
		GL(DepthMask(flag));
	}
	inline void Pipeline::SetDepthFunc(const CompareFunc &depthFun)
	{
		GL(DepthFunc(static_cast<GLenum>(depthFun)));
	}

	inline void Pipeline::EnableStencilTest()
	{
		GL(Enable(GL_STENCIL_TEST));
	}
	inline void Pipeline::DisableStencilTest()
	{
		GL(Disable(GL_STENCIL_TEST));
	}
	inline void Pipeline::SetStencilMask(const int &mask)
	{
		GL(StencilMask(mask));
	}
	inline void Pipeline::SetStencilFunc(const CompareFunc &stencilFun, const int &refVal, const int &mask)
	{
		GL(StencilFunc(static_cast<GLenum>(stencilFun), refVal, mask));
	}
	inline void Pipeline::SetStencilOperations(const Action &stencilFail, const Action &depthFail, const Action &depthPass)
	{
		GL(StencilOp(static_cast<GLenum>(stencilFail), static_cast<GLenum>(depthFail), static_cast<GLenum>(depthPass)));
	}

	inline void Pipeline::EnableBlending()
	{
		GL(Enable(GL_BLEND));
	}
	inline void Pipeline::DisableBlending()
	{
		GL(Disable(GL_BLEND));
	}
	inline void Pipeline::SetBlendFunc(const ComputOption &sourceFactor, const ComputOption &destinationFactor)
	{
		GL(BlendFunc(static_cast<GLenum>(sourceFactor), static_cast<GLenum>(destinationFactor)));
	}
	inline void Pipeline::SetBlendFuncSeparate(const ComputOption &sourceFactorRGB, const ComputOption &destinationFactorRGB, const ComputOption &sourceFactorAlpha, const ComputOption &destinationFactorAlpha)
	{
		GL(BlendFuncSeparate(static_cast<GLenum>(sourceFactorRGB), static_cast<GLenum>(destinationFactorRGB), static_cast<GLenum>(sourceFactorAlpha), static_cast<GLenum>(destinationFactorAlpha)));
	}
	inline void Pipeline::SetBlendEquation(const CombineMode &mode)
	{
		GL(BlendEquation(static_cast<GLenum>(mode)));
	}

	inline void Pipeline::EnableCullFace()
	{
		GL(Enable(GL_CULL_FACE));
	}
	inline void Pipeline::DisableCullFace()
	{
		GL(Disable(GL_CULL_FACE));
	}
	inline void Pipeline::SetCullFace(const Face &faceForCulling)
	{
		GL(CullFace(static_cast<GLenum>(faceForCulling)));
	}
	inline void Pipeline::SetFrontFace(const Orientation &orientOfFront)
	{
		GL(FrontFace(static_cast<GLenum>(orientOfFront)));
	}

	inline void Pipeline::EnableSeamlessCubeMaps()
	{
		GL(Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	}
	inline void Pipeline::DisableSeamlessCubeMaps()
	{
		GL(Disable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	}

	inline void Pipeline::EnableClipDistance(size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			GL(Enable(GL_CLIP_DISTANCE0 + i));
		}
	}
	inline void Pipeline::DisableClipDistance(size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			GL(Disable(GL_CLIP_DISTANCE0 + i));
		}
	}

	inline void Pipeline::EnableMultisampling()
	{
		GL(Enable(GL_MULTISAMPLE));
	}
	inline void Pipeline::DisableMultisampling()
	{
		GL(Disable(GL_MULTISAMPLE));
	}
}

#endif