//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <vector>
//#include "Vec2.h"
//#include "Polygon.h"
//
//// === 정다각형 생성 ===
//std::vector<Vec2> createRegularPolygon(int sides, float radius) {
//
//    std::vector<Vec2> vertices;
//
//    for (int i = 0; i < sides; i++) {
//
//        float angle = (2 * 3.14159f * i) / sides;
//        float x = std::cos(angle) * radius;
//        float y = std::sin(angle) * radius;
//        vertices.push_back(Vec2(x, y));
//
//    }
//
//    return vertices;
//
//}
//
//// === 박스 -> 다각형 ===
//std::vector<Vec2> createBox(float width, float height) {
//
//    std::vector<Vec2> vertices;
//    float halfW = width / 2;
//    float halfH = height / 2;
//
//    vertices.push_back(Vec2(-halfW, -halfH));
//    vertices.push_back(Vec2(halfW, -halfH));
//    vertices.push_back(Vec2(halfW, halfH));
//    vertices.push_back(Vec2(-halfW, halfH));
//
//    return vertices;
//
//}
//
//// === 삼각형 ===
//std::vector<Vec2> createTriangle(float size) {
//
//    std::vector<Vec2> vertices;
//    float height = size * std::sqrt(3.0f) / 2;
//
//    vertices.push_back(Vec2(0, -height * 2 / 3)); // 위
//    vertices.push_back(Vec2(size / 2, height / 3)); // 오른쪽 아래
//    vertices.push_back(Vec2(-size / 2, height / 3)); // 왼쪽 아래
//
//    return vertices;
//
//}
//
//// === 충돌 정보 ===
//struct CollisionInfo {
//
//    bool collided;
//    Vec2 normal;
//    float depth;
//
//    CollisionInfo() : collided(false), depth(0) {}
//
//};
//
//// === 투영 ===
//void projectVertices(const std::vector<Vec2>& vertices, Vec2 axis, float& min, float& max) {
//
//    min = max = vertices[0].dot(axis);
//
//    for (size_t i = 1; i < vertices.size(); i++) {
//
//        float projection = vertices[i].dot(axis);
//        if (projection < min) min = projection;
//        if (projection > max) max = projection;
//
//    }
//
//}
//
//// === SAT 충돌 감지 ===
//CollisionInfo checkPolygonCollision(const Polygon& a, const Polygon& b) {
//
//    CollisionInfo info;
//
//    std::vector<Vec2> verticesA = a.getWorldVertices();
//    std::vector<Vec2> verticesB = b.getWorldVertices();
//
//    std::vector<Vec2> axesA = a.getAxes();
//    std::vector<Vec2> axesB = b.getAxes();
//
//    float minOverlap = FLT_MAX;
//    Vec2 minAxis;
//
//    // 모든 축 검사
//    std::vector<Vec2> allAxes;
//    allAxes.insert(allAxes.end(), axesA.begin(), axesA.end());
//    allAxes.insert(allAxes.end(), axesB.begin(), axesB.end());
//
//    for (const Vec2& axis : allAxes) {
//
//        float minA, maxA, minB, maxB;
//
//        projectVertices(verticesA, axis, minA, maxA);
//        projectVertices(verticesB, axis, minB, maxB);
//
//        // 분리축 발견 -> 충돌 없음
//        if (maxA < minB || maxB < minA) {
//
//            info.collided = false;
//            return info;
//
//        }
//
//        // 겹침 계산
//        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
//
//        if (overlap < minOverlap) {
//
//            minOverlap = overlap;
//            minAxis = axis;
//
//            // 법선 방향 조정
//            Vec2 centerDiff = b.position - a.position;
//            if (centerDiff.dot(axis) < 0) {
//
//                minAxis = minAxis * -1.0f;
//
//            }
//
//        }
//
//    }
//
//    // 모든 축에서 겹침 -> 충돌
//    info.collided = true;
//    info.normal = minAxis.normalize();
//    info.depth = minOverlap;
//
//    return info;
//
//}
//
//// === 충돌 반응 ===
//void resolvePolygonCollision(Polygon& a, Polygon& b, const CollisionInfo& info) {
//
//    // ===== 추가 1: 충돌 threshold =====
//    if (info.depth < 0.01f) return;  // 너무 작은 침투는 무시
//
//    // ===== 수정 1: 무한 질량 체크 =====
//    bool aIsStatic = std::isinf(a.mass);
//    bool bIsStatic = std::isinf(b.mass);
//
//    // 둘 다 고정이면 무시
//    if (aIsStatic && bIsStatic) return;
//
//    // 위치 보정
//    float percent = 0.4f;  // ===== 수정 2: 0.8 → 0.4 =====
//    Vec2 correction = info.normal * info.depth * percent;
//
//    if (!aIsStatic && !bIsStatic) {
//        // 둘 다 움직이는 물체
//        float totalMass = a.mass + b.mass;
//        a.position -= correction * (b.mass / totalMass);
//        b.position += correction * (a.mass / totalMass);
//    }
//    else if (aIsStatic) {
//        // a만 고정 (바닥 등)
//        b.position += correction;
//    }
//    else {
//        // b만 고정
//        a.position -= correction;
//    }
//
//    // 충돌점
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
//    // ===== 추가 2: 작은 속도는 무시 =====
//    if (velAlongNormal > -0.01f) return;  // 거의 정지 상태
//
//    // Impulse 계산
//    float e = (a.restitution + b.restitution) / 2;
//
//    float rACrossN = rA.cross(info.normal);
//    float rBCrossN = rB.cross(info.normal);
//
//    // ===== 수정 3: 무한 질량/관성 처리 =====
//    float invMassA = aIsStatic ? 0.0f : 1.0f / a.mass;
//    float invMassB = bIsStatic ? 0.0f : 1.0f / b.mass;
//    float invInertiaA = aIsStatic ? 0.0f : 1.0f / a.inertia;
//    float invInertiaB = bIsStatic ? 0.0f : 1.0f / b.inertia;
//
//    float invMassSum = invMassA + invMassB;
//    float angularSum = (rACrossN * rACrossN) * invInertiaA +
//        (rBCrossN * rBCrossN) * invInertiaB;
//
//    float j = -(1 + e) * velAlongNormal / (invMassSum + angularSum);
//
//    Vec2 impulse = info.normal * j;
//
//    // 속도 업데이트
//    if (!aIsStatic) {
//        a.velocity -= impulse * invMassA;
//        a.angularVelocity -= rA.cross(impulse) * invInertiaA;
//    }
//    if (!bIsStatic) {
//        b.velocity += impulse * invMassB;
//        b.angularVelocity += rB.cross(impulse) * invInertiaB;
//    }
//
//    // === 마찰 Impulse ===
//    velA = a.velocity + Vec2(-rA.y * a.angularVelocity, rA.x * a.angularVelocity);
//    velB = b.velocity + Vec2(-rB.y * b.angularVelocity, rB.x * b.angularVelocity);
//    relativeVel = velB - velA;
//
//    Vec2 tangent = relativeVel - info.normal * relativeVel.dot(info.normal);
//    float tangentLength = tangent.length();
//
//    if (tangentLength > 0.0001f) {
//        tangent = tangent / tangentLength;
//
//        float velAlongTangent = relativeVel.dot(tangent);
//
//        float mu = (a.friction + b.friction) / 2;
//
//        float rACrossT = rA.cross(tangent);
//        float rBCrossT = rB.cross(tangent);
//
//        float tangentInvMassSum = invMassA + invMassB;
//        float tangentAngularSum = (rACrossT * rACrossT) * invInertiaA +
//            (rBCrossT * rBCrossT) * invInertiaB;
//
//        float jt = -velAlongTangent / (tangentInvMassSum + tangentAngularSum);
//
//        Vec2 frictionImpulse;
//        float maxFriction = std::abs(j * mu);
//
//        if (std::abs(jt) < maxFriction) {
//            frictionImpulse = tangent * jt;
//        }
//        else {
//            frictionImpulse = tangent * (-maxFriction * (velAlongTangent > 0 ? 1.0f : -1.0f));
//        }
//
//        // 마찰 Impulse 적용
//        if (!aIsStatic) {
//            a.velocity -= frictionImpulse * invMassA;
//            a.angularVelocity -= rA.cross(frictionImpulse) * invInertiaA;
//        }
//        if (!bIsStatic) {
//            b.velocity += frictionImpulse * invMassB;
//            b.angularVelocity += rB.cross(frictionImpulse) * invInertiaB;
//        }
//    }
//}
//
//int main() {
//    const float WIDTH = 800;
//    const float HEIGHT = 600;
//
//    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Friction Test!");
//    window.setFramerateLimit(60);
//
//    std::vector<Polygon> polygons;
//    Vec2 gravity(0, 9.8f * 100);  // 중력 약하게!
//
//    // === 마찰 테스트 시나리오 ===
//
//    // 1. 바닥 (매우 무겁고, 마찰 큼)
//    std::vector<Vec2> floorVerts = createBox(700, 20);
//    Polygon floor(Vec2(400, 550), floorVerts, INFINITY, 0.0f, 0.8f);
//    floor.inertia = INFINITY;
//    polygons.push_back(floor);
//
//    // 2. 경사면 (왼쪽 높음)
//    std::vector<Vec2> rampVerts;
//    rampVerts.push_back(Vec2(-150, 10));
//    rampVerts.push_back(Vec2(150, 10));
//    rampVerts.push_back(Vec2(150, -10));
//    rampVerts.push_back(Vec2(-150, -10));
//
//    Polygon ramp(Vec2(200, 400), rampVerts, INFINITY, 0.0f, 0.5f);
//    ramp.inertia = INFINITY;
//    ramp.angle = -0.3f;  // 약간 기울임
//    polygons.push_back(ramp);
//
//    // 3. 테스트 박스들 (마찰 다르게)
//
//    // 마찰 높음 (0.9) - 빨강
//    std::vector<Vec2> box1 = createBox(40, 40);
//    Polygon highFriction(Vec2(100, 200), box1, 1.0f, 0.2f, 0.9f);
//    polygons.push_back(highFriction);
//
//    // 마찰 중간 (0.5) - 초록
//    std::vector<Vec2> box2 = createBox(40, 40);
//    Polygon mediumFriction(Vec2(200, 200), box2, 1.0f, 0.2f, 0.5f);
//    polygons.push_back(mediumFriction);
//
//    // 마찰 낮음 (0.1) - 파랑
//    std::vector<Vec2> box3 = createBox(40, 40);
//    Polygon lowFriction(Vec2(300, 200), box3, 1.0f, 0.2f, 0.1f);
//    polygons.push_back(lowFriction);
//
//    // 4. 회전 테스트용 (바닥에서 회전 멈춤 테스트)
//    std::vector<Vec2> spinner = createRegularPolygon(6, 30);
//    Polygon spinTest(Vec2(600, 300), spinner, 1.0f, 0.2f, 0.7f);
//    spinTest.angularVelocity = 10.0f;  // 빠르게 회전
//    polygons.push_back(spinTest);
//
//    sf::Clock clock;
//
//    Polygon* draggedPolygon = nullptr;
//    Vec2 dragStartPos;
//    Vec2 dragOffset;
//
//    // 텍스트 표시용
//    sf::Font font;
//    // 폰트 로드 생략 (있으면 사용)
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//
//            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//                Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//
//                for (auto& poly : polygons) {
//                    if (poly.mass < 1000 && poly.contains(mousePos)) {  // 바닥/경사면 제외
//                        draggedPolygon = &poly;
//                        dragStartPos = mousePos;
//                        dragOffset = poly.position - mousePos;
//                        poly.velocity = Vec2(0, 0);
//                        poly.angularVelocity = 0;
//                        break;
//                    }
//                }
//            }
//
//            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
//                if (draggedPolygon != nullptr) {
//                    Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
//                    Vec2 dragVector = mousePos - dragStartPos;
//
//                    draggedPolygon->velocity = dragVector * 5.0f;  // 느리게
//
//                    Vec2 torqueArm = dragStartPos - draggedPolygon->position;
//                    float torque = torqueArm.cross(draggedPolygon->velocity) * 0.02f;
//                    draggedPolygon->angularVelocity = torque / draggedPolygon->inertia;
//
//                    draggedPolygon = nullptr;
//                }
//            }
//
//            if (event.type == sf::Event::KeyPressed) {
//                // R: 리셋
//                if (event.key.code == sf::Keyboard::R) {
//                    polygons[2].position = Vec2(100, 200);  // 높은 마찰
//                    polygons[3].position = Vec2(200, 200);  // 중간 마찰
//                    polygons[4].position = Vec2(300, 200);  // 낮은 마찰
//                    polygons[5].position = Vec2(600, 300);  // 회전 테스트
//
//                    for (size_t i = 2; i < polygons.size(); i++) {
//                        polygons[i].velocity = Vec2(0, 0);
//                        polygons[i].angularVelocity = 0;
//                        polygons[i].angle = 0;
//                    }
//                    polygons[5].angularVelocity = 10.0f;
//                }
//
//                // 1: 경사면에 놓기
//                if (event.key.code == sf::Keyboard::Num1) {
//                    polygons[2].position = Vec2(100, 350);
//                    polygons[2].velocity = Vec2(0, 0);
//                    polygons[2].angularVelocity = 0;
//
//                    polygons[3].position = Vec2(200, 350);
//                    polygons[3].velocity = Vec2(0, 0);
//                    polygons[3].angularVelocity = 0;
//
//                    polygons[4].position = Vec2(300, 350);
//                    polygons[4].velocity = Vec2(0, 0);
//                    polygons[4].angularVelocity = 0;
//
//                    std::cout << "경사면 테스트: 마찰 높음(빨강) vs 중간(초록) vs 낮음(파랑)" << std::endl;
//                }
//
//                // 2: 바닥에서 밀기
//                if (event.key.code == sf::Keyboard::Num2) {
//                    polygons[2].position = Vec2(150, 500);
//                    polygons[2].velocity = Vec2(200, 0);  // 오른쪽으로 밀기
//                    polygons[2].angularVelocity = 0;
//
//                    polygons[3].position = Vec2(350, 500);
//                    polygons[3].velocity = Vec2(200, 0);
//                    polygons[3].angularVelocity = 0;
//
//                    polygons[4].position = Vec2(550, 500);
//                    polygons[4].velocity = Vec2(200, 0);
//                    polygons[4].angularVelocity = 0;
//
//                    std::cout << "밀기 테스트: 마찰 높으면 빨리 멈춤" << std::endl;
//                }
//
//                // 3: 회전 테스트
//                if (event.key.code == sf::Keyboard::Num3) {
//                    polygons[5].position = Vec2(400, 500);
//                    polygons[5].velocity = Vec2(0, 0);
//                    polygons[5].angularVelocity = 15.0f;  // 빠르게 회전
//
//                    std::cout << "회전 테스트: 마찰이 회전을 멈춤" << std::endl;
//                }
//            }
//        }
//
//        if (draggedPolygon != nullptr) {
//            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
//            Vec2 mousePos(mousePixel.x, mousePixel.y);
//            draggedPolygon->position = mousePos + dragOffset;
//        }
//
//        float dt = clock.restart().asSeconds();
//
//        // 물리 업데이트
//        for (auto& poly : polygons) {
//            if (draggedPolygon != &poly && !std::isinf(poly.mass)) {  // ← 무한 질량 체크 추가
//                poly.applyForce(gravity * poly.mass);
//                poly.update(dt);
//                poly.checkBounds(WIDTH, HEIGHT);
//            }
//        }
//
//        // 충돌 처리
//        for (int iteration = 0; iteration < 3; iteration++) {
//            for (size_t i = 0; i < polygons.size(); i++) {
//                for (size_t j = i + 1; j < polygons.size(); j++) {
//                    if (draggedPolygon == &polygons[i] || draggedPolygon == &polygons[j]) {
//                        continue;
//                    }
//
//                    CollisionInfo info = checkPolygonCollision(polygons[i], polygons[j]);
//                    if (info.collided) {
//                        resolvePolygonCollision(polygons[i], polygons[j], info);
//                    }
//                }
//            }
//        }
//
//        // 렌더링
//        window.clear(sf::Color::Black);
//
//        // 바닥 (회색)
//        polygons[0].draw(window);
//
//        // 경사면 (회색)
//        polygons[1].draw(window);
//
//        // 테스트 박스들 (색깔로 구분)
//        // 높은 마찰 (빨강)
//        {
//            std::vector<Vec2> verts = polygons[2].getWorldVertices();
//            sf::ConvexShape shape(verts.size());
//            for (size_t i = 0; i < verts.size(); i++) {
//                shape.setPoint(i, sf::Vector2f(verts[i].x, verts[i].y));
//            }
//            shape.setFillColor(sf::Color(200, 50, 50));
//            shape.setOutlineColor(sf::Color::White);
//            shape.setOutlineThickness(2);
//            window.draw(shape);
//        }
//
//        // 중간 마찰 (초록)
//        {
//            std::vector<Vec2> verts = polygons[3].getWorldVertices();
//            sf::ConvexShape shape(verts.size());
//            for (size_t i = 0; i < verts.size(); i++) {
//                shape.setPoint(i, sf::Vector2f(verts[i].x, verts[i].y));
//            }
//            shape.setFillColor(sf::Color(50, 200, 50));
//            shape.setOutlineColor(sf::Color::White);
//            shape.setOutlineThickness(2);
//            window.draw(shape);
//        }
//
//        // 낮은 마찰 (파랑)
//        {
//            std::vector<Vec2> verts = polygons[4].getWorldVertices();
//            sf::ConvexShape shape(verts.size());
//            for (size_t i = 0; i < verts.size(); i++) {
//                shape.setPoint(i, sf::Vector2f(verts[i].x, verts[i].y));
//            }
//            shape.setFillColor(sf::Color(50, 50, 200));
//            shape.setOutlineColor(sf::Color::White);
//            shape.setOutlineThickness(2);
//            window.draw(shape);
//        }
//
//        // 회전 테스트 (노랑)
//        {
//            std::vector<Vec2> verts = polygons[5].getWorldVertices();
//            sf::ConvexShape shape(verts.size());
//            for (size_t i = 0; i < verts.size(); i++) {
//                shape.setPoint(i, sf::Vector2f(verts[i].x, verts[i].y));
//            }
//            shape.setFillColor(sf::Color(200, 200, 50));
//            shape.setOutlineColor(sf::Color::White);
//            shape.setOutlineThickness(2);
//            window.draw(shape);
//        }
//
//
//        window.display();
//    }
//
//    return 0;
//}
