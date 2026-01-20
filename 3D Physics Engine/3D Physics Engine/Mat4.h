#pragma once

#include "Vec3.h"
#include <cmath>
#include <cstring>

class Mat4 {

public:

	// Column-major order (OpenGL 방식)
	float m[16];

	// === Constructor ===
	Mat4() {

		for (int i = 0; i < 16; i++) m[i] = 0.0f;
		m[0] = m[5] = m[10] = m[15] = 1.0f;

	}

	Mat4(float* data) {

		memcpy(m, data, 16 * sizeof(float));

	}

	// === Static Constructor ===

	// 단위 행렬
	static Mat4 identity() {

		Mat4 result;
		return result;
	}

	// 이동 행렬
	static Mat4 translate(const Vec3& v) {

		Mat4 result = identity();
		result.m[12] = v.x;
		result.m[13] = v.y;
		result.m[14] = v.z;
		return result;

	}

	static Mat4 translate(float x, float y, float z) {

		return translate(Vec3(x, y, z));

	}

	// 크기 행렬
	static Mat4 scale(const Vec3& v) {

		Mat4 result = identity();
		result.m[0] = v.x;
		result.m[5] = v.y;
		result.m[10] = v.z;
		return result;

	}

	static Mat4 scale(float x, float y, float z) {

		return scale(Vec3(x, y, z));

	}

	static Mat4 scale(float s) {

		return scale(s, s, s);

	}

	// X축 회전
	static Mat4 rotateX(float angle) {

		Mat4 result = identity();
		float c = std::cos(angle);
		float s = std::sin(angle);

		result.m[5] = c;
		result.m[6] = s;
		result.m[9] = -s;
		result.m[10] = c;

		return result;

	}

	// Y축 회전
	static Mat4 rotateY(float angle) {

		Mat4 result = identity();
		float c = std::cos(angle);
		float s = std::sin(angle);

		result.m[0] = c;
		result.m[2] = -s;
		result.m[8] = s;
		result.m[10] = c;
		
		return result;

	}
	
	// Z축 회전
	static Mat4 rotateZ(float angle) {

		Mat4 result = identity();
		float c = std::cos(angle);
		float s = std::sin(angle);

		result.m[0] = c;
		result.m[1] = s;
		result.m[4] = -s;
		result.m[5] = c;

		return result;

	}

	// 임의 축 회전
	static Mat4 rotate(const Vec3& axis, float angle) {

		Mat4 result = identity();
		
		Vec3 a = axis.normalize();
		float c = std::cos(angle);
		float s = std::sin(angle);
		float t = 1.0f - c;

		result.m[0] = t * a.x * a.y + c;
		result.m[1] = t * a.x * a.y + s * a.z;
		result.m[2] = t * a.x * a.z - s * a.y;

		result.m[4] = t * a.x * a.y - s * a.z;
		result.m[5] = t * a.y * a.y + c;
		result.m[6] = t * a.y * a.z + s * a.x;

		result.m[8] = t * a.x * a.z + s * a.z;
		result.m[9] = t * a.y * a.z - s * a.x;
		result.m[10] = t * a.z * a.z + c;

		return result;

	}

	// === View Matrix (Camera) ===

	// LookAt (카메라 위치와 방향)
	static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {

		Vec3 f(center.x - eye.x, center.y - eye.y, center.z - eye.z);
		f = f.normalize(); // Forward
		Vec3 s = f.cross(up);
		s = s.normalize(); // Right (Side)
		Vec3 u = s.cross(f); // Up

		Mat4 result = identity();
		
		result.m[0] = s.x;
		result.m[4] = s.y;
		result.m[8] = s.z;

		result.m[1] = u.x;
		result.m[5] = u.y;
		result.m[9] = u.z;

		result.m[2] = -f.x;
		result.m[6] = -f.y;
		result.m[10] = -f.z;

		result.m[12] = -s.dot(eye);
		result.m[13] = -u.dot(eye);
		result.m[14] = f.dot(eye);

		return result;

	}

	// === Projection Matrix ===

	// 원근 투영
	static Mat4 perspective(float fov, float aspect, float near, float far) {

		Mat4 result;
		for (int i = 0; i < 16; i++) {

			result.m[i] = 0.0f;

		}

		float f = 1.0f / std::tan(fov / 2.0f);

		result.m[0] = f / aspect;
		result.m[5] = f;
		result.m[10] = (far + near) / (near - far);
		result.m[11] = -1.0f;
		result.m[14] = (2.0f * far * near) / (near - far);

		return result;

	}

	// 직교 투영
	static Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {

		Mat4 result = identity();

		result.m[0] = 2.0f / (right - left);
		result.m[5] = 2.0f / (top - bottom);
		result.m[10] = -2.0f / (far - near);

		result.m[12] = -(right + left) / (right - left);
		result.m[13] = -(top + bottom) / (top - bottom);
		result.m[14] = -(far + near) / (far - near);

		return result;

	}

	// === operators ===
	
	// 행렬 곱셈
	Mat4 operator*(const Mat4& other) const {

		Mat4 result;

		for (int i = 0; i < 4; i++) {
			
			for (int j = 0; j < 4; j++) {

				float sum = 0.0f;
				for (int k = 0; k < 4; k++) {

					sum += m[k * 4 + i] * other.m[j * 4 + k];

				}
				result.m[j * 4 + i] = sum;
			}

		}

		return result;

	}

	Mat4& operator*=(const Mat4& other) {

		*this = *this * other;
		return *this;

	}

	// 벡터 변환 (동차 좌표)
	Vec3 transformPoint(const Vec3& v) const {

		float x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
		float y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
		float z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
		float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];

		if (w != 0.0f && w != 1.0f) {

			return Vec3(x / w, y / w, z / w);

		}

		return Vec3(x, y, z);

	}

	// 방향 벡터 변환 (이동 무시)
	Vec3 transformDirection(const Vec3& v) const {

		float x = m[0] * v.x + m[4] * v.y + m[8] * v.z;
		float y = m[1] * v.x + m[5] * v.y + m[9] * v.z;
		float z = m[2] * v.x + m[6] * v.y + m[10] * v.z;
		return Vec3(x, y, z);

	}

	// === Utility Function ===

	// 전치
	Mat4 transpose() const {

		Mat4 result;
		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++) {

				result.m[j * 4 + i] = m[i * 4 + j];
			}

		}

		return result;

	}

	// 데이터 포인터 (OpenGL 전달용)
	const float* data() const {

		return m;

	}

	float* data() {

		return m;

	}

	// === Debug ===
	void print() const {

		for (int i = 0; i < 4; i++) {

			std::cout << "[ ";
			for (int j = 0; j < 4; j++) {

				std::cout << m[j * 4 + i] << " ";

			}
			std::cout << "]" << std::endl;
		}

	}

};