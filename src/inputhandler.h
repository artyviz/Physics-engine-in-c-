#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>  // for std::sqrt
#include <optional>
#include "particle.h"
#include "constraints.h"

const float CLICK_TOLERANCE = 5.0f;

class InputHandler {
public:
    static void handle_mouse_click(const sf::Event& event, std::vector<Particle>& particles,
                                   std::vector<Constraints>& constraints) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                float mouse_x = static_cast<float>(event.mouseButton.x);
                float mouse_y = static_cast<float>(event.mouseButton.y);
                deactivate_nearest_constraint_at_point(mouse_x, mouse_y, particles, constraints);
            }
        }
    }

private:
    // Calculate shortest distance from point (px, py) to segment (x1, y1)-(x2, y2)
    inline static float point_to_segment_distance(float px, float py, float x1, float y1, float x2, float y2) {
        float ABx = x2 - x1;
        float ABy = y2 - y1;

        float APx = px - x1;
        float APy = py - y1;

        float BPx = px - x2;
        float BPy = py - y2;

        float AB_AP = ABx * APx + ABy * APy;
        float AB_AB = ABx * ABx + ABy * ABy;

        float t = AB_AP / AB_AB;

        if (t < 0.0f) {
            return std::sqrt(APx * APx + APy * APy);
        } else if (t > 1.0f) {
            return std::sqrt(BPx * BPx + BPy * BPy);
        } else {
            float proj_x = x1 + t * ABx;
            float proj_y = y1 + t * ABy;
            return std::sqrt((px - proj_x) * (px - proj_x) + (py - proj_y) * (py - proj_y));
        }
    }

    // Find the nearest constraint segment to the mouse position within CLICK_TOLERANCE
    inline static std::optional<Constraints*> find_nearest_constraint(float mouse_x, float mouse_y,
                                                                      std::vector<Constraints>& constraints) {
        Constraints* nearest_constraint = nullptr;
        float min_distance = CLICK_TOLERANCE;

        for (auto& constraint : constraints) {
            float distance = point_to_segment_distance(mouse_x, mouse_y,
                                    constraint.p1->position.x, constraint.p1->position.y,
                                    constraint.p2->position.x, constraint.p2->position.y);
            if (distance < min_distance) {
                min_distance = distance;
                nearest_constraint = &constraint;
            }
        }
        return nearest_constraint;
    }

    // Deactivate the nearest constraint found close to the mouse click
    static void deactivate_nearest_constraint_at_point(float mouse_x, float mouse_y, std::vector<Particle>& particles,
                                                       std::vector<Constraints>& constraints) {
        auto nearest = find_nearest_constraint(mouse_x, mouse_y, constraints);
        if (nearest) {
            nearest->deactivate();
        }
    }
};

#endif
