#ifndef DOG_H
#define DOG_H

#include <SFML/Graphics.h>

// Dog states

typedef enum{
    DOG_WALK,
    DOG_ALERT,
    DOG_JUMP,
    DOG_GOT,
    DOG_LAUGH,
    DOG_DEFAULT,
    DOG_TOTAL_STATES
} DogState;

// Dog animation

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
} DogAnimation;


// Dog structure

typedef struct {
    DogAnimation animations[DOG_TOTAL_STATES];
    DogState currentState;
    sfVector2f position;
    sfVector2f velocity;
    int isLoaded;
} Dog;


// Function declarations

Dog* dog_create(void);

int dog_loadAnimations(Dog* dog, const char* walkPath, const char* alertPath, 
                      const char* jumpPath, const char* gotPath,
                      const char* laughPath,
                      const char* defaultPath,
                      int frameWidth, int frameHeight);

void dog_setState(Dog* dog, DogState state);

void dog_setPosition(Dog* dog, float x, float y);

void dog_move(Dog* dog, float deltaX, float deltaY);

void dog_update(Dog* dog, float deltaTime);

void dog_draw(sfRenderWindow* window, Dog* dog);

void dog_destroy(Dog* dog);


// Animation helper functions
DogAnimation* dog_getCurrentAnimation(Dog* dog);
void dog_updateAnimation(DogAnimation* anim);


#endif // DOG_H
