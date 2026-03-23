#include "tetris.h"
#include <time.h>

int main(int argc, char* argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    initTTF();

    SDL_Window* window = SDL_CreateWindow("TETRIS PRO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          FENETRE_LARG, FENETRE_HAUT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // --- VARIABLES INITIALES ---
    int terrain[HAUT_GRILLE][LARG_GRILLE] = {0};
    Piece actuelle, prochaine; // Ajout de "prochaine"
    EtatJeu etat = MENU;
    Difficulte diff = NORMAL; // Difficulté par défaut
    
    int score = 0, highScore = 0, selection = 0;
    Uint32 dernierTemps = SDL_GetTicks();
    Uint32 delai = 500;
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            if (event.type == SDL_KEYDOWN) {
                // 1. GESTION DU MENU PRINCIPAL
                if (etat == MENU) {
                    switch(event.key.keysym.sym) {
                        case SDLK_UP: selection = (selection - 1 + 3) % 3; break;
                        case SDLK_DOWN: selection = (selection + 1) % 3; break;
                        case SDLK_RETURN:
                            if (selection == 0) { // START
                                memset(terrain, 0, sizeof(terrain));
                                score = 0;
                                // Appliquer le délai selon la difficulté choisie
                                if (diff == FACILE) delai = 800;
                                else if (diff == NORMAL) delai = 500;
                                else delai = 250; // DIFFICILE
                                
                                actuelle = genererNouvellePiece();
                                prochaine = genererNouvellePiece();
                                etat = JEU;
                            } 
                            else if (selection == 1) etat = SETTINGS;
                            else if (selection == 2) running = false;
                            break;
                    }
                }
                // 2. GESTION DES RÉGLAGES (Difficulté)
                else if (etat == SETTINGS) {
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE: etat = MENU; break;
                        case SDLK_UP:     diff = (diff - 1 + 3) % 3; break;
                        case SDLK_DOWN:   diff = (diff + 1) % 3; break;
                    }
                }
                // 3. GESTION DE LA PAUSE / GAMEOVER
                else if (etat == PAUSE && event.key.keysym.sym == SDLK_p) etat = JEU;
                else if (etat == GAMEOVER && event.key.keysym.sym == SDLK_RETURN) etat = MENU;
                
                // 4. GESTION DU JEU EN COURS
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
                                figerPiece(actuelle, terrain);
                                score += nettoyerLignes(terrain, renderer) * 100;
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

        // --- LOGIQUE DE CHUTE AUTOMATIQUE ---
        if (etat == JEU && SDL_GetTicks() > dernierTemps + delai) {
            Piece test = actuelle;
            test.y++;
            if (estPositionValide(test, terrain)) {
                actuelle = test;
            } else {
                figerPiece(actuelle, terrain);
                int n = nettoyerLignes(terrain, renderer);
                score += n * 100;
                
                // Augmentation de la vitesse progressive
                if (n > 0) {
                    int reduction = (diff == FACILE) ? 10 : (diff == NORMAL) ? 20 : 35;
                    int seuilMin = (diff == DIFFICILE) ? 60 : 100;
                    if (delai > seuilMin) delai -= reduction;
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

        // --- RENDU GRAPHIQUE ---
        switch (etat) {
            case MENU:     dessinerMenu(renderer, selection, highScore); break;
            case SETTINGS: dessinerSettings(renderer, diff); break; // Ajout du paramètre diff
            case JEU: dessinerTout(renderer, terrain, actuelle, prochaine, score, highScore); break;
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