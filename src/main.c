#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "utilities.h"
#include <stdio.h>

#define DEBUG 0

//---------------- STRUCTS ----------------//
typedef struct {
    Vector2 Position;
    Vector2 Velocity;
    bool isActive;
} Object;

typedef struct {
	Rectangle frame;
	float current_time;
	int current_frame;
	bool isActive;
	Vector2 spawnVector;
} AnimatedSprite;

typedef enum {
    GAME_PLAYING,
    GAME_DEAD_SCREEN,
} GameState;

GameState currentGameState = GAME_PLAYING;

//---------------- GLOBAL DEFINITIONS ----------------//

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
#define MAX_HARDNESS 2.1
float hardness_multiplier = 1.0f;
int last_ten_points = 0; // temp which contains last (score % 10 = 0) value

// Bullets Configuration
#define BULLETS_AMOUNT 10
#define BULLETS_RADIUS 15
#define FIRE_COOLDOWN_STANDART_VALUE 0.40f
#define BULLET_SPAWN_Y_OFFSET -30
#define BULLET_SPEED 20.0f
float fire_cooldown = 0;
Object bullets[BULLETS_AMOUNT] = {0};

// Enemies Configuration
#define ENEMIES_AMOUNT 7
#define ENEMIES_SPEED 10.0f
#define ENEMY_RADIUS 25 
Object enemies[ENEMIES_AMOUNT] = {0};

//----------- GRAPHICS ---------------//
Texture2D background_texture = {0};
Texture2D player_texture = {0};
#define PLAYER_TEXTURE_SCALE 0.75

Texture2D alien_texture = {0};
#define ALIEN_TEXTURE_SCALE 0.6

Texture2D fire_bullet_texture = {0};

Texture2D explosion_sheet_texture = {0};


// Animated sprites
#define EXPLOSIONS_SHEET_LENGHT 7
#define EXPLOSIONS_SPRITE_SIZE 128 //width and lenght of 1 sprite
#define EXPLOSIONS_TIME_PER_FRAME 0.1f
#define EXPLOSION_AMOUNT 10
AnimatedSprite *explosions;

//----------- SOUND ---------------//
#define FIRE_SOUNDS_AMOUNT 4
Sound fireshots[FIRE_SOUNDS_AMOUNT] = {0};

#define ALIEN_DEATH_AMOUNT 7
Sound alien_death[ALIEN_DEATH_AMOUNT] = {0};

bool mustplay_gameover = true; // sound of death must be played only once
Sound game_over = {0};

// ---------------- FUNCTION DECLARATIONS ----------------
void ProperExit();
void GraphicsLoad();
void GraphicsFreeMemory();
void SoundsLoad();
void SoundsFreeMemory();
void Start();
//---------------- MAIN ----------------//

