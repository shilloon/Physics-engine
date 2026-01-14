#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Polygon {

public:
	Vec2 position;
	Vec2 velocity;
	Vec2 acceleration;

	float angle;
	float angularVelocity;
	float angularAcceleration;

	float mass;
	float inertia;
	float restitution;

	std::vector<Vec2> localVertices; // 로컬 좌표계 꼭짓점

	// === Constructor ===
	Polygon(Vec2 position, const std::vector<Vec2>& vertices, float mass, float restitution)
		:position(position), mass(mass), restitution(restitution), velocity(0, 0), acceleration(0, 0), angle(0), angularVelocity(0), angularAcceleration(0)
	{

		// 로컬 좌표계로 변환
		Vec2 centroid = calculateCentroid(vertices);
		localVertices.resize(vertices.size());

		for (size_t i = 0; i < vertices.size(); i++) {

			localVertices[i] = vertices[i] - centroid;

		}

		// 관성 모멘트 계산
		inertia = calculateInertia();

	}

	// === 중심점 계산 ===
	static Vec2 calculateCentroid(const std::vector<Vec2>& vertices) {

		Vec2 centroid(0, 0);
		for (const Vec2& v : vertices) {

			centroid += v;

		}
		return centroid / (float)vertices.size();

	}

	// === 관성 모멘트 계산 ===
	float calculateInertia() {

		float sum = 0;
		for (const Vec2& v : localVertices) {

			sum += v.lengthSq();

		}
		return (mass / localVertices.size()) * sum;
	}

	// === 월드 좌표계 꼭짓점 ===
	std::vector<Vec2> getWorldVertices() const {

		std::vector<Vec2> worldVertices(localVertices.size());

		float cos_a = std::cos(angle);
		float sin_a = std::sin(angle);

		for (size_t i = 0; i < localVertices.size(); i++) {

			// 회전
			float rotatedX = localVertices[i].x * cos_a - localVertices[i].y * sin_a;
			float rotatedY = localVertices[i].x * sin_a + localVertices[i].y * cos_a;

			// 이동
			worldVertices[i] = Vec2(rotatedX, rotatedY) + position;

		}

		return worldVertices;

	}

	// === 축(법선) 구하기 ===
	std::vector<Vec2> getAxes() const {

		std::vector<Vec2> worldVertices = getWorldVertices();
		std::vector<Vec2> axes;

		for (size_t i = 0; i < worldVertices.size(); i++) {

			Vec2 p1 = worldVertices[i];
			Vec2 p2 = worldVertices[(i + 1) % worldVertices.size()];

			Vec2 edge = p2 - p1;
			Vec2 normal(-edge.y, edge.x); // 수직
			axes.push_back(normal.normalize());

		}

		return axes;

	}

	// === 힘 적용 ===
	void applyForce(Vec2 force) {

		acceleration += force / mass;

	}

	void applyTorque(float torque) {

		angularAcceleration += torque / inertia;

	}

	void applyForceAtPoint(Vec2 force, Vec2 point) {

		applyForce(force);
		Vec2 r = point - position;
		float torque = r.cross(force);
		applyTorque(torque);

	}

	// === Update ===
	void update(float dt) {

		velocity += acceleration * dt;
		position += velocity * dt;

		angularVelocity += angularAcceleration * dt;
		angle += angularVelocity * dt;

		acceleration = Vec2(0, 0);
		angularAcceleration = 0;

	}

	// === Check Horizon ===
	void checkBounds(float width, float height) {

		float margin = 50;

		if (position.x < margin) {

			position.x = margin;
			velocity.x *= -restitution;

		}

		if (position.x > width - margin) {

			position.x = width - margin;
			velocity.x *= -restitution;

		}

		if (position.y < margin) {

			position.y = margin;
			velocity.y *= -restitution;

		}

		if (position.y > height - margin) {

			position.y = height - margin;
			velocity.y *= -restitution;
			angularVelocity *= 0.9f;

		}

	}

	// === 점 포함 체크 ===
	bool contains(Vec2 point) const {

		std::vector<Vec2> worldVerts = getWorldVertices();

		// Ray Casting algorithm
		int intersections = 0;
		for (size_t i = 0; i < worldVerts.size(); i++) {

			Vec2 p1 = worldVerts[i];
			Vec2 p2 = worldVerts[(i + 1) % worldVerts.size()];

			if ((p1.y > point.y) != (p2.y > point.y)) {

				float xIntersect = (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x;
				if (point.x < xIntersect) {

					intersections++;

				}

			}

		}

		return (intersections % 2) == 1;

	}

	// === 그리기 ===
	void draw(sf::RenderWindow& window) const {

		std::vector<Vec2> worldVertices = getWorldVertices();

		// 다각형 채우기
		sf::ConvexShape shape(worldVertices.size());
		for (size_t i = 0; i < worldVertices.size(); i++) {

			shape.setPoint(i, sf::Vector2f(worldVertices[i].x, worldVertices[i].y));

		}
		shape.setFillColor(sf::Color(100, 200, 100, 200));
		shape.setOutlineColor(sf::Color::White);
		shape.setOutlineThickness(2);
		window.draw(shape);

		// 방향 표시 (첫 꼭짓점으로)
		if (!worldVertices.empty()) {

			sf::Vertex line[] = {

				sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color::Red),
				sf::Vertex(sf::Vector2f(worldVertices[0].x, worldVertices[0].y), sf::Color::Red)

			};
			window.draw(line, 2, sf::Lines);

		}

		// 중심점
		sf::CircleShape center(3);
		center.setFillColor(sf::Color::Yellow);
		center.setPosition(position.x - 3, position.y - 3);
		window.draw(center);

	}
	


};