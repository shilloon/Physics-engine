#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <cmath>

class RigidBody {

public:

	// Linear Motion
	Vec2 position; // 위치
	Vec2 velocity; // 속도 
	Vec2 acceleration; // 가속도
	float mass; // 질량

	// Circular Motion
	float angle; // 회전 각도 
	float angularVelocity; // 각속도
	float angularAcceleration; // 각가속도
	float inertia; // 관성 모멘트

	// Physics property
	float restitution; // 탄성 계수
	float staticFriction; // 정지 마찰
	float dynamicFriction; // 운동 마찰

	// shape
	float width, height; // 박스 크기

	// Constructor
	RigidBody(Vec2 pos, float width, float height, float mass, float restitution = 0.5)
		: position(pos), velocity(0, 0), acceleration(0, 0), mass(mass), angle(0), angularVelocity(0), angularAcceleration(0),
		restitution(restitution), staticFriction(0.6f), dynamicFriction(0.4f), width(width), height(height)
	{

		// 관성 모멘트
		// I = (1/12) * m * (w^2 + h^2)
		inertia = (1.0f / 12.0f) * mass * (width * width + height * height);
	}

	// 힘 적용 ( 중심에 )
	void applyForce(Vec2 force) {

		acceleration += force / mass;

	}

	// torque 적용
	void applyTorque(float torque) {

		angularAcceleration += torque / inertia;

	}

	// 특정 지점 힘 작용 (Force + Torque)
	void applyForceAtPoint(Vec2 force, Vec2 point) {

		applyForce(force);

		// 토크 계산: τ = r × F
		Vec2 r = point - position;
		float torque = r.cross(force);
		applyTorque(torque);

	}

	// 점이 박스에 안에 있는지 체크
	bool contains(Vec2 point) {

		float halfW = width / 2;
		float halfH = height / 2;

		return (point.x >= position.x - halfW && point.x <= position.x + halfW && point.y >= position.y - halfH && point.y <= position.y + halfH);

	}

	// 회전 고려한 정확한 충돌 체크
	bool containsRotated(Vec2 point) {

		Vec2 localPoint = point - position;

		// 역회전
		float cos_a = std::cos(-angle);
		float sin_a = std::sin(-angle);

		float localx = localPoint.x * cos_a - localPoint.y * sin_a;
		float localy = localPoint.x * sin_a + localPoint.y * cos_a;

		// AABB 체크
		float halfW = width / 2;
		float halfH = height / 2;

		return (localx >= -halfW && localx <= halfW && localy >= -halfH && localy <= halfH);

	}

	void update(float dt) {

		// Linear Motion
		velocity += acceleration * dt;
		position += velocity * dt;
		acceleration = Vec2(0, 0);

		// Circular Motion
		angularVelocity += angularAcceleration * dt;
		angle += angularVelocity * dt;
		angularAcceleration = 0;

	}

	// 경계 체크
	void checkBounds(float screenWidth, float screenHeight) {

		if (position.y + height / 2 > screenHeight) {

			position.y = screenHeight - height / 2;
			velocity.y = -velocity.y * restitution;
			angularVelocity *= 0.9f;

		}

		if (position.y - height / 2 < 0) {

			position.y = height / 2;
			velocity.y = -velocity.y * restitution;

		}

		if (position.x + width / 2 > screenWidth) {

			position.x = screenWidth - width / 2;
			velocity.x = -velocity.x * restitution;

		}

		if (position.x - height / 2 < 0) {

			position.x = height / 2;
			velocity.x = -velocity.x * restitution;

		}

	}

	void draw(sf::RenderWindow& window) {

		sf::RectangleShape rect(sf::Vector2f(width, height));
		rect.setOrigin(width / 2, height / 2);
		rect.setPosition(position.x, position.y);
		rect.setRotation(angle * 180.0f / 3.14159f);
		rect.setFillColor(sf::Color::Cyan);
		rect.setOutlineThickness(2);
		rect.setOutlineColor(sf::Color::White);

		window.draw(rect);

		// 방향 표시
		Vec2 direction(std::cos(angle), std::sin(angle));
		Vec2 lineEnd = position + direction * (width * 0.5f);

		sf::Vertex line[] = {

			sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color::Red),
			sf::Vertex(sf::Vector2f(lineEnd.x, lineEnd.y), sf::Color::Yellow)

		};

		window.draw(line, 2, sf::Lines);

	}

};