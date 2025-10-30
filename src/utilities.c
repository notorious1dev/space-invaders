#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

int RandomValueInRange(int max, int min) {
	return min + rand() % (max - min + 1);
}

float vector2_magnitude(Vector2 *vector) {
    return sqrtf((vector->x * vector->x) + (vector->y * vector->y));
}

void vector2_normalize(Vector2 *vector) {
    float magnitude = vector2_magnitude(vector);
    vector->x /= magnitude;
    vector->y /= magnitude;
}

Vector2 ReadPlayerMovementInput() {
    Vector2 input = {0, 0};

    if (IsKeyDown(KEY_LEFT)) input.x += -1;
    if (IsKeyDown(KEY_RIGHT)) input.x += 1;

    return input;
}

Vector2 GetEnemySpawnVector(int height, int width) {
    int random_y = RandomValueInRange(-50, -800);
    int random_x = RandomValueInRange(width - 50, 50);
    return (Vector2){(float)random_x, (float)random_y};
}

bool CheckCircleCollision(Vector2 Circle1Center, float Circle1Radius,
                          Vector2 Circle2Center, float Circle2Radius) {
    float distance = sqrtf(powf((Circle2Center.x - Circle1Center.x), 2) +
                           powf((Circle2Center.y - Circle1Center.y), 2));
    return (distance <= Circle2Radius + Circle1Radius);
}



