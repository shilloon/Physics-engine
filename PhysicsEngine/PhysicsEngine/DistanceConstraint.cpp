//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <vector>
//#include "Vec2.h"
//#include "Particle.h"
//#include "DistanceConstraint.h"
//
//int main() {
//
//	const float WIDTH = 800;
//	const float HEIGHT = 600;
//
//	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - Constraint");
//	window.setFramerateLimit(60);
//
//	std::vector<Particle> particles;
//	std::vector<DistanceConstraint> constraints;
//
//	Vec2 gravity(0, 9.8 * 100);
//
//	// === 두 공을 막대로 연결 ===
//	particles.push_back(Particle(Vec2(300, 200), 1.0f, 15.0f, 0.8f));
//	particles.push_back(Particle(Vec2(500, 200), 1.0f, 15.0f, 0.8f));
//
//	// 초기 속도
//	particles[0].velocity = Vec2(0, 100);
//
//	// === 고정점에 매달린 추 ===
//	particles.push_back(Particle(Vec2(400, 100), 1000.0f, 10.0f, 0.8f)); //  무거운 추
//	particles.push_back(Particle(Vec2(400, 250), 1.0f, 20.0f, 0.8f)); // 추
//
//	constraints.push_back(DistanceConstraint(&particles[0], &particles[1], 1.0f));
//	constraints.push_back(DistanceConstraint(&particles[2], &particles[3], 1.0f));
//
//	sf::Clock clock;
//
//	// interaction
//	Particle* draggedParticle = nullptr;
//	Vec2 mouseOffset;
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
//			// === 마우스로 파티클 잡기 ===
//			if (event.type == sf::Event::MouseButtonPressed) {
//
//				if (event.mouseButton.button == sf::Mouse::Left) {
//
//					Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//
//					// 가장 가까운 파티클 찾기
//					for (auto& p : particles) {
//
//						Vec2 diff = p.position - mousePos;
//						if (diff.length() < p.radius + 10) {
//
//							draggedParticle = &p;
//							mouseOffset = p.position - mousePos;
//							break;
//
//						}
//
//					}
//
//				}
//
//			}
//
//			if (event.type == sf::Event::MouseButtonReleased) {
//
//				draggedParticle = nullptr;
//
//			}
//
//			// Reset
//			if (event.type == sf::Event::KeyPressed) {
//
//				if (event.key.code == sf::Keyboard::R) {
//
//					particles.clear();
//					constraints.clear();
//
//					// === 두 공을 막대로 연결 ===
//					particles.push_back(Particle(Vec2(300, 200), 1.0f, 15.0f, 0.8f));
//					particles.push_back(Particle(Vec2(500, 200), 1.0f, 15.0f, 0.8f));
//
//					constraints.push_back(DistanceConstraint(&particles[0], &particles[1], 1.0f));
//
//					// 초기 속도
//					particles[0].velocity = Vec2(0, 100);
//
//					// === 고정점에 매달린 추 ===
//					particles.push_back(Particle(Vec2(400, 100), 1000.0f, 10.0f, 0.8f)); //  무거운 추
//					particles.push_back(Particle(Vec2(400, 250), 1.0f, 20.0f, 0.8f)); // 추
//
//					constraints.push_back(DistanceConstraint(&particles[2], &particles[3], 1.0f));
//
//				}
//
//			}
//
//		}
//
//		// 드래그 중이면 마우스 위치로
//		if (draggedParticle != nullptr) {
//
//			sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
//			Vec2 mousePos(mousePixel.x, mousePixel.y);
//			draggedParticle->position = mousePos + mouseOffset;
//			draggedParticle->velocity = Vec2(0, 0); // 속도 초기화
//
//		}
//
//		float dt = clock.restart().asSeconds();
//
//		// === 물리 업데이트 ===
//		for (auto& p : particles) {
//
//			// 매우 무거운 파티클은 중력 무시
//			if (p.mass < 100) {
//				p.applyForce(gravity * p.mass);
//			}
//			p.update(dt);
//			p.checkBounds(WIDTH, HEIGHT);
//
//		}
//
//		// === 제약 해결 (여러 번 반복) ===
//		int iterations = 5; // 반복 횟수
//		for (int iter = 0; iter < iterations; iter++) {
//
//			for (auto& constraint : constraints) {
//
//				constraint.solve();
//
//			}
//
//		}
//
//		// === rendering ===
//		window.clear(sf::Color::Black);
//
//		// 제약 그리기(선)
//		for (auto& constraint : constraints) {
//			constraint.draw(window);
//		}
//
//		// 파티클 그리기
//		for (auto& p : particles) {
//
//			sf::CircleShape circle(p.radius);
//
//			// 무거운 파티클은 다른 색 (고정점)
//			if (p.mass > 100) {
//
//				circle.setFillColor(sf::Color::Red);
//
//			}
//			else {
//
//				circle.setFillColor(sf::Color::Green);
//
//			}
//
//			circle.setPosition(p.position.x - p.radius, p.position.y - p.radius);
//
//			window.draw(circle);
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