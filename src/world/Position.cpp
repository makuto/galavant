#include "Position.hpp"
#include <cmath>

namespace gv
{
Position::Position(float x, float y, float z) : X(x), Y(y), Z(z)
{
}

bool Position::Equals(const Position& otherPosition, float tolerance) const
{
	return (fabs(X - otherPosition.X) <= tolerance && fabs(Y - otherPosition.Y) <= tolerance &&
	        fabs(Z - otherPosition.Z) <= tolerance);
}

float& Position::operator[](int index)
{
	switch (index)
	{
		case 0:
			return X;
		case 1:
			return Y;
		case 2:
			return Z;
	}
	return X;
}

Position Position::operator+(const Position& otherPosition) const
{
	Position newPosition(*this);
	newPosition.X += otherPosition.X;
	newPosition.Y += otherPosition.Y;
	newPosition.Z += otherPosition.Z;
	return newPosition;
}

Position Position::operator-(const Position& otherPosition) const
{
	Position newPosition(*this);
	newPosition.X -= otherPosition.X;
	newPosition.Y -= otherPosition.Y;
	newPosition.Z -= otherPosition.Z;
	return newPosition;
}

Position Position::operator*(const Position& otherPosition) const
{
	Position newPosition(*this);
	newPosition.X *= otherPosition.X;
	newPosition.Y *= otherPosition.Y;
	newPosition.Z *= otherPosition.Z;
	return newPosition;
}

Position Position::operator/(const Position& otherPosition) const
{
	Position newPosition(*this);
	newPosition.X /= otherPosition.X;
	newPosition.Y /= otherPosition.Y;
	newPosition.Z /= otherPosition.Z;
	return newPosition;
}

Position& Position::operator+=(const Position& otherPosition)
{
	X += otherPosition.X;
	Y += otherPosition.Y;
	Z += otherPosition.Z;
	return *this;
}

Position& Position::operator-=(const Position& otherPosition)
{
	X -= otherPosition.X;
	Y -= otherPosition.Y;
	Z -= otherPosition.Z;
	return *this;
}

Position& Position::operator*=(const Position& otherPosition)
{
	X *= otherPosition.X;
	Y *= otherPosition.Y;
	Z *= otherPosition.Z;
	return *this;
}

Position& Position::operator/=(const Position& otherPosition)
{
	X /= otherPosition.X;
	Y /= otherPosition.Y;
	Z /= otherPosition.Z;
	return *this;
}

GlobalPosition::GlobalPosition(Position& localPosition) : LocalPosition(localPosition)
{
}
}