int main()
{
    Start();
	GraphicsLoad();
	SoundsLoad();

    Object player = {
        .Position = {width / 2, height / 1.2f},
        .Velocity = {0, 0},
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

	//Explosions initialization
	
	explosions = (AnimatedSprite *)malloc(sizeof(AnimatedSprite) * EXPLOSION_AMOUNT);
    if (explosions == NULL) {
        TraceLog(LOG_ERROR, "malloc AnimatedSprites explosions");
        exit(1);
    }

	for (int i = 0; i < EXPLOSION_AMOUNT; i ++){
		explosions[i] = (AnimatedSprite){
			.frame = (Rectangle) {0,0,EXPLOSIONS_SPRITE_SIZE, EXPLOSIONS_SPRITE_SIZE},
			.current_time = 0,
			.current_frame = 0,
			.isActive = false,
			.spawnVector = (Vector2){0,0},
		};
	}

	while (!WindowShouldClose()) 
	{
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
				
				// Explosions update
				for (int i = 0; i < EXPLOSION_AMOUNT; i++)
				{
    			if (!explosions[i].isActive) continue;

    			// Resed animation if it's the last frame
    			if (explosions[i].current_frame == EXPLOSIONS_SHEET_LENGHT - 1 &&
    			    explosions[i].current_time >= EXPLOSIONS_TIME_PER_FRAME)
    			{
    				explosions[i].frame = (Rectangle){0, 0, explosion_sheet_texture.height, explosion_sheet_texture.height};
     				explosions[i].current_frame = 0;
     				explosions[i].current_time = 0;
      				explosions[i].isActive = false;
      				continue;
    			}

   			 	// next frame
    			if (explosions[i].current_time >= EXPLOSIONS_TIME_PER_FRAME)
   			 	{
      				explosions[i].frame.x += explosion_sheet_texture.height;
        			explosions[i].current_frame += 1;
        			explosions[i].current_time = 0;
    			}

   				explosions[i].current_time += dt;
				}
	
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

							PlaySound(fireshots[RandomValueInRange(FIRE_SOUNDS_AMOUNT - 1, 0)]);
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
                // Bullet Collision:
                for (int bi = 0; bi < BULLETS_AMOUNT; bi++) {
                    if (!bullets[bi].isActive) continue;
                    for (int ei = 0; ei < ENEMIES_AMOUNT; ei++) {
                        if (CheckCircleCollision(bullets[bi].Position, BULLETS_RADIUS,
                                                 enemies[ei].Position, ENEMY_RADIUS)) {
                            bullets[bi].isActive = false;
                            enemies[ei].isActive = false;
                            points += 1;
							PlaySound(alien_death[RandomValueInRange(ALIEN_DEATH_AMOUNT - 1, 0)]);

							//Spawn explosion effect
							for (int i = 0; i < EXPLOSION_AMOUNT; i ++)
							{
								if (explosions[i].isActive) continue;

								explosions[i].spawnVector = enemies[ei].Position;
								explosions[i].isActive = true;
								break;
							}
                            break;
                        }
                    }
                }

                // Hardness multiplier
                if (points - last_ten_points >= 10 && hardness_multiplier < MAX_HARDNESS) {
                    hardness_multiplier += 0.1;
                    last_ten_points = points;
                }
                break;

            case GAME_DEAD_SCREEN:
				if (mustplay_gameover) {
					PlaySound(game_over);
					mustplay_gameover = false;
				}

                if (IsKeyPressed(KEY_UP)) {
                    health = 3;
                    points = 0;
                    hardness_multiplier = 1;
		    		last_ten_points = 0;
					mustplay_gameover = true;

                    for (int i = 0; i < ENEMIES_AMOUNT; i++)
                        enemies[i].isActive = false;
                    currentGameState = GAME_PLAYING;
                }
                break;
        }

        // DRAWING SECTION
        BeginDrawing();
		ClearBackground(background_color);
        DrawTexture(background_texture, 0, 0, WHITE);

        switch (currentGameState) {
            case GAME_DEAD_SCREEN:
                DrawText("YOU ARE DEAD", 180, 200, 30, RED);
                DrawText(TextFormat("Highest score: %d", points), 180, 250, 20, GRAY);
                DrawText("Press UP to restart", 180, 300, 20, GRAY);
                break;

			case GAME_PLAYING:
                // Draw bullets
                for (int i = 0; i < BULLETS_AMOUNT; i++) {
                	if (!bullets[i].isActive) continue;					
					Vector2 bullet_drawing_position = {
						bullets[i].Position.x - fire_bullet_texture.width / 2 * (float)(BULLETS_RADIUS + (i * 5)) / 100.0f,
						bullets[i].Position.y - fire_bullet_texture.height / 2 * (float)(BULLETS_RADIUS + (i * 5)) / 100.0f
					};
					DrawTextureEx(fire_bullet_texture, bullet_drawing_position, 0.0f, BULLETS_RADIUS / 100.0f, WHITE);	
				}

				//Draw explosion effect
				for (int i = 0; i < EXPLOSION_AMOUNT; i++) {
    				if (!explosions[i].isActive) continue;

        				DrawTextureRec(
        				explosion_sheet_texture,
						explosions[i].frame,
						(Vector2){explosions[i].spawnVector.x - explosions[i].frame.width / 2,
                  				explosions[i].spawnVector.y - explosions[i].frame.height / 2},
						WHITE);
}


				// Draw enemies
                for (int i = 0; i < ENEMIES_AMOUNT; i++) {
					if (!enemies[i].isActive) continue;
					Vector2 alien_drawing_position = {
						enemies[i].Position.x - alien_texture.width / 2 * (float)ALIEN_TEXTURE_SCALE,
						enemies[i].Position.y - alien_texture.height / 2 * (float)ALIEN_TEXTURE_SCALE
					};
					DrawTextureEx(alien_texture, alien_drawing_position, 0.0f, ALIEN_TEXTURE_SCALE, WHITE);
                }

                // Draw player
				Vector2 player_drawing_position = {
					player.Position.x - player_texture.width / 2 * (float)PLAYER_TEXTURE_SCALE,
					player.Position.y - player_texture.height / 2 * (float)PLAYER_TEXTURE_SCALE
				};
				DrawTextureEx(player_texture, player_drawing_position, 1, PLAYER_TEXTURE_SCALE, WHITE);

				// Stats
				#if DEBUG == 1
                DrawText("Debug mode", 50, 140, 20, YELLOW);    
                DrawText(TextFormat("Hardness: %.2f", hardness_multiplier), 50, 160, 20, YELLOW);
                DrawText(TextFormat("Fire CD: %.2f", fire_cooldown), 50, 180, 20, YELLOW);
                #endif

                DrawText(TextFormat("Score: %d", points), 50, 60, 20, WHITE);
                DrawText(TextFormat("Health: %d", health), 50, 100, 20, WHITE);
				DrawText(TextFormat("Space-Invaders: github@notorious1dev"), 10, height - 30, 20, (Color){255,255,255,25});
				DrawRectangle(35, 50, 135, 80, (Color){255,255,255,50});
                break;
        }

        EndDrawing();
    }
	ProperExit();
}

