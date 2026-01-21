#pragma once

#include "RigidBody3D.h"
#include "Vec3.h"
#include <cmath>
#include <algorithm>

struct CollisionInfo {

	bool hasCollision;
	Vec3 normal; // 충돌 법선
	float depth; // 침투 깊이

	CollisionInfo() : hasCollision(false), normal(0, 0, 0), depth(0) {}

};

// === 축에 박스 투영 ===
void projectBox(const RigidBody3D& box, const Vec3& axis, float& min, float& max) {

	Vec3 vertices[8];
	box.getWorldVertices(vertices);

	min = max = vertices[0].dot(axis);

	for (int i = 1; i < 8; i++) {

		float projection = vertices[i].dot(axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;

	}

}

// === 두 범위가 겹치는지 확인 ===
bool overlap(float minA, float maxA, float minB, float maxB, float& depth) {

	if (maxA < minB || maxB < minA) {

		return false;

	}

	// 겹침
	float d1 = maxA - minB;
	float d2 = maxB - minA;
	depth = (d1 < d2) ? d1 : d2;

	return true;

}

// === 3D SAT 충돌 검사 ===
CollisionInfo checkBoxCollision(const RigidBody3D& a, const RigidBody3D& b) {

	CollisionInfo info;

	Vec3 diff(
		a.position.x - b.position.x,
		a.position.y - b.position.y,
		a.position.z - b.position.z
	);

	float distSq = diff.lengthSq();
	float maxSize = (a.halfSize.length() + b.halfSize.length()) * 2.0f;
	if (distSq > maxSize * maxSize) {

		return info;

	}

	// 박스들의 축 구하기
	Vec3 axesA[3], axesB[3];
	a.getAxes(axesA);
	b.getAxes(axesB);

	float minDepth = 1000000.0f;
	Vec3 bestAxis(0, 0, 0);

	// === 15개 축 체크 ===

	// A의 3개 축
	for (int i = 0; i < 3; i++) {

		Vec3 axis = axesA[i];
		if (axis.lengthSq() < 0.0001f) continue;

		axis = axis.normalize();

		float minA, maxA, minB, maxB, depth;
		projectBox(a, axis, minA, maxA);
		projectBox(b, axis, minB, maxB);

		// 분리 축 발견시 충돌 없음
		if (!overlap(minA, maxA, minB, maxB, depth)) {

			return info;

		}

		if (depth < minDepth) {

			minDepth = depth;
			bestAxis = axis;

		}

	}

	// B의 3개 축
	for (int i = 0; i < 3; i++) {

		Vec3 axis = axesB[i];
		if (axis.lengthSq() < 0.0001f) continue;

		axis = axis.normalize();

		float minA, maxA, minB, maxB, depth;
		projectBox(a, axis, minA, maxA);
		projectBox(b, axis, minB, maxB);

		if (!overlap(minA, maxA, minB, maxB, depth)) {

			return info;

		}

		if (depth < minDepth) {

			minDepth = depth;
			bestAxis = axis;

		}

	}

	// 엣지 간 외적 (9개)
	for (int i = 0; i < 3; i++) {

		for (int j = 0; j < 3; j++) {

			Vec3 axis = axesA[i].cross(axesB[j]);

			if (axis.lengthSq() < 0.0001f) continue;

			axis = axis.normalize();

			float minA, maxA, minB, maxB, depth;
			projectBox(a, axis, minA, maxA);
			projectBox(b, axis, minB, maxB);

			if (!overlap(minA, maxA, minB, maxB, depth)) {

				return info;

			}

			if (depth < minDepth) {

				minDepth = depth;
				bestAxis = axis;

			}

		}

	}

	// 모든 축에서 겹침 = 충돌
	info.hasCollision = true;
	info.depth = minDepth;
	info.normal = bestAxis;

	// 법선 방향 확인(A -> B)
	Vec3 centerDiff(
		b.position.x - a.position.x,
		b.position.y - a.position.y,
		b.position.z - a.position.z
	);

	if (info.normal.dot(centerDiff) < 0) {

		info.normal = Vec3(-info.normal.x, -info.normal.y, -info.normal.z);

	}

	return info;

}

// === 충돌 응답 ===
void resolveCollision(RigidBody3D& a, RigidBody3D& b, const CollisionInfo& info) {

	if (!info.hasCollision) return;

	if (info.depth < 0.001f) return;

	// 위치 보정 (질량 비율)
	float totalInvMass = a.invMass + b.invMass;
	if (totalInvMass < 0.0001f) return;

	Vec3 correction(
		info.normal.x * info.depth * 1.05f,
		info.normal.y * info.depth * 1.05f,
		info.normal.z * info.depth * 1.05f
	);

	float aRatio = a.invMass / totalInvMass;
	float bRatio = b.invMass / totalInvMass;

	Vec3 aCorrection(correction.x * aRatio, correction.y * aRatio, correction.z * aRatio);
	Vec3 bCorrection(correction.x * bRatio, correction.y * bRatio, correction.z * bRatio);

	a.position = a.position - aCorrection;
	b.position = b.position + bCorrection;

	// 상대 속도
	Vec3 relativeVel(
		a.velocity.x - b.velocity.x,
		a.velocity.y - b.velocity.y,
		a.velocity.z - b.velocity.z
	);

	float velAlongNormal = relativeVel.dot(info.normal);

	if (velAlongNormal > 0) return;

	// 반발 계수
	float restitution = 0.4f;

	// Impulse
	float j = -(1.0f + restitution) * velAlongNormal;
	j /= totalInvMass;

	Vec3 impulse(
		info.normal.x * j,
		info.normal.y * j,
		info.normal.z * j
	);

	a.applyImpulse(impulse);
	Vec3 negImpulse(-impulse.x, -impulse.y, -impulse.z);
	b.applyImpulse(negImpulse);

	// Friction
	Vec3 tangent(
		relativeVel.x - info.normal.x * velAlongNormal,
		relativeVel.y - info.normal.y * velAlongNormal,
		relativeVel.z - info.normal.z * velAlongNormal
	);

	if (tangent.lengthSq() > 0.0001f) {

		tangent = tangent.normalize();

		float frictionCoeff = 0.3f;
		float jt = -relativeVel.dot(tangent) * frictionCoeff;
		jt /= totalInvMass;

		Vec3 frictionImpulse(
			tangent.x * jt,
			tangent.y * jt,
			tangent.z * jt
		);

		a.applyImpulse(frictionImpulse);
		Vec3 negFriction(-frictionImpulse.x, -frictionImpulse.y, -frictionImpulse.z);
		b.applyImpulse(negFriction);

	}

}
