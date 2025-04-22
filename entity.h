#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <iterator>

using namespace std;
class puddle{
    public:
    int xcord;
    int ycord;
    int width;
    int height;
    int centerx;
    int centery;
    puddle();
};
class player{
    public:
    int xcord;
    int ycord;
    int width;
    int height;
    bool water;
    int centerx;
    int centery;
    player();


};
class fire{
    public:
    int xcord;
    int ycord;
    int width;
    int height;
    int centerx;
    int centery;
    fire();
    //~fire();
};

fire fireSpread(fire predhodnik, vector<fire> fireVector);
player getPosition(player object, SDL_Event event, float deltaTime);
bool checkFirestartpos(vector<fire> fireVector, player playerObject, puddle puddleObject);
SDL_Texture* setTexture(SDL_Renderer* renderer, const char* path);
player checkPlayerWater(player object, puddle puddleObject);
player checkPlayerFire(player object, vector<fire>* fireVector);
void moveEnemiesAndIgniteFire(vector<player>& enemies, vector<fire>& fireVector, Uint32& lastActionTime);
void removeEnemyIfClose(player& object, vector<player>& enemies);