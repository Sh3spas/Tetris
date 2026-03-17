#include <stdio.h>
#include "tetris.h"

#include <time.h>
#include <stdlib.h>

int terrain[HAUT_GRILLE][LARG_GRILLE] = {0};

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("Tetris C", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          LARG_GRILLE * TAILLE_BLOC, HAUT_GRILLE * TAILLE_BLOC, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialisation de la première pièce
    Piece actuelle = { (rand() % 7) + 1, 0, 3, 0 }; 

    Uint32 dernierTemps = SDL_GetTicks();
    Uint32 delaiChute = 500; // 500ms entre chaque chute
    bool running = true;
    SDL_Event event;

    while (running) {
        Uint32 tempsActuel = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            // On peut déjà ajouter un peu de mouvement horizontal pour tester
            if (event.type == SDL_KEYDOWN) {
                Piece test = actuelle;
                if (event.key.keysym.sym == SDLK_LEFT) test.x--;
                if (event.key.keysym.sym == SDLK_RIGHT) test.x++;
                if (event.key.keysym.sym == SDLK_UP) test.rotation = (test.rotation + 1) % 4;

                if (estPositionValide(test, terrain)) actuelle = test;
            }
        }

        // --- Logique de chute ---
        if (tempsActuel > dernierTemps + delaiChute) {
            Piece test = actuelle;
            test.y++;

            if (estPositionValide(test, terrain)) {
                actuelle = test;
            } else {
                // La pièce a touché quelque chose : on la fixe dans le terrain
                for (int i = 0; i < 4; i++) {
                    int tx = actuelle.x + PIECES[actuelle.type][actuelle.rotation][i][0];
                    int ty = actuelle.y + PIECES[actuelle.type][actuelle.rotation][i][1];
                    if (ty >= 0) terrain[ty][tx] = actuelle.type;
                }
                // On génère une nouvelle pièce
                actuelle.type = (rand() % 7) + 1;
                actuelle.rotation = 0;
                actuelle.x = 3;
                actuelle.y = 0;
                
                // Game Over rudimentaire
                if (!estPositionValide(actuelle, terrain)) {
                    // Reset du terrain pour le test
                    for(int y=0; y<HAUT_GRILLE; y++) for(int x=0; x<LARG_GRILLE; x++) terrain[y][x] = 0;
                }
            }
            dernierTemps = tempsActuel;
        }

        // --- Rendu ---
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Dessiner le terrain (blocs posés)
        for (int y = 0; y < HAUT_GRILLE; y++) {
            for (int x = 0; x < LARG_GRILLE; x++) {
                if (terrain[y][x] != VIDE) {
                    SDL_Rect r = { x * TAILLE_BLOC, y * TAILLE_BLOC, TAILLE_BLOC - 1, TAILLE_BLOC - 1 };
                    SDL_SetRenderDrawColor(renderer, COULEURS[terrain[y][x]].r, COULEURS[terrain[y][x]].g, COULEURS[terrain[y][x]].b, 255);
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }

        // Dessiner la pièce qui tombe
        SDL_SetRenderDrawColor(renderer, COULEURS[actuelle.type].r, COULEURS[actuelle.type].g, COULEURS[actuelle.type].b, 255);
        for (int i = 0; i < 4; i++) {
            SDL_Rect r = { (actuelle.x + PIECES[actuelle.type][actuelle.rotation][i][0]) * TAILLE_BLOC, 
                           (actuelle.y + PIECES[actuelle.type][actuelle.rotation][i][1]) * TAILLE_BLOC, 
                           TAILLE_BLOC - 1, TAILLE_BLOC - 1 };
            SDL_RenderFillRect(renderer, &r);
        }

        SDL_RenderPresent(renderer);
    }
}


bool estPositionValide(Piece p, int terrain[HAUT_GRILLE][LARG_GRILLE]) {
    for (int i = 0; i < 4; i++) {
        int tx = p.x + PIECES[p.type][p.rotation][i][0];
        int ty = p.y + PIECES[p.type][p.rotation][i][1];

        // Murs et Sol
        if (tx < 0 || tx >= LARG_GRILLE || ty >= HAUT_GRILLE) return false;
        
        // Blocs déjà posés (si ty < 0, la pièce est encore au-dessus de l'écran, on ignore)
        if (ty >= 0 && terrain[ty][tx] != VIDE) return false;
    }
    return true;
}