#include "./../include/duck.h"
#include <stdlib.h>
#include <stdio.h>

static const int ANIMATION_FRAMES[4] = {
    3, // DUCK_FLY
    3, // DUCK_OUT
    4, // DUCK_DEATH
    1, // DUCK_SCARED
};


static const float ANIMATION_SPEEDS[4] = {
    0.13, // DUCK_FLY
    0.12, // DUCK_OUT
    0.06, // DUCK_DEATH
    0.2, // DUCK_SCARED
};

Duck* duck_create(void){
    Duck* duck = (Duck*)malloc(sizeof(Duck));
    if (!duck) return NULL;


    duck->currentState = DUCK_FLY;
    duck-> position = (sfVector2f){0, 0};
    duck->velocity = (sfVector2f){150, 150};
    duck->isLoaded = 0;



    // Initialize animations to NULL
    for (int i = 0; i < 4; i++) {
        duck->animations[i].texture = NULL;
        duck->animations[i].sprite = NULL;
        duck->animations[i].animationClock = NULL;
    }

    duck->sprite = sfSprite_create();
    if (!duck->sprite) {
        free(duck);
        return NULL;
    }

    return duck;
}



int duck_loadAnimations(Duck* duck, const char* flyPath, const char* outPath, 
                      const char* deathPath, const char* scaredPath,
                      int frameWidth, int frameHeight) {
    if (!duck) return 0;
    
    const char* paths[4] = {
        flyPath, // DUCK_FLY
        outPath, // DUCK_OUT
        deathPath, // DUCK_DEATH
        scaredPath, // DUCK_SCARED
    };
    
    for (int state = 0; state < 4; state++) {
        DuckAnimation* anim = &duck->animations[state];
        
        // Load separate texture for each state
        anim->texture = sfTexture_createFromFile(paths[state], NULL);
        if (!anim->texture) {
            printf("Failed to load animation: %s\n", paths[state]);
            
            // CRITICAL: Clean up previously loaded animations
            for (int cleanup = 0; cleanup < state; cleanup++) {
                if (duck->animations[cleanup].sprite) {
                    sfSprite_destroy(duck->animations[cleanup].sprite);
                    duck->animations[cleanup].sprite = NULL;
                }
                if (duck->animations[cleanup].texture) {
                    sfTexture_destroy(duck->animations[cleanup].texture);
                    duck->animations[cleanup].texture = NULL;
                }
                if (duck->animations[cleanup].animationClock) {
                    sfClock_destroy(duck->animations[cleanup].animationClock);
                    duck->animations[cleanup].animationClock = NULL;
                }
            }
            return 0;
        }
        
        anim->sprite = sfSprite_create();
        if (!anim->sprite) {
            // Clean up current texture and previous animations
            sfTexture_destroy(anim->texture);
            anim->texture = NULL;
            
            for (int cleanup = 0; cleanup < state; cleanup++) {
                if (duck->animations[cleanup].sprite) {
                    sfSprite_destroy(duck->animations[cleanup].sprite);
                    duck->animations[cleanup].sprite = NULL;
                }
                if (duck->animations[cleanup].texture) {
                    sfTexture_destroy(duck->animations[cleanup].texture);
                    duck->animations[cleanup].texture = NULL;
                }
                if (duck->animations[cleanup].animationClock) {
                    sfClock_destroy(duck->animations[cleanup].animationClock);
                    duck->animations[cleanup].animationClock = NULL;
                }
            }
            printf("Failed to create sprite for animation %d\n", state);
            return 0;
        }
        
        sfSprite_setTexture(anim->sprite, anim->texture, sfTrue);
        
        anim->frameWidth = frameWidth;
        anim->frameHeight = frameHeight;
        anim->currentFrame = 0;
        anim->totalFrames = ANIMATION_FRAMES[state];
        anim->startFrame = 0;
        anim->animationSpeed = ANIMATION_SPEEDS[state];
        
        anim->animationClock = sfClock_create();
        if (!anim->animationClock) {
            // Clean up current sprite, texture and previous animations
            sfSprite_destroy(anim->sprite);
            anim->sprite = NULL;
            sfTexture_destroy(anim->texture);
            anim->texture = NULL;
            
            for (int cleanup = 0; cleanup < state; cleanup++) {
                if (duck->animations[cleanup].sprite) {
                    sfSprite_destroy(duck->animations[cleanup].sprite);
                    duck->animations[cleanup].sprite = NULL;
                }
                if (duck->animations[cleanup].texture) {
                    sfTexture_destroy(duck->animations[cleanup].texture);
                    duck->animations[cleanup].texture = NULL;
                }
                if (duck->animations[cleanup].animationClock) {
                    sfClock_destroy(duck->animations[cleanup].animationClock);
                    duck->animations[cleanup].animationClock = NULL;
                }
            }
            printf("Failed to create clock for animation %d\n", state);
            return 0;
        }
        
        // Set initial frame (first frame of the animation)
        sfIntRect rect = {0, 0, frameWidth, frameHeight};
        sfSprite_setTextureRect(anim->sprite, rect);
    }
    
    duck->isLoaded = 1;
    return 1;
}



