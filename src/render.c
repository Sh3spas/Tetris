#include "tetris.h"
#include <stdio.h>

// Variables globales pour les effets
TTF_Font *font = NULL;
int decalageX = 0, decalageY = 0;
int forceTremblement = 0;
int frameLevelUp = 0;
Particule particules[MAX_PARTICULES];

const SDL_Color COULEURS_FOND[] = {
    {15, 15, 35, 255}, // Bleu nuit
    {40, 15, 15, 255}, // Rouge
    {15, 40, 15, 255}, // Vert
    {45, 30, 10, 255}, // Orange
    {35, 15, 45, 255}, // Violet
    {15, 45, 45, 255}, // Cyan
    {30, 30, 30, 255}  // Gris
};

void initTTF()
{
    TTF_Init();
    font = TTF_OpenFont("arcade.ttf", 26); // Taille légèrement réduite pour mieux fiter
    if (!font)
        printf("Erreur : arcade.ttf introuvable !\n");
}

void dessinerTexte(SDL_Renderer *renderer, const char *texte, int x, int y, SDL_Color couleur, bool centre)
{
    if (!font)
        return;
    SDL_Surface *surf = TTF_RenderText_Blended(font, texte, couleur);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    // Si centre est vrai, on centre par rapport à la fenêtre
    // Sinon, on utilise le x précis (très important pour le panneau latéral)
    int finalX = centre ? (FENETRE_LARG - surf->w) / 2 : x;

    SDL_Rect r = {finalX, y, surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, NULL, &r);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void updateTremblement()
{
    if (forceTremblement > 0)
    {
        decalageX = (rand() % forceTremblement) - forceTremblement / 2;
        decalageY = (rand() % forceTremblement) - forceTremblement / 2;
        forceTremblement--;
    }
    else
    {
        decalageX = 0;
        decalageY = 0;
    }
}

void declencherAnimLevelUp() { frameLevelUp = 100; }
void declencherTremblement(int force) { forceTremblement = force; }

void dessinerBlocStyle(SDL_Renderer *renderer, int x, int y, SDL_Color col, int alpha)
{
    SDL_Rect r = {x, y, TAILLE_BLOC - 1, TAILLE_BLOC - 1};
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, alpha);
    SDL_RenderFillRect(renderer, &r);
    // Petit relief
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha / 3);
    SDL_RenderDrawLine(renderer, r.x, r.y, r.x + r.w, r.y);
    SDL_RenderDrawLine(renderer, r.x, r.y, r.x, r.y + r.h);
}

void updateEtDessinerParticules(SDL_Renderer *renderer)
{
    for (int i = 0; i < MAX_PARTICULES; i++)
    {
        if (particules[i].vie > 0)
        {
            particules[i].x += particules[i].vx;
            particules[i].y += particules[i].vy;
            particules[i].vie--;
            SDL_SetRenderDrawColor(renderer, particules[i].couleur.r, particules[i].couleur.g, particules[i].couleur.b, (particules[i].vie * 4));
            SDL_Rect r = {(int)particules[i].x + decalageX, (int)particules[i].y + decalageY, 3, 3};
            SDL_RenderFillRect(renderer, &r);
        }
    }
}

