#include <SFML/Graphics.h>
#include <SFML/Graphics/Text.h>
#include <SFML/Window.h>
#include <SFML/System.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "texture.h"
#include "dog.h"
#include "duck.h"
#include "main.h"
#include <time.h>
#include <math.h>

float getRandomFloat(float min, float max) {
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

void startLaughAnim(AnimationTimer* timer, Dog* dog) {
    if (!timer->isActive) {
        timer->isActive = true;
        timer->timer = 0;
        dog_setState(dog, timer->targetState);
    }
}

void startGotAnim(AnimationTimer* timer, Dog* dog) {
    if (!timer->isActive) {
        timer->isActive = true;
        timer->timer = 0;
        dog_setState(dog, timer->targetState);
    }
}

void updateAnimationTimer(AnimationTimer* timer, Dog* dog, float deltaTime) {
    if (timer->isActive) {
        timer->timer += deltaTime;
        if (timer->timer >= timer->duration) {
            timer->isActive = false;
            dog_setState(dog, timer->returnState);
        }
    }
}

void updateDuckMovement(Duck* duck, float deltaTime, Dog* dog, AnimationTimer* laughTimer, AnimationTimer* gotTimer) {
    static float targetX = 0;
    static float targetY = 50;
    static bool hasTarget = false;
    
    if (duck->currentState == DUCK_FLY) {
        if (!hasTarget) {
            targetX = getRandomFloat(10, 700);
            targetY = 60;
            hasTarget = true;
        }
        
        float speed = 600;
        float deltaX = targetX - duck->position.x;
        float deltaY = targetY - duck->position.y;
        float distance = sqrtf(deltaX * deltaX + deltaY * deltaY);
        
        if (distance > 40) {
            float moveX = (deltaX / distance) * speed * deltaTime;
            float moveY = (deltaY / distance) * speed * deltaTime;
            duck_move(duck, moveX, moveY);
        } else {
            hasTarget = false;
            
            if (rand() % 3 == 0) { 
                duck_setState(duck, DUCK_OUT); 
            }
        }
    }
    else if (duck->currentState == DUCK_OUT) {
        duck_move(duck, 0, -500 * deltaTime);
        if (duck->position.y < -100) {
            duck_setPosition(duck, rand() % 700, 430);
            duck_setState(duck, DUCK_FLY);
            startLaughAnim(laughTimer, dog);
            hasTarget = false;
        }
    }
    else if (duck->currentState == DUCK_DEATH) {
        duck_move(duck, 0, 500 * deltaTime);
        if (duck->position.y > 550) {
            duck_setPosition(duck, rand() % 700, 430);
            duck_setState(duck, DUCK_FLY);
            hasTarget = false;
            startGotAnim(gotTimer, dog);
        }
    }
}

int main() {
    // Game Data
    char scoreText[50];
    int score = 0;
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    // Timer variables
    sfClock* gameTimer = sfClock_create();
    float gameTime = 60.0f; // 60 seconds
    char timerText[50];
    bool gameOver = false;

    sfFont* font = sfFont_createFromFile("/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/gameFont.ttf");
    if (!font) {
        printf("Failed to load font!\n");
        return 1;
    }

    // Score display
    sfText* scoreDisplay = sfText_create();
    sfText_setFont(scoreDisplay, font);
    sfText_setCharacterSize(scoreDisplay, 30);
    sfText_setFillColor(scoreDisplay, sfColor_fromRGB(176, 35, 0));
    sfText_setPosition(scoreDisplay, (sfVector2f){10, 10});
    sfText_setString(scoreDisplay, scoreText);

    // Timer display
    sfText* timerDisplay = sfText_create();
    sfText_setFont(timerDisplay, font);
    sfText_setCharacterSize(timerDisplay, 30);
    sfText_setFillColor(timerDisplay, sfColor_fromRGB(176, 35, 0));
    sfText_setPosition(timerDisplay, (sfVector2f){10, 50});
    snprintf(timerText, sizeof(timerText), "Time: %.1f", gameTime);
    sfText_setString(timerDisplay, timerText);

    // Game over display
    sfText* gameOverText = sfText_create();
    sfText_setFont(gameOverText, font);
    sfText_setCharacterSize(gameOverText, 60);
    sfText_setFillColor(gameOverText, sfWhite);
    sfText_setString(gameOverText, "GAME OVER");
    sfFloatRect textRect = sfText_getLocalBounds(gameOverText);
    sfText_setOrigin(gameOverText, (sfVector2f){textRect.width/2, textRect.height/2});


    srand(time(NULL));

    sfVideoMode mode = {768, 720, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "MY Duck Hunt Game", sfResize | sfClose, NULL);
    if (!window) return 1;
    
    sfRenderWindow_setFramerateLimit(window, 60);
    
    Assets assets;
    if (loadAssets(&assets)) {
        printf("Failed to load assets!\n");
        sfRenderWindow_destroy(window);
        return 1;
    }
    
    Dog* dog = dog_create();
    if (!dog) {
        sfRenderWindow_destroy(window);
        return 1;
    }
    
    if (!dog_loadAnimations(dog, 
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/dog/sprite_newdog_walk.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/dog/sprite_newdog_alert.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/dog/sprite_newdog_jump.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/dog/sprite_newdog_got1.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/dog/sprite_newdog_laughing.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/dog/sprite_newdog_not2.png",
        256, 256)) {
        printf("Failed to load dog animations\n");
        dog_destroy(dog);
        sfRenderWindow_destroy(window);
        return 1;
    }
    
    AnimationTimer laughTimer = {false, 0, 1.0f, DOG_LAUGH, DOG_DEFAULT};
    AnimationTimer gotTimer = {false, 0, 1.0f, DOG_GOT, DOG_DEFAULT};
    
    Duck* duck = duck_create();
    if (!duck) {
        printf("Failed to create duck\n");
        dog_destroy(dog);
        freeAssets(&assets);
        sfRenderWindow_destroy(window);
        return 1;
    }
    
    if (!duck_loadAnimations(duck, 
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/duck/sprite_newduck_right.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/duck/sprite_newduck_up.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/duck/sprite_newduck_death.png",
        "/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/duck/sprite_newduck_scared.png",
        128, 128)) {
        printf("Failed to load duck animations\n");
        duck_destroy(duck);
        dog_destroy(dog);
        freeAssets(&assets);
        sfRenderWindow_destroy(window);
        return 1;
    }
    
    duck_setPosition(duck, rand() % 700, 500); 
    duck_setState(duck, DUCK_FLY);
    
    GamePhase phase = CUTSCENE_START;
    sfClock* cutsceneClock = sfClock_create();
    float dogStartX = 50;
    float dogTargetX = 250;
    float dogY = 400;
    
    dog_setPosition(dog, dogStartX, dogY);
    dog_setState(dog, DOG_WALK);

    
    sfClock* gameClock = sfClock_create();



    // cursor settings

    sfRenderWindow_setMouseCursorVisible(window, sfFalse);
    sfTexture* cursorTexture = sfTexture_createFromFile("/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/ui/sprite_aim.png", NULL);
    sfSprite* cursorSprite = sfSprite_create();
    sfSprite_setTexture(cursorSprite, cursorTexture, sfTrue);

    
    while (sfRenderWindow_isOpen(window)) {
        float deltaTime = sfTime_asSeconds(sfClock_restart(gameClock));
        float cutsceneTime = sfTime_asSeconds(sfClock_getElapsedTime(cutsceneClock));
        
        if (!gameOver) {
            // Update game timer
            gameTime -= deltaTime;
            if (gameTime <= 0) {
                gameTime = 0;
                gameOver = true;
                sfText_setPosition(gameOverText, (sfVector2f){mode.width/2, mode.height/2});
            }
            
            // Update timer display
            snprintf(timerText, sizeof(timerText), "Time: %.1f", gameTime);
            sfText_setString(timerDisplay, timerText);
        }
        
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (!gameOver && event.type == sfEvtMouseButtonPressed 
                && phase == GAME_PLAYING
                && event.mouseButton.button == sfMouseLeft) {
                sfVector2f mousePos = {
                    (float)event.mouseButton.x,
                    (float)event.mouseButton.y
                };
    
                if (fabs((float) mousePos.x - (duck->position.x + 64)) < 80 && 
                    fabs((float) mousePos.y - (duck->position.y + 64)) < 80) {
                    if (duck->currentState == DUCK_FLY) {
                        duck_setState(duck, DUCK_DEATH);
                        startGotAnim(&gotTimer, dog);
                        score++;
                        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
                        sfText_setString(scoreDisplay, scoreText);
                    }
                }
            }

            sfVector2i mousePosPixel = sfMouse_getPositionRenderWindow(window);
            sfVector2f mousePos = { (float)mousePosPixel.x, (float)mousePosPixel.y };
            sfSprite_setPosition(cursorSprite, mousePos);

        }
        
        if (!gameOver) {
            switch (phase) {
                case CUTSCENE_START:
                    phase = CUTSCENE_WALKING;
                    sfClock_restart(cutsceneClock);
                    break;
                    
                case CUTSCENE_WALKING:
                    dog_move(dog, 120 * deltaTime, 0);
                    
                    if (dog->position.x >= dogTargetX) {
                        dog_setPosition(dog, dogTargetX, dogY);
                        phase = CUTSCENE_ALERT;
                        dog_setState(dog, DOG_ALERT);
                        sfClock_restart(cutsceneClock);
                    }
                    break;
                    
                case CUTSCENE_ALERT:
                    if (cutsceneTime >= 1.0f) {
                        phase = CUTSCENE_JUMP;
                        dog_setState(dog, DOG_JUMP);
                        sfClock_restart(cutsceneClock);
                    }
                    break;
                    
                case CUTSCENE_JUMP:
                    dog_move(dog, 100 * deltaTime, -100 * deltaTime);
                    if (dog->position.y <= dogY - 100) {
                        dog_setPosition(dog, 260, dogY - 100);
                        sfClock_restart(cutsceneClock);
                    }
                    if (cutsceneTime >= 0.2f) {
                        phase = GAME_PLAYING;
                        dog_setState(dog, DOG_DEFAULT);
                    }
                    break;
                    
                case GAME_PLAYING:
                    updateDuckMovement(duck, deltaTime, dog, &laughTimer, &gotTimer);
                    break;
            }
            
            updateAnimationTimer(&laughTimer, dog, deltaTime);
            updateAnimationTimer(&gotTimer, dog, deltaTime);
            
            dog_update(dog, deltaTime);
            duck_update(duck, deltaTime);
        }
        
        sfRenderWindow_clear(window, sfBlack);
        
        if (!gameOver) {
            // Draw normal game elements
            sfRenderWindow_drawSprite(window, assets.bg, NULL);
            sfRenderWindow_drawSprite(window, assets.cloud1, NULL);
            sfRenderWindow_drawSprite(window, assets.cloud2, NULL);
            duck_draw(window, duck);
            sfRenderWindow_drawSprite(window, assets.ground, NULL);
            dog_draw(window, dog);
            
            // Draw HUD
            sfRenderWindow_drawText(window, scoreDisplay, NULL);
            sfRenderWindow_drawText(window, timerDisplay, NULL);
            sfRenderWindow_drawSprite(window, cursorSprite, NULL);

        } else {
            // Draw game over screen
            char finalScoreText[100];
            snprintf(finalScoreText, sizeof(finalScoreText), "Final Score: %d", score);
            
            sfText* finalScoreDisplay = sfText_create();
            sfText_setFont(finalScoreDisplay, font);
            sfText_setCharacterSize(finalScoreDisplay, 40);
            sfText_setFillColor(finalScoreDisplay, sfWhite);
            sfFloatRect scoreRect = sfText_getLocalBounds(finalScoreDisplay);
            sfText_setOrigin(finalScoreDisplay, (sfVector2f){scoreRect.width/2, scoreRect.height/2});
            sfText_setPosition(finalScoreDisplay, (sfVector2f){mode.width/2, mode.height/2 + 80});
            sfText_setString(finalScoreDisplay, finalScoreText);
            
            sfRenderWindow_drawText(window, gameOverText, NULL);
            sfRenderWindow_drawText(window, finalScoreDisplay, NULL);
            
            sfText_destroy(finalScoreDisplay);
            sfRenderWindow_drawSprite(window, cursorSprite, NULL);

        }
        
        sfRenderWindow_display(window);
    }
    
    // Cleanup
    sfClock_destroy(cutsceneClock);
    sfClock_destroy(gameClock);
    sfClock_destroy(gameTimer);
    duck_destroy(duck);
    dog_destroy(dog);
    freeAssets(&assets);
    sfText_destroy(scoreDisplay);
    sfText_destroy(timerDisplay);
    sfText_destroy(gameOverText);
    sfFont_destroy(font);
    sfRenderWindow_destroy(window);
    return 0;
}