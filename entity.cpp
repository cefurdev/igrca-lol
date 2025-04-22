#include "entity.h"
#include "loadscreen.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


player::player(){
    
    xcord=rand()%SCREEN_WIDTH;
    ycord=rand()%SCREEN_HEIGHT;
    width=100;
    height=100;
    water=false;
    centerx=xcord+width/2;
    centery=ycord+height/2;
}
fire::fire(){
    
    xcord=50*(rand()%(SCREEN_WIDTH/50));
    ycord=50*(rand()%(SCREEN_HEIGHT/50));
    centerx=xcord+width/2;
    centery=ycord+height/2;
    width=50;
    height=50;
}
puddle::puddle(){
    
    xcord=SCREEN_WIDTH/2;
    ycord=SCREEN_HEIGHT/2;
    width=120;
    height=120;
    centerx=xcord+width/2;
    centery=ycord+height/2;

}

//funkcija ki preveri ce se zacetna pozicija ognja nahaja na igralcu ali vodi
bool checkFirestartpos(vector<fire> fireVector, player playerObject, puddle puddleObject){
    
        if(fireVector[0].xcord==playerObject.xcord && fireVector[0].ycord==playerObject.ycord){
            return false;
        }
        if(fireVector[0].xcord==puddleObject.xcord && fireVector[0].ycord==puddleObject.ycord){
            return false;
        }
    return true;
}
//funkcija za sirjenje ognja
fire fireSpread(fire predhodnik, vector<fire> fireVector) {
    srand(time(NULL));
    fire newFire;
    int attempts = 0;
    const int maxAttempts = 10;
    
    do {
        // Randomly choose direction
        switch (rand() % 4) {
            case 0: newFire.xcord = predhodnik.xcord + 50; break;
            case 1: newFire.xcord = predhodnik.xcord - 50; break;
            case 2: newFire.ycord = predhodnik.ycord + 50; break;
            case 3: newFire.ycord = predhodnik.ycord - 50; break;
        }
        
        // Boundary checks
        newFire.xcord = max(0, min(SCREEN_WIDTH - newFire.width, newFire.xcord));
        newFire.ycord = max(0, min(SCREEN_HEIGHT - newFire.height, newFire.ycord));
        
        // Check water proximity
        int waterLeft = SCREEN_WIDTH/2 - 60;
        int waterRight = SCREEN_WIDTH/2 + 60;
        int waterTop = SCREEN_HEIGHT/2 - 60;
        int waterBottom = SCREEN_HEIGHT/2 + 60;
        
        if (!(newFire.xcord < waterRight && 
              newFire.xcord + newFire.width > waterLeft &&
              newFire.ycord < waterBottom &&
              newFire.ycord + newFire.height > waterTop)) {
            break; // Valid position found
        }
        
        attempts++;
    } while (attempts < maxAttempts);
    
    return newFire;
}

/*fire::~fire(){
    
}*/

