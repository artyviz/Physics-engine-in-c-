#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f prev_pos;
    sf::Vector2f acceleration;
    bool is_attached;

    Particle(float x, float y, bool attached = false)
        : position(x, y), prev_pos(x, y), acceleration(0, 0), is_attached(attached) {}

    void apply_force(const sf::Vector2f& force);
    void update(float time_step);
    void bounds(float width, float height);
};

void Particle::apply_force(const sf::Vector2f& force) {
    if (!is_attached) {
        acceleration += force;
    }
}

void Particle::update(float time_step) {
    if (!is_attached) {
        sf::Vector2f velocity = position - prev_pos;
        prev_pos = position;
        position += velocity + acceleration * time_step * time_step;
        acceleration = sf::Vector2f(0, 0);
    }
}

void Particle::bounds(float width, float height) {
    if (position.x < 0) position.x = 0;
    if (position.x > width) position.x = width;
    if (position.y < 0) position.y = 0;
    if (position.y > height) position.y = height;
}

#endif
