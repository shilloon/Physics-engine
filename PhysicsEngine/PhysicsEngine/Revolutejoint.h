#pragma once

#include "Vec2.h"
#include "Polygon.h"
#include <SFML/Graphics.hpp>

class RevoluteJoint{

public:

	Polygon* bodyA; // 연결된 물체 A
	Polygon* bodyB; // 연결된 물체 B

	Vec2 localAnchorA; // A의 local 좌표계에서 앵커 위치
	Vec2 localAnchorB; // B의 local 좌표계에서 앵커 위치

	// === 선택적 각도 제한 ===
	bool enableLimit; // 각도 제한 on / off
	float lowerAngle; // 최소 각도
	float upperAngle; // 최대 각도

	// === 선택적 모터 ===
	bool enableMotor; // 모터 on / off
	float motorSpeed; // 목표 회전 속도
	float maxMotorTorque; // 최대 토크

	// === Constructor ===
	RevoluteJoint(Polygon* a, Polygon* b, Vec2 worldAnchor, bool enableLimit = false, float lower = 0, float upper = 0, bool enableMotor = false, float speed = 0, float maxTorque = 0)
		: bodyA(a), bodyB(b), enableLimit(enableLimit), lowerAngle(lower), upperAngle(upper), enableMotor(enableMotor), motorSpeed(speed), maxMotorTorque(maxTorque)
	{

		// 월드 좌표를 각 물체의 로컬 좌표로 변환
		localAnchorA = worldToLocal(a, worldAnchor);
		localAnchorB = worldToLocal(b, worldAnchor);

	}

	// === 월드 좌표 -> 로컬 좌표 변환 ===
	static Vec2 worldToLocal(Polygon* body, Vec2 worldPoint) {

		Vec2 relative = worldPoint - body->position;

		// 역회전 적용
		float cos_a = std::cos(-body->angle);
		float sin_a = std::sin(-body->angle);

		float localX = relative.x * cos_a - relative.y * sin_a;
		float localY = relative.x * sin_a + relative.y * cos_a;

		return Vec2(localX, localY);

	}

	// === 로컬 좌표 -> 월드 좌표 변환 ===
	static Vec2 localToWorld(Polygon* body, Vec2 localPoint) {

		// 회전 적용
		float cos_a = std::cos(body->angle);
		float sin_a = std::sin(body->angle);

		float rotX = localPoint.x * cos_a - localPoint.y * sin_a;
		float rotY = localPoint.x * sin_a + localPoint.y * cos_a;

		return Vec2(rotX, rotY) + body->position;

	}

	// === 제약 해결 ===
	void solve() {

		// 앵커 포인트 계산
		Vec2 anchorA = localToWorld(bodyA, localAnchorA);
		Vec2 anchorB = localToWorld(bodyB, localAnchorB);

		// 오차 계산
		Vec2 error = anchorB - anchorA;

		if (error.lengthSq() < 0.0001f) return;

		// 위치 보정
		float percent = 0.5f;
		Vec2 correction = error * percent;

		// 무한 질량 체크
		bool aIsStatic = std::isinf(bodyA->mass);
		bool bIsStatic = std::isinf(bodyB->mass);

		// 둘다 움직이는 물체
		if (!aIsStatic && !bIsStatic) {

			float totalMass = bodyA->mass + bodyB->mass;

			// 위치 보정
			bodyA->position += correction * (bodyB->mass / totalMass);
			bodyB->position -= correction * (bodyA->mass / totalMass);
			
			// 회전 보정
			Vec2 rA = anchorA - bodyA->position;
			Vec2 rB = anchorB - bodyB->position;

			float angularErrorA = rA.cross(correction) / bodyA->inertia;
			float angularErrorB = rB.cross(correction) / bodyB->inertia;

			bodyA->angle += angularErrorA * 0.1f;
			bodyB->angle -= angularErrorB * 0.1f;

		}
		// A만 고정
		else if (aIsStatic) {

			bodyB->position -= correction;

			Vec2 rB = anchorB - bodyB->position;
			float angularErrorB = rB.cross(correction) / bodyB->inertia;
			bodyB->angle -= angularErrorB * 0.1f;

		}
		// B만 고정
		else {

			bodyA->position += correction;

			Vec2 rA = anchorA - bodyA->position;
			float angularErrorA = rA.cross(correction) / bodyA->inertia;
			bodyA->angle += angularErrorA * 0.1f;

		}

		// 각도 제한 적용
		if (enableLimit) {

			float relativeAngle = bodyB->angle - bodyA->angle;

			// -π ~ π 범위로 정규화
			while (relativeAngle > 3.14159f) relativeAngle -= 2 * 3.14159f;
			while (relativeAngle < -3.14159f) relativeAngle += 2 * 3.14159f;

			if (relativeAngle < lowerAngle) {

				float angleError = lowerAngle - relativeAngle;
				if (!aIsStatic) bodyA->angle += angleError * 0.5f;
				if (!bIsStatic) bodyB->angle -= angleError * 0.5f;

			}

		}
		
		// 모터 적용
		if (enableMotor) {

			// 현재 상대 각속도
			float relativeAngVel = bodyB->angularVelocity - bodyA->angularVelocity;

			// 목표 속도와의 차이
			float velError = motorSpeed - relativeAngVel;

			// Torque 계산
			float torque = velError * 0.01f;
			torque = std::max(-maxMotorTorque, std::min(torque, maxMotorTorque));

			// Torque 적용
			if (!aIsStatic) bodyA->angularVelocity -= torque / bodyA->inertia;
			if (!bIsStatic) bodyB->angularVelocity += torque / bodyB->inertia;

		}


	}
	
	// === 시각화 ===
	void draw(sf::RenderWindow& window) const {

		Vec2 anchorA = localToWorld(bodyA, localAnchorA);
		Vec2 anchorB = localToWorld(bodyB, localAnchorB);

		// 연결선 (A -> anchor)
		sf::Vertex lineA[] = {
			sf::Vertex(sf::Vector2f(bodyA->position.x, bodyA->position.y), sf::Color::Cyan),
			sf::Vertex(sf::Vector2f(anchorA.x, anchorA.y), sf::Color::Yellow)
		};
		window.draw(lineA, 2, sf::Lines);

		// 연결선 (B -> anchor)
		sf::Vertex lineB[] = {
			sf::Vertex(sf::Vector2f(bodyB->position.x, bodyB->position.y), sf::Color::Magenta),
			sf::Vertex(sf::Vector2f(anchorB.x, anchorB.y), sf::Color::Yellow)
		};
		window.draw(lineB, 2, sf::Lines);

		// Joint 중심점
		sf::CircleShape jointCircle(5); 
		jointCircle.setFillColor(sf::Color::Yellow);
		jointCircle.setPosition(anchorA.x - 5, anchorA.y - 5);
		window.draw(jointCircle);

		// 모터 표시
		if (enableMotor) {

			sf::CircleShape motorIndicator(8);
			motorIndicator.setFillColor(sf::Color(255, 100, 0));
			motorIndicator.setPosition(anchorA.x - 8, anchorA.y - 8);
			window.draw(motorIndicator);

		}

	}

};