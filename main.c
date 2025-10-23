#include "raylib.h"
#include <stdbool.h>
#include <math.h>

#define DEBUG 1

//Screen Settings
const int width = 600;
const int height = 800;
Color background_color = (Color){0,10,0,255};


//Player 
float player_speed = 5.0f;
typedef struct{
	Vector2 Position;
	Vector2 Velocity;
	bool isActive;	
} Player;

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

int main() {
	InitWindow(width, height, "Space-Invaders");
	SetTargetFPS(60);

	Player player = {
		.Position = (Vector2){width/2, height/1.2},
		.Velocity = (Vector2){0, 0},
		.isActive = true
	};

	while(!WindowShouldClose())
	{
		//Update
		player.Velocity = ReadPlayerMovementInput();
		player.Position.x += player.Velocity.x * player_speed;
		
		//CheckPlayerBoundaries
		if (player.Position.x <= 50)
			player.Position.x = 50;

		if (player.Position.x >= 550)
			player.Position.x = 550;

		//Drawing
		BeginDrawing();
			ClearBackground(background_color);
			DrawCircle(player.Position.x, player.Position.y, 50, WHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
