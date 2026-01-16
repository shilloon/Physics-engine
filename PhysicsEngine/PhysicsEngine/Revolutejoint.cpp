#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Vec2.h"
#include "Polygon.h"
#include "Revolutejoint.h"

// === 정다각형 생성 ===
std::vector<Vec2> createRegularPolygon(int sides, float radius) {
    std::vector<Vec2> vertices;
    for (int i = 0; i < sides; i++) {
        float angle = (2 * 3.14159f * i) / sides;
        float x = std::cos(angle) * radius;
        float y = std::sin(angle) * radius;
        vertices.push_back(Vec2(x, y));
    }
    return vertices;
}

// === 박스 생성 ===
std::vector<Vec2> createBox(float width, float height) {
    std::vector<Vec2> vertices;
    float halfW = width / 2;
    float halfH = height / 2;

    vertices.push_back(Vec2(-halfW, -halfH));
    vertices.push_back(Vec2(halfW, -halfH));
    vertices.push_back(Vec2(halfW, halfH));
    vertices.push_back(Vec2(-halfW, halfH));

    return vertices;
}

// === 충돌 정보 ===
struct CollisionInfo {
    bool collided;
    Vec2 normal;
    float depth;

    CollisionInfo() : collided(false), depth(0) {}
};

// === 투영 ===
void projectVertices(const std::vector<Vec2>& vertices, Vec2 axis, float& min, float& max) {
    min = max = vertices[0].dot(axis);
    for (size_t i = 1; i < vertices.size(); i++) {
        float projection = vertices[i].dot(axis);
        if (projection < min) min = projection;
        if (projection > max) max = projection;
    }
}

// === SAT 충돌 감지 ===
CollisionInfo checkPolygonCollision(const Polygon& a, const Polygon& b) {
    CollisionInfo info;

    std::vector<Vec2> verticesA = a.getWorldVertices();
    std::vector<Vec2> verticesB = b.getWorldVertices();
    std::vector<Vec2> axesA = a.getAxes();
    std::vector<Vec2> axesB = b.getAxes();

    float minOverlap = FLT_MAX;
    Vec2 minAxis;

    std::vector<Vec2> allAxes;
    allAxes.insert(allAxes.end(), axesA.begin(), axesA.end());
    allAxes.insert(allAxes.end(), axesB.begin(), axesB.end());

    for (const Vec2& axis : allAxes) {
        float minA, maxA, minB, maxB;
        projectVertices(verticesA, axis, minA, maxA);
        projectVertices(verticesB, axis, minB, maxB);

        if (maxA < minB || maxB < minA) {
            info.collided = false;
            return info;
        }

        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axis;

            Vec2 centerDiff = b.position - a.position;
            if (centerDiff.dot(axis) < 0) {
                minAxis = minAxis * -1.0f;
            }
        }
    }

    info.collided = true;
    info.normal = minAxis.normalize();
    info.depth = minOverlap;

    return info;
}

