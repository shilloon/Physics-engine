#pragma once

#include <cmath>
#include <iostream>

class Vec3 {

public:
	float x, y, z;

	// === Constructor ===
	Vec3() : x(0), y(0), z(0) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3(float val) : x(val), y(val), z(val) {}

	// === Operator ===

	// Add
	Vec3 operator+(const Vec3& v) const {

		return Vec3(x + v.x, y + v.y, z + v.z);

	}

	// Negation
	Vec3 operator-(const Vec3 v) const {

		return Vec3(x- v.x, y - v.y, z - v.z);

	}

	// Multiplication
	Vec3 operator*(float s) const {

		return Vec3(x * s, y * s, z * s);

	}

	// Division
	Vec3 operator/(float s) const {

		return Vec3(x / s, y / s, z / s);

	}

	// Compound assignment
	Vec3& operator+=(const Vec3& v) {

		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vec3& operator-=(const Vec3& v) {

		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vec3& operator*=(float s) {

		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vec3& operator/=(float s) {

		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	// === Utility Function ===

	// Length magnitude
	float length() const {

		return std::sqrt(x * x + y * y + z * z);

	}

	// Length squared
	float lengthSq() const {

		return x * x + y * y + z * z;

	}

	// Normalize
	Vec3 normalize() const {

		float len = length();
		if (len > 0.0001f) {

			return Vec3(x / len, y / len, z / len);

		}
		return Vec3(0, 0, 0);
	}

	// Dot product
	float dot(const Vec3& v) const {

		return  x * v.x + y * v.y + z * v.z;

	}

	// Cross product
	Vec3 cross(const Vec3& v) const {

		return Vec3(

			y * v.z - z * v.y, // x 성분
			z * v.x - x * v.z, // y 성분
			x * v.y - y * v.x  // z 성분

		);

	}

	// Distance to another vector
	float distanceTo(const Vec3& v) const {

		Vec3 diff(x - v.x, y - v.y, z - v.z);
		return diff.length();

	}

	// Distance squared
	float distanceToSq(const Vec3& v) const {

		Vec3 diff(x - v.x, y - v.y, z - v.z);
		return diff.lengthSq();

	}

	// === Static Utility Function ===

	// Zero Vector
	static Vec3 zero() {

		return Vec3(0, 0, 0);

	}

	// Unit Vector
	static Vec3 unitX() { return Vec3(1, 0, 0); }
	static Vec3 unitY() { return Vec3(0, 1, 0); }
	static Vec3 unitZ() { return Vec3(0, 0, 1); }

	// Linear interpolation
	static Vec3 lerp(const Vec3& a, const Vec3& b, float t) {

		return Vec3(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t
		);

	}

	// === Debug ouput ===
	void print() const {

		std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;

	}

};

// Scaler * Vec3 (왼쪽 스칼라)
inline Vec3 operator*(float s, const Vec3& v) {

	return v * s;

}