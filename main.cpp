#include "raylib-cpp.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <functional>

struct Particle
{
    raylib::Vector2 position, velocity;
};

const int screenWidth = 800;
const int screenHeight = 600;

const int gridWidth = 24;
const int gridHeight = 18;
const float vectorLength = 12.0f;

raylib::Vector2 VectorFieldFunction(const raylib::Vector2 &v) {
    // Vx(x, y) = x^2 - y^2 - 4
    // Vy(x, y) = 2 * x * y
    const float magnetLength = 4096.0f;
    const float strength = 1e-4f;
    float Vx = v.x * v.x - v.y * v.y - magnetLength;
    float Vy = 2 * v.x * v.y;
    return (raylib::Vector2 { Vx, Vy } * strength);
}

struct Derivative {
    raylib::Vector2 dPosition; // velocity
    raylib::Vector2 dVelocity; // acceleration
};

struct State {
    raylib::Vector2 position;
    raylib::Vector2 velocity;
};

Derivative Evaluate(const State &state) {
    Derivative output;
    output.dPosition = state.velocity;
    output.dVelocity = VectorFieldFunction(state.position); // already offset externally
    return output;
}


State IntegrateRK4(const State &state, float h) {
    Derivative k1 = Evaluate(state);

    State s2;
    s2.position = state.position + k1.dPosition * (h * 0.5f);
    s2.velocity = state.velocity + k1.dVelocity * (h * 0.5f);
    Derivative k2 = Evaluate(s2);

    State s3;
    s3.position = state.position + k2.dPosition * (h * 0.5f);
    s3.velocity = state.velocity + k2.dVelocity * (h * 0.5f);
    Derivative k3 = Evaluate(s3);

    State s4;
    s4.position = state.position + k3.dPosition * h;
    s4.velocity = state.velocity + k3.dVelocity * h;
    Derivative k4 = Evaluate(s4);

    State result = state;
    result.position += (k1.dPosition + k2.dPosition * 2.0f + k3.dPosition * 2.0f + k4.dPosition) * (h / 6.0f);
    result.velocity += (k1.dVelocity + k2.dVelocity * 2.0f + k3.dVelocity * 2.0f + k4.dVelocity) * (h / 6.0f);

    return result;
}


void UpdateParticlePosition(Particle &particle, float deltaTime, int subSteps = 4) {
    raylib::Vector2 mousePos = GetMousePosition();
    float subSteptime = deltaTime / subSteps;
    State state{ particle.position - mousePos, particle.velocity };
    for (int i = 0; i < subSteps; ++i) {
        state = IntegrateRK4(state, subSteptime);
    }
    // re-apply mouse offset when storing position back
    particle.position = state.position + mousePos;
    particle.velocity = state.velocity;

    const float maxSpeed = 10.0f;
    float speed = particle.velocity.Length();
    if (speed > maxSpeed) {
        particle.velocity = particle.velocity.Normalize() * maxSpeed;
    }
}

void DrawVector(raylib::Vector2 position, raylib::Vector2 direction) {
    float intensity = direction.Clamp(-1.0f, 1.0f).Length();
    auto arrowColor = ColorLerp(GREEN, RED, intensity);
    direction = Vector2Normalize(direction);
    
    raylib::Vector2 endPos = { position.x + direction.x * vectorLength, position.y + direction.y * vectorLength };
    
    float arrowSize = 4.0f;
    float angle = std::atan2(direction.y, direction.x);

    DrawLineV(position, endPos, arrowColor);

    // Calculate the two points for the arrowhead
    raylib::Vector2 arrowLeft = { endPos.x - arrowSize * cos(angle - 0.4f), endPos.y - arrowSize * sin(angle - 0.4f) };
    raylib::Vector2 arrowRight = { endPos.x - arrowSize * cos(angle + 0.4f), endPos.y - arrowSize * sin(angle + 0.4f) };

    // Draw the arrowhead (as a triangle)
    DrawLineV(arrowLeft, endPos, arrowColor);
    DrawLineV(arrowRight, endPos, arrowColor);
}

void DrawVectorField(int gridWidth, int gridHeight) {
    raylib::Vector2 mousePos = GetMousePosition();
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    for (int y = 0, ySkip = gridHeight; y < height; y += ySkip) {
        for (int x = 0, xSkip = gridWidth; x < width; x += xSkip) {
            raylib::Vector2 position = { static_cast<float>(x), static_cast<float>(y) };
            raylib::Vector2 vector = VectorFieldFunction(position - mousePos);
            DrawVector(position, vector);
        }
    }
}

int main() {
    raylib::Window window(screenWidth, screenHeight, "Vector Field Generator");
    window.SetTargetFPS(60);
    
    std::vector<Particle> particles(1000);
    std::generate(particles.begin(), particles.end(), []() -> Particle {
        return {
            {
                static_cast<float>(GetRandomValue(0, screenWidth)),
                static_cast<float>(GetRandomValue(0, screenHeight))
            },
            { 0.0f, 0.0f }
        };
    });

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        for (auto &particle : particles) {
            UpdateParticlePosition(particle, deltaTime, 4);
        }
        window.BeginDrawing();
        window.ClearBackground(RAYWHITE);

        raylib::Vector2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);
        // Loop over grid positions
        raylib::Vector2 mousePosition = GetMousePosition();
        DrawVectorField(gridWidth, gridHeight);
        
        for (const auto& particle : particles) {
            Vector2 rectSize = { 5.0f, 5.0f };
            DrawRectangleV(particle.position, rectSize, BLUE);
        }
        DrawText("Vector Field Visualization", 10, 10, 20, DARKGRAY);
        DrawFPS(30, 30);
        window.EndDrawing();
    }
    return 0;
}
