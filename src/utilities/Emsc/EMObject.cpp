#ifdef __EMSCRIPTEN__
#include "EMObject.h"

namespace util
{
	emscripten::val EMObject::mDocument = emscripten::val::global("document");

	void EMObject::Create(const std::string &name)
	{
		mJSObject = mDocument.call<emscripten::val>("createElement", name);
	}

	void EMObject::Destroy()
	{
		mJSObject = emscripten::val::undefined();
	}

	EMObject EMObject::Global(const std::string &name)
	{
		EMObject glob;
		glob.mJSObject = emscripten::val::global(name.c_str());
		return glob;
	}

	EMObject::EMObject(const EMObject &other) : mJSObject(other.mJSObject) {}
	EMObject &EMObject::operator=(const EMObject &other)
	{
		mJSObject = other.mJSObject;
		return *this;
	}

	EMObject::EMObject(EMObject &&other) : mJSObject(std::move(other.mJSObject))
	{
	}
	EMObject &EMObject::operator=(EMObject &&other)
	{
		mJSObject = std::move(other.mJSObject);
		return *this;
	}
	EMObject::~EMObject() {}

	EMObject EMObject::operator[](const char *key)
	{
		EMObject prop;
		prop.mJSObject = mJSObject[key];
		return prop;
	}

	void EMObject::ConsoleLog()
	{
		emscripten::val console = emscripten::val::global("console");
		console.call<void>("log", mJSObject);
	}
}

#endif