#pragma once

#include "Vec2.h"
#include "Particle.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Spring {

public:
	int p1Index;
	int p2Index;
	float restLength;
	float stiffness; // k
	float damping; // 감쇠 계수

	Spring(int p1Idx, int p2Idx, float restLength, float stiffness, float damping = 0.5f)
		: p1Index(p1Idx), p2Index(p2Idx), restLength(restLength), stiffness(stiffness), damping(damping)
	{}

	// 스프링 힘 작용(F = -k * x)
	void applyForce(std::vector<Particle>& particles) {

		Particle& p1 = particles[p1Index];
		Particle& p2 = particles[p2Index];

		Vec2 diff = p2.position - p1.position;
		float currentLength = diff.length();

		if (currentLength == 0) return;

		// 변위
		float extension = currentLength - restLength;

		// 방향
		Vec2 direction = diff / currentLength;

		// F = -k * x
		float forceMagnitude = -stiffness * extension;

		// 감쇠력 추가 (상대 속도 비례)
		Vec2 relativeVel = p2.velocity - p1.velocity;
		float dampingForce = -damping * relativeVel.dot(direction);

		// 총 힘
		float totalForce = forceMagnitude + dampingForce;
		Vec2 force = direction * totalForce;

		// 양쪽 파티클에 반대 방향 힘 적용
		p1.applyForce(force * -1.0f);
		p2.applyForce(force);

	}

	void draw(sf::RenderWindow& window, const std::vector<Particle>& particles) {

		const Particle& p1 = particles[p1Index];
		const Particle& p2 = particles[p2Index];

		Vec2 diff = p2.position - p1.position;
		float currentLength = diff.length();

		if (currentLength < 0.1f) return;

		float extension = currentLength - restLength;
		Vec2 direction = diff / currentLength;
		Vec2 perpendicular(-direction.y, direction.x);

		sf::Color color;
		float extRatio = extension / restLength;

		if (std::abs(extRatio) < 0.05f) {

			color = sf::Color(255, 255, 0);

		}
		else if (extension > 0) {

			int intensity = std::min(200, (int)(std::abs(extRatio) * 400));
			color = sf::Color(255, 255 - intensity, 100);

		}
		else {

			int intensity = std::min(200, (int)(std::abs(extRatio) * 400));
			color = sf::Color(100, 255 - intensity, 255);

		}

		int coils = 8;
		float coilRadius = 8.0f;
		int pointsPerCoil = 8;

		std::vector<sf::Vertex> vertices;

		for (int i = 0; i <= coils * pointsPerCoil; i++) {

			float t = (float)i / (coils * pointsPerCoil);

			Vec2 center = p1.position + diff * t;

			float angle = t * coils * 2 * 3.14159f;
			float offset = std::sin(angle) * coilRadius;
			 
			Vec2 pos = center + perpendicular * offset;
			vertices.push_back(sf::Vertex(sf::Vector2f(pos.x, pos.y), color));

		}

		window.draw(&vertices[0], vertices.size(), sf::LineStrip);

		sf::Vertex startLine[] = {
			sf::Vertex(sf::Vector2f(p1.position.x, p1.position.y), color),
			sf::Vertex(sf::Vector2f(vertices[0].position.x, vertices[0].position.y), color)
		};
		window.draw(startLine, 2, sf::Lines);

		sf::Vertex endLine[] = {
			sf::Vertex(sf::Vector2f(vertices.back().position.x, vertices.back().position.y), color),
			sf::Vertex(sf::Vector2f(p2.position.x, p2.position.y), color)
		};
		window.draw(endLine, 2, sf::Lines);

	}

};	