// === 충돌 반응 (간단 버전) ===
void resolvePolygonCollision(Polygon& a, Polygon& b, const CollisionInfo& info) {
    if (info.depth < 0.01f) return;

    bool aIsStatic = std::isinf(a.mass);
    bool bIsStatic = std::isinf(b.mass);

    if (aIsStatic && bIsStatic) return;

    // 위치 보정
    float percent = 0.3f;
    Vec2 correction = info.normal * info.depth * percent;

    if (!aIsStatic && !bIsStatic) {
        float totalMass = a.mass + b.mass;
        a.position -= correction * (b.mass / totalMass);
        b.position += correction * (a.mass / totalMass);
    }
    else if (aIsStatic) {
        b.position += correction;
    }
    else {
        a.position -= correction;
    }

    // 속도 반응 (간단히)
    Vec2 contactPoint = (a.position + b.position) * 0.5f;
    Vec2 rA = contactPoint - a.position;
    Vec2 rB = contactPoint - b.position;

    Vec2 velA = a.velocity + Vec2(-rA.y * a.angularVelocity, rA.x * a.angularVelocity);
    Vec2 velB = b.velocity + Vec2(-rB.y * b.angularVelocity, rB.x * b.angularVelocity);
    Vec2 relativeVel = velB - velA;

    float velAlongNormal = relativeVel.dot(info.normal);
    if (velAlongNormal > 0) return;

    float e = (a.restitution + b.restitution) / 2;

    float invMassA = aIsStatic ? 0.0f : 1.0f / a.mass;
    float invMassB = bIsStatic ? 0.0f : 1.0f / b.mass;
    float invInertiaA = aIsStatic ? 0.0f : 1.0f / a.inertia;
    float invInertiaB = bIsStatic ? 0.0f : 1.0f / b.inertia;

    float rACrossN = rA.cross(info.normal);
    float rBCrossN = rB.cross(info.normal);

    float denominator = invMassA + invMassB +
        (rACrossN * rACrossN) * invInertiaA +
        (rBCrossN * rBCrossN) * invInertiaB;

    float j = -(1 + e) * velAlongNormal / denominator;
    Vec2 impulse = info.normal * j;

    if (!aIsStatic) {
        a.velocity -= impulse * invMassA;
        a.angularVelocity -= rA.cross(impulse) * invInertiaA;
    }
    if (!bIsStatic) {
        b.velocity += impulse * invMassB;
        b.angularVelocity += rB.cross(impulse) * invInertiaB;
    }

}

