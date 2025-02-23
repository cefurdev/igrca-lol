#include "entity.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <vector>
#include <iterator>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


player::player(){
    srand(time(NULL));
    xcord=rand()%SCREEN_WIDTH;
    ycord=rand()%SCREEN_HEIGHT;
    width=100;
    height=100;
    water=false;
    centerx=xcord+width/2;
    centery=ycord+height/2;
}
fire::fire(){
    srand(time(NULL));
    xcord=50*(rand()%(SCREEN_WIDTH/50));
    ycord=50*(rand()%(SCREEN_HEIGHT/50));
    centerx=xcord+width/2;
    centery=ycord+height/2;
    width=50;
    height=50;
}
puddle::puddle(){
    srand(time(NULL));
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
    // Randomly choose the direction for fire spread
    switch (rand() % 4) {
        case 0:
            newFire.xcord = predhodnik.xcord + 50;
            newFire.ycord = predhodnik.ycord;
            break;
        case 1:
            newFire.xcord = predhodnik.xcord - 50;
            newFire.ycord = predhodnik.ycord;
            break;
        case 2:
            newFire.xcord = predhodnik.xcord;
            newFire.ycord = predhodnik.ycord + 50;
            break;
        case 3:
            newFire.xcord = predhodnik.xcord;
            newFire.ycord = predhodnik.ycord - 50;
            break;
        default:
            break;
    }
    /*ne dela trenutno
    // Ensure the fire does not spread near the water
    if (newFire.xcord <= SCREEN_WIDTH / 2 + 120 && newFire.xcord >= SCREEN_WIDTH / 2 - 120 &&
        newFire.ycord <= SCREEN_HEIGHT / 2 + 120 && newFire.ycord >= SCREEN_HEIGHT / 2 - 120) {
        newFire.xcord = predhodnik.xcord;
        newFire.ycord = predhodnik.ycord;
    }
    return newFire;
    // Ensure the fire stays within the window boundaries
    if (newFire.xcord >= SCREEN_WIDTH) {
        newFire.xcord = predhodnik.xcord - 50;
    } else if (newFire.xcord < 0) {
        newFire.xcord = predhodnik.xcord + 50;
    } else if (newFire.ycord >= SCREEN_HEIGHT) {
        newFire.ycord = predhodnik.ycord - 50;
    } else if (newFire.ycord < 0) {
        newFire.ycord = predhodnik.ycord + 50;
    }
*/   
    return newFire; 
}

/*fire::~fire(){
    
}*/

//funkcija ki nastavi pozicijo igralca
player getPosition(player object, SDL_Event event){
    //preverjanje ce je bila pritisnjena tipka
    switch(event.type){
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case SDLK_w:
                    object.ycord-=15;
                    object.centery-=15;
                    break;
                case SDLK_s:
                    object.ycord+=15;
                    object.centery+=15;
                    break;
                case SDLK_a:
                    object.xcord-=15;
                    object.centerx-=15;
                    break;
                case SDLK_d:
                    object.xcord+=15;
                    object.centerx+=15;
                    break;
                default:
                   break;
                }
                break;
            default:
                break;
        }
    //preverjanje ce je igralec v oknu
    if(object.xcord%SCREEN_WIDTH-object.height){
        object.xcord%SCREEN_WIDTH-object.height;
    }
    else if(object.xcord<0){
        object.xcord=0;
    }
    else if(object.ycord>SCREEN_HEIGHT-object.width){
        object.ycord=SCREEN_HEIGHT-object.width;
    }
    else if(object.ycord<0){
        object.ycord=0;
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
player checkPlayerWater(player object, puddle puddleObject){
    if(object.water){
        return object;
    }

    else if(sqrt(pow(object.centerx-puddleObject.centerx, 2)+pow(object.centery-puddleObject.centery, 2))<60){
        
        object.water=true;
    }
    else{
        object.water=false;
    }
    cout<<sqrt(pow(object.centerx-puddleObject.centerx, 2)+pow(object.centery-puddleObject.centery, 2))<<endl;
    return object;
}
//kolizija med igrlacem in ognjem

void playerFireCollision(player *object, vector<fire> fireVector){
    for(int i=0; i<fireVector.size(); i++){
        if(sqrt(pow(object->centerx-fireVector[i].centerx, 2)+pow(object->centery-fireVector[i].centery, 2))<50){
            cout<<sqrt(pow(object->centerx-fireVector[i].centerx, 2)+pow(object->centery-fireVector[i].centery, 2))<<endl;
            if(object->water==true){
                fireVector.erase(fireVector.begin()+i);
                object->water=false;
            }
            else{
                cout<<"Game over"<<endl;
                exit(0);
            }
        }
    }
    
}