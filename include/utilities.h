#ifndef UTILITIES_H
#define UTILITIES_H

#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

// --- Utility Functions ---
int RandomValueInRange(int max, int min);

float vector2_magnitude(Vector2 *vector);
void vector2_normalize(Vector2 *vector);

Vector2 ReadPlayerMovementInput();
Vector2 GetEnemySpawnVector(int height, int width);

bool CheckCircleCollision(Vector2 Circle1Center, float Circle1Radius,
                          Vector2 Circle2Center, float Circle2Radius);
#endif
