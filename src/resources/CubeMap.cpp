#include "CubeMap.h"
#include <stb_image.h>
#include "Shaders.h"
#include <opengl/Pipeline.h>
#include <opengl/Render.h>
#include <opengl/VertexAttribute.h>
#include <fstream>

namespace rsrc
{
    static float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    constexpr uint32_t CubeMap::TEXT_COUNT;

    CubeMap::CubeMap(/* args */) : mIsReady(false), mIsLoaded(false)
    {
    }

    CubeMap::~CubeMap()
    {
    }

    float CubeMap::GetProgress()
    {
        float progress = 0;
        for (uint32_t i = 0; i < TEXT_COUNT; i++)
        {
            progress += mProgress[i];
        }
        return progress / TEXT_COUNT;
    }

    void CubeMap::Loaded(int i)
    {
        mIsLoadedText[i] = true;
        mIsLoaded = true;
        for (uint32_t i = 0; i < TEXT_COUNT; i++)
        {
            mIsLoaded = mIsLoadedText[i] && mIsLoaded;
        }

        if (mIsLoaded)
        {
            mVBO.SetData(sizeof(skyboxVertices), skyboxVertices);
            mVAO.AddVertexLayout(mVBO, {gl::VertexAttribute::Entry<glm::vec3>()});

            mGLCubeMap.SetWrapModeS(gl::Texture::WrapMode::CLAMP_TO_EDGE);
            mGLCubeMap.SetWrapModeT(gl::Texture::WrapMode::CLAMP_TO_EDGE);

            mGLCubeMap.GenerateMipmaps();

            mIsReady = true;
        }
    }

    void CubeMap::Draw(gl::Program *prog, const glm::mat4 &view, const glm::mat4 &proj)
    {
        prog->Use();
        if (mIsReady && prog)
        {
            gl::Pipeline::SetDepthFunc(gl::CompareFunc::LEQUAL);
            mGLCubeMap.Activate(0);
            mVAO.Bind();
            gl::Render::DrawVertices(gl::Primitive::TRIANGLES, 36, 0);
            gl::Pipeline::SetDepthFunc(gl::CompareFunc::LESS);
        }
    }

    void CubeMap::Load(const char *path)
    {
        std::string yokohama[TEXT_COUNT] = {
            path + std::string("posx.jpg"),
            path + std::string("negx.jpg"),

            path + std::string("posy.jpg"),
            path + std::string("negy.jpg"),

            path + std::string("posz.jpg"),
            path + std::string("negz.jpg")};
#ifdef __EMSCRIPTEN__
        emscripten_async_wget2_data(
            yokohama[0].c_str(), "GET", NULL, this, true,
            [](unsigned handle, void *arg, void *data, unsigned size)
            {
                int width, height, nrChannels;
                unsigned char *img_data = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &nrChannels, 0);
                printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
                if (img_data)
                {
                    CubeMap *thiz = (CubeMap *)arg;
                    thiz->mGLCubeMap.SetPositiveX(0, gl::Texture::Format::RGB, width, height, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, img_data);
                    stbi_image_free(img_data);
                    thiz->Loaded(0);
                }
                assert(img_data && "stb not load texture");
            },
            [](unsigned handle, void *arg, int error_code, const char *status)
            {
                fprintf(stderr, "Failed to load cube_map texture error_code: %d, status: %s\n", error_code, status);
            },
            [](unsigned handle, void *arg, int bytes_loaded, int total_size)
            {
                CubeMap *cubeMap = (CubeMap *)arg;
                if (total_size)
                    cubeMap->mProgress[0] = ((float)bytes_loaded / total_size) * 100.f;
            });