int main() {

    const float WIDTH = 800;
    const float HEIGHT = 600;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Revolute Joint Test");
    window.setFramerateLimit(60);

    std::vector<Polygon> polygons;
    std::vector<RevoluteJoint> joints;
    Vec2 gravity(0, 9.8f * 100);

    polygons.reserve(20);
    joints.reserve(10);

    // === Pendulm ===

    // 고정점
    std::vector<Vec2> anchor1 = createRegularPolygon(6, 8);
    Polygon fixedPoint1(Vec2(150, 100), anchor1, INFINITY, 0.3f);
    fixedPoint1.inertia = INFINITY;
    polygons.push_back(fixedPoint1);

    // 진자
    std::vector<Vec2> pendulm = createBox(20, 60);
    Polygon pendulmBody(Vec2(150, 100), pendulm, 1.5f, 0.5f);
    polygons.push_back(pendulmBody);

    // 조인트 연결
    RevoluteJoint pendulmJoint(&polygons[0], &polygons[1], Vec2(150, 100));
    joints.push_back(pendulmJoint);

    // === Chain ===
    
    // 고정점
    std::vector<Vec2> anchor2 = createRegularPolygon(6, 8);
    Polygon fixedPoint2(Vec2(400, 100), anchor2, INFINITY, 0.3f);
    fixedPoint2.inertia = INFINITY;
    polygons.push_back(fixedPoint2);

    // 링크
    int chainLength = 5;
    for (int i = 0; i < chainLength; i++) {

        std::vector<Vec2> link = createBox(15, 40);
        Polygon linkBody(Vec2(400, 150 + i * 50), link, 0.8f, 0.4f);
        polygons.push_back(linkBody);

        // 조인트 연결
        if (i == 0) {

            // 첫 번째는 고정점과 연결
            RevoluteJoint joint(&polygons[2], &polygons[3 + i], Vec2(400, 130));
            joints.push_back(joint);

        }
        else {

            // 나머지는 이전 링크와 연결
            Vec2 jointPos(400, 150 + (i - 1) * 50 + 20);
            RevoluteJoint joint(&polygons[2 + i], &polygons[3 + i], jointPos);
            joints.push_back(joint);

        }
    }

    // === Door ===

    // 문틀
    std::vector<Vec2> doorFrame = createBox(10, 100);
    Polygon frame(Vec2(650, 200), doorFrame, INFINITY, 0.3f);
    frame.inertia = INFINITY;
    polygons.push_back(frame);

    // 문
    std::vector<Vec2> door = createBox(60, 100);
    Polygon doorBody(Vec2(680, 200), door, 2.0f, 0.3f);
    polygons.push_back(doorBody);

    // 경첩 ( -90 ~ 90 )
    float lowerLimit = -1.57f; // -90
    float upperLimit = 1.57f; // 90
    // 각도 제한 활성화
    RevoluteJoint doorHinge(&polygons[8], &polygons[9], Vec2(650, 150), true, lowerLimit, upperLimit);
    joints.push_back(doorHinge);

    // === 선풍기 ===

    // 선풍기 몸체
    std::vector<Vec2> fanBase = createRegularPolygon(8, 15);
    Polygon fanBody(Vec2(650, 450), fanBase, INFINITY, 0.3f);
    fanBody.inertia = INFINITY;
    polygons.push_back(fanBody);

    // 선풍기 날개
    std::vector<Vec2> blade = createBox(80, 8);
    Polygon fanBlade(Vec2(650, 450), blade, 0.5f, 0.3f);
    polygons.push_back(fanBlade);

    // 모터
    float motorSpeed = 5.0f;
    float maxTorque = 10.0f;
    RevoluteJoint fanMotor(&polygons[10], &polygons[11], Vec2(650, 450), false, 0, 0, true, motorSpeed, maxTorque);
    joints.push_back(fanMotor);

    // === Sminulation ===

    sf::Clock clock;
    Polygon* draggedPolygon = nullptr;
    Vec2 dragOffset;

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {

                window.close();

            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);

                for (auto& poly : polygons) {

                    if (!std::isinf(poly.mass) && poly.contains(mousePos)) {

                        draggedPolygon = &poly;
                        dragOffset = poly.position - mousePos;
                        poly.velocity = Vec2(0, 0);
                        poly.angularVelocity = 0;
                        break;

                    }

                }

            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {

                draggedPolygon = nullptr;

            }

            if (event.type == sf::Event::KeyPressed) {

                if (event.key.code == sf::Keyboard::R) {

                    for (auto& poly : polygons) {

                        poly.velocity = Vec2(0, 0);
                        poly.angularVelocity = 0;

                    }
           
                }

                // 모터 on / off
                if (event.key.code == sf::Keyboard::M) {

                    joints.back().enableMotor = !joints.back().enableMotor;
                    std::cout << "Motor Speed: " << joints.back().motorSpeed << std::endl;

                }

                // 방향키 : 모터 속도 조절
                if (event.key.code == sf::Keyboard::Up) {

                    joints.back().motorSpeed += 1.0f;

                }
                if (event.key.code == sf::Keyboard::Down) {

                    joints.back().motorSpeed -= 1.0f;
                    std::cout << "Motor Speed: " << joints.back().motorSpeed << std::endl;

                }

            }

        }

        if (draggedPolygon != nullptr) {

            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            Vec2 mousePos(mousePixel.x, mousePixel.y);
            draggedPolygon->position = mousePos + dragOffset;

        }

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.02f);

        // Physics Update
        for (auto& poly : polygons) {

            if (draggedPolygon != &poly && !std::isinf(poly.mass)) {

                poly.applyForce(gravity * poly.mass);
                poly.update(dt);
                poly.checkBounds(WIDTH, HEIGHT);

            }

        }

        // 조인트 제약 해결
        int iterations = 10;
        for (int iter = 0; iter < iterations; iter++) {

            for (auto& joint : joints) {

                joint.solve();

            }

        }

        // === Rendering ===
        window.clear(sf::Color::Black);

        // 다각형
        for (const auto& poly : polygons) {

            poly.draw(window);

        }

        // 조인트
        for (const auto& joint : joints) {

            joint.draw(window);

        }

        window.display();

    }

    return 0;

}