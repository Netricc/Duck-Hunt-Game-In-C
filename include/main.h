#ifndef MAIN_H
#define MAIN_H


#include <SFML/Graphics.h>
#include "dog.h"


typedef struct {
    bool isActive;
    float timer;
    float duration;
    DogState targetState;
    DogState returnState;
} AnimationTimer;

typedef enum {
    CUTSCENE_START,
    CUTSCENE_WALKING,
    CUTSCENE_ALERT,
    CUTSCENE_JUMP,
    GAME_PLAYING
} GamePhase;


#endif // DOG_H
