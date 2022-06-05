#include "Shader.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

namespace gl
{
	template <IShader::Type Shader_Type>
	Shader<Shader_Type>::Shader() : IShader(), mID(0)
	{
		mID = GL(CreateShader(static_cast<GLenum>(Shader_Type)));
		assert(mID != 0 && "Failed to create shader!");
	}

	template <IShader::Type Shader_Type>
	Shader<Shader_Type>::~Shader()
	{
		if (!mID)
		{
			GL(DeleteShader(mID));
		}
	}

	template <IShader::Type Shader_Type>
	bool Shader<Shader_Type>::LoadSources(const unsigned short &count, const char **shaderCodes, const int *codeLengths)
	{
		GL(ShaderSource(mID, count, shaderCodes, codeLengths));

		GL(CompileShader(mID));

		return CheckCompile();
	}

	template <IShader::Type Shader_Type>
	bool Shader<Shader_Type>::CheckCompile()
	{
		GLint is_success;
		char infoLog[1024];

		GL(GetShaderiv(mID, GL_COMPILE_STATUS, &is_success));
		if (!is_success)
		{
			GL(GetShaderInfoLog(mID, 1024, nullptr, infoLog));
			if (Shader_Type == IShader::Type::FRAGMENT)
				fprintf(stderr, "Failed to compile fragment shader: %s\n", infoLog);

			else if (Shader_Type == IShader::Type::VERTEX)
				fprintf(stderr, "Failed to compile vertex shader: %s\n", infoLog);

			GL(DeleteShader(mID));
			mID = 0;

			return false;
		}
		return true;
	}

	template class Shader<IShader::Type::VERTEX>;
	template class Shader<IShader::Type::FRAGMENT>;
	template class Shader<IShader::Type::GEOMETRY>;
	template class Shader<IShader::Type::TESS_CONTROL>;
	template class Shader<IShader::Type::TESS_EVALUATION>;
}