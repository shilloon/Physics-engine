#pragma once

#include "Vec3.h"
#include "Mat4.h"

class RigidBody3D {

public:

	// === 기본 속성 ===
	Vec3 position;
	Vec3 velocity;

	// === 회전 속성 ===
	Vec3 rotation;
	Vec3 angularVelocity;

	// === 물리 속성 ===
	float mass;
	float invMass;

	// === 박스 크기 ===
	Vec3 halfSize;

	// === 기타 ===
	Vec3 color;
	bool isStatic;
	bool isSleeping;

	// === Constructor ===

	RigidBody3D()
		: position(0, 0, 0),
		velocity(0, 0, 0),
		rotation(0, 0, 0),
		angularVelocity(0, 0, 0),
		mass(1.0f),
		invMass(1.0f),
		halfSize(0.5f, 0.5f, 0.5f),
		color(1, 1, 1),
		isStatic(false),
		isSleeping(false)
	{}

	RigidBody3D(Vec3 pos, Vec3 size, float m = 1.0f)
		: position(pos),
		velocity(0, 0, 0),
		rotation(0, 0, 0),
		angularVelocity(0, 0, 0),
		mass(m),
		invMass(m > 0 ? 1.0f / m : 0.0f),
		halfSize(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f),
		color(1, 1, 1),
		isStatic(false),
		isSleeping(false)
	{}

	// === Create Static Object ===
	static RigidBody3D createStatic(Vec3 pos, Vec3 size) {

		RigidBody3D body(pos, size, 0.0f);
		body.isStatic = true;
		body.invMass = 0.0f;
		return body;

	}

	// === 박스 8개 꼭짓점 구하기 (local 좌표) ===
	void getVertices(Vec3 vertices[8]) const {

		vertices[0] = Vec3(-halfSize.x, -halfSize.y, -halfSize.z);
		vertices[1] = Vec3(halfSize.x, -halfSize.y, -halfSize.z);
		vertices[2] = Vec3(halfSize.x, halfSize.y, -halfSize.z);
		vertices[3] = Vec3(-halfSize.x, halfSize.y, -halfSize.z);
		vertices[4] = Vec3(-halfSize.x, -halfSize.y, halfSize.z);
		vertices[5] = Vec3(halfSize.x, -halfSize.y, halfSize.z);
		vertices[6] = Vec3(halfSize.x, halfSize.y, halfSize.z);
		vertices[7] = Vec3(-halfSize.x, halfSize.y, halfSize.z);

	}

	// === 박스 8개 꼭짓점 구하기 (World 좌표) ===
	void getWorldVertices(Vec3 vertices[8]) const {

		getVertices(vertices);

		// 회전 행렬
		Mat4 rotMat = Mat4::rotateX(rotation.x) * Mat4::rotateY(rotation.y) * Mat4::rotateZ(rotation.z);

		// 각 장점을 회전시키고 이동
		for (int i = 0; i < 8; i++) {

			vertices[i] = rotMat.transformDirection(vertices[i]);
			Vec3 translated(
				vertices[i].x + position.x,
				vertices[i].y + position.y,
				vertices[i].z + position.z
			);
			vertices[i] = translated;
		}

	}

	// === 박스 3개 축 구하기 (회전된 x, y, ,z 축) ===
	void getAxes(Vec3 axes[3]) const {

		Mat4 rotMat = Mat4::rotateX(rotation.x) * Mat4::rotateY(rotation.y) * Mat4::rotateZ(rotation.z);

		axes[0] = rotMat.transformDirection(Vec3(1, 0, 0));
		axes[1] = rotMat.transformDirection(Vec3(0, 1, 0));
		axes[2] = rotMat.transformDirection(Vec3(0, 0, 1));

	}

	// === Model 행렬 구하기 ===
	Mat4 getModelMatrix() const {

		Mat4 model = Mat4::translate(position);
		model = model * Mat4::rotateX(rotation.x);
		model = model * Mat4::rotateY(rotation.y);
		model = model * Mat4::rotateZ(rotation.z);
		
		return model;

	}

	// Physics Update
	void applyForce(const Vec3& force, float dt) {

		if (isStatic) return;
		Vec3 accel(force.x * invMass, force.y * invMass, force.z * invMass);
		Vec3 deltaV(accel.x * dt, accel.y * dt, accel.z * dt);
		velocity = velocity + deltaV;

	}

	void applyImpulse(const Vec3& impulse) {

		if (isStatic) return;
		Vec3 deltaV(impulse.x * invMass, impulse.y * invMass, impulse.z * invMass);
		velocity = velocity + deltaV;

	}

	void integrate(float dt) {

		if (isStatic) return;

		Vec3 deltaPos(velocity.x * dt, velocity.y * dt, velocity.z * dt);
		position = position + deltaPos;

		Vec3 deltaRot(angularVelocity.x * dt, angularVelocity.y * dt, angularVelocity.z * dt);
		rotation = rotation + deltaRot;

	}

};