#include "Shaders.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

namespace rsrc
{
    bool Shaders::mIsReady = false;

    Shaders::Shaders() : mPrograms{this, this, this} {}
    Shaders::~Shaders() {}

    void Shaders::Program::LoadVertSh(const char *shaderCodes, const int length)
    {
        char *shaders_code[2] = {(char *)GLSL_VERSION, (char *)shaderCodes};
        int sizes[2] = {};
        sizes[0] = strlen(GLSL_VERSION);
        sizes[1] = length;

        if (vertSh.LoadSources(2, (const char **)shaders_code, sizes))
            vertShLoaded = true;
        CreateProg();
    }
    void Shaders::Program::LoadFragSh(const char *shaderCodes, const int length)
    {
        char *shaders_code[2] = {(char *)GLSL_VERSION, (char *)shaderCodes};
        int sizes[2] = {};
        sizes[0] = strlen(GLSL_VERSION);
        sizes[1] = length;

        if (fragSh.LoadSources(2, (const char **)shaders_code, sizes))
            fragShLoaded = true;
        CreateProg();
    }

    void Shaders::Program::CreateProg()
    {
        if (vertShLoaded && fragShLoaded)
        {
            program.Attach(&vertSh, &fragSh, NULL);
            if (program.Link())
            {
                isReady = true;
                shaders->CheckIsReady();
            }
        }
    }

    void Shaders::CheckIsReady()
    {
        mIsReady = true;
        for (uint32_t i = 0; i < PROGRAM_COUNT; i++)
        {
            mIsReady = mIsReady && mPrograms[i].isReady;
        }
    }

    float Shaders::GetProgress()
    {
        float progress = 0.f;
        for (uint32_t i = 0; i < PROGRAM_COUNT; i++)
        {
            progress += mPrograms[i].vertShProgress;
            progress += mPrograms[i].fragShProgress;
        }
        return progress / SHADER_COUNT;
    }

#ifndef __EMSCRIPTEN__
    static void LoadShader(const char *file, std::string &sh_data)
    {
        std::ifstream in;

        in.open(file, std::ios::in);

        std::stringstream shader_str_stream;
        shader_str_stream << in.rdbuf();

        sh_data = shader_str_stream.str();
    }
#endif

    void Shaders::Load(const char *path)
    {
        for (uint32_t i = 0; i < PROGRAM_COUNT; i++)
        {
            std::string vert_sh_path = std::string(path) + mShaderFileNames[mProgramShadersMap[i][0]];
            std::string frag_sh_path = std::string(path) + mShaderFileNames[mProgramShadersMap[i][1]];
#ifdef __EMSCRIPTEN__
            emscripten_async_wget2_data(
                vert_sh_path.c_str(), "GET", NULL, mPrograms + i, true,
                [](unsigned handle, void *arg, void *data, unsigned size)
                {
                    Program *program = (Program *)arg;
                    program->LoadVertSh((const char *)data, size);
                },
                [](unsigned handle, void *arg, int error_code, const char *status) {

                },
                [](unsigned handle, void *arg, int bytes_loaded, int total_size)
                {
                    Program *program = (Program *)arg;
                    if (total_size)
                        program->vertShProgress = ((float)bytes_loaded / total_size) * 100.f;
                });
            emscripten_async_wget2_data(
                frag_sh_path.c_str(), "GET", NULL, mPrograms + i, true,
                [](unsigned handle, void *arg, void *data, unsigned size)
                {
                    Program *program = (Program *)arg;
                    program->LoadFragSh((const char *)data, size);
                },
                [](unsigned handle, void *arg, int error_code, const char *status) {

                },
                [](unsigned handle, void *arg, int bytes_loaded, int total_size)
                {
                    Program *program = (Program *)arg;
                    if (total_size)
                        program->fragShProgress = ((float)bytes_loaded / total_size) * 100.f;
                });
#else
            std::string vert_sh_data;
            std::string frag_sh_data;

            LoadShader(vert_sh_path.c_str(), vert_sh_data);
            LoadShader(frag_sh_path.c_str(), frag_sh_data);

            mPrograms[i].LoadVertSh(vert_sh_data.data(), vert_sh_data.size());
            mPrograms[i].LoadFragSh(frag_sh_data.data(), frag_sh_data.size());

            CheckIsReady();

            mPrograms[i].vertShProgress = 100.f;
            mPrograms[i].fragShProgress = 100.f;

            CheckIsReady();

            // mPrograms[i].CreateProg();
#endif
        }
    }
}