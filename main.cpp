#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_image.h>
#include "entity.h"
#include <math.h>
#include <vector>

using namespace std;

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* player2Texture = nullptr;
SDL_Texture* fireTexture = nullptr;
SDL_Texture* puddleTexture = nullptr;

int main(int argc, char** argv) {
    srand(time(NULL));
    // zagon sdl in sdl_image
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG);
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow("lol loser", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // nalaganj tekstur
    backgroundTexture = setTexture(renderer, "background.jpg");
    playerTexture = setTexture(renderer, "player.png");
    player2Texture = setTexture(renderer, "player2.png");
    fireTexture = setTexture(renderer, "fire.png");
    puddleTexture = setTexture(renderer, "puddle.png");
    //definiranje igralca, nasprotnikov, ognja
    player object;
    vector<fire> fireVector(1);
    puddle puddleObject;
    while(!checkFirestartpos(fireVector, object, puddleObject)){
        fireVector[0]=fire();
    }
    
    bool running = true;
    int counter=0;
    // Main loop
    //spremenljivke za preverjanje pozicije ognja
    fire current=fireVector[0];
    fire next;
    int fireNumber=1;
    while (running) {
        cout<<counter<<"  "<<fireNumber<<endl;
        counter++;
        Uint32 frameStart = SDL_GetTicks();
        if(counter>=40)
        {   
            next=fireSpread(fireVector[rand()%fireVector.size()], fireVector);
            while((next.xcord==current.xcord) && (next.ycord==current.ycord)){
                next=fireSpread(current, fireVector);
            }
            fireVector.push_back(next);
            current=next;
            counter=0;
            fireNumber++;
        }
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    object = getPosition(object, event);
                    break;
                default:
                    break;
            }
        }
        //preverjanje ce je igralec v vodi
        object=checkPlayerWater(object, puddleObject);
        // brisanje starega frejma
        SDL_RenderClear(renderer);
        //risanje ozadja
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        //render luža
        SDL_Rect puddleRect = {puddleObject.xcord, puddleObject.ycord, puddleObject.width, puddleObject.height};
        SDL_RenderCopy(renderer, puddleTexture, nullptr, &puddleRect);
        
        //render ogenj
        for(int i=0; i<fireVector.size(); i++){
            SDL_Rect fireRect = {fireVector[i].xcord, fireVector[i].ycord, fireVector[i].width, fireVector[i].height};
            SDL_RenderCopy(renderer, fireTexture, nullptr, &fireRect);
            
        }
        // risanje igralca brez al z vodo
        if(object.water==true){
            SDL_Rect playerRect = {object.xcord, object.ycord, object.width, object.height};
            SDL_RenderCopy(renderer, player2Texture, nullptr, &playerRect);
        }
        else {SDL_Rect playerRect = {object.xcord, object.ycord, object.width, object.height};
        SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
        }
        
        
        
        //xcordprev=object.xcord; 
        // risnje novega frejma
        SDL_RenderPresent(renderer);

        // frame rate kontrola
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // ugasni sdl in sdl_image
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
