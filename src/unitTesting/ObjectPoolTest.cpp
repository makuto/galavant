#include <iostream>

#include "../util/ObjectPool.hpp"

int main()
{
	ObjectPool<int> testPool(100);
	
	for (unsigned int i = 0; i < testPool.GetSize(); i++)
	{
		int* newData = testPool.GetNewData();
		if (newData)
			*newData = i;
		else
			break;
	}

	for (auto data:testPool)
	{
		std::cout << data << "\n";
	}

//	for (PoolContainer<int>::iterator it = testPool.begin(); it != testPool.end(); ++it)
//	{
//		std::cout << (*it) << "\n";
//	}

	return 1;
}