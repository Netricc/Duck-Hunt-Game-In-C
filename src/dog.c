#include "./../include/dog.h"
#include <stdlib.h>
#include <stdio.h>

static const int ANIMATION_FRAMES[6] = {
    4, // DOG_WALK
    1, // DOG_ALERT
    2, // DOG_JUMP
    2, // DOG_GOT
    2, // DOG_LAUGH,
    1, // DOG_DEFAULT
};


static const float ANIMATION_SPEEDS[6] = {
 0.15f, // DOG_WALK
 0.2f, // DOG_ALERT
 0.6f, // DOG_JUMP
 0.2f, // DOG_GOT
 0.1f, // DOG_LAUGH
 0.2f, // DOG_DEFAULT
};

Dog* dog_create(void){
    Dog* dog = (Dog*)malloc(sizeof(Dog));
    if (!dog) return NULL;


    dog->currentState = DOG_WALK;
    dog-> position = (sfVector2f){0, 400};
    dog->velocity = (sfVector2f){70, 10};
    dog->isLoaded = 0;



    // Initialize animations to NULL
    for (int i = 0; i < 6; i++) {
        dog->animations[i].texture = NULL;
        dog->animations[i].sprite = NULL;
        dog->animations[i].animationClock = NULL;
    }
    
    return dog;
}



int dog_loadAnimations(Dog* dog, const char* walkPath, const char* alertPath, 
                      const char* jumpPath, const char* gotPath,
                      const char* laughPath,
                      const char* defaultPath,
                      int frameWidth, int frameHeight) {
    if (!dog) return 0;
    
    const char* paths[6] = {
        walkPath,     // DOG_WALK
        alertPath,    // DOG_ALERT
        jumpPath,     // DOG_JUMP
        gotPath,      // DOG_GOT
        laughPath,    // DOG_LAUGH
        defaultPath   // DOG_DEFAULT
    };
    
    for (int state = 0; state < 6; state++) {
        DogAnimation* anim = &dog->animations[state];
        
        // Load separate texture for each state
        anim->texture = sfTexture_createFromFile(paths[state], NULL);
        if (!anim->texture) {
            printf("Failed to load animation: %s\n", paths[state]);
            
            // CRITICAL: Clean up previously loaded animations
            for (int cleanup = 0; cleanup < state; cleanup++) {
                if (dog->animations[cleanup].sprite) {
                    sfSprite_destroy(dog->animations[cleanup].sprite);
                    dog->animations[cleanup].sprite = NULL;
                }
                if (dog->animations[cleanup].texture) {
                    sfTexture_destroy(dog->animations[cleanup].texture);
                    dog->animations[cleanup].texture = NULL;
                }
                if (dog->animations[cleanup].animationClock) {
                    sfClock_destroy(dog->animations[cleanup].animationClock);
                    dog->animations[cleanup].animationClock = NULL;
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
                if (dog->animations[cleanup].sprite) {
                    sfSprite_destroy(dog->animations[cleanup].sprite);
                    dog->animations[cleanup].sprite = NULL;
                }
                if (dog->animations[cleanup].texture) {
                    sfTexture_destroy(dog->animations[cleanup].texture);
                    dog->animations[cleanup].texture = NULL;
                }
                if (dog->animations[cleanup].animationClock) {
                    sfClock_destroy(dog->animations[cleanup].animationClock);
                    dog->animations[cleanup].animationClock = NULL;
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
                if (dog->animations[cleanup].sprite) {
                    sfSprite_destroy(dog->animations[cleanup].sprite);
                    dog->animations[cleanup].sprite = NULL;
                }
                if (dog->animations[cleanup].texture) {
                    sfTexture_destroy(dog->animations[cleanup].texture);
                    dog->animations[cleanup].texture = NULL;
                }
                if (dog->animations[cleanup].animationClock) {
                    sfClock_destroy(dog->animations[cleanup].animationClock);
                    dog->animations[cleanup].animationClock = NULL;
                }
            }
            printf("Failed to create clock for animation %d\n", state);
            return 0;
        }
        
        // Set initial frame (first frame of the animation)
        sfIntRect rect = {0, 0, frameWidth, frameHeight};
        sfSprite_setTextureRect(anim->sprite, rect);
    }
    
    dog->isLoaded = 1;
    return 1;
}



void dog_setState(Dog* dog, DogState state) {
    if (!dog || state >= 6) return;
    
    if (dog->currentState != state) {
        dog->currentState = state;
        // Reset animation when changing state
        dog->animations[state].currentFrame = 0;
        sfClock_restart(dog->animations[state].animationClock);
    }
}

void dog_setPosition(Dog* dog, float x, float y) {
    if (!dog) return;
    
    dog->position = (sfVector2f){x, y};
    
    // Update sprite positions
    for (int i = 0; i < 6; i++) {
        if (dog->animations[i].sprite) {
            sfSprite_setPosition(dog->animations[i].sprite, dog->position);
        }
    }
}

void dog_move(Dog* dog, float deltaX, float deltaY) {
    if (!dog) return;
    
    dog->position.x += deltaX;
    dog->position.y += deltaY;
    
    // Update current animation sprite position
    DogAnimation* currentAnim = dog_getCurrentAnimation(dog);
    if (currentAnim && currentAnim->sprite) {
        sfSprite_setPosition(currentAnim->sprite, dog->position);
    }
}


DogAnimation* dog_getCurrentAnimation(Dog* dog) {
    if (!dog || !dog->isLoaded) return NULL;
    return &dog->animations[dog->currentState];
}


void dog_updateAnimation(DogAnimation* anim) {
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

void dog_update(Dog* dog, float ) {
    if (!dog || !dog->isLoaded) return;
    
    DogAnimation* currentAnim = dog_getCurrentAnimation(dog);
    if (currentAnim) {
        dog_updateAnimation(currentAnim);
    }
}


void dog_draw(sfRenderWindow* window, Dog* dog) {
    if (!dog || !dog->isLoaded || !window) return;
    
    DogAnimation* currentAnim = dog_getCurrentAnimation(dog);
    if (currentAnim && currentAnim->sprite) {
        sfRenderWindow_drawSprite(window, currentAnim->sprite, NULL);
    }
}



void dog_destroy(Dog* dog) {
    if (!dog) return;
    
    // Destroy animations
    for (int i = 0; i < 6; i++) {
        if (dog->animations[i].sprite) {
            sfSprite_destroy(dog->animations[i].sprite);
            dog->animations[i].sprite = NULL;
        }
        if (dog->animations[i].animationClock) {
            sfClock_destroy(dog->animations[i].animationClock);
            dog->animations[i].animationClock = NULL;
        }
        if (dog->animations[i].texture) {
            sfTexture_destroy(dog->animations[i].texture);
            dog->animations[i].texture = NULL;
        }
    }
    
    free(dog);
}