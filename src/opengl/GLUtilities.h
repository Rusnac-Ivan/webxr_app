#ifndef _GL_UTILITIES_
#define _GL_UTILITIES_

#include <core/Platform.h>

#ifdef NDEBUG
#define GL(line) gl##line
#else

#define GL(line)                                                \
	gl##line;                                                   \
	do                                                          \
	{                                                           \
		GLenum error = glGetError();                            \
		switch (error)                                          \
		{                                                       \
		case GL_INVALID_ENUM:                                   \
			assert("GL_INVALID_ENUM" && NULL);                  \
			break;                                              \
		case GL_INVALID_VALUE:                                  \
			assert("GL_INVALID_VALUE" && NULL);                 \
			break;                                              \
		case GL_INVALID_OPERATION:                              \
			assert("GL_INVALID_OPERATION" && NULL);             \
			break;                                              \
		case GL_STACK_OVERFLOW:                                 \
			assert("GL_STACK_OVERFLOW" && NULL);                \
			break;                                              \
		case GL_STACK_UNDERFLOW:                                \
			assert("GL_STACK_UNDERFLOW" && NULL);               \
			break;                                              \
		case GL_OUT_OF_MEMORY:                                  \
			assert("GL_OUT_OF_MEMORY" && NULL);                 \
			break;                                              \
		case GL_INVALID_FRAMEBUFFER_OPERATION:                  \
			assert("GL_INVALID_FRAMEBUFFER_OPERATION" && NULL); \
			break;                                              \
		default:                                                \
			break;                                              \
		}                                                       \
	} while (0);

#endif

#ifdef __EMSCRIPTEN__
#define GLSL(str) (const char *)"#version 300 es\n" #str
#define GL_ES
#else
#define GLSL(str) (const char *)"#version 330 core\n" #str
#endif

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace gl
{
	enum struct DataType
	{
		BYTE = GL_BYTE,
		UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		UNSIGNED_INT = GL_UNSIGNED_INT,
		HALF_FLOAT = GL_HALF_FLOAT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE
	};

	extern inline int DataSize(const DataType &dataType)
	{
		switch (dataType)
		{
		case DataType::BYTE:
			return sizeof(GLbyte);
		case DataType::UNSIGNED_BYTE:
			return sizeof(GLubyte);
		case DataType::SHORT:
			return sizeof(GLshort);
		case DataType::UNSIGNED_SHORT:
			return sizeof(GLushort);
		case DataType::INT:
			return sizeof(GLint);
		case DataType::UNSIGNED_INT:
			return sizeof(GLuint);
		case DataType::FLOAT:
			return sizeof(GLfloat);
		case DataType::DOUBLE:
			return sizeof(GLdouble);
		default:
		{
			assert(false && "Invalid Data Type!");
			return -1;
		}
		}
	}

}

#endif