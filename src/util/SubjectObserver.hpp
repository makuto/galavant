#pragma once

#include <vector>

#include "../entityComponentSystem/EntityTypes.hpp"

namespace gv
{
template <class T>
class Observer
{
public:
	Observer() = default;
	virtual ~Observer() = default;

	virtual void OnNotify(const T& event) = 0;
};

template <class T>
class Subject
{
private:
	std::vector<Observer<T>*> Observers;

public:
	Subject() = default;
	~Subject() = default;

	void AddObserver(Observer<T>* observer)
	{
		if (observer && std::find(Observers.begin(), Observers.end(), observer) == Observers.end())
			Observers.push_back(observer);
	}
	void RemoveObserver(Observer<T>* observer)
	{
		typename std::vector<Observer<T>*>::iterator foundObserver =
		    std::find(Observers.begin(), Observers.end(), observer);
		if (foundObserver != Observers.end())
			Observers.remove(foundObserver);
	}

	// TODO: This could be improved by sending all events in one Notify...
	void Notify(const T& event)
	{
		for (Observer<T>* currentObserver : Observers)
			currentObserver->OnNotify(event);
	}
};
};