        emscripten_async_wget2_data(
            yokohama[1].c_str(), "GET", NULL, this, true,
            [](unsigned handle, void *arg, void *data, unsigned size)
            {
                int width, height, nrChannels;
                unsigned char *img_data = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &nrChannels, 0);
                printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
                if (img_data)
                {
                    CubeMap *thiz = (CubeMap *)arg;
                    thiz->mGLCubeMap.SetNegativeX(0, gl::Texture::Format::RGB, width, height, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, img_data);
                    stbi_image_free(img_data);
                    thiz->Loaded(1);
                }
                assert(img_data && "stb not load texture");
            },
            [](unsigned handle, void *arg, int error_code, const char *status)
            {
                fprintf(stderr, "Failed to load cube_map texture error_code: %d, status: %s\n", error_code, status);
            },
            [](unsigned handle, void *arg, int bytes_loaded, int total_size)
            {
                CubeMap *cubeMap = (CubeMap *)arg;
                if (total_size)
                    cubeMap->mProgress[1] = ((float)bytes_loaded / total_size) * 100.f;
            });
        emscripten_async_wget2_data(
            yokohama[2].c_str(), "GET", NULL, this, true,
            [](unsigned handle, void *arg, void *data, unsigned size)
            {
                int width, height, nrChannels;
                unsigned char *img_data = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &nrChannels, 0);
                printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
                if (img_data)
                {
                    CubeMap *thiz = (CubeMap *)arg;
                    thiz->mGLCubeMap.SetPositiveY(0, gl::Texture::Format::RGB, width, height, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, img_data);
                    stbi_image_free(img_data);
                    thiz->Loaded(2);
                }
                assert(img_data && "stb not load texture");
            },
            [](unsigned handle, void *arg, int error_code, const char *status)
            {
                fprintf(stderr, "Failed to load cube_map texture error_code: %d, status: %s\n", error_code, status);
            },
            [](unsigned handle, void *arg, int bytes_loaded, int total_size)
            {
                CubeMap *cubeMap = (CubeMap *)arg;
                if (total_size)
                    cubeMap->mProgress[2] = ((float)bytes_loaded / total_size) * 100.f;
            });
        emscripten_async_wget2_data(
            yokohama[3].c_str(), "GET", NULL, this, true,
            [](unsigned handle, void *arg, void *data, unsigned size)
            {
                int width, height, nrChannels;
                unsigned char *img_data = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &nrChannels, 0);
                printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
                if (img_data)
                {
                    CubeMap *thiz = (CubeMap *)arg;
                    thiz->mGLCubeMap.SetNegativeY(0, gl::Texture::Format::RGB, width, height, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, img_data);
                    stbi_image_free(img_data);
                    thiz->Loaded(3);
                }
                assert(img_data && "stb not load texture");
            },
            [](unsigned handle, void *arg, int error_code, const char *status)
            {
                fprintf(stderr, "Failed to load cube_map texture error_code: %d, status: %s\n", error_code, status);
            },
            [](unsigned handle, void *arg, int bytes_loaded, int total_size)
            {
                CubeMap *cubeMap = (CubeMap *)arg;
                if (total_size)
                    cubeMap->mProgress[3] = ((float)bytes_loaded / total_size) * 100.f;
            });
        emscripten_async_wget2_data(
            yokohama[4].c_str(), "GET", NULL, this, true,
            [](unsigned handle, void *arg, void *data, unsigned size)
            {
                int width, height, nrChannels;
                unsigned char *img_data = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &nrChannels, 0);
                printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
                if (img_data)
                {
                    CubeMap *thiz = (CubeMap *)arg;
                    thiz->mGLCubeMap.SetPositiveZ(0, gl::Texture::Format::RGB, width, height, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, img_data);
                    stbi_image_free(img_data);
                    thiz->Loaded(4);
                }
                assert(img_data && "stb not load texture");
            },
            [](unsigned handle, void *arg, int error_code, const char *status)
            {
                fprintf(stderr, "Failed to load cube_map texture error_code: %d, status: %s\n", error_code, status);
            },
            [](unsigned handle, void *arg, int bytes_loaded, int total_size)
            {
                CubeMap *cubeMap = (CubeMap *)arg;
                if (total_size)
                    cubeMap->mProgress[4] = ((float)bytes_loaded / total_size) * 100.f;
            });
        emscripten_async_wget2_data(
            yokohama[5].c_str(), "GET", NULL, this, true,
            [](unsigned handle, void *arg, void *data, unsigned size)
            {
                int width, height, nrChannels;
                unsigned char *img_data = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &nrChannels, 0);
                printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
                if (img_data)
                {
                    CubeMap *thiz = (CubeMap *)arg;
                    thiz->mGLCubeMap.SetNegativeZ(0, gl::Texture::Format::RGB, width, height, 0, gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE, img_data);
                    stbi_image_free(img_data);
                    thiz->Loaded(5);
                }
                assert(img_data && "stb not load texture");
            },
            [](unsigned handle, void *arg, int error_code, const char *status)
            {
                fprintf(stderr, "Failed to load cube_map texture error_code: %d, status: %s\n", error_code, status);
            },
            [](unsigned handle, void *arg, int bytes_loaded, int total_size)
            {
                CubeMap *cubeMap = (CubeMap *)arg;
                if (total_size)
                    cubeMap->mProgress[5] = ((float)bytes_loaded / total_size) * 100.f;
            });
#else
        for (uint32_t i = 0; i < TEXT_COUNT; i++)
        {
            int width, height, nrChannels;
            unsigned char *img_data = stbi_load(yokohama[i].c_str(), &width, &height, &nrChannels, 0);
            printf("load image w: %d, h: %d, c: %d\n", width, height, nrChannels);
            if (img_data)
            {
                mGLCubeMap.SetTarget(
                    static_cast<gl::CubeMap::Target>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0,
                    gl::Texture::Format::RGB, width, height, 0,
                    gl::Texture::Format::RGB, gl::DataType::UNSIGNED_BYTE,
                    img_data);
                stbi_image_free(img_data);
                Loaded(i);
                mProgress[i] = 100.f;
            }
            else
                assert(img_data && "stb not load texture");
        }

#endif
    }
}