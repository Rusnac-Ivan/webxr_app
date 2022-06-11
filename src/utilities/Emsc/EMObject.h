#ifndef _EM_OBJECT_H_
#define _EM_OBJECT_H_

#include <core/Platform.h>

namespace util
{
	class EMObject
	{
	private:
		static emscripten::val mDocument;
		emscripten::val mJSObject;

	public:
		EMObject() : mJSObject(emscripten::val::undefined()) {}
		EMObject(emscripten::val val) : mJSObject(val) {}

		EMObject(const EMObject &other);
		EMObject &operator=(const EMObject &other);
		EMObject(EMObject &&other);
		EMObject &operator=(EMObject &&other);
		~EMObject();

		emscripten::val &GetJSObject() { return mJSObject; }

		static EMObject Global(const std::string &name);

		void Create(const std::string &name);
		void Destroy();

		EMObject operator[](const char *key);

		template <typename... Args>
		inline EMObject Call(const char *func_name, Args &&...args);

		template <typename... Args>
		inline EMObject CallAwait(const char *func_name, Args &&...args);

		template <typename RetValType>
		inline RetValType GetAs();

		void ConsoleLog();

		inline bool IsValid() { return !mJSObject.isNull() || !mJSObject.isUndefined(); }
		inline bool IsNull() { return mJSObject.isNull(); }
		inline bool IsUndefined() { return mJSObject.isUndefined(); }
		inline void SetNull() { mJSObject = emscripten::val::null(); }
		inline void SetUndefined() { mJSObject = emscripten::val::undefined(); }
		inline bool HasProperty(char *name) { return mJSObject.hasOwnProperty(name); }
	};

	template <typename... Args>
	inline EMObject EMObject::Call(const char *func_name, Args &&...args)
	{
		EMObject res;
		res.mJSObject = this->mJSObject.call<emscripten::val>(func_name, args...);
		return res;
	}

	template <typename... Args>
	inline EMObject EMObject::CallAwait(const char *func_name, Args &&...args)
	{
		EMObject res;
		res.mJSObject = this->mJSObject.call<emscripten::val>(func_name, args...).await();

		return res;
	}

	template <typename RetValType>
	inline RetValType EMObject::GetAs()
	{
		return this->mJSObject.as<RetValType>();
	}

}

#endif