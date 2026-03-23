#include "tetris.h"
#include <stdlib.h>

bool estPositionValide(Piece p, int terrain[HAUT_GRILLE][LARG_GRILLE]) {
    for (int i = 0; i < 4; i++) {
        int tx = p.x + PIECES[p.type][p.rotation][i][0];
        int ty = p.y + PIECES[p.type][p.rotation][i][1];
        if (tx < 0 || tx >= LARG_GRILLE || ty >= HAUT_GRILLE) return false;
        if (ty >= 0 && terrain[ty][tx] != VIDE) return false;
    }
    return true;
}

void figerPiece(Piece p, int terrain[HAUT_GRILLE][LARG_GRILLE]) {
    for (int i = 0; i < 4; i++) {
        int tx = p.x + PIECES[p.type][p.rotation][i][0];
        int ty = p.y + PIECES[p.type][p.rotation][i][1];
        if (ty >= 0) terrain[ty][tx] = p.type;
    }
}

Piece genererNouvellePiece() {
    return (Piece){ (rand() % 7) + 1, 0, 3, 0 };
}

int nettoyerLignes(int terrain[HAUT_GRILLE][LARG_GRILLE], SDL_Renderer* renderer) {
    int lignesSupprimees = 0;
    bool pleines[HAUT_GRILLE] = {false};
    bool auMoinsUne = false;

    // 1. Détection
    for (int y = 0; y < HAUT_GRILLE; y++) {
        bool lignePleine = true;
        for (int x = 0; x < LARG_GRILLE; x++) {
            if (terrain[y][x] == VIDE) { lignePleine = false; break; }
        }
        if (lignePleine) {
            pleines[y] = true;
            auMoinsUne = true;
            lignesSupprimees++;
        }
    }

    // 2. Flash collectif
    if (auMoinsUne) {
        for (int f = 0; f < 3; f++) { // Clignote 3 fois
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int y = 0; y < HAUT_GRILLE; y++) {
                if (pleines[y]) {
                    SDL_Rect r = { 0, y * TAILLE_BLOC, LARG_GRILLE * TAILLE_BLOC, TAILLE_BLOC };
                    SDL_RenderFillRect(renderer, &r);
                }
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(50);
            
            // On redessine le fond noir entre les flashs pour l'effet clignotant
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderPresent(renderer);
            SDL_Delay(30);
        }
    }

    // 3. Suppression réelle
    for (int y = HAUT_GRILLE - 1; y >= 0; y--) {
        if (pleines[y]) {
            for (int k = y; k > 0; k--) {
                for (int x = 0; x < LARG_GRILLE; x++) terrain[k][x] = terrain[k - 1][x];
            }
            for (int x = 0; x < LARG_GRILLE; x++) terrain[0][x] = VIDE;
            
            // Une ligne a été supprimée, on doit décaler notre tableau de "pleines"
            // ou simplement recommencer la vérification sur cet index.
            for (int i = y; i > 0; i--) pleines[i] = pleines[i-1];
            pleines[0] = false;
            y++; 
        }
    }
    return lignesSupprimees;
}