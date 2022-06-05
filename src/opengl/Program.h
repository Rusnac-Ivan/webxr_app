#ifndef _GL_PROGRAM_H_
#define _GL_PROGRAM_H_

#include "GLUtilities.h"
#include <glm/glm.hpp>

namespace gl
{
    class IShader;

    class Program
    {
        unsigned short mID;

    public:
        Program();
        ~Program();

        void Attach(IShader *shader, ...);
        void Detach(IShader* shader, ...);

        void Use() const;
        bool IsInUse() const;
        void StopUsing() const;
        bool Link() const;

        unsigned int Uniform(const char *name) const;
        int GetAttribLocation(const char *name) const;

        void SetBool(const unsigned short &location, const bool &value) const;
        void SetInt(const unsigned short &location, const int &value) const;
        void SetFloat(const unsigned short &location, const float &value) const;
        void SetFloat2(const unsigned short &location, const float &x, const float &y) const;
        void SetFloat2(const unsigned short &location, const glm::vec2 &value) const;
        void SetFloat3(const unsigned short &location, const float &x, const float &y, const float &z) const;
        void SetFloat3(const unsigned short &location, const glm::vec3 &value) const;
        void SetFloat4(const unsigned short &location, const float &x, const float &y, const float &z, const float &w) const;
        void SetFloat4(const unsigned short &location, const glm::vec4 &value) const;
        void SetMatrix2(const unsigned short &location, const glm::mat2 &mat) const;
        void SetMatrix3(const unsigned short &location, const glm::mat3 &mat) const;
        void SetMatrix4(const unsigned short &location, const glm::mat4 &mat) const;

    private:
        bool CheckLinkProgramErrors() const;
    };
}

#endif