// ---------------- FUNCTION IMPLEMENTATIONS ----------------

void ProperExit() {
	GraphicsFreeMemory();
	SoundsFreeMemory();
	CloseAudioDevice();
    CloseWindow();
	free(explosions);
}

void Start() {
    InitWindow(width, height, "Space-Invaders");
    InitAudioDevice(); 
    SetTargetFPS(144);
}

void GraphicsLoad() {
    background_texture = LoadTexture("./assets/background.png");
    if (background_texture.id == 0){
        TraceLog(LOG_ERROR, "Failed to load background_texture");
		goto HandleGraphicsLoadingError;
	}

    player_texture = LoadTexture("./assets/player_ship.png");
    if (player_texture.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load player_texture");
		goto HandleGraphicsLoadingError;
	}

    alien_texture = LoadTexture("./assets/alien_ship.png");
    if (alien_texture.id == 0){
        TraceLog(LOG_ERROR, "Failed to load alien_texture");
		goto HandleGraphicsLoadingError;
	}

    fire_bullet_texture = LoadTexture("./assets/bullet.png");
    if (fire_bullet_texture.id == 0){
        TraceLog(LOG_ERROR, "Failed to load fire_bullet_texture");
		goto HandleGraphicsLoadingError;
	}

	explosion_sheet_texture = LoadTexture("./assets/explosion.png");
    if (explosion_sheet_texture.id == 0){
        TraceLog(LOG_ERROR, "Failed to load explosion_sheet_texture");
		goto HandleGraphicsLoadingError;
	}

	return;

	HandleGraphicsLoadingError:
	ProperExit();
	exit(1);
}

void GraphicsFreeMemory() {
    UnloadTexture(background_texture);
    UnloadTexture(player_texture);
    UnloadTexture(alien_texture);
    UnloadTexture(fire_bullet_texture);
	UnloadTexture(explosion_sheet_texture);
}

void SoundsLoad() {
    char filename[128] = {0};

    // Load shoot sounds
    for (int i = 0; i < FIRE_SOUNDS_AMOUNT; i++) {
        sprintf(filename, "./assets/sounds/fire%d.wav", i + 1); 
        fireshots[i] = LoadSound(filename);

        if (fireshots[i].frameCount == 0){
            TraceLog(LOG_ERROR, "Failed to load sound: %s", filename);
			goto HandleSoundLoadingExit;
		}
    }

    // Load alien death sounds
    for (int i = 0; i < ALIEN_DEATH_AMOUNT; i++) {
        sprintf(filename, "./assets/sounds/explosion_%d.wav", i + 1);
        alien_death[i] = LoadSound(filename);

        if (alien_death[i].frameCount == 0) {
            TraceLog(LOG_ERROR, "Failed to load sound: %s", filename);
			goto HandleSoundLoadingExit;
		}
    }

    // Load game over sound
    game_over = LoadSound("./assets/sounds/game_over.wav");

	if (game_over.frameCount == 0){
		TraceLog(LOG_ERROR, "Failed to load sound: ./assets/sounds/game_over.wav");
		goto HandleSoundLoadingExit;
	}

	return;

	HandleSoundLoadingExit:
	ProperExit();
	exit(1);
}

void SoundsFreeMemory() {
    for (int i = 0; i < FIRE_SOUNDS_AMOUNT; i++)
        UnloadSound(fireshots[i]);

    for (int i = 0; i < ALIEN_DEATH_AMOUNT; i++)
        UnloadSound(alien_death[i]);

    UnloadSound(game_over);
}
