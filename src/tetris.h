#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define LARG_GRILLE 10
#define HAUT_GRILLE 20
#define TAILLE_BLOC 30

enum { VIDE, I, O, T, S, Z, J, L };


extern const int PIECES[8][4][4][2];
extern const SDL_Color COULEURS[8];

typedef struct {
    int type;
    int rotation;
    int x, y; // Position sur la grille
} Piece;

#endif