// RigidBody.h
#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <cmath>

class RigidBody {
public:
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    float angle;
    float angularVelocity;
    float angularAcceleration;

    float width, height;
    float mass;
    float inertia;
    float restitution;

    RigidBody(Vec2 position, float width, float height, float mass, float restitution)
        : position(position), width(width), height(height), mass(mass), restitution(restitution),
        velocity(0, 0), acceleration(0, 0),
        angle(0), angularVelocity(0), angularAcceleration(0)
    {
        // 관성 모멘트 (직사각형)
        inertia = (1.0f / 12.0f) * mass * (width * width + height * height);
    }

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

    void update(float dt) {
        velocity += acceleration * dt;
        position += velocity * dt;

        angularVelocity += angularAcceleration * dt;
        angle += angularVelocity * dt;

        acceleration = Vec2(0, 0);
        angularAcceleration = 0;
    }

    void checkBounds(float width, float height) {
        float halfW = this->width / 2;
        float halfH = this->height / 2;

        // 간단한 경계 체크 (회전 무시)
        if (position.x - halfW < 0) {
            position.x = halfW;
            velocity.x *= -restitution;
        }
        if (position.x + halfW > width) {
            position.x = width - halfW;
            velocity.x *= -restitution;
        }
        if (position.y - halfH < 0) {
            position.y = halfH;
            velocity.y *= -restitution;
        }
        if (position.y + halfH > height) {
            position.y = height - halfH;
            velocity.y *= -restitution;
            angularVelocity *= 0.9f;  // 바닥 마찰
        }
    }

    bool containsRotated(Vec2 point) const {
        Vec2 localPoint = point - position;
        float cos_a = std::cos(-angle);
        float sin_a = std::sin(-angle);

        float localX = localPoint.x * cos_a - localPoint.y * sin_a;
        float localY = localPoint.x * sin_a + localPoint.y * cos_a;

        return std::abs(localX) <= width / 2 && std::abs(localY) <= height / 2;
    }

    void draw(sf::RenderWindow& window) const {
        sf::RectangleShape rect(sf::Vector2f(width, height));
        rect.setOrigin(width / 2, height / 2);
        rect.setPosition(position.x, position.y);
        rect.setRotation(angle * 180.0f / 3.14159f);
        rect.setFillColor(sf::Color(100, 200, 100));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2);
        window.draw(rect);

        // 방향 표시 (빨간 선)
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        Vec2 dirEnd = position + Vec2(cos_a * width / 2, sin_a * width / 2);

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color::Red),
            sf::Vertex(sf::Vector2f(dirEnd.x, dirEnd.y), sf::Color::Red)
        };
        window.draw(line, 2, sf::Lines);
    }
};