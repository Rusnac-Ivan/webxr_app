#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <functional>

namespace util
{
	template <typename... Args>
	class CallBack
	{
	public:
		CallBack()
		{
		}

		CallBack(std::function<void(Args...)> call_back)
		{
			mFunction = call_back;
		}

		CallBack(const CallBack &other)
		{
			mFunction = other.mFunction;
		}

		CallBack &operator=(const CallBack &other)
		{
			mFunction = other.mFunction;
			return *this;
		}

		virtual ~CallBack()
		{
		}

		void Set(std::function<void(Args...)> call_back)
		{
			mFunction = call_back;
		}

		void UnSet()
		{
			mFunction = nullptr;
		}

		void operator()(Args... args)
		{
			if (mFunction)
				mFunction(args...);
		}

		inline bool IsNotEmpty() { return mFunction != nullptr; }

	private:
		std::function<void(Args...)> mFunction;
	};

}

#endif