//funkcija ki nastavi pozicijo igralca
player getPosition(player object, SDL_Event event, float deltaTime) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(SDL_GetWindowFromID(event.window.windowID), &windowWidth, &windowHeight);

    // Calculate speed relative to the window height
    const float speed = windowHeight * 1; 

    // Check if a key was pressed
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    object.ycord -= speed * deltaTime;
                    object.centery -= speed * deltaTime;
                    break;
                case SDLK_s:
                    object.ycord += speed * deltaTime;
                    object.centery += speed * deltaTime;
                    break;
                case SDLK_a:
                    object.xcord -= speed * deltaTime;
                    object.centerx -= speed * deltaTime;
                    break;
                case SDLK_d:
                    object.xcord += speed * deltaTime;
                    object.centerx += speed * deltaTime;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    // Ensure the player stays within the window boundaries
    if (object.xcord > windowWidth - object.width) {
        object.xcord = windowWidth - object.width;
        object.centerx = windowWidth - object.width / 2;
    } else if (object.xcord < 0) {
        object.xcord = 0;
        object.centerx = object.width / 2;
    }
    if (object.ycord > windowHeight - object.height) {
        object.ycord = windowHeight - object.height;
        object.centery = windowHeight - object.height / 2;
    } else if (object.ycord < 0) {
        object.ycord = 0;
        object.centery = object.height / 2;
    }

    return object;
}
//funkcija ki nastavi teksturo
SDL_Texture* setTexture(SDL_Renderer* renderer, const char* path){
    SDL_Surface* convertToTexture=IMG_Load(path);
    SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer, convertToTexture);
    SDL_FreeSurface(convertToTexture);
    return texture;
}
//funkcija za pobiranje vode
player checkPlayerWater(player object, puddle puddleObject) {
    float dx = object.centerx - puddleObject.centerx;
    float dy = object.centery - puddleObject.centery;
    float distanceSquared = dx*dx + dy*dy;
    
    // Compare squared distances (75² = 5625)
    if (distanceSquared < 5625.0f) {
        object.water = true;
    }
    
    return object;
}
//kolizija med igrlacem in ognjem
player checkPlayerFire(player object, vector<fire>* fireVector) {
    
    
    // Find closest fire using squared distance
    unsigned long long closestDistSq = -1;
    int closestIndex = 0;
    
    for (int i = 0; i < fireVector->size(); i++) {
        long long  dx = object.centerx - (*fireVector)[i].xcord;
        long long dy = object.centery - (*fireVector)[i].ycord;
        unsigned long long distSq = dx*dx + dy*dy;
        //я нихуя не знаю, что делать с этой хуйней
        //я не понимаю, что делать с этой хуйней
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closestIndex = i;
        }
    }

    cout << "Closest fire at index: " << closestIndex << endl;
    cout << "Distance squared: " << closestDistSq << endl;
    cout << "Player water status: " << (object.water ? "true" : "false") << endl;

    // Only extinguish if player has water
    if (closestDistSq < 7225 && object.water) { // 85² = 7225
        fireVector->erase(fireVector->begin() + closestIndex);
        object.water = false; // Player used their water
    }
   
    
    return object;
}

void moveEnemiesAndIgniteFire(vector<player>& enemies, vector<fire>& fireVector, Uint32& lastActionTime) {
    Uint32 currentTime = SDL_GetTicks();

    // Get the current window size
    int windowWidth, windowHeight;
    SDL_GetWindowSize(SDL_GetWindowFromID(1), &windowWidth, &windowHeight);

    // Calculate movement step relative to the window size
    int step = windowHeight / 20; // 5% of the window height

    // Perform action every 5 seconds (5000 milliseconds)
    if (currentTime - lastActionTime >= 5000) {
        for (auto& enemy : enemies) {
            // Move the enemy in a random direction
            int direction = rand() % 4; // Random direction: 0 = up, 1 = down, 2 = left, 3 = right
            switch (direction) {
                case 0: // Move up
                    enemy.ycord = max(0, enemy.ycord - step);
                    enemy.centery = enemy.ycord + enemy.height / 2;
                    break;
                case 1: // Move down
                    enemy.ycord = min(windowHeight - enemy.height, enemy.ycord + step);
                    enemy.centery = enemy.ycord + enemy.height / 2;
                    break;
                case 2: // Move left
                    enemy.xcord = max(0, enemy.xcord - step);
                    enemy.centerx = enemy.xcord + enemy.width / 2;
                    break;
                case 3: // Move right
                    enemy.xcord = min(windowWidth - enemy.width, enemy.xcord + step);
                    enemy.centerx = enemy.xcord + enemy.width / 2;
                    break;
            }

            // Place fire at the enemy's new position if no fire exists there
            bool fireExists = false;
            for (const auto& fireObj : fireVector) {
                if (fireObj.xcord == enemy.xcord && fireObj.ycord == enemy.ycord) {
                    fireExists = true;
                    break;
                }
            }
            if (!fireExists) {
                fire newFire;
                newFire.xcord = enemy.xcord;
                newFire.ycord = enemy.ycord;
                newFire.width = windowWidth / 30; // Scale fire width
                newFire.height = windowHeight / 30; // Scale fire height
                newFire.centerx = newFire.xcord + newFire.width / 2;
                newFire.centery = newFire.ycord + newFire.height / 2;
                fireVector.push_back(newFire);
            }
        }

        // Update the last action time
        lastActionTime = currentTime;
    }
}

void removeEnemyIfClose(player& object, vector<player>& enemies) {
    const int thresholdDistanceSquared = 7225; // 85² = 7225 (distance threshold for removal)
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        int dx = object.centerx - it->centerx;
        int dy = object.centery - it->centery;
        int distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < thresholdDistanceSquared) {
            cout << "Enemy removed at position: (" << it->xcord << ", " << it->ycord << ")" << endl;
            it = enemies.erase(it); // Remove the enemy and update the iterator
        } else {
            ++it; // Move to the next enemy
        }
    }
}

