#pragma once

#include "Vec2.h"
#include "Particle.h"
#include <SFML/Graphics.hpp>

class Spring {

public:
	Particle* p1;
	Particle* p2;
	float restLength;
	float stiffness; // k
	float damping; // 감쇠 계수

	Spring(Particle* p1, Particle* p2, float stiffness, float damping = 0.1f)
		: p1(p1), p2(p2), stiffness(stiffness), damping(damping)
	{
		Vec2 diff = p2->position - p1->position;
		restLength = diff.length();
	}

	// 스프링 힘 작용( F = -k * x)
	void applyForce() {

		Vec2 diff = p2->position - p1->position;
		float currentLength = diff.length();

		if (currentLength == 0) return;

		// 변위
		float extension = currentLength - restLength;

		// 방향
		Vec2 direction = diff / currentLength;

		// F = -k * x
		float forceMagnitude = -stiffness * extension;

		// 감쇠력 추가 (상대 속도 비례)
		Vec2 relativeVel = p2->velocity - p1->velocity;
		float dampingForce = -damping * relativeVel.dot(direction);

		// 총 힘
		float totalForce = forceMagnitude + dampingForce;
		Vec2 force = direction * totalForce;

		// 양쪽 파티클에 반대 방향 힘 적용
		p1->applyForce(force * -1.0f);
		p2->applyForce(force);

	}

	void draw(sf::RenderWindow& window) {

		Vec2 diff = p2->position - p1->position;
		float currentLength = diff.length();

		// 늘어나면 빨강, 줄어들면 파랑
		float extension = currentLength - restLength;
		sf::Color color;

		if (extension > 0) {

			// 늘어남
			int intensity = std::min(255, (int)(std::abs(extension) * 2));
			color = sf::Color(255, 255 - intensity, 255 - intensity);

		}
		else {

			// 줄어듦
			int intensity = std::min(255, (int)(std::abs(extension) * 2));
			color = sf::Color(255 - intensity, 255 - intensity, 255);

		}

		// 물결선
		int segments = 10;
		Vec2 direction = diff / currentLength;
		Vec2 perpendicular(-direction.y, direction.x);

		for (int i = 0; i < segments; i++) {

			float t1 = (float)i / segments;
			float t2 = (float)(i + 1) / segments;

			Vec2 pos1 = p1->position + diff * t1;
			Vec2 pos2 = p2->position + diff * t2;

			// 물결 효과
			float wave1 = std::sin(t1 * 3.14159f * 4) * 5.0f;
			float wave2 = std::sin(t2 * 3.14159f * 4) * 5.0f;

			pos1 += perpendicular * wave1;
			pos2 += perpendicular * wave2;

			sf::Vertex line[] = {

				sf::Vertex(sf::Vector2f(pos1.x, pos1.y), color),
				sf::Vertex(sf::Vector2f(pos2.x, pos2.y), color)

			};

			window.draw(line, 2, sf::Lines);

		}

	}

};