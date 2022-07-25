#include "Program.h"
#include <cstdarg>
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

namespace gl
{
    Program::Program() : mID(0)
    {
        mID = GL(CreateProgram());
        assert(mID != 0 && "Failed to create program!");
    }

    Program::~Program()
    {
        if (mID)
        {
            GL(DeleteProgram(mID));
            mID = 0;
        }
    }

    void Program::Attach(IShader *shader, ...)
    {
        va_list vl;
        va_start(vl, shader);
        do
        {
            GL(AttachShader(mID, shader->GetID()));
        } while ((shader = va_arg(vl, IShader *)) != NULL);
        va_end(vl);
    }

    void Program::Detach(IShader *shader, ...)
    {
        va_list vl;
        va_start(vl, shader);
        do
        {
            GL(DetachShader(mID, shader->GetID()));
        } while ((shader = va_arg(vl, IShader *)) != NULL);
        va_end(vl);
    }

    void Program::Use() const
    {
        GL(UseProgram(mID));
    }

    inline bool Program::IsInUse() const
    {
        int programId;
        GL(GetIntegerv(GL_CURRENT_PROGRAM, &programId));
        return mID == static_cast<uint32_t>(programId);
    }

    void Program::StopUsing()
    {
        GL(UseProgram(0));
    }

    bool Program::Link()
    {
        GL(LinkProgram(mID));
        mIsReady = CheckLinkProgramErrors();
        return mIsReady;
    }

    unsigned int Program::Uniform(const char *name) const
    {
        int32_t location = GL(GetUniformLocation(mID, reinterpret_cast<const GLchar *>(name)));
        assert(location >= 0 && "Invalid uniform name!");
        return location;
    }

    int Program::GetAttribLocation(const char *name) const
    {
        int32_t attribLocation = GL(GetAttribLocation(mID, reinterpret_cast<const GLchar *>(name)));
        assert(attribLocation >= 0 && "Invalid attribute name!");
        return attribLocation;
    }

    void Program::SetBool(const int location, const bool value) const
    {
        GL(Uniform1i(location, (int)value));
    }

    void Program::SetInt(const int location, const int value) const
    {
        GL(Uniform1i(location, value));
    }

    void Program::SetInt2(const int location, const int x, const int y) const
    {
        GL(Uniform2i(location, x, y));
    }
    void Program::SetInt3(const int location, const glm::ivec3 &value) const
    {
        GL(Uniform3iv(location, 1, glm::value_ptr(value)));
    }
    void Program::SetInt4(const int location, const glm::ivec4 &value) const
    {
        GL(Uniform4iv(location, 1, glm::value_ptr(value)));
    }

    void Program::SetFloat(const int location, const float value) const
    {
        GL(Uniform1f(location, value));
    }

    void Program::SetFloat2(const int location, const float x, const float y) const
    {
        GL(Uniform2f(location, x, y));
    }

    void Program::SetFloat2(const int location, const glm::vec2 value) const
    {
        GL(Uniform2fv(location, 1, glm::value_ptr(value)));
    }

    void Program::SetFloat3(const int location, const float x, const float y, const float z) const
    {
        GL(Uniform3f(location, x, y, z));
    }

    void Program::SetFloat3(const int location, const glm::vec3 &value) const
    {
        GL(Uniform3fv(location, 1, glm::value_ptr(value)));
    }

    void Program::SetFloat4(const int location, const float x, const float y, const float z, const float w) const
    {
        GL(Uniform4f(location, x, y, z, w));
    }

    void Program::SetFloat4(const int location, const glm::vec4 &value) const
    {
        GL(Uniform4fv(location, 1, glm::value_ptr(value)));
    }

    void Program::SetMatrix2(const int location, const glm::mat2 &mat) const
    {
        GL(UniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
    }

    void Program::SetMatrix3(const int location, const glm::mat3 &mat) const
    {
        GL(UniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
    }

    void Program::SetMatrix4(const int location, const glm::mat4 &mat) const
    {

        GL(UniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
    }

    bool Program::CheckLinkProgramErrors() const
    {
        GLint success;
        char infoLog[1024];

        GL(GetProgramiv(mID, GL_LINK_STATUS, &success));
        if (!success)
        {
            GL(GetProgramInfoLog(mID, 1024, NULL, infoLog));
            fprintf(stderr, "Failed to compile shader: %s\n", infoLog);
            return false;
        }
        return true;
    }
}