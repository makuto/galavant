#ifndef POOL_HPP
#define POOL_HPP
#include <stdlib.h>
#include <vector>

// This is some old code copied from the Horizon iteration. It's due for a rewrite.
// TODO: Add standard iterator syntax (See ObjectPool.hpp)

/* --FragmentedPool--
 * FragmentedPool holds any data. It initializes the requested amount of data on construction.
 * Any subsequent calls to getNewData() and removeData() cost very little
 * because no more memory is allocated or freed. This provides significant
 * speed improvements.
 *
 * It uses a doubly linked list for faster iteration through the active data
 * and a singly linked list for iteration through free/inactive data.
 *
 * Vector is used to store the data due to its resize function, which allows
 * the pool's size to be set on initialization. The pool cannot be resized
 * after initialization because vector must move all data to get one complete
 * block.
 *
 * Note: You must compile with -std=c++11 if you want your data type's
 * constructors to be called.
 *
 * TODO: Need to construct inUse list to be in order (better locality)!
 * */

// Generic data container. Holds the requested data as well as pointers to the next
// free data (if inactive), and the previous and next active/used data (if active)
template <class T>
class FragmentedPoolData
{
public:
	T data;
	bool isActive;
	FragmentedPoolData<T>* nextFreeData;
	FragmentedPoolData<T>* nextUsedData;
	FragmentedPoolData<T>* prevUsedData;
};

template <class R>
class FragmentedPool
{
private:
	std::vector<FragmentedPoolData<R>> pool;  // The pool data itself
	FragmentedPoolData<R>* firstFreeData;     // The head of the free data linked list
	FragmentedPoolData<R>* firstUsedData;     // The head of the used data linked list
	unsigned int size;                        // The size of the pool
	unsigned int totalActiveData;             // The number of active data in the pool

	// Prepare the data pointers as a linked list
	void ResetPool()
	{
		// Iterate through all data, resetting pointers and isActive status
		firstFreeData = &pool[0];
		for (unsigned int i = 0; i < size - 1; ++i)
		{
			FragmentedPoolData<R>* currentData = &pool[i];
			currentData->nextFreeData = &pool[i + 1];
			currentData->nextUsedData = nullptr;
			currentData->prevUsedData = nullptr;
			currentData->isActive = false;
		}
		// Last datum needs nullptr status to signal end of linked list
		FragmentedPoolData<R>* lastData = &pool[size - 1];
		lastData->nextFreeData = nullptr;
		lastData->nextUsedData = nullptr;
		lastData->prevUsedData = nullptr;
		lastData->isActive = false;
	}

public:
	// Initializes the FragmentedPool with newSize elements. Once this constructor
	// is called, no more memory is allocated.
	FragmentedPool(unsigned int newSize)
	{
		size = newSize;
		totalActiveData = 0;
		firstFreeData = nullptr;
		firstUsedData = nullptr;
		pool.resize(size);
		ResetPool();
	}
	// Resets the pool. Note that this isn't cheap because it must fix up the linked list
	// that underlies the FragmentedPool structure
	void Clear()
	{
		totalActiveData = 0;
		firstFreeData = nullptr;
		firstUsedData = nullptr;
		ResetPool();
	}
	// Returns the first free data in the pool, or nullptr if the pool is
	// full.
	FragmentedPoolData<R>* GetNewData()
	{
		if (firstFreeData != nullptr)  // Make sure the pool isn't full
		{
			FragmentedPoolData<R>* freeData = firstFreeData;
			firstFreeData = firstFreeData->nextFreeData;
			// Link this data into the usedData linked list
			if (firstUsedData != nullptr)
				firstUsedData->nextUsedData = freeData;
			freeData->nextUsedData = nullptr;
			freeData->prevUsedData = firstUsedData;
			firstUsedData = freeData;
			freeData->isActive = true;
			totalActiveData++;
			return freeData;
		}
		return nullptr;  // Pool is full
	}

	// Sets the data to inactive so that it can be used again.
	void RemoveData(FragmentedPoolData<R>* dataToRemove)
	{
		dataToRemove->nextFreeData = firstFreeData;
		firstFreeData = dataToRemove;
		// Unlinks this data from the usedData linked list
		if (firstFreeData->prevUsedData)
			firstFreeData->prevUsedData->nextUsedData = firstFreeData->nextUsedData;
		totalActiveData--;
		dataToRemove->isActive = false;
	}

	// Gets data at the requested index.
	// Returns nullptr if index isn't within range or data is not active
	// Use getNextActiveData if you want to iterate through the pool
	//(it uses nextUsedData to skip over inactive data)
	FragmentedPoolData<R>* GetActiveDataAtIndex(unsigned int index)
	{
		// Index out of bounds
		if (index > size)
			return nullptr;
		FragmentedPoolData<R>* data = &pool[index];
		if (data->isActive)
			return data;
		else
			return nullptr;  // Data isn't in use
	}

	// Uses prevUsedData to skip over inactive data. This function will
	// all fast iteration through active data in the pool. Break on nullptr
	FragmentedPoolData<R>* GetNextActiveData(FragmentedPoolData<R>* currentData)
	{
		return currentData->prevUsedData;
	}

	// Returns the first active data (use in conjunction with getNextActiveData)
	// to traverse the pool. Returns nullptr if the pool is empty
	FragmentedPoolData<R>* GetFirstActiveData()
	{
		return firstUsedData;
	}

	// Returns the total number of active data in the pool
	unsigned int GetTotalActiveData()
	{
		return totalActiveData;
	}

	int GetPoolSize()
	{
		return size;
	}
};
#endif
