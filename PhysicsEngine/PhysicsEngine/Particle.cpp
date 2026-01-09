//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <vector>
//#include "Vec2.h"
//#include "Particle.h"
//#include <cstdlib>
//#include <ctime>
//
//// Particle collision 확인
//bool checkCollision(const Particle& a, const Particle& b) {
//
//    Vec2 diff = b.position - a.position;
//    float distance = diff.length();
//    float radiusSum = a.radius + b.radius;
//
//    return distance < radiusSum;
//
//}
//
//// Collision (Impulse 기반)
//void resolveCollision(Particle& a, Particle& b) {
//
//    // 충돌 법선 벡터 (a -> b)
//    Vec2 normal = (b.position - a.position).normalize();
//
//    // 상대 속도
//    Vec2 relativeVel = b.velocity - a.velocity;
//
//    // 법선 방향 상대 속도
//    float velAlongNormal = relativeVel.dot(normal);
//
//    // 이미 분리 중이면 무시
//    if (velAlongNormal > 0) return;
//
//    // 반발 계수 (두 물체의 평균)
//    float e = (a.restitution + b.restitution) / 2.0f;
//
//    // Impulse 크기 계산
//    float j = -(1 + e) * velAlongNormal;
//    j /= (1 / a.mass + 1 / b.mass);
//
//    // Impulse Vector
//    Vec2 impulse = normal * j;
//
//    // 속도 변경(Impulse = mass * delta_velocity)
//    a.velocity -= impulse / a.mass;
//    b.velocity += impulse / b.mass;
//
//    // 위치 보정
//    Vec2 diff = b.position - a.position;
//    float distance = diff.length();
//    float overlap = a.radius + b.radius - distance;
//
//    if (overlap > 0) {
//        Vec2 correction = normal * (overlap / 2.0f);
//        a.position -= correction;
//        b.position += correction;
//    }
//
//}
//
//enum class VisualizationMode {
//
//	ARROW,
//	SLINGSHOT
//
//};
//
//int main() {
//
//    const float WIDTH = 800;
//    const float HEIGHT = 600;
//
//    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - Forces");
//    window.setFramerateLimit(60);
//
//    std::vector<Particle> particles;
//    Vec2 gravity(0, 9.8f * 100);
//	Vec2 wind(0, 0);
//
//    // 드래그 관련 변수
//    bool isDragging = false;
//    Vec2 dragStart;
//    Vec2 dragCurrent;
//
//	VisualizationMode vizMode = VisualizationMode::SLINGSHOT;
//
//	// 물리 설정
//	bool windEnabled = false;
//	bool dragEnabled = true;
//	bool frictionEnabled = true;
//	float frictionCoeff = 0.3f;
//
//    // 시간 측정
//    sf::Clock clock;
//
//    // main loop
//    while (window.isOpen()) {
//
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//
//			// === 키보드 입력 (모드 전환) ===
//			if (event.type == sf::Event::KeyPressed) {
//
//				// MODE Toggle
//				if(event.key.code == sf::Keyboard::Num1) {
//
//					vizMode = VisualizationMode::ARROW;
//					std::cout << "Mode : ARROW" << std::endl;
//
//				}
//
//				if(event.key.code == sf::Keyboard::Num2) {
//
//					vizMode = VisualizationMode::SLINGSHOT;
//					std::cout << "Mode : SLINGSHOT" << std::endl;
//
//				}
//
//				// Force Toggle
//				if (event.key.code == sf::Keyboard::W) {
//
//					windEnabled = !windEnabled;
//					std::cout << "Wind: " << (windEnabled ? "ON" : "OFF") << std::endl;
//
//				}
//				
//				if (event.key.code == sf::Keyboard::D) {
//
//					dragEnabled = !dragEnabled;
//					std::cout << "Drag: " << (dragEnabled ? "ON" : "OFF") << std::endl;
//
//				}
//
//				if(event.key.code == sf::Keyboard::F){
//				
//					frictionEnabled = !frictionEnabled;
//					std::cout << "Friction: " << (frictionEnabled ? "ON" : "OFF") << std::endl;
//
//				}
//
//				// Wind Control Toggle
//				if (event.key.code == sf::Keyboard::Left) {
//
//					wind.x -= 50.0f;
//					std::cout << "Wind: " << wind.x << ", " << wind.y << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::Right) {
//
//					wind.x += 50.0f;
//					std::cout << "Wind: " << wind.x << ", " << wind.y << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::Up) {
//
//					wind.y -= 50.0f;
//					std::cout << "Wind: " << wind.x << ", " << wind.y << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::Down) {
//
//					wind.y += 50.0f;
//					std::cout << "Wind: " << wind.x << ", " << wind.y << std::endl;
//
//				}
//
//				if (event.key.code == sf::Keyboard::Space) {
//
//					wind = Vec2(0, 0);
//					std::cout << "Wind Reset" << std::endl;
//
//				}
//
//				// Particle Reset Toggle
//				if (event.key.code == sf::Keyboard::C) {
//
//					particles.clear();
//					std::cout << "Cleaned all Particles" << std::endl;
//
//				}
//
//			}
//
//            //  === 마우스 버튼 눌렀을 때 ===
//            if (event.type == sf::Event::MouseButtonPressed) {
//
//                if (event.mouseButton.button == sf::Mouse::Left) {
//
//                    isDragging = true;
//                    dragStart = Vec2(event.mouseButton.x, event.mouseButton.y);
//                    dragCurrent = dragStart;
//
//                }
//
//            }
//
//            // === 마우슨 버튼 뗐을 때 ===
//            if (event.type == sf::Event::MouseButtonReleased) {
//
//                if (event.mouseButton.button == sf::Mouse::Left && isDragging) {
//
//                    isDragging = false;
//
//                    Vec2 launchVel = (dragStart - dragCurrent) * 2.0f;
//
//                    Particle p(dragStart, 1.0f, 20.0f, 0.8);
//                    p.velocity = launchVel;
//					particles.push_back(p);
//
//                }
//
//            }
//
//            // === 마우스 움직임 (드래그 중) ===
//			if (event.type == sf::Event::MouseMoved && isDragging) {
//
//				dragCurrent = Vec2(event.mouseMove.x, event.mouseMove.y);
//
//			}
//
//        }
//
//        float dt = clock.restart().asSeconds();
//
//        // === physics update ===
//        for (auto& p : particles) {
//
//            p.applyForce(gravity * p.mass);
//
//			if (windEnabled) {
//
//				p.applyForce(wind * p.mass);
//
//			}
//
//			if (dragEnabled) {
//			
//				p.applyDrag();
//			
//			}
//
//			if (frictionEnabled) {
//
//				p.applyFriction(frictionCoeff);
//
//			}
//
//            p.update(dt);
//            p.checkBounds(WIDTH, HEIGHT);
//
//        }
//
//        for (size_t i = 0; i < particles.size(); i++) {
//
//            for (size_t j = i + 1; j < particles.size(); j++) {
//
//                if (checkCollision(particles[i], particles[j])) {
//
//                    resolveCollision(particles[i], particles[j]);
//
//                }
//
//            }
//
//        }
//
//        // === rendering ===
//        window.clear(sf::Color::Black);
//
//        // 원 그리기
//        for (const auto& p : particles) {
//            sf::CircleShape circle(p.radius);
//            
//			if (p.onGround && frictionEnabled) {
//
//				circle.setFillColor(sf::Color(100, 200, 100));
//
//			}
//			else {
//
//				circle.setFillColor(sf::Color::Green);
//
//			}
//
//            circle.setPosition(p.position.x - p.radius, p.position.y - p.radius);
//            window.draw(circle);
//        }
//        
//		// === 바람 시각화 ===
//		if (windEnabled && wind.lengthSq() > 1.0f) {
//
//			for (int i = 0; i < 10; i++) {
//
//				for (int j = 0; j < 8; j++) {
//
//					float x = 80 + i * 70;
//					float y = 75 + j * 70;
//
//					Vec2 windDir = wind.normalize() * 20.0f;
//
//					sf::Vertex windLine[] = {
//						sf::Vertex(sf::Vector2f(x, y), sf::Color(100, 100, 255, 100)),
//						sf::Vertex(sf::Vector2f(x + windDir.x, y + windDir.y), sf::Color(150, 150, 255, 100))
//					};
//					window.draw(windLine, 2, sf::Lines);
//				}
//
//			}
//
//		}
//
//		// === 드래그 시각화 ===
//		if (isDragging) {
//
//			Vec2 launchVel = (dragStart - dragCurrent) * 2.0f;
//
//
//			// ARROW 모드
//			if (vizMode == VisualizationMode::ARROW) {
//
//				sf::CircleShape previewCircle(20.0f);
//				previewCircle.setFillColor(sf::Color(255, 255, 255, 150));
//				previewCircle.setOutlineThickness(2);
//				previewCircle.setOutlineColor(sf::Color::Yellow);
//				previewCircle.setPosition(dragStart.x - 20, dragStart.y - 20);
//				window.draw(previewCircle);
//
//				// 발사 궤적 예측선
//				Vec2 trajectoryEnd = dragStart + launchVel * 0.1;
//
//				sf::Vertex trajectory[] = {
//
//					sf::Vertex(sf::Vector2f(dragStart.x, dragStart.y), sf::Color::Yellow),
//					sf::Vertex(sf::Vector2f(trajectoryEnd.x, trajectoryEnd.y), sf::Color::Red)
//
//				};
//				window.draw(trajectory, 2, sf::Lines);
//
//				// 화살촉
//				if (launchVel.lengthSq() > 0.1f) {
//
//					Vec2 direction = launchVel.normalize();
//					Vec2 arrowTip = trajectoryEnd;
//					float arrowSize = 15.0f;
//
//					Vec2 perpendicular(-direction.y, direction.x);
//
//					sf::ConvexShape arrowHead;
//					arrowHead.setPointCount(3);
//					arrowHead.setPoint(0, sf::Vector2f(arrowTip.x, arrowTip.y));
//					arrowHead.setPoint(1, sf::Vector2f(arrowTip.x - direction.x * arrowSize + perpendicular.x * arrowSize * 0.5f, arrowTip.y - direction.y * arrowSize + perpendicular.y * arrowSize * 0.5f));
//					arrowHead.setPoint(2, sf::Vector2f(arrowTip.x - direction.x * arrowSize - perpendicular.x * arrowSize * 0.5f, arrowTip.y - direction.y - arrowSize + perpendicular.y * arrowSize * 0.5f));
//					arrowHead.setFillColor(sf::Color::Red);
//					window.draw(arrowHead);
//
//				}
//
//				// 현재 마우스 위치
//				sf::CircleShape mouseDot(5);
//				mouseDot.setFillColor(sf::Color(150, 150, 150));
//				mouseDot.setPosition(dragCurrent.x - 5, dragCurrent.y - 5);
//				window.draw(mouseDot);
//
//				// Drag 가이드 선
//				sf::Vertex guideLine[] = {
//					sf::Vertex(sf::Vector2f(dragStart.x, dragStart.y), sf::Color(100, 100, 100, 100)),
//					sf::Vertex(sf::Vector2f(dragCurrent.x, dragCurrent.y), sf::Color(100, 100, 100, 100))
//				};
//				window.draw(guideLine, 2, sf::Lines);
//			}
//			// === SLINGSHOT MODE ===
//			else {
//
//				sf::Vertex slingshot[] = {
//
//					sf::Vertex(sf::Vector2f(dragStart.x, dragStart.y), sf::Color::White),
//					sf::Vertex(sf::Vector2f(dragCurrent.x, dragCurrent.y), sf::Color::White)
//
//				};
//
//				window.draw(slingshot, 2, sf::Lines);
//
//				// 당겨진 공
//				sf::CircleShape pulledBall(20.0f);
//				pulledBall.setFillColor(sf::Color::Yellow);
//				pulledBall.setOutlineThickness(2);
//				pulledBall.setOutlineColor(sf::Color::Red);
//				pulledBall.setPosition(dragCurrent.x - 20, dragCurrent.y - 20);
//				window.draw(pulledBall);
//
//				// 발사 궤적
//				if (launchVel.lengthSq() > 0.1f) {
//
//					Vec2 direction = launchVel.normalize();
//					float speed = launchVel.length();
//
//					for (int i = 1; i <= 5; i++) {
//
//						Vec2 dotPos = dragCurrent + direction * (i * 30.0f * (speed / 200.0f));
//						sf::CircleShape dot(4);
//						dot.setFillColor(sf::Color(255, 255, 0, 255 - i * 40));
//						dot.setPosition(dotPos.x - 4, dotPos.y - 4);
//						window.draw(dot);
//
//					}
//
//				}
//
//				// 시작점
//				sf::CircleShape startPoint(8);
//				startPoint.setFillColor(sf::Color::White);
//				startPoint.setPosition(dragStart.x - 8, dragStart.y - 8);
//				window.draw(startPoint);
//
//			}
//
//		}
//
//		std::string info = "Particles" + std::to_string(particles.size());
//		info += " | Mode: " + std::string(vizMode == VisualizationMode::ARROW ? "ARROW(1)" : "SLINGSHOT(2)");
//		info += " | Clear: C";
//
//		window.display();
//
//    }
//
//    return 0;
//
//}
//
