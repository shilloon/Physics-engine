#pragma once
#include "Vec2.h"

class Particle {

public:
	Vec2 position; // 위치
	Vec2 velocity; // 속도
	Vec2 acceleration; // 가속도
	float mass; // 질량
	float radius; // 반지름
	float restitution; // 탄성 계수 (0 ~ 1)
	float drag; // 공기 저항 계수
	bool onGround;

	// Constructor
	Particle(Vec2 pos, float mass, float radius, float restitution = 0.8f, float drag = 0.01f)
		: position(pos), velocity(0, 0), acceleration(0, 0), mass(mass), radius(radius), restitution(restitution), drag(drag), onGround(false) {}

	// F = ma
	void applyForce(Vec2 force) {
		Vec2 accel = force / mass;
		acceleration += accel;
	}

	// Drag
	void applyDrag() {
		// Drag force = -k * v^2 * direction
		// Drag = -drag * velocity * |velocity|
		float speed = velocity.length();
		if (speed > 0) {

			Vec2 dragForce = velocity * (-drag * speed);
			applyForce(dragForce * mass);

		}

	}

	// Friction
	void applyFriction(float frictionCoeff) {

		if (onGround && velocity.lengthSq() > 0.01f) {

			// Friction = -μ * N * direction
			// N (수직항력) ~ mass * gravity
			Vec2 frictionDir = velocity.normalize() * -1.0f;
			Vec2 friction = frictionDir * frictionCoeff * mass * 9.8f * 100;
			applyForce(friction);

			if (velocity.lengthSq() < 100.0f) {
				velocity = Vec2(0, 0);
			}

		}

	}

	// Physics update (오일러 기법)
	void update(float dt) {

		velocity += acceleration * dt;
		position += velocity * dt;
		acceleration = Vec2(0, 0);

	}

	// 경계 충돌 처리
	void checkBounds(float width, float height) {

		onGround = false;

		// 바닥 충돌
		if (position.y + radius > height) {
			position.y = height - radius;
			velocity.y = -velocity.y * restitution;
			onGround = true;

			if (abs(velocity.y) < 10.0f) {
				velocity.y = 0;
			}

		}

		// 천장 충돌
		if (position.y - radius < 0) {
			position.y = radius;
			velocity.y = -velocity.y * restitution;
		}

		// 오른쪽 벽 충돌
		if (position.x + radius > width) {
			position.x = width - radius;
			velocity.x = -velocity.x * restitution;
		}

		// 왼쪽 벽 충돌
		if (position.x - radius < 0) {
			position.x = radius;
			velocity.x = -velocity.x * restitution;
		}

	}

};