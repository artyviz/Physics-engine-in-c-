#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>

#include "particle.h"
#include "constraints.h"
#include "inputhandler.h"

const int WIDTH = 1080;
const int HEIGHT = 640;
const float PARTICLE_RADIUS = 10.0f;
const float GRAVITY = 10.0f;
const float TIME_STEP = 0.016f;   // ~60 FPS step (instead of 0.1f, smoother simulation)
const float DAMPING = 0.99f;

const int ROW = 10;
const int COL = 10;
const float REST_DISTANCE = 30.0f;

class Renderer {
public:
    void drawParticles(sf::RenderWindow& window, const std::vector<Particle>& particles) {
        for (const auto& particle : particles) {
            sf::CircleShape circle(PARTICLE_RADIUS);
            circle.setFillColor(sf::Color::White);
            circle.setPosition(particle.position.x - PARTICLE_RADIUS, particle.position.y - PARTICLE_RADIUS);
            window.draw(circle);
        }
    }

    void drawConstraints(sf::RenderWindow& window, const std::vector<Constraints>& constraints, const Constraints* highlightedConstraint = nullptr) {
        sf::VertexArray lines(sf::Lines);
        for (const auto& constraint : constraints) {
            if (!constraint.active) continue;
            sf::Color color = (constraint == highlightedConstraint) ? sf::Color::Red : sf::Color::White;
            lines.append(sf::Vertex(constraint.p1->position, color));
            lines.append(sf::Vertex(constraint.p2->position, color));
        }
        window.draw(lines);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cloth Simulation");
    window.setFramerateLimit(60);

    std::vector<Particle> particles;
    std::vector<Constraints> constraints;

    // Initialize particles grid
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            float x = col * REST_DISTANCE + WIDTH / 3.f;
            float y = row * REST_DISTANCE + HEIGHT / 3.f;
            bool pinned = (row == 0);  // Pin top row
            particles.emplace_back(x, y, pinned);
        }
    }

    // Initialize constraints (horizontal and vertical)
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            if (col < COL - 1) {
                constraints.emplace_back(&particles[row * COL + col], &particles[row * COL + col + 1]);
            }
            if (row < ROW - 1) {
                constraints.emplace_back(&particles[row * COL + col], &particles[(row + 1) * COL + col]);
            }
        }
    }

    Renderer renderer;
    Constraints* highlightedConstraint = nullptr;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    float mouse_x = static_cast<float>(event.mouseButton.x);
                    float mouse_y = static_cast<float>(event.mouseButton.y);
                    InputHandler::deactivate_nearest_constraint_at_point(mouse_x, mouse_y, particles, constraints);
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                float mouse_x = static_cast<float>(event.mouseMove.x);
                float mouse_y = static_cast<float>(event.mouseMove.y);
                highlightedConstraint = InputHandler::find_nearest_constraint(mouse_x, mouse_y, constraints);
            }
        }

        // Physics update
        for (auto& particle : particles) {
            particle.apply_force(sf::Vector2f(0.f, GRAVITY));
            particle.update(TIME_STEP);
            particle.position *= DAMPING;
            particle.bounds(WIDTH, HEIGHT);
        }

        // Constraint satisfaction iterations (improve cloth rigidity)
        const int CONSTRAINT_ITERATIONS = 5;
        for (int i = 0; i < CONSTRAINT_ITERATIONS; i++) {
            for (auto& constraint : constraints) {
                constraint.satisfy();
            }
        }

        // Rendering
        window.clear(sf::Color::Black);
        renderer.drawParticles(window, particles);
        renderer.drawConstraints(window, constraints, highlightedConstraint);
        window.display();
    }

    return 0;
}
