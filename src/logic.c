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

    for (int y = 0; y < HAUT_GRILLE; y++) {
        bool lignePleine = true;
        for (int x = 0; x < LARG_GRILLE; x++) {
            if (terrain[y][x] == VIDE) { lignePleine = false; break; }
        }
        if (lignePleine) {
            pleines[y] = true;
            auMoinsUne = true;
            lignesSupprimees++;
            creerParticulesLigne(y, COULEURS[terrain[y][0]]);
        }
    }

    if (auMoinsUne) {
        declencherAnimationNyanCat();
        // On ne baisse pas la force si elle est déjà plus haute (ex: Hard Drop)
        extern int forceTremblement;
        if (forceTremblement < 15) declencherTremblement(15); 

        // Animation de flash
        for (int f = 0; f < 6; f++) {
            updateTremblement(); // On recalcule le décalage à chaque étape du flash !
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            if (f % 2 == 0) {
                for (int y = 0; y < HAUT_GRILLE; y++) {
                    if (pleines[y]) {
                        SDL_Rect r = { OFFSET_X + decalageX, y * TAILLE_BLOC + decalageY, LARG_GRILLE * TAILLE_BLOC, TAILLE_BLOC };
                        SDL_RenderFillRect(renderer, &r);
                    }
                }
            }
            updateEtDessinerParticules(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(40);
        }
        
        // Suppression réelle (ton code actuel est bon ici)
        for (int y = HAUT_GRILLE - 1; y >= 0; y--) {
            if (pleines[y]) {
                for (int k = y; k > 0; k--) 
                    for (int x = 0; x < LARG_GRILLE; x++) terrain[k][x] = terrain[k - 1][x];
                for (int x = 0; x < LARG_GRILLE; x++) terrain[0][x] = VIDE;
                for (int i = y; i > 0; i--) pleines[i] = pleines[i-1];
                pleines[0] = false;
                y++; 
            }
        }
    }
    return lignesSupprimees;
}