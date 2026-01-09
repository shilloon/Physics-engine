#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Vec2.h"
#include "Particle.h"
#include "DistanceConstraint.h"
#include "Spring.h"

int main() {

	const float WIDTH = 800;
	const float HEIGHT = 600;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - Spring");
	window.setFramerateLimit(60);

	std::vector<Particle> particles;
	std::vector<DistanceConstraint> constraints;
	std::vector<Spring> springs;

	Vec2 gravity(0, 9.8f * 100);

	// 제약
	particles.push_back(Particle(Vec2(200, 200), 1.0f, 15.0f, 0.8f));
	particles.push_back(Particle(Vec2(350, 200), 1.0f, 15.0f, 0.8f));

	// 스프링
	particles.push_back(Particle(Vec2(450, 200), 1.0f, 15.0f, 0.8f));
	particles.push_back(Particle(Vec2(600, 200), 1.0f, 15.0f, 0.8f));

	// 고정점 + 스프링
	particles.push_back(Particle(Vec2(400, 100), 1000.0f, 10.0f, 0.8f));
	particles.push_back(Particle(Vec2(400, 250), 1.0f, 20.0f, 0.8f));

	// ==== 제약, 스프링 생성 ===

	// 제약
	constraints.push_back(DistanceConstraint(&particles[0], &particles[1], 1.0f));

	// 스프링
	springs.push_back(Spring(&particles[2], &particles[3], 50.0f, 0.5f)); 
	springs.push_back(Spring(&particles[4], &particles[5], 100.0f, 0.3f)); 

	sf::Clock clock;

	// interaction
	Particle* draggedParticle = nullptr;
	Vec2 mouseOffset;

	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {

				window.close();

			}

			// === 마우스로 파티클 잡기 ===
			if (event.type == sf::Event::MouseButtonPressed) {

				if (event.mouseButton.button == sf::Mouse::Left) {

					Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);

					for (auto& p : particles) {

						Vec2 diff = p.position - mousePos;
						if (diff.length() < p.radius + 10) {

							draggedParticle = &p;
							mouseOffset = p.position - mousePos;
							break;

						}

					}

				}

			}

			if (event.type == sf::Event::MouseButtonReleased) {

				draggedParticle = nullptr;

			}

			// === 키보드 ===
			if (event.type == sf::Event::KeyPressed) {

				if (event.key.code == sf::Keyboard::R) {

					particles.clear();
					constraints.clear();
					springs.clear();

					particles.push_back(Particle(Vec2(200, 200), 1.0f, 15.0f, 0.8f));
					particles.push_back(Particle(Vec2(350, 200), 1.0f, 15.0f, 0.8f));
					particles.push_back(Particle(Vec2(450, 200), 1.0f, 15.0f, 0.8f));
					particles.push_back(Particle(Vec2(600, 200), 1.0f, 15.0f, 0.8f));
					particles.push_back(Particle(Vec2(400, 100), 1000.0f, 10.0f, 0.8f));
					particles.push_back(Particle(Vec2(400, 250), 1.0f, 20.0f, 0.8f));

					constraints.push_back(DistanceConstraint(&particles[0], &particles[1], 1.0f));
					springs.push_back(Spring(&particles[2], &particles[3], 50.0f, 0.5f));
					springs.push_back(Spring(&particles[4], &particles[5], 100.0f, 0.3f));

				}

			}

		}

		// 드래그 중
		if (draggedParticle != nullptr) {

			sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
			Vec2 mousePos(mousePixel.x, mousePixel.y);
			draggedParticle->position = mousePos + mouseOffset;
			draggedParticle->velocity = Vec2(0, 0);

		}

		float dt = clock.restart().asSeconds();

		// === 물리 업데이트 ===

		// 중력
		for (auto& p : particles) {

			if (p.mass < 100) {

				p.applyForce(gravity * p.mass);

			}

		}

		// 스프링 힘
		for (auto& spring : springs) {

			spring.applyForce();

		}

		// 파티클 업데이트
		for (auto& p : particles) {

			p.update(dt);
			p.checkBounds(WIDTH, HEIGHT);

		}

		// 제약 해결
		int iterations = 5;
		for (int iter = 0; iter < iterations; iter++) {

			for (auto& constraint : constraints) {

				constraint.solve();

			}

		}

		// === rendering ===
		window.clear(sf::Color::Black);

		// 제약 그리기
		for (auto& constraint : constraints) {

			constraint.draw(window);

		}

		// 스프링 그리기
		for (auto& spring : springs) {

			spring.draw(window);

		}

		// 파티클 그리기
		for (auto& p : particles) {

			sf::CircleShape circle(p.radius);

			if (p.mass > 100) {

				circle.setFillColor(sf::Color::Red);

			}
			else {

				circle.setFillColor(sf::Color::Green);

			}

			circle.setPosition(p.position.x - p.radius, p.position.y - p.radius);
			window.draw(circle);

		}

		window.display();

	}

	return 0;

}