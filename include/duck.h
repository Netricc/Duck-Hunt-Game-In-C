#ifndef DUCK_H
#define DUCK_H

#include <SFML/Graphics.h>
#include <stdbool.h>


// Duck states

typedef enum{
    DUCK_FLY,
    DUCK_OUT,
    DUCK_DEATH,
    DUCK_SCARED,
    DUCK_TOTAL_STATES
} DuckState;

// Duck animation

typedef struct {
    sfTexture* texture;
    sfSprite* sprite;
    int frameWidth;
    int frameHeight;
    int currentFrame;
    int totalFrames;
    int startFrame;
    float animationSpeed;
    sfClock* animationClock;
} DuckAnimation;


// Duck structure

typedef struct {
    DuckAnimation animations[DUCK_TOTAL_STATES];
    DuckState currentState;
    sfVector2f position;
    sfVector2f velocity;
    sfSprite* sprite;
    int isLoaded;
} Duck;


// Function declarations

Duck* duck_create(void);

int duck_loadAnimations(Duck* duck, const char* flyPath, const char* outPath, 
                      const char* deathPath, const char* scaredPath,
                      int frameWidth, int frameHeight);

void duck_setState(Duck* duck, DuckState state);

void duck_setPosition(Duck* duck, float x, float y);

void duck_move(Duck* duck, float deltaX, float deltaY);

void duck_setFlip(Duck* duck, bool flipHorizontal, bool flipVertical);

void duck_update(Duck* duck, float deltaTime);

void duck_draw(sfRenderWindow* window, Duck* duck);

void duck_destroy(Duck* duck);


// Animation helper functions
DuckAnimation* duck_getCurrentAnimation(Duck* duck);
void duck_updateAnimation(DuckAnimation* anim);


#endif // Duck_H
