#ifndef _GL_SHADER_H_
#define _GL_SHADER_H_

#include "GLUtilities.h"

namespace gl
{
    class IShader
    {
    public:
        enum struct Type
        {
            VERTEX = GL_VERTEX_SHADER,
            FRAGMENT = GL_FRAGMENT_SHADER,
            GEOMETRY = GL_GEOMETRY_SHADER,
            TESS_CONTROL = GL_TESS_CONTROL_SHADER,
            TESS_EVALUATION = GL_TESS_EVALUATION_SHADER
        };

        IShader() {}
        ~IShader() {}

        virtual bool LoadSources(const unsigned short &count, const char **shaderCodes, const int *codeLengths) = 0;
        virtual Type GetType() = 0;
        virtual unsigned int GetID() = 0;
    };

    template <IShader::Type Shader_Type>
    class Shader : public IShader
    {
    private:
        unsigned int mID;

    public:
        Shader();
        ~Shader();

        bool LoadSources(const unsigned short& count, const char** shaderCodes, const int* codeLengths);

        Type GetType() { return Shader_Type; }
        unsigned int GetID() { return mID; }

    private:
        bool CheckCompile();
    };

}

#endif
