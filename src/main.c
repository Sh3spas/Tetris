#include <stdio.h>
#include "tetris.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("Tetris C", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          LARG_GRILLE * TAILLE_BLOC, HAUT_GRILLE * TAILLE_BLOC, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        // Fond d'écran
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, COULEURS[I].r, COULEURS[I].g, COULEURS[I].b, 255);
        for(int i=0; i<4; i++) {
            SDL_Rect r = { PIECES[I][0][i][0] * TAILLE_BLOC, 
                           PIECES[I][0][i][1] * TAILLE_BLOC, 
                           TAILLE_BLOC - 1, TAILLE_BLOC - 1 };
            SDL_RenderFillRect(renderer, &r);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}