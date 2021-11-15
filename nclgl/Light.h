#pragma once

#include "Vector3.h"
#include "Vector4.h"
class Light
{
public:
	Light() {};
	Light(const Vector3& position, const Vector4& colour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(const float& val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

private:
	Vector3 position;
	float radius;
	Vector4 colour;
};
