#pragma once
#include "Vec2.h"
#include "Particle.h"

class DistanceConstraint {

public:
	Particle* p1;
	Particle* p2;
	float restLength; // 원래 거리
	float stiffness; // 강성

	DistanceConstraint(Particle* p1, Particle* p2, float stiffness = 1.0f)
		:p1(p1), p2(p2), stiffness(stiffness)
	{

		// 현재 거리를 원래 거리로 
		Vec2 diff = p2->position - p1->position;
		restLength = diff.length();

	}

	DistanceConstraint(Particle* p1, Particle* p2, float restLength, float stiffness)
		: p1(p1), p2(p2), restLength(restLength), stiffness(stiffness) 
	{}

	// 제약 해결
	void solve() {

		Vec2 diff = p2->position - p1->position;
		float currentLength = diff.length();

		if (currentLength == 0) return; // 같은 위치 무시

		// 거리 차이
		float delta = currentLength - restLength;

		// 정규화된 방향
		Vec2 direction = diff / currentLength;

		// 보정량 계산
		float correction = delta * stiffness * 0.5f;

		// 질량 비율로 보정(클수록 움직임 하락)
		float totalMass = p1->mass + p2->mass;
		float m1Ratio = p2->mass / totalMass; // p1이 움직일 비율
		float m2Ratio = p1->mass / totalMass; // p2가 움직일 비율

		// 위치 보정
		p1->position += direction * correction * m1Ratio;
		p2->position -= direction * correction * m2Ratio;

	}

	// 그리기
	void draw(sf::RenderWindow& window) {

		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(p1->position.x, p1->position.y), sf::Color::White),
			sf::Vertex(sf::Vector2f(p2->position.x, p2->position.y), sf::Color::White)
		};
		window.draw(line, 2, sf::Lines);
	}

};
