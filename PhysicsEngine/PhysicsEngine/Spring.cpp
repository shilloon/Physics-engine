#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Vec2.h"
#include "Particle.h"
#include "DistanceConstraint.h"
#include "Spring.h"

// === 범용 체인 생성 ===
void createChain(std::vector<Particle>& particles,
    std::vector<DistanceConstraint>& constraints,
    Vec2 startPos, int numLinks, float linkLength,
    Vec2 direction,
    float particleMass, float particleRadius, bool fixFirst = true) {

    int startIdx = particles.size();
    Vec2 normalizedDir = direction.normalize();

    for (int i = 0; i < numLinks; i++) {

        Vec2 pos = startPos + normalizedDir * (linkLength * i);
        float mass = (i == 0 && fixFirst) ? 10000.0f : particleMass;
        particles.push_back(Particle(pos, mass, particleRadius, 0.8f));

    }

    for (int i = 0; i < numLinks - 1; i++) {

        int idx1 = startIdx + i;
        int idx2 = startIdx + i + 1;

        Vec2 diff = particles[idx2].position - particles[idx1].position;
        float distance = diff.length();
        constraints.push_back(DistanceConstraint(idx1, idx2, distance,1.0f));

    }
}

// === 범용 로프 생성 ===
void createRope(std::vector<Particle>& particles,
    std::vector<Spring>& springs,
    Vec2 startPos, int numLinks, float linkLength,
    Vec2 direction,
    float particleMass, float particleRadius,
    float stiffness, float damping, bool fixFirst = true) {

    int startIdx = particles.size();
    Vec2 normalizedDir = direction.normalize();

    for (int i = 0; i < numLinks; i++) {

        Vec2 pos = startPos + normalizedDir * (linkLength * i);
        float mass = (i == 0 && fixFirst) ? 10000.0f : particleMass;
        particles.push_back(Particle(pos, mass, particleRadius, 0.8f));

    }

    for (int i = 0; i < numLinks - 1; i++) {

        int idx1 = startIdx + i;
        int idx2 = startIdx + i + 1;

        Vec2 diff = particles[idx2].position - particles[idx1].position;
        float distance = diff.length();
        springs.push_back(Spring(idx1, idx2, distance, stiffness, damping));

    }
}

void createCloth(std::vector<Particle>& particles,
    std::vector<DistanceConstraint>& constraints,
    Vec2 topLeft, int width, int height,
    float spacing, float particleMass, float particleRadius,
    bool pinCorners = true, bool pinTop = false) {

    int startIdx = particles.size();

    // === Create Particle (Grid) ===
    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            Vec2 pos = topLeft + Vec2(x * spacing, y * spacing);

            // 고정점
            float mass = particleMass;

            // 양쪽 모서리 고정
            if (pinCorners && y == 0 && (x == 0 || x == width - 1)) {

                mass = 10000.0f;

            }
            // 윗줄 전체 고정
            else if (pinTop && y == 0) {

                mass = 10000.0f;

            }

            particles.push_back(Particle(pos, mass, particleRadius, 0.8f));

        }

    }

    // === Create Constraint ===

    // 수평 연결
    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width - 1; x++) {

            int idx1 = startIdx + y * width + x;
            int idx2 = startIdx + y * width + (x + 1);

            Vec2 diff = particles[idx2].position - particles[idx1].position;
            float distance = diff.length();
            constraints.push_back(DistanceConstraint(idx1, idx2, distance, 0.9f));

        }

    }

    // 수직 연결
    for (int y = 0; y < height - 1; y++) {

        for (int x = 0; x < width; x++) {

            int idx1 = startIdx + y * width + x;
            int idx2 = startIdx + (y + 1) * width + x;

            Vec2 diff = particles[idx2].position - particles[idx1].position;
            float distance = diff.length();
            constraints.push_back(DistanceConstraint(idx1, idx2, distance, 0.9f));

        }

    }

    // 대각선 연결
    for (int y = 0; y < height - 1; y++) {

        for (int x = 0; x < height - 1; x++) {

            int idx1 = startIdx + y * width + x;
            int idx2 = startIdx + (y + 1) * width + (x + 1);

            Vec2 diff = particles[idx2].position - particles[idx1].position;
            float distance = diff.length();
            constraints.push_back(DistanceConstraint(idx1, idx2, distance, 0.9f));

        }

    }

    for (int y = 0; y < height - 1; y++) {

        for (int x = 1; x < width; x++) {

            int idx1 = startIdx + y * width + x;
            int idx2 = startIdx + (y + 1) * width + (x - 1);

            Vec2 diff = particles[idx2].position - particles[idx1].position;
            float distance = diff.length();
            constraints.push_back(DistanceConstraint(idx1, idx2, distance, 0.9f));

        }

    }

}

