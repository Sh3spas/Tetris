#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>

// Dimensions de la fenêtre
#define FENETRE_LARG 600
#define FENETRE_HAUT 600

// Dimensions de la grille
#define LARG_GRILLE 10
#define HAUT_GRILLE 20
#define TAILLE_BLOC 30
#define OFFSET_X 30 // Marge à gauche pour la grille

// Ajoute ceci dans les enum
typedef enum { FACILE, NORMAL, DIFFICILE } Difficulte;

// Modifie le prototype de dessinerSettings

typedef enum { MENU, JEU, PAUSE, GAMEOVER, SETTINGS } EtatJeu;

enum { VIDE, I, O, T, S, Z, J, L };

typedef struct {
    int type;
    int rotation;
    int x, y;
} Piece;

extern const int PIECES[8][4][4][2];
extern const SDL_Color COULEURS[8];

// Logique
bool estPositionValide(Piece p, int terrain[HAUT_GRILLE][LARG_GRILLE]);
void figerPiece(Piece p, int terrain[HAUT_GRILLE][LARG_GRILLE]);
Piece genererNouvellePiece();
int nettoyerLignes(int terrain[HAUT_GRILLE][LARG_GRILLE], SDL_Renderer* renderer);

// Rendu
void initTTF();
void dessinerMenu(SDL_Renderer* renderer, int selection, int highScore);
void dessinerPause(SDL_Renderer* renderer);
void dessinerGameOver(SDL_Renderer* renderer, int score);
void dessinerSettings(SDL_Renderer* renderer, Difficulte diffActuelle);
void quitterTTF();

typedef struct {
    float x, y;
    float vx, vy;
    int vie;
    SDL_Color couleur;
} Particule;

#define MAX_PARTICULES 200

// Fonctions d'effets
void declencherTremblement(int force);
void creerParticulesLigne(int y, SDL_Color couleur);
void updateEtDessinerParticules(SDL_Renderer* renderer);

// Variables globales pour le tremblement (accessibles via extern ou gérées dans render.c)
extern int decalageX, decalageY;

void dessinerTout(SDL_Renderer* renderer, int terrain[HAUT_GRILLE][LARG_GRILLE], Piece actuelle, Piece prochaine, int score, int highScore, int level, int combo);

// On ajoute une fonction pour mettre à jour le tremblement proprement
void updateTremblement();

#endif