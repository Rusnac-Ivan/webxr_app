#ifndef _UTIL_SHADERS_H_
#define _UTIL_SHADERS_H_

#include <opengl/Shader.h>
#include <opengl/Program.h>
#include <memory>
#include <core/Platform.h>

namespace rsrc
{
    class Shaders
    {
        enum TheShader
        {
            PBR_VERT_COMPUTE_SH,
            PBR_FRAG_COMPUTE_SH,
            CUBE_MAP_VERT_SH,
            CUBE_MAP_FRAG_SH,
            RAY_VERT_SH,
            RAY_FRAG_SH,
            MENU_VERT_SH,
            MENU_FRAG_SH,
            SHADER_COUNT
        };
        enum TheProgram
        {
            PBR_PROG,
            CUBE_MAP_PROG,
            RAY_PROG,
            MENU_PROG,
            PROGRAM_COUNT
        };

        const int mProgramShadersMap[PROGRAM_COUNT][2] =
            {
                {PBR_VERT_COMPUTE_SH, PBR_FRAG_COMPUTE_SH},
                {CUBE_MAP_VERT_SH, CUBE_MAP_FRAG_SH},
                {RAY_VERT_SH, RAY_FRAG_SH},
                {MENU_VERT_SH, MENU_FRAG_SH},
            };

        const char *mShaderFileNames[SHADER_COUNT] = {"pbr_compute_vert.vs", "pbr_compute_frag.fs", "cube_map.vs", "cube_map.fs", "ray.vs", "ray.fs", "menu.vs", "menu.fs"};

        struct Program
        {
            Shaders *shaders;
            gl::Shader<gl::IShader::Type::VERTEX> vertSh;
            gl::Shader<gl::IShader::Type::FRAGMENT> fragSh;
            float vertShProgress;
            float fragShProgress;
            bool vertShLoaded;
            bool fragShLoaded;
            bool isReady;
            gl::Program program;
            Program(Shaders *sh) : shaders(sh), vertShProgress(0.f), fragShProgress(0.f), vertShLoaded(false), fragShLoaded(false), isReady(false) {}
            void LoadVertSh(const char *shaderCodes, const int length);
            void LoadFragSh(const char *shaderCodes, const int length);
            void CreateProg();

            gl::Program *GetProgram() { return &program; }
        };

        Program mPrograms[PROGRAM_COUNT];

        static bool mIsReady;

    public:
        Shaders(Shaders &) = delete;
        Shaders &operator=(Shaders &) = delete;
        Shaders(Shaders &&) = delete;
        Shaders &operator=(Shaders &&) = delete;
        Shaders();
        ~Shaders();

        float GetProgress();

        bool IsReady() { return mIsReady; }

        gl::Program *GetPBRProg() { return mPrograms[PBR_PROG].GetProgram(); }
        gl::Program *GetCubeMapProg() { return mPrograms[CUBE_MAP_PROG].GetProgram(); }
        gl::Program *GetRayProg() { return mPrograms[RAY_PROG].GetProgram(); }
        gl::Program* GetMenuProg() { return mPrograms[MENU_PROG].GetProgram(); }

        void Load(const char *path);

    private:
        void CheckIsReady();
    };
} // namespace util

#endif