void dessinerTout(SDL_Renderer *renderer, int terrain[HAUT_GRILLE][LARG_GRILLE], Piece actuelle, Piece prochaine, int score, int highScore, int level, int combo)
{
    updateTremblement();

    // CRUCIAL : On force le mode de mélange ici pour éviter le bug après pause
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Color fond = COULEURS_FOND[(level - 1) % 7];
    SDL_SetRenderDrawColor(renderer, fond.r, fond.g, fond.b, 255);
    SDL_RenderClear(renderer);

    // --- ZONE DE JEU (OFFSET_X = 30) ---
    // On dessine un fond légèrement plus sombre pour le terrain
    SDL_Rect aireJeu = {OFFSET_X + decalageX, decalageY, LARG_GRILLE * TAILLE_BLOC, HAUT_GRILLE * TAILLE_BLOC};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &aireJeu);

    // Grille
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 20);
    for (int x = 0; x <= LARG_GRILLE; x++)
        SDL_RenderDrawLine(renderer, x * TAILLE_BLOC + OFFSET_X + decalageX, 0, x * TAILLE_BLOC + OFFSET_X + decalageX, HAUT_GRILLE * TAILLE_BLOC);
    for (int y = 0; y <= HAUT_GRILLE; y++)
        SDL_RenderDrawLine(renderer, OFFSET_X + decalageX, y * TAILLE_BLOC, LARG_GRILLE * TAILLE_BLOC + OFFSET_X + decalageX, y * TAILLE_BLOC);

    // Terrain
    for (int y = 0; y < HAUT_GRILLE; y++)
        for (int x = 0; x < LARG_GRILLE; x++)
            if (terrain[y][x] != VIDE)
                dessinerBlocStyle(renderer, x * TAILLE_BLOC + OFFSET_X + decalageX, y * TAILLE_BLOC + decalageY, COULEURS[terrain[y][x]], 255);

    // Ghost Piece
    Piece ghost = actuelle;
    while (estPositionValide(ghost, terrain))
        ghost.y++;
    ghost.y--;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
    for (int i = 0; i < 4; i++)
    {
        SDL_Rect r = {(ghost.x + PIECES[ghost.type][ghost.rotation][i][0]) * TAILLE_BLOC + OFFSET_X + decalageX + 2,
                      (ghost.y + PIECES[ghost.type][ghost.rotation][i][1]) * TAILLE_BLOC + decalageY + 2,
                      TAILLE_BLOC - 5, TAILLE_BLOC - 5};
        SDL_RenderDrawRect(renderer, &r);
    }

    // Pièce actuelle
    for (int i = 0; i < 4; i++)
        dessinerBlocStyle(renderer, (actuelle.x + PIECES[actuelle.type][actuelle.rotation][i][0]) * TAILLE_BLOC + OFFSET_X + decalageX,
                          (actuelle.y + PIECES[actuelle.type][actuelle.rotation][i][1]) * TAILLE_BLOC + decalageY,
                          COULEURS[actuelle.type], 255);

    updateEtDessinerParticules(renderer);

    // --- PANNEAU LATÉRAL (X = 350) ---
    SDL_Rect panelRect = {350, 20, 230, 560};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, &panelRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
    SDL_RenderDrawRect(renderer, &panelRect);

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color or_col = {255, 215, 0, 255};
    SDL_Color rouge = {255, 100, 0, 255};
    char b[32];

    // Note : On utilise 'false' pour dessinerTexte ici pour utiliser nos coordonnées X
    dessinerTexte(renderer, "SCORE", 370, 50, blanc, false);
    sprintf(b, "%06d", score);
    dessinerTexte(renderer, b, 370, 80, or_col, false);

    dessinerTexte(renderer, "LEVEL", 370, 140, blanc, false);
    sprintf(b, "%d", level);
    dessinerTexte(renderer, b, 370, 170, or_col, false);

    if (combo > 1)
    {
        sprintf(b, "COMBO X%d", combo);
        dessinerTexte(renderer, b, 370, 220, rouge, false);
    }

    // NEXT PIECE
    dessinerTexte(renderer, "NEXT", 370, 300, blanc, false);
    SDL_Rect nextBox = {390, 340, 130, 130};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 30);
    SDL_RenderDrawRect(renderer, &nextBox);

    for (int i = 0; i < 4; i++)
    {
        // Centrage de la pièce NEXT dans sa boîte
        dessinerBlocStyle(renderer, 410 + PIECES[prochaine.type][0][i][0] * TAILLE_BLOC,
                          360 + PIECES[prochaine.type][0][i][1] * TAILLE_BLOC,
                          COULEURS[prochaine.type], 255);
    }

    // Animation Level Up
    if (frameLevelUp > 0)
    {
        if ((frameLevelUp / 10) % 2 == 0)
            dessinerTexte(renderer, "LEVEL UP !", 0, 250, rouge, true);
        frameLevelUp--;
    }

    SDL_RenderPresent(renderer);
}

void dessinerMenu(SDL_Renderer *renderer, int selection, int highScore)
{
    SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
    SDL_RenderClear(renderer);
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color jaune = {255, 255, 0, 255};

    dessinerTexte(renderer, "TETRIS C-PRO", 0, 100, blanc, true);
    dessinerTexte(renderer, (selection == 0) ? "> START <" : "START", 0, 250, (selection == 0) ? jaune : blanc, true);
    dessinerTexte(renderer, (selection == 1) ? "> SETTINGS <" : "SETTINGS", 0, 320, (selection == 1) ? jaune : blanc, true);
    dessinerTexte(renderer, (selection == 2) ? "> QUIT <" : "QUIT", 0, 390, (selection == 2) ? jaune : blanc, true);

    char b[32];
    sprintf(b, "HIGH SCORE: %06d", highScore);
    dessinerTexte(renderer, b, 0, 500, blanc, true);
    SDL_RenderPresent(renderer);
}

void dessinerPause(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect r = {0, 0, FENETRE_LARG, FENETRE_HAUT};
    SDL_RenderFillRect(renderer, &r);
    dessinerTexte(renderer, "PAUSED", 0, 250, (SDL_Color){255, 255, 255, 255}, true);
    SDL_RenderPresent(renderer);
}

void dessinerGameOver(SDL_Renderer *renderer, int score)
{
    SDL_SetRenderDrawColor(renderer, 60, 10, 10, 255);
    SDL_RenderClear(renderer);
    char b[32];
    sprintf(b, "FINAL SCORE: %d", score);
    dessinerTexte(renderer, "GAME OVER", 0, 200, (SDL_Color){255, 0, 0, 255}, true);
    dessinerTexte(renderer, b, 0, 300, (SDL_Color){255, 255, 255, 255}, true);
    dessinerTexte(renderer, "PRESS ENTER", 0, 450, (SDL_Color){200, 200, 200, 255}, true);
    SDL_RenderPresent(renderer);
}

void dessinerSettings(SDL_Renderer *renderer, Difficulte diffActuelle)
{
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

void creerParticulesLigne(int y, SDL_Color couleur)
{

    for (int i = 0; i < 40; i++)
    {

        for (int p = 0; p < MAX_PARTICULES; p++)
        {

            if (particules[p].vie <= 0)
            {

                particules[p].x = (rand() % (LARG_GRILLE * TAILLE_BLOC)) + OFFSET_X;

                particules[p].y = y * TAILLE_BLOC;

                particules[p].vx = (rand() % 10 - 5) / 1.5f;

                particules[p].vy = (rand() % 10 - 5) / 1.5f;

                particules[p].vie = 40 + (rand() % 20);

                particules[p].couleur = couleur;

                break;
            }
        }
    }
}

void quitterTTF()
{
    if (font)
        TTF_CloseFont(font);
    TTF_Quit();
}