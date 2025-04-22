#pragma once

#include <SDL2/SDL.h>       // Include SDL2 for SDL_Renderer and SDL_Texture
#include <SDL2/SDL_image.h> // Include SDL2_image if needed
#include <vector>
#include <string>
using namespace std;

// Forward declaration of `player` and `fire` classes
class player;
class fire;

void saveGame(player& object, vector<player>& enemy, vector<fire>& fireVector, bool lv1, bool lv2);
void loadSave(player& object, vector<player>& enemy, vector<fire>& fireVector, bool& lv1, bool& lv2);
void clearSave();
void loadScreen(SDL_Renderer* renderer, SDL_Texture* loadingScreen, player& object, vector<player>& enemy, vector<fire>& fireVector, bool& lv1, bool& lv2);
