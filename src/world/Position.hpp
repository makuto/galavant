#pragma once

namespace gv
{
struct Position
{
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;

	Position() = default;
	Position(float x, float y, float z);

	bool Equals(const Position& otherPosition, float tolerance) const;

	float& operator[](int index);

	Position operator+(const Position& otherPosition) const;
	Position operator-(const Position& otherPosition) const;
	Position operator*(const Position& otherPosition) const;
	Position operator/(const Position& otherPosition) const;

	Position& operator+=(const Position& otherPosition);
	Position& operator-=(const Position& otherPosition);
	Position& operator*=(const Position& otherPosition);
	Position& operator/=(const Position& otherPosition);
};

struct GlobalPosition
{
	Position LocalPosition;
	// Will be added once chunks/world is figured out
	// Position ChunkPosition;

	GlobalPosition() = default;
	GlobalPosition(Position& localPosition);
};
};