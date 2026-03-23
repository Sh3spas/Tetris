#include "tetris.h"
#include <stdio.h>

TTF_Font* font = NULL;

void initTTF() {
    TTF_Init();
    font = TTF_OpenFont("arcade.ttf", 28); 
    if (!font) printf("Erreur : arcade.ttf introuvable !\n");
}

void dessinerTexte(SDL_Renderer* renderer, const char* texte, int x, int y, SDL_Color couleur, bool centre) {
    if (!font) return;
    SDL_Surface* surf = TTF_RenderText_Blended(font, texte, couleur);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    int finalX = centre ? (FENETRE_LARG - surf->w) / 2 : x;
    SDL_Rect r = { finalX, y, surf->w, surf->h };
    SDL_RenderCopy(renderer, tex, NULL, &r);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void dessinerTout(SDL_Renderer* renderer, int terrain[HAUT_GRILLE][LARG_GRILLE], Piece actuelle, Piece prochaine, int score, int highScore) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 10, 10, 25, 255);
    SDL_RenderClear(renderer);

    // Grille de fond   
    SDL_SetRenderDrawColor(renderer, 0,90, 90, 255);
    for (int x = 0; x <= LARG_GRILLE; x++)
        SDL_RenderDrawLine(renderer, x * TAILLE_BLOC + OFFSET_X, 0, x * TAILLE_BLOC + OFFSET_X, HAUT_GRILLE * TAILLE_BLOC);
    for (int y = 0; y <= HAUT_GRILLE; y++)
        SDL_RenderDrawLine(renderer, OFFSET_X, y * TAILLE_BLOC, LARG_GRILLE * TAILLE_BLOC + OFFSET_X, y * TAILLE_BLOC);

    // Terrain
    for (int y = 0; y < HAUT_GRILLE; y++) {
        for (int x = 0; x < LARG_GRILLE; x++) {
            if (terrain[y][x] != VIDE) {
                SDL_Rect r = { x * TAILLE_BLOC + OFFSET_X, y * TAILLE_BLOC, TAILLE_BLOC - 1, TAILLE_BLOC - 1 };
                SDL_SetRenderDrawColor(renderer, COULEURS[terrain[y][x]].r, COULEURS[terrain[y][x]].g, COULEURS[terrain[y][x]].b, 255);
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    // Ghost Piece
    Piece ghost = actuelle;
    while (estPositionValide(ghost, terrain)) ghost.y++;
    ghost.y--;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    for (int i = 0; i < 4; i++) {
        SDL_Rect r = { (ghost.x + PIECES[ghost.type][ghost.rotation][i][0]) * TAILLE_BLOC + OFFSET_X + 2, 
                       (ghost.y + PIECES[ghost.type][ghost.rotation][i][1]) * TAILLE_BLOC + 2, 
                       TAILLE_BLOC - 5, TAILLE_BLOC - 5 };
        SDL_RenderDrawRect(renderer, &r);
    }

    // Pièce actuelle
    SDL_SetRenderDrawColor(renderer, COULEURS[actuelle.type].r, COULEURS[actuelle.type].g, COULEURS[actuelle.type].b, 255);
    for (int i = 0; i < 4; i++) {
        SDL_Rect r = { (actuelle.x + PIECES[actuelle.type][actuelle.rotation][i][0]) * TAILLE_BLOC + OFFSET_X, 
                       (actuelle.y + PIECES[actuelle.type][actuelle.rotation][i][1]) * TAILLE_BLOC, 
                       TAILLE_BLOC - 1, TAILLE_BLOC - 1 };
        SDL_RenderFillRect(renderer, &r);
    }

    // Panneau latéral
    SDL_Color blanc = {255, 255, 255, 255};
    char b[32];
    sprintf(b, "SCORE: %06d", score);
    dessinerTexte(renderer, b, 335, 50, blanc, false);
    sprintf(b, "BEST:  %06d", highScore);
    dessinerTexte(renderer, b, 335, 100, blanc, false);

    // --- AFFICHAGE PROCHAINE PIÈCE ---
    dessinerTexte(renderer, "NEXT:", 350, 200, blanc, false);
    
    // On dessine un petit cadre pour la pièce suivante
    SDL_SetRenderDrawColor(renderer, 40, 40, 80, 255);
    SDL_Rect cadreNext = { 350, 240, 120, 120 };
    SDL_RenderDrawRect(renderer, &cadreNext);

    // Dessin de la pièce prochaine
    SDL_SetRenderDrawColor(renderer, COULEURS[prochaine.type].r, COULEURS[prochaine.type].g, COULEURS[prochaine.type].b, 255);
    for (int i = 0; i < 4; i++) {
        // On calcule la position pour qu'elle soit centrée dans le cadre à droite
        // On utilise la rotation 0 pour la preview
        SDL_Rect r = { 
            360 + PIECES[prochaine.type][0][i][0] * TAILLE_BLOC, 
            260 + PIECES[prochaine.type][0][i][1] * TAILLE_BLOC, 
            TAILLE_BLOC - 1, TAILLE_BLOC - 1 
        };
        SDL_RenderFillRect(renderer, &r);
    }

    SDL_RenderPresent(renderer);
}

void dessinerMenu(SDL_Renderer* renderer, int selection, int highScore) {
    SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
    SDL_RenderClear(renderer);
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color jaune = {255, 255, 0, 255};

    dessinerTexte(renderer, "TETRIS C-PRO", 0, 100, blanc, true);
    dessinerTexte(renderer, (selection == 0) ? "> START <" : "START", 0, 250, (selection == 0) ? jaune : blanc, true);
    dessinerTexte(renderer, (selection == 1) ? "> SETTINGS <" : "SETTINGS", 0, 320, (selection == 1) ? jaune : blanc, true);
    dessinerTexte(renderer, (selection == 2) ? "> QUIT <" : "QUIT", 0, 390, (selection == 2) ? jaune : blanc, true);

    char b[32]; sprintf(b, "HIGH SCORE: %06d", highScore);
    dessinerTexte(renderer, b, 0, 500, blanc, true);
    SDL_RenderPresent(renderer);
}

void dessinerPause(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect r = {0,0, FENETRE_LARG, FENETRE_HAUT};
    SDL_RenderFillRect(renderer, &r);
    dessinerTexte(renderer, "PAUSED", 0, 250, (SDL_Color){255,255,255,255}, true);
    SDL_RenderPresent(renderer);
}

void dessinerGameOver(SDL_Renderer* renderer, int score) {
    SDL_SetRenderDrawColor(renderer, 60, 10, 10, 255);
    SDL_RenderClear(renderer);
    char b[32]; sprintf(b, "FINAL SCORE: %d", score);
    dessinerTexte(renderer, "GAME OVER", 0, 200, (SDL_Color){255,0,0,255}, true);
    dessinerTexte(renderer, b, 0, 300, (SDL_Color){255,255,255,255}, true);
    dessinerTexte(renderer, "PRESS ENTER", 0, 450, (SDL_Color){200,200,200,255}, true);
    SDL_RenderPresent(renderer);
}

void dessinerSettings(SDL_Renderer* renderer, Difficulte diffActuelle) {
    SDL_SetRenderDrawColor(renderer, 20, 40, 20, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color jaune = {255, 255, 0, 255};

    dessinerTexte(renderer, "SETTINGS", 0, 100, blanc, true);
    dessinerTexte(renderer, "DIFFICULTY:", 0, 200, blanc, true);

    // Affichage des modes
    dessinerTexte(renderer, (diffActuelle == FACILE) ? "> EASY <" : "EASY", 0, 280, (diffActuelle == FACILE) ? jaune : blanc, true);
    dessinerTexte(renderer, (diffActuelle == NORMAL) ? "> NORMAL <" : "NORMAL", 0, 340, (diffActuelle == NORMAL) ? jaune : blanc, true);
    dessinerTexte(renderer, (diffActuelle == DIFFICILE) ? "> HARD <" : "HARD", 0, 400, (diffActuelle == DIFFICILE) ? jaune : blanc, true);

    dessinerTexte(renderer, "USE UP/DOWN TO CHANGE", 0, 500, (SDL_Color){150, 150, 150, 255}, true);
    dessinerTexte(renderer, "PRESS ESC TO BACK", 0, 550, blanc, true);
    
    SDL_RenderPresent(renderer);
}

void quitterTTF() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}