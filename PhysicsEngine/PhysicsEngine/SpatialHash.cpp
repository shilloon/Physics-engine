//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <vector>
//#include <set>
//#include "Vec2.h"
//#include "Polygon.h"
//#include "SpatialHash.h"
//
//// === 정다각형 생성 ===
//std::vector<Vec2> createRegularPolygon(int sides, float radius) {
//
//	std::vector<Vec2> vertices;
//
//	for (int i = 0; i < sides; i++) {
//
//		float angle = (2 * 3.14159f * i) / sides;
//		float x = std::cos(angle) * radius;
//		float y = std::sin(angle) * radius;
//		vertices.push_back(Vec2(x, y));
//
//	}
//
//	return vertices;
//
//}
//
//// === 박스 -> 다각형 ===
//std::vector<Vec2> createBox(float width, float height) {
//
//	std::vector<Vec2> vertices;
//	float halfW = width / 2;
//	float halfH = height / 2;
//
//	vertices.push_back(Vec2(-halfW, -halfH));
//	vertices.push_back(Vec2(halfW, -halfH));
//	vertices.push_back(Vec2(halfW, halfH));
//	vertices.push_back(Vec2(-halfW, halfH));
//
//	return vertices;
//
//}
//
//// === 삼각형 ===
//std::vector<Vec2> createTriangle(float size) {
//
//	std::vector<Vec2> vertices;
//	float height = size * std::sqrt(3.0f) / 2;
//
//	vertices.push_back(Vec2(0, -height * 2 / 3)); // 위
//	vertices.push_back(Vec2(size / 2, height / 3)); // 오른쪽 아래
//	vertices.push_back(Vec2(-size / 2, height / 3)); // 왼쪽 아래
//
//	return vertices;
//
//}
//
//// === 충돌 정보 ===
//struct CollisionInfo {
//
//	bool collided;
//	Vec2 normal;
//	float depth;
//
//	CollisionInfo() : collided(false), depth(0) {}
//
//};
//
//// === 투영 ===
//void projectVertices(const std::vector<Vec2>& vertices, Vec2 axis, float& min, float& max) {
//
//	min = max = vertices[0].dot(axis);
//
//	for (size_t i = 1; i < vertices.size(); i++) {
//
//		float projection = vertices[i].dot(axis);
//		if (projection < min) min = projection;
//		if (projection > max) max = projection;
//
//	}
//
//}
//
//// === SAT 충돌 감지 ===
//CollisionInfo checkPolygonCollision(const Polygon& a, const Polygon& b) {
//
//	CollisionInfo info;
//
//	std::vector<Vec2> verticesA = a.getWorldVertices();
//	std::vector<Vec2> verticesB = b.getWorldVertices();
//
//	std::vector<Vec2> axesA = a.getAxes();
//	std::vector<Vec2> axesB = b.getAxes();
//
//	float minOverlap = FLT_MAX;
//	Vec2 minAxis;
//
//	// 모든 축 검사
//	std::vector<Vec2> allAxes;
//	allAxes.insert(allAxes.end(), axesA.begin(), axesA.end());
//	allAxes.insert(allAxes.end(), axesB.begin(), axesB.end());
//
//	for (const Vec2& axis : allAxes) {
//
//		float minA, maxA, minB, maxB;
//
//		projectVertices(verticesA, axis, minA, maxA);
//		projectVertices(verticesB, axis, minB, maxB);
//
//		// 분리축 발견 -> 충돌 없음
//		if (maxA < minB || maxB < minA) {
//
//			info.collided = false;
//			return info;
//
//		}
//
//		// 겹침 계산
//		float overlap = std::min(maxA, maxB) - std::max(minA, minB);
//
//		if (overlap < minOverlap) {
//
//			minOverlap = overlap;
//			minAxis = axis;
//
//			// 법선 방향 조정
//			Vec2 centerDiff = b.position - a.position;
//			if (centerDiff.dot(axis) < 0) {
//
//				minAxis = minAxis * -1.0f;
//
//			}
//
//		}
//
//	}
//
//	// 모든 축에서 겹침 -> 충돌
//	info.collided = true;
//	info.normal = minAxis.normalize();
//	info.depth = minOverlap;
//
//	return info;
//
//}
//
//// === 충돌 반응 ===
//void resolvePolygonCollision(Polygon& a, Polygon& b, const CollisionInfo& info) {
//
//	// 위치 보정
//	float totalMass = a.mass + b.mass;
//	float percent = 0.3f;
//	Vec2 correction = info.normal * info.depth * percent;
//
//	a.position -= correction * (b.mass / totalMass);
//	b.position += correction * (a.mass / totalMass);
//
//	// 충돌점
//	Vec2 contactPoint = (a.position + b.position) * 0.5f;
//
//	Vec2 rA = contactPoint - a.position;
//	Vec2 rB = contactPoint - b.position;
//
//	// 충돌점 속도
//	Vec2 velA = a.velocity + Vec2(-rA.y * a.angularVelocity, rA.x * a.angularVelocity);
//	Vec2 velB = b.velocity + Vec2(-rB.y * b.angularVelocity, rB.x * b.angularVelocity);
//	Vec2 relativeVel = velB - velA;
//
//	float velAlongNormal = relativeVel.dot(info.normal);
//
//	if (velAlongNormal > 0) return;
//
//	// Impulse
//	float e = (a.restitution + b.restitution) / 2;
//
//	float rACrossN = rA.cross(info.normal);
//	float rBCrossN = rB.cross(info.normal);
//
//	float denominator = 1 / a.mass + 1 / b.mass + (rACrossN * rACrossN) / a.inertia + (rBCrossN * rBCrossN) / b.inertia;
//
//	float j = -(1 + e) * velAlongNormal / denominator;
//
//	Vec2 impulse = info.normal * j;
//
//	a.velocity -= impulse / a.mass;
//	b.velocity += impulse / b.mass;
//
//	a.angularVelocity -= rA.cross(impulse) / a.inertia;
//	b.angularVelocity += rB.cross(impulse) / b.inertia;
//
//	// === 마찰 Impulse ===
//
//	velA = a.velocity + Vec2(-rA.y * a.angularVelocity, rA.x * a.angularVelocity);
//	velB = b.velocity + Vec2(-rB.y * b.angularVelocity, rB.x * b.angularVelocity);
//	relativeVel = velB - velA;
//
//	Vec2 tangent = relativeVel - info.normal * relativeVel.dot(info.normal);
//	float tangentLength = tangent.length();
//
//	if (tangentLength > 0.0001f) {
//
//		// 정규화
//		tangent = tangent / tangentLength;
//
//		// 접선 방향 상대 속도
//		float velAlongTangent = relativeVel.dot(tangent);
//
//		// 마찰 계수
//		float mu = (a.friction + b.friction) / 2;
//
//		// 접선 Impulse 계산
//		float rACrossT = rA.cross(tangent);
//		float rBCrossT = rB.cross(tangent);
//
//		float tangentDenominator = 1 / a.mass + 1 / b.mass + (rACrossT * rACrossT) / a.inertia + (rBCrossT * rBCrossT) / b.inertia;
//
//		float jt = -velAlongTangent / tangentDenominator;
//
//		// 쿨롱 마찰 법칙
//		Vec2 frictionImpulse;
//		if (std::abs(jt) < j * mu) {
//
//			//  정지 마찰(static friction)
//			frictionImpulse = tangent * jt;
//
//		}
//		else {
//
//			// 운동 마찰(Kinetic friction)
//			frictionImpulse = tangent * (-j * mu);
//
//		}
//
//		// 마찰 Impulse 적용
//		a.velocity -= frictionImpulse / a.mass;
//		b.velocity += frictionImpulse / b.mass;
//
//		a.angularVelocity -= rA.cross(frictionImpulse) / a.inertia;
//		b.angularVelocity += rB.cross(frictionImpulse) / b.inertia;
//
//	}
//
//}
//
//int main() {
//	
//	const float WIDTH = 800;
//	const float HEIGHT = 600;
//
//	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - optimize");
//	window.setFramerateLimit(60);
//
//	std::vector<Polygon> polygons;
//	Vec2 gravity(0, 9.8f * 100);
//
//	// === 객체 생성 ===
//	srand(time(0));
//
//	for (int i = 0; i < 80; i++) {
//
//		float x = 50 + rand() % 700;
//		float y = 50 + rand() % 200;
//
//		int shapeType = rand() % 4;
//		std::vector<Vec2> vertices;
//
//		switch (shapeType) {
//
//		case 0: vertices = createTriangle(20 + rand() % 30); break;
//		case 1: vertices = createBox(20 + rand() % 40, 20 + rand() % 40); break;
//		case 2: vertices = createRegularPolygon(5, 15 + rand() % 25); break;
//		case 3: vertices = createRegularPolygon(6, 15 + rand() % 20); break;
//
//		}
//
//		float mass = 0.5f + (rand() % 20) / 10.0f;
//		float restitution = 0.3f + (rand() % 50) / 100.0f;
//		float friction = 0.2f + (rand() % 60) / 100.0f;
//
//		polygons.push_back(Polygon(Vec2(x, y), vertices, mass, restitution, friction));
//
//		// 랜덤 초기 속도
//		polygons.back().velocity = Vec2(rand() % 100 - 50, rand() % 100 - 50);
//		polygons.back().angularVelocity = (rand() % 60 - 30) / 10.0f;
//
//	}
//
//	// === Spatial Mesh 생성
//	SpatialHash<Polygon> spatialHash(100.0f);
//
//	sf::Clock clock;
//
//	Polygon* draggedPolygon = nullptr;
//	Vec2 dragStartPos;
//	Vec2 dragOffset;
//
//	// 성능 측정
//	int totalChecks = 0;
//	bool showGrid = true;
//	bool useOptimization = true;
//
//	sf::Font font;
//
//	while (window.isOpen()) {
//
//		sf::Event event;
//		while (window.pollEvent(event)) {
//
//			if (event.type == sf::Event::Closed) {
//
//				window.close();
//
//			}
//
//			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//
//				Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//
//				for (auto& poly : polygons) {
//
//					if (poly.contains(mousePos)) {
//
//						draggedPolygon = &poly;
//						dragStartPos = mousePos;
//						dragOffset = poly.position - mousePos;
//						poly.velocity = Vec2(0, 0);
//						poly.angularVelocity = 0;
//						break;
//
//					}
//
//				}
//
//			}
//
//			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
//
//				if (draggedPolygon != nullptr) {
//
//					Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//					Vec2 dragVector = mousePos - dragStartPos;
//
//					draggedPolygon->velocity = dragVector * 8.0f;
//
//					Vec2 torqueArm = dragStartPos - draggedPolygon->position;
//					float torque = torqueArm.cross(draggedPolygon->velocity) * 0.02f;
//					draggedPolygon->angularVelocity = torque / draggedPolygon->inertia;
//
//					draggedPolygon = nullptr;
//
//				}
//
//			}
//
//			if (event.type == sf::Event::KeyPressed) {
//
//				if (event.key.code == sf::Keyboard::R) {
//
//					draggedPolygon = nullptr;
//					for (auto& poly : polygons) {
//
//						poly.velocity = Vec2(rand() % 100 - 50, rand() % 100 - 50);
//						poly.angularVelocity = (rand() % 60 - 30) / 10.0f;
//						poly.angle = 0;
//						poly.position = Vec2(50 + rand() % 700, 50 + rand() % 200);
//
//					}
//
//				}
//
//				if (event.key.code == sf::Keyboard::Space) {
//
//					for (auto& poly : polygons) {
//
//						poly.velocity = Vec2(rand() % 300 - 150, rand() % 300 - 150);
//						poly.angularVelocity = (rand() % 100 - 50) / 10.0f;
//
//					}
//
//				}
//
//				if (event.key.code == sf::Keyboard::G) {
//
//					showGrid = !showGrid;
//					std::cout << "Grid: " << (showGrid ? "ON" : "OFF") << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::O) {
//
//					useOptimization = !useOptimization;
//					std::cout << "Optimization: " << (useOptimization ? "ON" : "OFF") << std::endl;
//
//				}
//				
//				if (event.key.code == sf::Keyboard::F) {
//
//					// 모든 마찰 계수 변경
//					static bool highFriction = false;
//					highFriction = !highFriction;
//
//					for (auto& poly : polygons) {
//
//						poly.friction = highFriction ? 0.9f : 0.3f;
//
//					}
//
//					std::cout << "Friction: " << (highFriction ? "0.9" : "0.3") << std::endl;
//
//				}
//
//			}
//
//		}
//
//		if (draggedPolygon != nullptr) {
//
//			sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
//			Vec2 mousePos(mousePixel.x, mousePixel.y);
//			draggedPolygon->position = mousePos + dragOffset;
//
//		}
//
//		float dt = clock.restart().asSeconds();
//
//		// === physics update ===
//		for (auto& poly : polygons) {
//
//			if (draggedPolygon != &poly) {
//
//				poly.applyForce(gravity* poly.mass);
//				poly.update(dt);
//				poly.checkBounds(WIDTH, HEIGHT);
//
//			}
//
//		}
//
//		// === 충돌 처리 ===
//		totalChecks = 0;
//
//		if (useOptimization) {
//			
//			// Spatial Hash
//			auto pairs = spatialHash.getPotentialCollisions(
//				polygons,
//				[](const Polygon& p) {return p.position; },
//				[](const Polygon& p) {
//
//					float maxDist = 0;
//					for (const Vec2& v : p.localVertices) {
//						maxDist = std::max(maxDist, v.length());
//					}
//					return maxDist;
//				}
//			);
//
//			totalChecks = pairs.size();
//
//			for (auto& pair : pairs) {
//
//				if (draggedPolygon == pair.first || draggedPolygon == pair.second) {
//					continue;
//				}
//
//				CollisionInfo info = checkPolygonCollision(*pair.first, *pair.second);
//				if (info.collided) {
//
//					resolvePolygonCollision(*pair.first, *pair.second, info);
//
//				}
//
//			}
//
//		}
//		
//		else {
//
//			for (size_t i = 0; i < polygons.size(); i++) {
//
//				for (size_t j = i + 1; j < polygons.size(); j++) {
//
//					if (draggedPolygon == &polygons[i] || draggedPolygon == &polygons[j]) {
//
//						continue;
//
//					}
//
//					totalChecks++;
//
//					CollisionInfo info = checkPolygonCollision(polygons[i], polygons[j]);
//					if (info.collided) {
//
//						resolvePolygonCollision(polygons[i], polygons[j], info);
//
//					}
//
//				}
//
//			}
//
//		}
//
//		// === rendering ===
//		window.clear(sf::Color::Black);
//
//		if (showGrid) {
//
//			spatialHash.drawGrid(window, WIDTH, HEIGHT);
//
//		}
//
//		for (const auto& poly : polygons) {
//
//			poly.draw(window);
//
//		}
//
//		if (draggedPolygon != nullptr) {
//
//			sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
//			Vec2 mousePos(mousePixel.x, mousePixel.y);
//
//			sf::Vertex line[] = {
//				sf::Vertex(sf::Vector2f(dragStartPos.x, dragStartPos.y), sf::Color::Yellow),
//				sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y), sf::Color::Red)
//			};
//
//			window.draw(line, 2, sf::Lines);
//
//		}
//
//		// 성능 정보 출력
//		static int frameCount = 0;
//		if (++frameCount % 60 == 0) {
//
//			std::cout << "Collision checks: " << totalChecks
//				<< " | objects: " << polygons.size()
//				<< " | Max possible: " << (polygons.size() * (polygons.size() - 1) / 2)
//				<< std::endl;
//
//		}
//
//		window.display();
//
//	}
//
//	return 0;
//
//}