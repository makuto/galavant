#pragma once

#include <vector>

namespace gv
{
template <class CallbackType>
struct CallbackCall
{
	CallbackType Callback;
	void* UserData;
};

template <class CallbackType>
class CallbackContainer
{
public:
	CallbackContainer() = default;
	~CallbackContainer() = default;

	// It's your job to actually call the callbacks
	std::vector<CallbackCall<CallbackType>> Callbacks;

	typename std::vector<CallbackCall<CallbackType>>::iterator FindCallback(const CallbackType callbackToFind,
	                                                          void* UserData)
	{
		for (typename std::vector<CallbackCall<CallbackType>>::iterator callbackIt = Callbacks.begin();
		     callbackIt != Callbacks.end(); ++callbackIt)
		{
			if ((*callbackIt).Callback == callbackToFind && (*callbackIt).UserData == UserData)
				return callbackIt;
		}
		return Callbacks.end();
	}

	void AddCallback(const CallbackType callbackToAdd, void* UserData)
	{
		if (callbackToAdd && FindCallback(callbackToAdd, UserData) == Callbacks.end())
		{
			Callbacks.push_back({callbackToAdd, UserData});
		}
	}

	void RemoveCallback(const CallbackType callback, void* UserData)
	{
		typename std::vector<CallbackCall<CallbackType>>::iterator foundCallbackIt =
		    FindCallback(callback, UserData);
		if (foundCallbackIt != Callbacks.end())
			Callbacks.erase(foundCallbackIt);
	}
};
}