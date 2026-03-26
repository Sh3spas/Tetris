#include "tetris.h"
#include <time.h>

int main(int argc, char* argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    initTTF();

    SDL_Window* window = SDL_CreateWindow("TETRIS C-PRO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          FENETRE_LARG, FENETRE_HAUT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // --- VARIABLES DE JEU ---
    int terrain[HAUT_GRILLE][LARG_GRILLE] = {0};
    Piece actuelle, prochaine;
    EtatJeu etat = MENU;
    Difficulte diff = NORMAL; 
    
    int score = 0, highScore = 0, selection = 0;
    int level = 1, combo = 0, lignesTotales = 0;
    Uint32 dernierTemps = SDL_GetTicks();
    Uint32 delai = 500;
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            if (event.type == SDL_KEYDOWN) {
                // 1. MENU
                if (etat == MENU) {
                    switch(event.key.keysym.sym) {
                        case SDLK_UP: selection = (selection - 1 + 3) % 3; break;
                        case SDLK_DOWN: selection = (selection + 1) % 3; break;
                        case SDLK_RETURN:
                            if (selection == 0) { 
                                memset(terrain, 0, sizeof(terrain));
                                score = 0; level = 1; combo = 0; lignesTotales = 0;
                                if (diff == FACILE) delai = 800;
                                else if (diff == NORMAL) delai = 500;
                                else delai = 250; 
                                
                                actuelle = genererNouvellePiece();
                                prochaine = genererNouvellePiece();
                                etat = JEU;
                            } 
                            else if (selection == 1) etat = SETTINGS;
                            else if (selection == 2) running = false;
                            break;
                    }
                }
                // 2. RÉGLAGES
                else if (etat == SETTINGS) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) etat = MENU;
                    else if (event.key.keysym.sym == SDLK_UP) diff = (diff - 1 + 3) % 3;
                    else if (event.key.keysym.sym == SDLK_DOWN) diff = (diff + 1) % 3;
                }
                // 3. PAUSE / GAMEOVER
                else if (etat == PAUSE && event.key.keysym.sym == SDLK_p) etat = JEU;
                else if (etat == GAMEOVER && event.key.keysym.sym == SDLK_RETURN) etat = MENU;
                
                // 4. JEU EN COURS
                else if (etat == JEU) {
                    if (event.key.keysym.sym == SDLK_p) etat = PAUSE;
                    else {
                        Piece test = actuelle;
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT:  test.x--; break;
                            case SDLK_RIGHT: test.x++; break;
                            case SDLK_UP:    test.rotation = (test.rotation + 1) % 4; break;
                            case SDLK_DOWN:  test.y++; break;
                            case SDLK_SPACE: // HARD DROP
                                while (estPositionValide(actuelle, terrain)) actuelle.y++;
                                actuelle.y--;
                                declencherTremblement(25); // Secousse maximum
                                figerPiece(actuelle, terrain);
                                
                                // --- LOGIQUE DE SCORING ---
                                int n = nettoyerLignes(terrain, renderer);
                                if (n > 0) {
                                    int base = (n == 1) ? 100 : (n == 2) ? 300 : (n == 3) ? 500 : 1200;
                                    score += (base * level) + (combo * 50 * level);
                                    combo++;
                                    lignesTotales += n;
                                    if (lignesTotales >= level * 10) {
                                        level++;
                                        if (delai > 60) delai -= (diff == DIFFICILE) ? 40 : 25;
                                    }
                                } else {
                                    combo = 0;
                                }

                                actuelle = prochaine;
                                prochaine = genererNouvellePiece();
                                if (!estPositionValide(actuelle, terrain)) {
                                    if (score > highScore) highScore = score;
                                    etat = GAMEOVER;
                                }
                                dernierTemps = SDL_GetTicks();
                                goto fin_evenement;
                        }
                        if (estPositionValide(test, terrain)) actuelle = test;
                    }
                }
            }
            fin_evenement: ;
        }

        // --- CHUTE AUTOMATIQUE ---
        if (etat == JEU && SDL_GetTicks() > dernierTemps + delai) {
            Piece test = actuelle;
            test.y++;
            if (estPositionValide(test, terrain)) {
                actuelle = test;
            } else {
                figerPiece(actuelle, terrain);
                int n = nettoyerLignes(terrain, renderer);
                
                if (n > 0) {
                    int base = (n == 1) ? 100 : (n == 2) ? 300 : (n == 3) ? 500 : 1200;
                    score += (base * level) + (combo * 50 * level);
                    combo++;
                    lignesTotales += n;
                    if (lignesTotales >= level * 10) {
                        level++;
                        if (delai > 60) delai -= (diff == DIFFICILE) ? 40 : 25;
                    }
                } else {
                    combo = 0;
                }

                actuelle = prochaine;
                prochaine = genererNouvellePiece();
                if (!estPositionValide(actuelle, terrain)) {
                    if (score > highScore) highScore = score;
                    etat = GAMEOVER;
                }
            }
            dernierTemps = SDL_GetTicks();
        }

        // --- DESSIN ---
        switch (etat) {
            case MENU:     dessinerMenu(renderer, selection, highScore); break;
            case SETTINGS: dessinerSettings(renderer, diff); break;
            case JEU:      dessinerTout(renderer, terrain, actuelle, prochaine, score, highScore, level, combo); break;
            case PAUSE:    dessinerPause(renderer); break;
            case GAMEOVER: dessinerGameOver(renderer, score); break;
        }
    }

    quitterTTF();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}