#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#define GLSL_VERSION "#version 150\n"
#elif __EMSCRIPTEN__
#define GLSL_VERSION "#version 300 es\n"
#else
#define GLSL_VERSION "#version 400\n"
#endif

#endif