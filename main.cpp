#include "raylib-cpp.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

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
    float Vx = v.x * v.x - v.y * v.y - 1024.0f;
    float Vy = 2 * v.x * v.y;
    return { Vx, Vy };
}

void UpdateParticlePosition(Particle &particle, float deltaTime, int subSteps = 4) {
    raylib::Vector2 mousePos = GetMousePosition();
    float subSteptime = deltaTime / subSteps;
    for (int i = 0; i < subSteps; ++i) {
        raylib::Vector2 field = VectorFieldFunction(particle.position - mousePos);
        particle.velocity = field * subSteptime;
        particle.position += particle.velocity * subSteptime;
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
    const float drawScaleFactor = 2.0e-4f;
    for (int y = 0, ySkip = gridHeight; y < height; y += ySkip) {
        for (int x = 0, xSkip = gridWidth; x < width; x += xSkip) {
            raylib::Vector2 position = { static_cast<float>(x), static_cast<float>(y) };
            raylib::Vector2 vector = VectorFieldFunction(position - mousePos);
            DrawVector(position, vector * drawScaleFactor);
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
            UpdateParticlePosition(particle, deltaTime, 16);
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
