#ifndef OBJECTID_HPP
#define OBJECTID_HPP

typedef int ObjectID;

// The Object hasn't been assigned an ID
extern const ObjectID OBJECT_ID_NONE;

// The last possible ID that can be assigned
extern const ObjectID OBJECT_ID_MAX;

// Start assigning IDs at this value
extern const ObjectID OBJECT_ID_FIRST;

#endif
