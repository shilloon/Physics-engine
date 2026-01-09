#pragma once
#include <cmath>

class Vec2 {

public:
	float x, y;

	// Constructor
	Vec2() : x(0), y(0) {}
	Vec2(float x, float y) : x(x), y(y) {}


	// ======= operator =======

	// Plus +
	Vec2 operator+(const Vec2& v) const {
		return Vec2(x + v.x, y + v.y);
	}

	// minus -
	Vec2 operator-(const Vec2& v) const {
		return Vec2(x - v.x, y - v.y);
	}

	// Scaler Multiplication *
	Vec2 operator*(float s) const {
		return Vec2(x * s, y * s);
	}

	// Scaler division /
	Vec2 operator/(float s) const {
		return Vec2(x / s, y / s);
	}

	// += 
	Vec2 operator+=(const Vec2& v) {
		
		x += v.x;
		y += v.y;
		return *this;

	}

	// -=
	Vec2 operator-=(const Vec2& v) {
		
		x -= v.x;
		y -= v.y;
		return *this;

	}

	// *=
	Vec2 operator*=(float s) {
		
		x *= s;
		y *= s;
		return *this;

	}

	// /=
	Vec2 operator/=(float s) {

		x /= s;
		y /= s;
		return *this;

	}

	// ======= utility function =======

	// Vector Length
	float length() const {
		return std::sqrt(x * x + y * y);
	}

	// Vector Length sqrt(sqrt 연산 비용 절약) 두 개의 Vector 길이 비교할 때 사용
	float lengthSq() const {
		return x * x + y * y;
	}

	// Normalize(Vector Scale)
	Vec2 normalize() const {
		float len = length();
		if (len > 0) {
			return Vec2(x / len, y / len);
		}
		return Vec2(0, 0);
	}

	// dot product
	float dot(const Vec2& v) const {
		return x * v.x + y * v.y;
	}

	// cross product 2D에서 Scaler Value 반환
	float cross(const Vec2& v) const {
		return x * v.y - y * v.x;
	}

};