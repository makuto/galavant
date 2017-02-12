#ifndef OBJECTPOOL_H__
#define OBJECTPOOL_H__

#include <vector>

typedef unsigned int ObjectPoolHandle;

template <class T>
class ObjectPool
{
public:
	template <class R>
	using PoolContainer = std::vector<R>;

private:
	typedef unsigned int PoolIndex;
	// Provide a layer of indirection so ObjectPool can move data as it pleases
	// Fuck - what if you run out of handles? Don't allow that? What if pool resizes? Multiple
	// pools? How would handles work?
	typedef std::vector<PoolIndex> HandleTable;

	PoolContainer<T> Pool;
	HandleTable Handles;

	unsigned int NextFreeData;

	ObjectPoolHandle CreateHandleInternal(PoolIndex index)
	{
		Handles.push_back(index);
		return Handles.size() - 1;
	}

public:
	ObjectPool(unsigned int size)
	{
		Pool.resize(size);
		// Somewhat arbitrary size for handles
		Handles.resize(size);
		NextFreeData = 0;
	}

	typename PoolContainer<T>::iterator begin()
	{
		return Pool.begin();
	}

	typename PoolContainer<T>::iterator end()
	{
		// Only return the range of data which are actually active in the pool
		return Pool.begin() + NextFreeData;
	}

	ObjectPoolHandle GetHandleFromIterator(typename PoolContainer<T>::iterator& it)
	{
		return 0;  // TODO
	}

	unsigned int GetSize()
	{
		return Pool.size();
	}

	unsigned int GetNumActiveElements()
	{
		// Because NextFreeData is the index of the free data nearest to the last active data, it is
		// equivalent to the number of active data
		return NextFreeData;
	}

	T* GetNewData()
	{
		T* newData = GetUnsafe(NextFreeData);
		NextFreeData++;
		return newData;
	}

	// Use if you know you want to have the handle afterwards
	T* GetNewDataWithHandle(ObjectPoolHandle& handle)
	{
	}

	// Returns nullptr if the data at handle isn't active
	// Note that this pointer should NOT be relied upon if you are removing other data
	T* Get(ObjectPoolHandle handle)
	{
		if (handle < NextFreeData)
			return GetUnsafe(handle);
		return nullptr;
	}

	// Don't perform bounds checking and don't make sure the data is active
	inline T* GetUnsafe(ObjectPoolHandle handle)
	{
		return &Pool[handle];
	}

	// Remove the element and replace its space with the element at the end of the pool (swap)
	void Remove(ObjectPoolHandle handle)
	{
	}

	void Remove(typename PoolContainer<T>::iterator& it)
	{
	}
};

#endif /* end of include guard: OBJECTPOOL_H__ */