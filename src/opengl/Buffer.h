#ifndef _GL_BUFFER_H_
#define _GL_BUFFER_H_

#include "GLUtilities.h"

namespace gl
{
	class Buffer
	{
		public:
			enum struct Type
			{
				ARRAY				= GL_ARRAY_BUFFER,
				ELEMENT_ARRAY		= GL_ELEMENT_ARRAY_BUFFER,
				SHADER_STORAGE 		= GL_SHADER_STORAGE_BUFFER,
				UNIFORM				= GL_UNIFORM_BUFFER,

				UNKNOWN				= GL_FALSE,
			};
			enum struct UsageMode
			{
				STREAM_DRAW		= GL_STREAM_DRAW,
				STREAM_READ		= GL_STREAM_READ,
				STREAM_COPY		= GL_STREAM_COPY,
				
				STATIC_DRAW		= GL_STATIC_DRAW,
				STATIC_READ		= GL_STATIC_READ,
				STATIC_COPY		= GL_STATIC_COPY,
				
				DYNAMIC_DRAW	= GL_DYNAMIC_DRAW,
				DYNAMIC_READ	= GL_DYNAMIC_READ,
				DYNAMIC_COPY	= GL_DYNAMIC_COPY,
			};
		private:
			using ObjectId = unsigned int;

			ObjectId	mID;
			Type		mCurrentTarget;
			UsageMode	mAccessFreq;
			size_t		mByteSize;
			
		public:
			Buffer();
			~Buffer();

			Buffer(const Buffer&) = delete;
			Buffer(Buffer&&) noexcept;
			Buffer& operator=(const Buffer&) = delete;
			Buffer& operator=(Buffer&&) noexcept;

			ObjectId GetObjectId() { return mID; }
			UsageMode GetUsageMode() const { return mAccessFreq; }

			void UnBind();
			void UnBindBase(size_t index);

		protected:
			Type GetCurrentType() const { return mCurrentTarget; }

			void Bind(Type target);
			void BindBase(Type target, size_t index);

			void SetData(const size_t size, const void* data, UsageMode accessFreq);
			void SetSubData(const size_t offset, const size_t size, const void* data);
			void SetDataWithResize(size_t byteSize, const void* data);
		private:
			void Create();
			void Destroy();

			
	};
}

#endif
