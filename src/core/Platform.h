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

#endif