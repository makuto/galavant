#pragma once

#define LIST_FIND(list, object) std::find(list.begin(), list.end(), object)
#define IS_IN_LIST(list, object) (std::find(list.begin(), list.end(), object) != list.end())

#define FOR_ITERATE(listType, list, iteratorName) for (listType::iterator iteratorName = list.begin(); iteratorName != list.end(); ++iteratorName)
#define FOR_ITERATE_NO_INCR(listType, list, iteratorName) for (listType::iterator iteratorName = list.begin(); iteratorName != list.end();)