#include "raylib.h"
#include <stdbool.h>
#include <math.h>

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
#define BULLET_SPEED 15.0f
float fire_cooldown = 0;
Object bullets[BULLETS_AMOUNT] = {0};

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

	while(!WindowShouldClose())
	{
		dt = GetFrameTime();

		//Update
		player.Velocity = ReadPlayerMovementInput();
		player.Position.x += player.Velocity.x * player_speed * dt;
		
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

		//CheckPlayerBoundaries
		if (player.Position.x <= 50)
			player.Position.x = 50;

		if (player.Position.x >= 550)
			player.Position.x = 550;

		//BulletUpdate
		for (int i = 0; i < BULLETS_AMOUNT; i++) {
            		if (!bullets[i].isActive) continue;

            		bullets[i].Position.x += bullets[i].Velocity.x * BULLET_SPEED* dt;
            		bullets[i].Position.y += bullets[i].Velocity.y * BULLET_SPEED * dt;

            		if (bullets[i].Position.y < 0)
                	bullets[i].isActive = false;
		}

		//Drawing
	BeginDrawing();
		ClearBackground(background_color);
		
		//Draw Bullets
		for (int i = 0; i < BULLETS_AMOUNT; i++){
			if (!bullets[i].isActive) continue;
			
			DrawCircle(bullets[i].Position.x, bullets[i].Position.y, 15, RED);
		}

		DrawCircle(player.Position.x, player.Position.y, 50, WHITE);

	EndDrawing();
	}

	CloseWindow();
	return 0;
}