void duck_setState(Duck* duck, DuckState state) {
    
    if (!duck || state >= 4) return;
    
    if (duck->currentState != state) {
        duck->currentState = state;
        // Reset animation when changing state
        duck->animations[state].currentFrame = 0;
        sfClock_restart(duck->animations[state].animationClock);
    }
}

void duck_setPosition(Duck* duck, float x, float y) {
    if (!duck) return;
    
    duck->position = (sfVector2f){x, y};
    
    // Update sprite positions
    for (int i = 0; i < 4; i++) {
        if (duck->animations[i].sprite) {
            sfSprite_setPosition(duck->animations[i].sprite, duck->position);
        }
    }
}

void duck_move(Duck* duck, float deltaX, float deltaY) {
    if (!duck) return;
    
    duck->position.x += deltaX;
    duck->position.y += deltaY;
    
    // Update current animation sprite position
    DuckAnimation* currentAnim = duck_getCurrentAnimation(duck);
    if (currentAnim && currentAnim->sprite) {
        sfSprite_setPosition(currentAnim->sprite, duck->position);
    }
}



DuckAnimation* duck_getCurrentAnimation(Duck* duck) {
    if (!duck || !duck->isLoaded) return NULL;
    return &duck->animations[duck->currentState];
}


void duck_updateAnimation(DuckAnimation* anim) {
    if (!anim || !anim->animationClock) return;
    
    float elapsed = sfTime_asSeconds(sfClock_getElapsedTime(anim->animationClock));
    
    if (elapsed >= anim->animationSpeed) {
        anim->currentFrame = (anim->currentFrame + 1) % anim->totalFrames;
        
        int frameX = anim->currentFrame * anim->frameWidth;
        int frameY = 0;
        
        sfIntRect rect = {frameX, frameY, anim->frameWidth, anim->frameHeight};
        sfSprite_setTextureRect(anim->sprite, rect);
        
        sfClock_restart(anim->animationClock);
    }
}

void duck_update(Duck* duck, float ) {
    if (!duck || !duck->isLoaded) return;
    
    DuckAnimation* currentAnim = duck_getCurrentAnimation(duck);
    if (currentAnim) {
        duck_updateAnimation(currentAnim);
    }
}


void duck_draw(sfRenderWindow* window, Duck* duck) {
    if (!duck || !duck->isLoaded || !window) return;
    
    DuckAnimation* currentAnim = duck_getCurrentAnimation(duck);
    if (currentAnim && currentAnim->sprite) {
        sfRenderWindow_drawSprite(window, currentAnim->sprite, NULL);
    }
}



void duck_destroy(Duck* duck) {
    if (!duck) return;
    
    // Destroy animations
    for (int i = 0; i < 4; i++) {
        if (duck->animations[i].sprite) {
            sfSprite_destroy(duck->animations[i].sprite);
            duck->animations[i].sprite = NULL;
        }
        if (duck->animations[i].animationClock) {
            sfClock_destroy(duck->animations[i].animationClock);
            duck->animations[i].animationClock = NULL;
        }
        if (duck->animations[i].texture) {
            sfTexture_destroy(duck->animations[i].texture);
            duck->animations[i].texture = NULL;
        }
    }
    
    free(duck);
}