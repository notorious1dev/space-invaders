#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "utilities.h"

#define DEBUG 1

typedef struct {
    Vector2 Position;
    Vector2 Velocity;
    bool isActive;
} Object;

typedef enum {
    GAME_PLAYING,
    GAME_DEAD_SCREEN,
} GameState;

GameState currentGameState = GAME_PLAYING;

// Screen Settings
const int width = 600;
const int height = 800;
float dt = 0;
Color background_color = (Color){0, 0, 10, 255};

// Player Statistics
int points = 0;
int health = 3;

// Player Configuration
#define PLAYER_RADIUS 50
float player_speed = 500.0f;

// Gameplay Configuration
float hardness_multiplier = 1.0f;
int last_ten_points = 0;
float delta = 0;

// Bullets Configuration
#define BULLETS_AMOUNT 10
#define BULLETS_RADIUS 15
#define FIRE_COOLDOWN_STANDART_VALUE 0.45f
#define BULLET_SPAWN_Y_OFFSET -30
#define BULLET_SPEED 20.0f
float fire_cooldown = 0;
Object bullets[BULLETS_AMOUNT] = {0};

// Enemies Configuration
#define ENEMIES_AMOUNT 7
#define ENEMIES_SPEED 10.0f
#define ENEMY_RADIUS 25
Object enemies[ENEMIES_AMOUNT] = {0};

void Start() {
    InitWindow(width, height, "Space-Invaders");
    SetTargetFPS(60);
}

int main() {
    Start();

    Object player = {
        .Position = (Vector2){width / 2, height / 1.2f},
        .Velocity = (Vector2){0, 0},
        .isActive = true
    };

    // Bullets initialization
    for (int i = 0; i < BULLETS_AMOUNT; i++) {
        bullets[i] = (Object){.Position = {0, 0}, .Velocity = {0, 0}, .isActive = false};
    }

    // Enemies initialization
    for (int i = 0; i < ENEMIES_AMOUNT; i++) {
        Vector2 enemy_spawn_position = GetEnemySpawnVector(height, width);
        enemies[i] = (Object){.Position = enemy_spawn_position, .Velocity = {0, ENEMIES_SPEED}, .isActive = true};
    }

    while (!WindowShouldClose()) {
        dt = GetFrameTime();

        // Game State switch
        if (health <= 0)
            currentGameState = GAME_DEAD_SCREEN;
        else
            currentGameState = GAME_PLAYING;

        switch (currentGameState) {
            case GAME_PLAYING:
                // Player Movement
                player.Velocity = ReadPlayerMovementInput();
                player.Position.x += player.Velocity.x * player_speed * dt;

                // Boundaries
                if (player.Position.x <= 50) player.Position.x = 50;
                if (player.Position.x >= 550) player.Position.x = 550;

                // Shooting
                fire_cooldown -= dt;
                if (IsKeyDown(KEY_UP) && fire_cooldown <= 0) {
                    for (int i = 0; i < BULLETS_AMOUNT; i++) {
                        if (!bullets[i].isActive) {
                            bullets[i].Position.x = player.Position.x;
                            bullets[i].Position.y = player.Position.y + BULLET_SPAWN_Y_OFFSET;
                            bullets[i].Velocity = (Vector2){0, -BULLET_SPEED};
                            bullets[i].isActive = true;
                            fire_cooldown = FIRE_COOLDOWN_STANDART_VALUE;
                            break;
                        }
                    }
                }

                // Bullet Update
                for (int i = 0; i < BULLETS_AMOUNT; i++) {
                    if (!bullets[i].isActive) continue;
                    bullets[i].Position.x += bullets[i].Velocity.x * BULLET_SPEED * dt * hardness_multiplier;
                    bullets[i].Position.y += bullets[i].Velocity.y * BULLET_SPEED * dt * hardness_multiplier;
                    if (bullets[i].Position.y < 0)
                        bullets[i].isActive = false;
                }

                // Enemies Update
                for (int i = 0; i < ENEMIES_AMOUNT; i++) {
                    if (!enemies[i].isActive) {
                        enemies[i].Position = GetEnemySpawnVector(height, width);
                        enemies[i].isActive = true;
                    }

                    enemies[i].Position.y += enemies[i].Velocity.y * ENEMIES_SPEED * dt * hardness_multiplier;

                    if (enemies[i].Position.y >= 850) {
                        enemies[i].isActive = false;
                        health -= 1;
                    }
                }

                // Bullet Collision
                for (int bi = 0; bi < BULLETS_AMOUNT; bi++) {
                    if (!bullets[bi].isActive) continue;
                    for (int ei = 0; ei < ENEMIES_AMOUNT; ei++) {
                        if (CheckCircleCollision(bullets[bi].Position, BULLETS_RADIUS,
                                                 enemies[ei].Position, ENEMY_RADIUS)) {
                            bullets[bi].isActive = false;
                            enemies[ei].isActive = false;
                            points += 1;
                            break;
                        }
                    }
                }

                // Hardness multiplier
                if (points - last_ten_points >= 10) {
                    hardness_multiplier += 0.1;
                    last_ten_points = points;
                }

                break;

            case GAME_DEAD_SCREEN:
                if (IsKeyPressed(KEY_UP)) {
                    health = 3;
                    points = 0;
                    hardness_multiplier = 1;
		    last_ten_points = 0;

                    for (int i = 0; i < ENEMIES_AMOUNT; i++)
                        enemies[i].isActive = false;
                    currentGameState = GAME_PLAYING;
                }
                break;
        }

        // DRAWING SECTION
        BeginDrawing();
        ClearBackground(background_color);

        switch (currentGameState) {
            case GAME_DEAD_SCREEN:
                DrawText("YOU ARE DEAD", 180, 200, 30, RED);
                DrawText(TextFormat("Highest score: %d", points), 180, 250, 20, GRAY);
                DrawText("Press UP to restart", 180, 300, 20, GRAY);
                break;

            case GAME_PLAYING:

                #ifdef DEBUG
                DrawText(TextFormat("Debug mode", hardness_multiplier), 50, 40, 20, YELLOW);    
                DrawText(TextFormat("Hardess: %.2f", hardness_multiplier), 50, 160, 20, YELLOW);
                #endif

                // Draw bullets
                for (int i = 0; i < BULLETS_AMOUNT; i++) {
                    if (!bullets[i].isActive) continue;
                    DrawCircle(bullets[i].Position.x, bullets[i].Position.y, BULLETS_RADIUS, RED);
                }

                // Draw enemies
                for (int i = 0; i < ENEMIES_AMOUNT; i++) {
                    if (!enemies[i].isActive) continue;
                    DrawCircle(enemies[i].Position.x, enemies[i].Position.y, ENEMY_RADIUS, YELLOW);
                }

                // Draw player
                DrawCircle(player.Position.x, player.Position.y, PLAYER_RADIUS, WHITE);

                // Draw stats
                DrawText(TextFormat("Score: %d", points), 50, 80, 20, WHITE);
                DrawText(TextFormat("Health: %d", health), 50, 120, 20, WHITE);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
