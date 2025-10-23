#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#define DEBUG 1

//Screen Settings
const int width = 600;
const int height = 800;
float dt = 0;
Color background_color = (Color){0,0,10,255};

typedef struct{
	Vector2 Position;
	Vector2 Velocity;
	bool isActive;	
} Object;

//Player Configuration
float player_speed = 500.0f;

//Bullets Configuration
#define BULLETS_AMOUNT 10
#define FIRE_COOLDOWN_STANDART_VALUE 0.75f
#define BULLET_SPAWN_Y_OFFSET - 30
#define BULLET_SPEED 20.0f
float fire_cooldown = 0;
Object bullets[BULLETS_AMOUNT] = {0};

//Enemies Configuration
#define ENEMIES_AMOUNT 15
#define ENEMIES_SPEED 10.0f
#define ENEMIES_SPAWN_Y_OFFSET
Object enemies[ENEMIES_AMOUNT] = {0};

float vector2_magnitude(Vector2 *vector) {
	return sqrt((vector->x*vector->x) + (vector->y*vector->y));
}

void vector2_normalize(Vector2 *vector) {
	float magnitude = vector2_magnitude(vector);
	vector->x /= magnitude;
	vector->y /= magnitude;
}

Vector2 ReadPlayerMovementInput(){
	Vector2 input = {0,0};

	if(IsKeyDown(KEY_LEFT)) input.x += -1;
	if(IsKeyDown(KEY_RIGHT)) input.x += 1;

	return input;
}

void Start()
{
	InitWindow(width, height, "Space-Invaders");
	SetTargetFPS(60);
}

Vector2 GetEnemySpawnVector() {
	int yMax = -50;
	int yMin = -800;

	int random_y = yMin + rand() % (yMax - yMin + 1);

	int xMax = width - 50;
	int xMin = 50;
	int random_x = xMin + rand() % (xMax - xMin + 1);

	return (Vector2){(float)random_x, (float)random_y};
}

int main() {
	Start();
	
	Object player = {
		.Position = (Vector2){width/2, height/1.2},
		.Velocity = (Vector2){0, 0},
		.isActive = true
	};

	//Bullets initialization
	for (int i = 0; i < BULLETS_AMOUNT; i ++){
		bullets[i] = (Object){.Position = {0,0}, .Velocity = {0,0}, .isActive = false};
	}

	//Enemies initialization
	for (int i = 0; i < ENEMIES_AMOUNT; i++){
		Vector2 enemy_spawn_position = GetEnemySpawnVector();
		enemies[i] = (Object){.Position = enemy_spawn_position, .Velocity = {0,ENEMIES_SPEED}, .isActive = true};
	}

	while(!WindowShouldClose())
	{
		dt = GetFrameTime();

		//Update
		player.Velocity = ReadPlayerMovementInput();
		player.Position.x += player.Velocity.x * player_speed * dt;

		//CheckPlayerBoundaries
		if (player.Position.x <= 50)
			player.Position.x = 50;

		if (player.Position.x >= 550)
			player.Position.x = 550;	

		//BulletShoot(Arrow_UP)
		fire_cooldown -= dt;
		if (IsKeyDown(KEY_UP) && fire_cooldown <= 0){
			for (int i = 0; i < BULLETS_AMOUNT; i++)
			{
				if (!bullets[i].isActive)
				{
					bullets[i].Position.x = player.Position.x; 
					bullets[i].Position.y = player.Position.y + BULLET_SPAWN_Y_OFFSET;
					bullets[i].Velocity = (Vector2){0, - BULLET_SPEED};
					bullets[i].isActive = true;
					fire_cooldown = FIRE_COOLDOWN_STANDART_VALUE;
					break;
				}
			}
		}

		//BulletUpdate
		for (int i = 0; i < BULLETS_AMOUNT; i++) {
            		if (!bullets[i].isActive) continue;

            		bullets[i].Position.x += bullets[i].Velocity.x * BULLET_SPEED * dt;
            		bullets[i].Position.y += bullets[i].Velocity.y * BULLET_SPEED * dt;

            		if (bullets[i].Position.y < 0)
                	bullets[i].isActive = false;
		}

		//EnemiesUpdate
		for (int i = 0; i < ENEMIES_AMOUNT; i++){
			if (!enemies[i].isActive) {
				enemies[i].Position = GetEnemySpawnVector();
				enemies[i].isActive = true;
			}

			enemies[i].Position.x += enemies[i].Velocity.x * ENEMIES_SPEED * dt;
			enemies[i].Position.y += enemies[i].Velocity.y * ENEMIES_SPEED * dt;

			if (enemies[i].Position.y >= 850)
				enemies[i].isActive = false;
		}

		//Drawing
	BeginDrawing();
		//Draw Bullets
		for (int i = 0; i < BULLETS_AMOUNT; i++){
			if (!bullets[i].isActive) continue;
			
			DrawCircle(bullets[i].Position.x, bullets[i].Position.y, 15, RED);
		}

		//Draw Enemies
		for (int i = 0; i < ENEMIES_AMOUNT; i++){
			if (!enemies[i].isActive) continue;
			DrawCircle(enemies[i].Position.x, enemies[i].Position.y, 25, YELLOW);
		}

		DrawCircle(player.Position.x, player.Position.y, 50, WHITE);
		ClearBackground(background_color);
	EndDrawing();
	}

	CloseWindow();
	return 0;
}