int main() {

    const float WIDTH = 800;
    const float HEIGHT = 600;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Physics Engine - Better Chains!");
    window.setFramerateLimit(60);

    std::vector<Particle> particles;
    std::vector<DistanceConstraint> constraints;
    std::vector<Spring> springs;

    Vec2 gravity(0, 9.8f * 100);

    auto createScenarios = [&]() {

        constraints.clear();
        springs.clear();
        particles.clear();

        // 작은 천
        createCloth(particles, constraints, Vec2(50, 50), 10, 8, 15.0f, 0.5f, 5.0f, true, false);

        // 큰 천
        createCloth(particles, constraints, Vec2(250, 50), 15, 12, 15.0f, 0.5f, 5.0f, false, true);

        // 깃발
        createCloth(particles, constraints, Vec2(550, 50), 12, 8, 15.0f, 0.5f, 5.0f, false, false);

        // 깃발 왼쪽 고정
        for (int y = 0; y < 8; y++) {

            particles[particles.size() - 12 * 8 + y * 12].mass = 10000.0f;

        }

    };

    createScenarios();

    sf::Clock clock;
    Particle* draggedParticle = nullptr;
    Vec2 mouseOffset;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

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

                // 천 찢기
                if (event.mouseButton.button == sf::Mouse::Right) {

                    Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);

                    // 마우스 근처 Constraint 제거
                    constraints.erase(
                        std::remove_if(constraints.begin(), constraints.end(), [&](const DistanceConstraint& c) {

                            Vec2 p1Pos = particles[c.p1Index].position;
                            Vec2 p2Pos = particles[c.p2Index].position;
                            Vec2 midPoint = (p1Pos + p2Pos) * 0.5f;

                            float dist = (midPoint - mousePos).length();
                            return dist < 20.0f;

                        }),
                        constraints.end()
                    );

                    std::cout << "Rip constraints: " << constraints.size() << std::endl;

                }

            }

            if (event.type == sf::Event::MouseButtonReleased) {
            
                draggedParticle = nullptr;
        
            }

            if (event.type == sf::Event::KeyPressed) {

                if (event.key.code == sf::Keyboard::R) {

                    createScenarios();
                
                }

                if (event.key.code == sf::Keyboard::W) {

                    for (auto& p : particles) {

                        if (p.mass < 100) {

                            p.applyForce(Vec2(0, -5000));

                        }

                    }

                    std::cout << "Up" << std::endl;

                }


                if (event.key.code == sf::Keyboard::A) {

                    for (auto& p : particles) {

                        if (p.mass < 100) {

                            p.applyForce(Vec2(-5000, 0));

                        }

                    }

                    std::cout << "Left" << std::endl;

                }


                if (event.key.code == sf::Keyboard::S) {

                    for (auto& p : particles) {

                        if (p.mass < 100) {

                            p.applyForce(Vec2(0, 5000));

                        }

                    }

                    std::cout << "down" << std::endl;

                }


                if (event.key.code == sf::Keyboard::D) {

                    for (auto& p : particles) {

                        if (p.mass < 100) {

                            p.applyForce(Vec2(5000, 0));

                        }

                    }

                    std::cout << "Right" << std::endl;

                }

                if (event.key.code == sf::Keyboard::Space) {

                    for (auto& p : particles) {
                        
                        if (p.mass < 100) {
                        
                            p.velocity += Vec2((rand() % 200 - 100), -300);
                        
                        }

                    }

                }

            }

        }

        if (draggedParticle != nullptr) {

            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            Vec2 mousePos(mousePixel.x, mousePixel.y);
            draggedParticle->position = mousePos + mouseOffset;
            draggedParticle->velocity = Vec2(0, 0);
        
        }

        float dt = clock.restart().asSeconds();

        // Gravity
        for (auto& p : particles) {

            if (p.mass < 100) {
            
                p.applyForce(gravity * p.mass);
            
            }

        }

        // Spring
        for (auto& spring : springs) {
        
            spring.applyForce(particles);
        
        }

        // Physics Update
        for (auto& p : particles) {

            p.update(dt);
            p.checkBounds(WIDTH, HEIGHT);
        
        }

        // 제약 해결
        int iterations = 15;
        for (int iter = 0; iter < iterations; iter++) {
        
            for (auto& constraint : constraints) {
            
                constraint.solve(particles);
            
            }

        }

        // === Rendering ===
        window.clear(sf::Color::Black);

        for (auto& constraint : constraints) {

            constraint.draw(window, particles);

        }

        for (auto& spring : springs) {

            spring.draw(window, particles);
        
        }

        for (auto& p : particles) {

            sf::CircleShape circle(p.radius);
            circle.setFillColor(p.mass > 100 ? sf::Color::Red : sf::Color::Green);
            circle.setPosition(p.position.x - p.radius, p.position.y - p.radius);
            window.draw(circle);

        }

        window.display();
    }

    return 0;
}