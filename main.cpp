#include "raylib-cpp.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

struct Particle
{
    raylib::Vector2 position, velocity;
};

// Define screen dimensions
const int screenWidth = 800;
const int screenHeight = 600;

// Define grid dimensions
const int gridWidth = 60;
const int gridHeight = 45;
const float vectorLength = 15.0f;

// Function to generate a random vector direction
Vector2 GenerateRandomDirection() {
    float angle = GetRandomValue(0, 360) * DEG2RAD;
    return { std::cos(angle), std::sin(angle) };
}

Vector2 VectorFieldFunction(float x, float y) {
    // Vx(x, y) = x^2 - y^2 - 4
    // Vy(x, y) = 2 * x * y
    float Vx = x * x - y * y - 4;
    float Vy = 2 * x * y;
    return { Vx, Vy };
}

void UpdateParticlePosition(Particle &particle, float deltaTime, int subSteps = 4) {
    // Get grid scale
    float widthScale = (float)screenWidth / gridWidth;
    float heightScale = (float)screenHeight / gridHeight;

    // Convert mouse from screen coords → grid coords
    raylib::Vector2 mousePos = GetMousePosition();
    mousePos.x /= widthScale;
    mousePos.y /= heightScale;

    // Convert particle position from screen space to grid space
    float particleX = particle.position.x / widthScale;
    float particleY = particle.position.y / heightScale;

    float subSteptime = deltaTime / subSteps;
    for (int i = 0; i < subSteps; ++i) {
        // Get the vector field at the particle's current position in grid space
        raylib::Vector2 field = VectorFieldFunction(particleX - mousePos.x, particleY - mousePos.y);

        // Move the particle in the direction of the field
        particle.velocity = field.Normalize() * 10.0f; // Apply the vector field directly to velocity (you could also scale it for effect)

        // Update the particle position in screen space
        particle.position.x += particle.velocity.x * widthScale * subSteptime; // Convert back to screen space
        particle.position.y += particle.velocity.y * heightScale * subSteptime;
    }
}

// Function to draw a vector as an arrow
void DrawVector(raylib::Vector2 position, raylib::Vector2 direction, float length) {
    // Normalize the direction vector
    direction = Vector2Normalize(direction);
    
    // Calculate the endpoint of the vector
    raylib::Vector2 endPos = { position.x + direction.x * length, position.y + direction.y * length };
    
    // Draw the line for the vector
    DrawLineV(position, endPos, BLUE);

    // Arrowhead size
    float arrowSize = 6.0f;  // Adjust arrowhead size as needed
    float angle = std::atan2(direction.y, direction.x);  // Get the angle of the vector

    // Calculate the two points for the arrowhead
    raylib::Vector2 arrowLeft = { endPos.x - arrowSize * cos(angle - 0.4f), endPos.y - arrowSize * sin(angle - 0.4f) };
    raylib::Vector2 arrowRight = { endPos.x - arrowSize * cos(angle + 0.4f), endPos.y - arrowSize * sin(angle + 0.4f) };

    // Draw the arrowhead (as a triangle)
    DrawLineV(arrowLeft, endPos, BLUE);
    DrawLineV(arrowRight, endPos, BLUE);
}

void DrawVectorField(int gridWidth, int gridHeight) {
    float widthScale = (float)screenWidth / gridWidth;
    float heightScale = (float)screenHeight / gridHeight;

    // Convert mouse from screen coords → grid coords
    raylib::Vector2 mousePos = GetMousePosition();
    mousePos.x /= widthScale;
    mousePos.y /= heightScale;

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            raylib::Vector2 position = { x * widthScale, y * heightScale };

            // Shift so the field is centered at the mouse position
            float fieldX = static_cast<float>(x) - mousePos.x;
            float fieldY = static_cast<float>(y) - mousePos.y;

            Vector2 vector = VectorFieldFunction(fieldX, fieldY);

            DrawVector(position, vector, vectorLength);
        }
    }
}

int main() {
    raylib::Window window(screenWidth, screenHeight, "Vector Field Generator");
    window.SetTargetFPS(60);
    
    std::vector<Particle> particles(1000);
    std::generate(particles.begin(), particles.end(), []() -> Particle {
        return {{ GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight) }, { 0.0f, 0.0f }};
    });


    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        for (auto &particle : particles) {
            UpdateParticlePosition(particle, deltaTime, 16);
        }
        BeginDrawing();
        window.ClearBackground(RAYWHITE);

        raylib::Vector2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);
        // Loop over grid positions
        raylib::Vector2 mousePosition = GetMousePosition();
        DrawVectorField(gridWidth, gridHeight);
        
        for (const auto& particle : particles) {
            DrawCircleV(particle.position, 5.0f, RED);
        }
        DrawText("Vector Field Visualization", 10, 10, 20, DARKGRAY);
        DrawFPS(30, 30);
        EndDrawing();
    }
    return 0;
}
