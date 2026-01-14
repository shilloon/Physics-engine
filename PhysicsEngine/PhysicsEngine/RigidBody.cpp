//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <vector>
//#include <algorithm>
//#include "Vec2.h"
//#include "RigidBody.h"
//
//// === 충돌 정보 ===
//struct CollisionInfo {
//    bool collided;
//    Vec2 normal;
//    float depth;
//
//    CollisionInfo() : collided(false), depth(0) {}
//};
//
//// === 박스 꼭짓점 구하기 === 
//std::vector<Vec2> getBoxVertices(const RigidBody& box) {
//    std::vector<Vec2> vertices(4);
//
//    float halfW = box.width / 2;
//    float halfH = box.height / 2;
//
//    Vec2 localVertices[4] = {
//        Vec2(-halfW, -halfH),
//        Vec2(halfW, -halfH),
//        Vec2(halfW, halfH),
//        Vec2(-halfW, halfH)
//    };
//
//    float cos_a = std::cos(box.angle);
//    float sin_a = std::sin(box.angle);
//
//    for (int i = 0; i < 4; i++) {
//        float rotatedX = localVertices[i].x * cos_a - localVertices[i].y * sin_a;
//        float rotatedY = localVertices[i].x * sin_a + localVertices[i].y * cos_a;
//        vertices[i] = Vec2(rotatedX, rotatedY) + box.position;
//    }
//
//    return vertices;
//}
//
//// === 박스 축 구하기 ===
//std::vector<Vec2> getBoxAxes(const RigidBody& box) {
//    std::vector<Vec2> axes(2);
//
//    float cos_a = std::cos(box.angle);
//    float sin_a = std::sin(box.angle);
//
//    axes[0] = Vec2(cos_a, sin_a);
//    axes[1] = Vec2(-sin_a, cos_a);
//
//    return axes;
//}
//
//// === 투영 ===
//void projectVertices(const std::vector<Vec2>& vertices, Vec2 axis, float& min, float& max) {
//    min = max = vertices[0].dot(axis);
//
//    for (size_t i = 1; i < vertices.size(); i++) {
//        float projection = vertices[i].dot(axis);
//        if (projection < min) min = projection;
//        if (projection > max) max = projection;
//    }
//}
//
//// === OBB 충돌 감지 (SAT) ===
//CollisionInfo checkOBB(const RigidBody& a, const RigidBody& b) {
//    CollisionInfo info;
//
//    std::vector<Vec2> verticesA = getBoxVertices(a);
//    std::vector<Vec2> verticesB = getBoxVertices(b);
//
//    std::vector<Vec2> axesA = getBoxAxes(a);
//    std::vector<Vec2> axesB = getBoxAxes(b);
//
//    float minOverlap = FLT_MAX;
//    Vec2 minAxis;
//
//    std::vector<Vec2> allAxes;
//    allAxes.insert(allAxes.end(), axesA.begin(), axesA.end());
//    allAxes.insert(allAxes.end(), axesB.begin(), axesB.end());
//
//    for (const Vec2& axis : allAxes) {
//        float minA, maxA, minB, maxB;
//
//        projectVertices(verticesA, axis, minA, maxA);
//        projectVertices(verticesB, axis, minB, maxB);
//
//        if (maxA < minB || maxB < minA) {
//            info.collided = false;
//            return info;
//        }
//
//        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
//
//        if (overlap < minOverlap) {
//            minOverlap = overlap;
//            minAxis = axis;
//
//            Vec2 centerDiff = b.position - a.position;
//            if (centerDiff.dot(axis) < 0) {
//                minAxis = minAxis * -1.0f;
//            }
//        }
//    }
//
//    info.collided = true;
//    info.normal = minAxis.normalize();
//    info.depth = minOverlap;
//
//    return info;
//}
//
//// === OBB 충돌 반응 ===
//void resolveOBB(RigidBody& a, RigidBody& b, const CollisionInfo& info) {
//    // 위치 보정
//    float totalMass = a.mass + b.mass;
//    float percent = 0.8f;
//    Vec2 correction = info.normal * info.depth * percent;
//
//    a.position -= correction * (b.mass / totalMass);
//    b.position += correction * (a.mass / totalMass);
//
//    // 충돌점 (간단 버전)
//    Vec2 contactPoint = (a.position + b.position) * 0.5f;
//
//    Vec2 rA = contactPoint - a.position;
//    Vec2 rB = contactPoint - b.position;
//
//    // 충돌점 속도
//    Vec2 velA = a.velocity + Vec2(-rA.y * a.angularVelocity, rA.x * a.angularVelocity);
//    Vec2 velB = b.velocity + Vec2(-rB.y * b.angularVelocity, rB.x * b.angularVelocity);
//    Vec2 relativeVel = velB - velA;
//
//    float velAlongNormal = relativeVel.dot(info.normal);
//
//    if (velAlongNormal > 0) return;
//
//    // Impulse
//    float e = (a.restitution + b.restitution) / 2;
//
//    float rACrossN = rA.cross(info.normal);
//    float rBCrossN = rB.cross(info.normal);
//
//    float denominator = 1 / a.mass + 1 / b.mass +
//        (rACrossN * rACrossN) / a.inertia +
//        (rBCrossN * rBCrossN) / b.inertia;
//
//    float j = -(1 + e) * velAlongNormal / denominator;
//
//    Vec2 impulse = info.normal * j;
//
//    a.velocity -= impulse / a.mass;
//    b.velocity += impulse / b.mass;
//
//    a.angularVelocity -= rA.cross(impulse) / a.inertia;
//    b.angularVelocity += rB.cross(impulse) / b.inertia;
//}
//
//int main() {
//    const float WIDTH = 800;
//    const float HEIGHT = 600;
//
//    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - OBB!");
//    window.setFramerateLimit(60);
//
//    std::vector<RigidBody> bodies;
//    Vec2 gravity(0, 9.8f * 100);
//
//    // 박스 생성
//    bodies.push_back(RigidBody(Vec2(400, 100), 80, 60, 1.5f, 0.6f));
//    bodies.push_back(RigidBody(Vec2(300, 200), 100, 50, 2.0f, 0.7f));
//    bodies.push_back(RigidBody(Vec2(500, 250), 70, 70, 1.8f, 0.5f));
//    bodies.push_back(RigidBody(Vec2(400, 350), 90, 40, 1.2f, 0.8f));
//
//    bodies[0].velocity = Vec2(50, 100);
//    bodies[0].angularVelocity = 2.0f;
//
//    sf::Clock clock;
//
//    // 드래그 변수
//    RigidBody* draggedBody = nullptr;
//    Vec2 dragStartWorldPos;  // 드래그 시작 (월드 좌표)
//    Vec2 dragClickOffset;    // 클릭 지점과 박스 중심 차이
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//
//            // 마우스 눌림
//            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//                Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//
//                for (auto& body : bodies) {
//                    if (body.containsRotated(mousePos)) {
//                        draggedBody = &body;
//                        dragStartWorldPos = mousePos;
//                        dragClickOffset = body.position - mousePos;
//
//                        // 드래그 시작 시 속도 제거
//                        body.velocity = Vec2(0, 0);
//                        body.angularVelocity = 0;
//                        break;
//                    }
//                }
//            }
//
//            // 마우스 뗌
//            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
//                if (draggedBody != nullptr) {
//                    Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//
//                    // 던지기 속도 = 드래그 벡터 * 속도 계수
//                    Vec2 dragVector = mousePos - dragStartWorldPos;
//                    draggedBody->velocity = dragVector * 8.0f;
//
//                    // 토크 계산 (클릭 위치 기준)
//                    Vec2 clickLocalPos = dragStartWorldPos - (draggedBody->position - dragVector);
//                    Vec2 torqueArm = clickLocalPos - draggedBody->position;
//                    float torque = torqueArm.cross(draggedBody->velocity) * 0.02f;
//                    draggedBody->angularVelocity = torque / draggedBody->inertia;
//
//                    draggedBody = nullptr;
//                }
//            }
//
//            // 키보드
//            if (event.type == sf::Event::KeyPressed) {
//                if (event.key.code == sf::Keyboard::R) {
//                    draggedBody = nullptr;
//                    for (size_t i = 0; i < bodies.size(); i++) {
//                        bodies[i].velocity = Vec2(0, 0);
//                        bodies[i].angularVelocity = 0;
//                        bodies[i].angle = 0;
//                    }
//                    bodies[0].position = Vec2(400, 100);
//                    bodies[1].position = Vec2(300, 200);
//                    bodies[2].position = Vec2(500, 250);
//                    bodies[3].position = Vec2(400, 350);
//                }
//
//                if (event.key.code == sf::Keyboard::Space) {
//                    for (auto& body : bodies) {
//                        body.velocity = Vec2(rand() % 300 - 150, rand() % 300 - 150);
//                        body.angularVelocity = (rand() % 100 - 50) / 10.0f;
//                    }
//                }
//            }
//        }
//
//        // 드래그 중 위치 업데이트
//        if (draggedBody != nullptr) {
//            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
//            Vec2 mousePos(mousePixel.x, mousePixel.y);
//            draggedBody->position = mousePos + dragClickOffset;
//        }
//
//        float dt = clock.restart().asSeconds();
//
//        // 물리 업데이트
//        for (auto& body : bodies) {
//            if (draggedBody != &body) {
//                body.applyForce(gravity * body.mass);
//                body.update(dt);
//                body.checkBounds(WIDTH, HEIGHT);
//            }
//        }
//
//        // 충돌 처리
//        for (size_t i = 0; i < bodies.size(); i++) {
//            for (size_t j = i + 1; j < bodies.size(); j++) {
//                if (draggedBody == &bodies[i] || draggedBody == &bodies[j]) {
//                    continue;
//                }
//
//                CollisionInfo info = checkOBB(bodies[i], bodies[j]);
//                if (info.collided) {
//                    resolveOBB(bodies[i], bodies[j], info);
//                }
//            }
//        }
//
//        // 렌더링
//        window.clear(sf::Color::Black);
//
//        for (const auto& body : bodies) {
//            body.draw(window);
//        }
//
//        // 드래그 시각화
//        if (draggedBody != nullptr) {
//            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
//            Vec2 mousePos(mousePixel.x, mousePixel.y);
//
//            sf::Vertex line[] = {
//                sf::Vertex(sf::Vector2f(dragStartWorldPos.x, dragStartWorldPos.y), sf::Color::Yellow),
//                sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y), sf::Color::Red)
//            };
//            window.draw(line, 2, sf::Lines);
//
//            sf::CircleShape dot(5);
//            dot.setFillColor(sf::Color::Yellow);
//            dot.setPosition(dragStartWorldPos.x - 5, dragStartWorldPos.y - 5);
//            window.draw(dot);
//        }
//
//        window.display();
//    }
//
//    return 0;
//}