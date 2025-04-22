#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_image.h>
#include "entity.h"
#include "loadscreen.h"
#include <vector>
#include <iterator>

using namespace std;

int SCREEN_WIDTH = 1280; 
int SCREEN_HEIGHT = 720;
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* player2Texture = nullptr;
SDL_Texture* fireTexture = nullptr;
SDL_Texture* puddleTexture = nullptr;
SDL_Texture* loadingScreen = nullptr;
SDL_Texture* enemyTexture = nullptr;

int main(int argc, char** argv) {
    srand(time(NULL));
    // Initialize SDL and SDL_image
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG);
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow("Resi amazonski pragozd!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load textures
    backgroundTexture = setTexture(renderer, "background.jpg");
    playerTexture = setTexture(renderer, "player.png");
    player2Texture = setTexture(renderer, "player2.png");
    fireTexture = setTexture(renderer, "fire.png");
    puddleTexture = setTexture(renderer, "puddle.png");
    loadingScreen = setTexture(renderer, "loadingscreen.png");
    enemyTexture = setTexture(renderer, "enemy.png");

    bool running = true;
    int counter = 0;
    bool lv1 = true;
    bool lv2 = false; // Add a new level flag
    bool endGame = false;
    Uint32 lastIgniteTime = 0; // Track the last time enemies ignited a fire
    Uint32 lastMoveTime = 0; // Track the last time enemies moved
    Uint32 lastActionTime = 0; // Track the last time enemies moved and placed fire
    Uint32 lastFrameTime = SDL_GetTicks(); // Initialize the last frame time
    
    // Define player, enemies, fire, and puddle
    player object;
    vector<player> enemy(2);
    vector<fire> fireVector(1);
    puddle puddleObject;

    // Starting screen
    loadScreen(renderer, loadingScreen, object, enemy, fireVector, lv1, lv2);

    // Initialize fire if empty
    if (fireVector.size() == 0) {
        while (!checkFirestartpos(fireVector, object, puddleObject)) {
            fireVector[0] = fire();
        }
    }

   

    // Main loop
    fire current = fireVector[0];
    fire next;
    while (running) {
        while (lv1) {
            Uint32 frameStart = SDL_GetTicks();

            // Calculate delta time
            float deltaTime = (frameStart - lastFrameTime) / 1000.0f; // Convert milliseconds to seconds
            lastFrameTime = frameStart; // Update the last frame time

            // Spread fire
            counter++;
            if (counter >= 600) {
                next = fireSpread(fireVector[rand() % fireVector.size()], fireVector);
                while ((next.xcord == current.xcord) && (next.ycord == current.ycord)) {
                    next = fireSpread(current, fireVector);
                }
                fireVector.push_back(next);
                current = next;
                counter = 0;
            }

            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        saveGame(object, enemy, fireVector, lv1, lv2);
                        running = false;
                        lv1= false;
                        lv2 = false;
                        break;
                    case SDL_KEYDOWN:
                        object = getPosition(object, event, deltaTime);
                        break;
                    case SDL_WINDOWEVENT:
                        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                            SCREEN_WIDTH = event.window.data1;
                            SCREEN_HEIGHT = event.window.data2;
                            SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

                            // Adjust puddle position and size
                            puddleObject.xcord = SCREEN_WIDTH / 2 - puddleObject.width / 2;
                            puddleObject.ycord = SCREEN_HEIGHT / 2 - puddleObject.height / 2;
                            puddleObject.width = SCREEN_WIDTH / 10;  // Scale puddle width
                            puddleObject.height = SCREEN_HEIGHT / 10; // Scale puddle height
                            puddleObject.centerx = puddleObject.xcord + puddleObject.width / 2;
                            puddleObject.centery = puddleObject.ycord + puddleObject.height / 2;

                            // Adjust player size
                            object.width = SCREEN_WIDTH / 20;  // Scale player width
                            object.height = SCREEN_HEIGHT / 20; // Scale player height
                            object.centerx = object.xcord + object.width / 2;
                            object.centery = object.ycord + object.height / 2;

                            // Adjust enemy sizes
                            for (auto& enemyObj : enemy) {
                                enemyObj.width = SCREEN_WIDTH / 20;  // Scale enemy width
                                enemyObj.height = SCREEN_HEIGHT / 20; // Scale enemy height
                                enemyObj.centerx = enemyObj.xcord + enemyObj.width / 2;
                                enemyObj.centery = enemyObj.ycord + enemyObj.height / 2;
                            }

                            // Adjust fire sizes
                            for (auto& fireObj : fireVector) {
                                fireObj.width = SCREEN_WIDTH / 30;  // Scale fire width
                                fireObj.height = SCREEN_HEIGHT / 30; // Scale fire height
                                fireObj.centerx = fireObj.xcord + fireObj.width / 2;
                                fireObj.centery = fireObj.ycord + fireObj.height / 2;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            // Check if level 1 is complete
            if (fireVector.size() == 0) {
                lv1 = false;
                lv2 = true;

                // Initialize level 2
                enemy.clear();
                for (int i = 0; i < 4; ++i) {
                    player newEnemy;
                    newEnemy.xcord = rand() % (SCREEN_WIDTH - 50);
                    newEnemy.ycord = rand() % (SCREEN_HEIGHT - 50);
                    newEnemy.width = 50;
                    newEnemy.height = 50;
                    newEnemy.centerx = newEnemy.xcord + newEnemy.width / 2;
                    newEnemy.centery = newEnemy.ycord + newEnemy.height / 2;
                    enemy.push_back(newEnemy);
                }
            }

            // Check player interactions
            object = checkPlayerWater(object, puddleObject);
            object = checkPlayerFire(object, &fireVector);

            // Remove enemies if the player is close
            removeEnemyIfClose(object, enemy);

            // Move enemies and place fire every 5 seconds
            moveEnemiesAndIgniteFire(enemy, fireVector, lastActionTime);

            // Render everything
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

            // Render puddle
            SDL_Rect puddleRect = {puddleObject.xcord, puddleObject.ycord, puddleObject.width, puddleObject.height};
            SDL_RenderCopy(renderer, puddleTexture, nullptr, &puddleRect);

            // Render fire
            for (const auto& fireObj : fireVector) {
                SDL_Rect fireRect = {fireObj.xcord, fireObj.ycord, fireObj.width, fireObj.height};
                SDL_RenderCopy(renderer, fireTexture, nullptr, &fireRect);
            }

            // Render player
            SDL_Rect playerRect = {object.xcord, object.ycord, object.width, object.height};
            if (object.water) {
                SDL_RenderCopy(renderer, player2Texture, nullptr, &playerRect);
            } else {
                SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
            }

            // Render enemies
            for (const auto& enemyObj : enemy) {
                SDL_Rect enemyRect = {enemyObj.xcord, enemyObj.ycord, enemyObj.width, enemyObj.height};
                SDL_RenderCopy(renderer, enemyTexture, nullptr, &enemyRect);
            }

            SDL_RenderPresent(renderer);

            // Frame rate control
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < FRAME_DELAY) {
                SDL_Delay(FRAME_DELAY - frameTime);
            }
        }

        while (lv2) {
            Uint32 frameStart = SDL_GetTicks();

            // Calculate delta time
            float deltaTime = (frameStart - lastFrameTime) / 1000.0f; // Convert milliseconds to seconds
            lastFrameTime = frameStart; // Update the last frame time

            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                        saveGame(object, enemy, fireVector, lv1, lv2);
                        running = false;
                        lv1= false;
                        lv2 = false;
                        break;
                    case SDL_KEYDOWN:
                        object = getPosition(object, event, deltaTime);
                        break;
                        case SDL_WINDOWEVENT:
                        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                            SCREEN_WIDTH = event.window.data1;
                            SCREEN_HEIGHT = event.window.data2;
                            SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
                    
                            // Adjust puddle position and size
                            puddleObject.xcord = SCREEN_WIDTH / 2 - SCREEN_WIDTH / 10 / 2;
                            puddleObject.ycord = SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10 / 2;
                            puddleObject.width = SCREEN_WIDTH / 10;  // Scale puddle width
                            puddleObject.height = SCREEN_HEIGHT / 10; // Scale puddle height
                            puddleObject.centerx = puddleObject.xcord + puddleObject.width / 2;
                            puddleObject.centery = puddleObject.ycord + puddleObject.height / 2;
                    
                            // Adjust player size
                            object.width = SCREEN_WIDTH / 20;  // Scale player width
                            object.height = SCREEN_HEIGHT / 20; // Scale player height
                            object.centerx = object.xcord + object.width / 2;
                            object.centery = object.ycord + object.height / 2;
                    
                            // Adjust enemy sizes
                            for (auto& enemyObj : enemy) {
                                enemyObj.width = SCREEN_WIDTH / 20;  // Scale enemy width
                                enemyObj.height = SCREEN_HEIGHT / 20; // Scale enemy height
                                enemyObj.centerx = enemyObj.xcord + enemyObj.width / 2;
                                enemyObj.centery = enemyObj.ycord + enemyObj.height / 2;
                            }
                    
                            // Adjust fire sizes
                            for (auto& fireObj : fireVector) {
                                fireObj.width = SCREEN_WIDTH / 30;  // Scale fire width
                                fireObj.height = SCREEN_HEIGHT / 30; // Scale fire height
                                fireObj.centerx = fireObj.xcord + fireObj.width / 2;
                                fireObj.centery = fireObj.ycord + fireObj.height / 2;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            counter++;
            if (counter >= 600) {
                next = fireSpread(fireVector[rand() % fireVector.size()], fireVector);
                while ((next.xcord == current.xcord) && (next.ycord == current.ycord)) {
                    next = fireSpread(current, fireVector);
                }
                fireVector.push_back(next);
                current = next;
                counter = 0;
            }

            // Check if the player interacts with water or fire
            object = checkPlayerWater(object, puddleObject);
            object = checkPlayerFire(object, &fireVector);

            // Remove enemies if the player is close
            removeEnemyIfClose(object, enemy);

            // Move enemies and place fire every 5 seconds
            moveEnemiesAndIgniteFire(enemy, fireVector, lastActionTime);

            // Render everything
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

            // Render puddle
            SDL_Rect puddleRect = {puddleObject.xcord, puddleObject.ycord, puddleObject.width, puddleObject.height};
            SDL_RenderCopy(renderer, puddleTexture, nullptr, &puddleRect);

            // Render fire
            for (const auto& fireObj : fireVector) {
                SDL_Rect fireRect = {fireObj.xcord, fireObj.ycord, fireObj.width, fireObj.height};
                SDL_RenderCopy(renderer, fireTexture, nullptr, &fireRect);
            }

            // Render player
            SDL_Rect playerRect = {object.xcord, object.ycord, object.width, object.height};
            if (object.water) {
                SDL_RenderCopy(renderer, player2Texture, nullptr, &playerRect);
            } else {
                SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
            }

            // Render enemies
            for (const auto& enemyObj : enemy) {
                SDL_Rect enemyRect = {enemyObj.xcord, enemyObj.ycord, enemyObj.width, enemyObj.height};
                SDL_RenderCopy(renderer, enemyTexture, nullptr, &enemyRect);
            }

            SDL_RenderPresent(renderer);

            // Frame rate control
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < FRAME_DELAY) {
                SDL_Delay(FRAME_DELAY - frameTime);
            }
            if(enemy.size()==0 && fireVector.size()==0){
                endGame = true;
                lv2 = false;
                cout << "You have completed the game!" << endl;
                cout << "You have saved the Amazon rainforest!" << endl;
            }
        }
    }

    // Save or clear save file
    if (endGame) {
        clearSave();
    } else {
        saveGame(object, enemy, fireVector, lv1, lv2);
    }

    // Clean up SDL and SDL_image
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
