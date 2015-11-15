#ifndef OBJECTID_CPP
#define OBJECTID_CPP

#include "ObjectID.hpp"

// The Object hasn't been assigned an ID
const ObjectID OBJECT_ID_NONE = -1;

// The last possible ID that can be assigned
// TODO: Set to a better value (this was completely arbitrary)
const ObjectID OBJECT_ID_MAX = 1000000;

// Start assigning IDs at this value
const ObjectID OBJECT_ID_FIRST = 1;

#endif
