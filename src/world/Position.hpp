#pragma once

#include <vector>

#include "../util/Logging.hpp"

namespace gv
{
extern const float POSITION_TOLERANCE;

// Galavant will use Z as the up axis
struct Position
{
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;

	Position() = default;
	Position(float x, float y, float z);
	Position(float floatArray[3]);
	~Position() = default;

	bool Equals(const Position& otherPosition, float tolerance) const;

	void Reset();
	void Set(float x, float y, float z);

	float ManhattanTo(const Position& otherPosition) const;

	// Returns true if the Position is exactly zero
	explicit operator bool() const;

	float& operator[](int index);

	Position operator+(const Position& otherPosition) const;
	Position operator-(const Position& otherPosition) const;
	Position operator*(const Position& otherPosition) const;
	Position operator/(const Position& otherPosition) const;

	Position& operator+=(const Position& otherPosition);
	Position& operator-=(const Position& otherPosition);
	Position& operator*=(const Position& otherPosition);
	Position& operator/=(const Position& otherPosition);

	bool operator==(const Position& otherPosition) const;
};

struct GlobalPosition
{
	Position LocalPosition;
	// Will be added once chunks/world is figured out
	// Position ChunkPosition;

	GlobalPosition() = default;
	GlobalPosition(Position& localPosition);
};

typedef std::vector<Position> PositionList;
typedef std::vector<Position*> PositionRefList;
};

namespace plog
{
Record& operator<<(Record& record, const gv::Position& position);
}