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

// DONE: Callbacks which only care about a single event; how to remove? Who owns void* userdata?
//  I think most useful solution would be to add "AddOwnedCallback" which, when called, would be
//  set to "Called" then add "RemoveOwnedCalledCallbacks" (god) which cleans them out
//  And the callback should own void* userdata. CallbackContainer doesn't give a damn who owns it
// DONE: So I sort of did the above and it made this much dirtier. I'm not sure what to do
//
// TODO: Don't use this class; should use std::function if UserData is needed. Things shouldn't be generic
// and try to use this container anyhow
template <class CallbackType>
class CallbackContainer
{
public:
	CallbackContainer() = default;
	~CallbackContainer() = default;

	// It's your job to actually call the callbacks
	std::vector<CallbackCall<CallbackType>> Callbacks;

	typename std::vector<CallbackCall<CallbackType>>::iterator FindCallback(
	    const CallbackType callbackToFind, void* UserData)
	{
		for (typename std::vector<CallbackCall<CallbackType>>::iterator callbackIt =
		         Callbacks.begin();
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
			CallbackCall<CallbackType> newCall = {callbackToAdd, UserData};
			Callbacks.push_back(newCall);
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