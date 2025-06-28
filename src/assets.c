#include "./../include/texture.h"
#include <stdio.h>

static sfSprite* createSpriteFromFile(const char* filename, sfTexture** outTexture, sfVector2f position) {
    sfTexture* texture = sfTexture_createFromFile(filename, NULL);
    if (!texture) {
        printf("Failed to load image: %s\n", filename);
        return NULL;
    }
    sfSprite* sprite = sfSprite_create();
    sfSprite_setTexture(sprite, texture, sfTrue);
    sfSprite_setPosition(sprite, position);

    *outTexture = texture;
    return sprite;
}

int loadAssets(Assets* assets) {
    assets->bg = createSpriteFromFile("/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/bg/sprite_bg.png", &assets->bgTexture, (sfVector2f){0,0});
    assets->ground = createSpriteFromFile("/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/bg/sprite_bg_ground.png", &assets->groundTexture, (sfVector2f){0,0});
    assets->cloud1 = createSpriteFromFile("/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/bg/sprite_bg_clouds1.png", &assets->cloud1Texture, (sfVector2f){0,0});
    assets->cloud2 = createSpriteFromFile("/home/netric/Desktop/work/programming/main-projects/C Projects/duck-hunt-game/assets/bg/sprite_bg_clouds2.png", &assets->cloud2Texture, (sfVector2f){0,0});

    if (!assets->bg || !assets->ground || !assets->cloud1 || !assets->cloud2) {
        return 1;
    }
    return 0;
}

void freeAssets(Assets* assets) {
    if (assets->bg) sfSprite_destroy(assets->bg);
    if (assets->ground) sfSprite_destroy(assets->ground);
    if (assets->cloud1) sfSprite_destroy(assets->cloud1);
    if (assets->cloud2) sfSprite_destroy(assets->cloud2);

    if (assets->bgTexture) sfTexture_destroy(assets->bgTexture);
    if (assets->groundTexture) sfTexture_destroy(assets->groundTexture);
    if (assets->cloud1Texture) sfTexture_destroy(assets->cloud1Texture);
    if (assets->cloud2Texture) sfTexture_destroy(assets->cloud2Texture);
}
