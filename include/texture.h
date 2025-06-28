#ifndef ASSETS_H
#define ASSETS_H

#include <SFML/Graphics.h>

// Struct to hold all your game assets in one place
typedef struct {
    sfTexture* bgTexture;
    sfSprite* bg;

    sfTexture* groundTexture;
    sfSprite* ground;

    sfTexture* cloud1Texture;
    sfSprite* cloud1;

    sfTexture* cloud2Texture;
    sfSprite* cloud2;

} Assets;

// Function declarations
int loadAssets(Assets* assets);
void freeAssets(Assets* assets);

#endif // ASSETS_H
