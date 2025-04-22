#include "loadscreen.h"
#include "entity.h" // Use the player and fire classes from entity.h
#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void saveGame(player& object, vector<player>& enemy, vector<fire>& fireVector, bool lv1, bool lv2) {
    ofstream saveFile("save.txt");
    if (!saveFile.is_open()) {
        cout << "Error opening save file for writing." << endl;
        return;
    }

    // Save the current level
    if (lv1) {
        saveFile << "LEVEL 1" << endl;
    } else if (lv2) {
        saveFile << "LEVEL 2" << endl;
    }

    // Save player data
    saveFile << "PLAYER " << object.xcord << " " << object.ycord << " "
             << object.width << " " << object.height << " "
             << object.water << " " << object.centerx << " " << object.centery << endl;

    // Save enemy data
    saveFile << "ENEMIES " << enemy.size() << endl;
    for (const auto& enemyObj : enemy) {
        saveFile << enemyObj.xcord << " " << enemyObj.ycord << " "
                 << enemyObj.width << " " << enemyObj.height << " "
                 << enemyObj.centerx << " " << enemyObj.centery << endl;
    }

    // Save fire data
    saveFile << "FIRES " << fireVector.size() << endl;
    for (const auto& fireObj : fireVector) {
        saveFile << fireObj.xcord << " " << fireObj.ycord << " "
                 << fireObj.width << " " << fireObj.height << " "
                 << fireObj.centerx << " " << fireObj.centery << endl;
    }

    saveFile.close();
    cout << "Game saved successfully!" << endl;
}

void loadSave(player& object, vector<player>& enemy, vector<fire>& fireVector, bool& lv1, bool& lv2) {
    ifstream saveFile("save.txt");
    if (!saveFile.is_open()) {
        cout << "Error opening save file for reading." << endl;
        lv1 = true;  // Default to Level 1 if no save file exists
        lv2 = false;
        return;
    }

    string line;
    fireVector.clear();
    enemy.clear();

    while (getline(saveFile, line)) {
        stringstream ss(line);
        string type;
        ss >> type;

        if (type == "LEVEL") {
            string level;
            ss >> level;
            if (level == "1") {
                lv1 = true;
                lv2 = false;
            } else if (level == "2") {
                lv1 = false;
                lv2 = true;
            } else {
                lv1 = true;  // Default to Level 1 if level is invalid
                lv2 = false;
            }
        } else if (type == "PLAYER") {
            ss >> object.xcord >> object.ycord
               >> object.width >> object.height
               >> object.water
               >> object.centerx >> object.centery;
        } else if (type == "ENEMIES") {
            int enemyCount;
            ss >> enemyCount;
            for (int i = 0; i < enemyCount; ++i) {
                getline(saveFile, line);
                stringstream enemySS(line);
                player enemyObj;
                enemySS >> enemyObj.xcord >> enemyObj.ycord
                        >> enemyObj.width >> enemyObj.height
                        >> enemyObj.centerx >> enemyObj.centery;
                enemy.push_back(enemyObj);
            }
        } else if (type == "FIRES") {
            int fireCount;
            ss >> fireCount;
            for (int i = 0; i < fireCount; ++i) {
                getline(saveFile, line);
                stringstream fireSS(line);
                fire fireObj;
                fireSS >> fireObj.xcord >> fireObj.ycord
                       >> fireObj.width >> fireObj.height
                       >> fireObj.centerx >> fireObj.centery;
                fireVector.push_back(fireObj);
            }
        }
    }

    saveFile.close();
    cout << "Game loaded successfully!" << endl;
}

void loadScreen(SDL_Renderer* renderer, SDL_Texture* loadingScreen, player& object, vector<player>& enemy, vector<fire>& fireVector, bool& lv1, bool& lv2) {
    int mouse_y, mouse_x;
    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                    cout << "Mouse clicked at: " << mouse_x << ", " << mouse_y << endl;

                    // Check if the mouse is within the play button area
                    if (mouse_x >= 320 && mouse_x <= 960 && mouse_y >= 320 && mouse_y <= 440) {
                        cout << "Play button clicked!" << endl;
                        lv1 = true;  // Start at Level 1
                        lv2 = false; // Ensure Level 2 is not active
                        running = false; // Exit the loading screen loop
                    } else if (mouse_x >= 320 && mouse_x <= 960 && mouse_y >= 490 && mouse_y <= 590) {
                        cout << "Load button clicked!" << endl;
                        loadSave(object, enemy, fireVector, lv1, lv2);
                        running = false; // Exit the loading screen loop
                    }
                }
            }
        }

        // Render the loading screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, loadingScreen, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}

void clearSave() {
    ofstream saveFile("save.txt", ios::trunc); // Open the file in truncate mode to clear its contents
    if (!saveFile.is_open()) {
        cout << "Error opening save file for clearing." << endl;
        return;
    }
    saveFile.close();
    cout << "Save file cleared successfully!" << endl;
}