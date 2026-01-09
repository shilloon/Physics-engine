//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <vector>
//#include "Vec2.h"
//#include "RigidBody.h"
//
//int main() {
//
//	const float WIDTH = 800;
//	const float HEIGHT = 600;
//
//	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - Rigid Bodies");
//	window.setFramerateLimit(60);
//
//	std::vector<RigidBody> bodies;
//	Vec2 gravity(0, 9.8f * 100);
//
//	bodies.push_back(RigidBody(Vec2(400, 300), 100, 60, 2.0f));
//	bodies.push_back(RigidBody(Vec2(200, 200), 80, 80, 1.5f));
//	bodies.push_back(RigidBody(Vec2(600, 250), 120, 40, 2.5f));
//
//	// 초기 각속도
//	bodies[0].angularVelocity = 2.0f;
//	bodies[1].angularVelocity = -1.5f;
//
//	// 초기 속도
//	bodies[1].velocity = Vec2(100, -50);
//
//	sf::Clock clock;
//
//	enum class InteractionMode {
//		THROW,
//		PUSH
//	};
//
//	InteractionMode mode = InteractionMode::PUSH;
//
//	bool isDragging = false;
//	Vec2 dragStart;
//	Vec2 dragCurrent;
//	RigidBody* selectedBody = nullptr;
//	Vec2 clickPoint;
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
//			if (event.type == sf::Event::KeyPressed) {
//
//				if (event.key.code == sf::Keyboard::Num1) {
//
//					mode = InteractionMode::THROW;
//					std::cout << "MODE : THROW" << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::Num2) {
//
//					mode = InteractionMode::PUSH;
//					std::cout << "MODE : PUSH" << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::C) {
//
//					bodies.clear();
//					std::cout << "Cleared" << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::R) {
//
//					bodies.clear();
//					bodies.push_back(RigidBody(Vec2(400, 300), 100, 60, 2.0f));
//					bodies.push_back(RigidBody(Vec2(200, 200), 80, 80, 1.5f));
//					bodies.push_back(RigidBody(Vec2(600, 250), 120, 40, 2.5f));
//					std::cout << "Reset" << std::endl;
//
//				}
//
//			}
//
//			if (event.type == sf::Event::MouseButtonPressed) {
//
//				if (event.mouseButton.button == sf::Mouse::Left) {
//
//					isDragging = true;
//					dragStart = Vec2(event.mouseButton.x, event.mouseButton.y);
//					dragCurrent = dragStart;
//
//					if (mode == InteractionMode::PUSH) {
//
//						// 클릭한 위치 박스 체크
//						for (auto& body : bodies) {
//
//							if (body.containsRotated(dragStart)) {
//
//								selectedBody = &body;
//								clickPoint = dragStart;
//								std::cout << "Selected body!" << std::endl;
//								break;
//
//							}
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
//				if (event.mouseButton.button == sf::Mouse::Left && isDragging) {
//
//					isDragging = false;
//					
//					if (mode == InteractionMode::THROW) {
//
//						Vec2 launchVel = (dragStart - dragCurrent) * 2.0f;
//
//						float w = 40 + rand() % 60;
//						float h = 30 + rand() % 50;
//
//						RigidBody box(dragStart, w, h, 1.5f);
//						box.velocity = launchVel;
//						box.angularVelocity = (rand() % 100 - 50) / 10.0f;
//
//						bodies.push_back(box);
//
//					}
//					else if (mode == InteractionMode::PUSH && selectedBody != nullptr) {
//
//						Vec2 force = (dragCurrent - dragStart) * 100.0f;
//						selectedBody->applyForceAtPoint(force, clickPoint);
//
//						std::cout << "Applied force at point!" << std::endl;
//						selectedBody = nullptr;
//
//					}
//
//				}
//
//			}
//
//			if (event.type == sf::Event::MouseMoved && isDragging) {
//
//				dragCurrent = Vec2(event.mouseMove.x, event.mouseMove.y);
//
//			}
// 
//		}
//
//		float dt = clock.restart().asSeconds();
//
//		// ===  물리 업데이트 ===
//		for (auto& body : bodies) {
//
//			body.applyForce(gravity * body.mass);
//			body.update(dt);
//			body.checkBounds(WIDTH, HEIGHT);
//
//		}
//
//		// === Rendering ===
//		window.clear(sf::Color::Black);
//
//		for (auto& body : bodies) {
//
//			body.draw(window);
//
//		}
//
//		// === Interaction 시각화 ===
//		if (isDragging) {
//
//			if (mode == InteractionMode::THROW) {
//
//				sf::Vertex line[] = {
//
//					sf::Vertex(sf::Vector2f(dragStart.x, dragStart.y), sf::Color::Yellow),
//					sf::Vertex(sf::Vector2f(dragCurrent.x, dragCurrent.y), sf::Color::Red)
//
//				};
//				window.draw(line, 2, sf::Lines);
//			}
//			else if (mode == InteractionMode::PUSH) {
//
//				if (selectedBody != nullptr) {
//
//					// 클릭 지점 표시
//					sf::CircleShape clickDot(5);
//					clickDot.setFillColor(sf::Color::Yellow);
//					clickDot.setPosition(clickPoint.x - 5, clickPoint.y - 5);
//					window.draw(clickDot);
//
//					// 힘의 방향 표시
//					Vec2 forceVec = (dragCurrent - dragStart) * 2.0f;
//					Vec2 forceEnd = clickPoint + forceVec;
//
//					sf::Vertex forceLine[] = {
//						sf::Vertex(sf::Vector2f(clickPoint.x, clickPoint.y), sf::Color::Yellow),
//						sf::Vertex(sf::Vector2f(forceEnd.x, forceEnd.y), sf::Color::Red)
//					};
//					window.draw(forceLine, 2, sf::Lines);
//
//					// 중심에서 클릭까지 Torque Arm
//					sf::Vertex torqueArm[] = {
//						sf::Vertex(sf::Vector2f(selectedBody->position.x, selectedBody->position.y), sf::Color(100, 255, 100)),
//						sf::Vertex(sf::Vector2f(clickPoint.x, clickPoint.y), sf::Color(100, 255, 100))
//					};
//					window.draw(torqueArm, 2, sf::Lines);
//				}
//
//			}
//		}
//
//		window.display();
//
//	}
//
//	return 0;
//
//}