#include<iostream>
#include<cmath>
#include<ctime>
#include<sstream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

using namespace std;

SDL_Window*windowMenu = NULL;
SDL_Window*windowParam = NULL;
SDL_Window*windowSim = NULL;
SDL_Window*windowEdit = NULL;
SDL_Renderer*ecranMenu = NULL;
SDL_Renderer*ecranParam = NULL;
SDL_Renderer*ecranSim = NULL;
SDL_Renderer*ecranEdit = NULL;
TTF_Font*fontLarge = NULL;
TTF_Font*fontSmall = NULL;

const int HAUTEUR = 70;
const int LARGEUR = 70;
const int TAILLECASE = 8;
const int XPOSFOURMILLIERE = LARGEUR / 2;
const int YPOSFOURMILLIERE = HAUTEUR / 2;
const int LONGEUR_AXE_GRAPHE = LARGEUR * TAILLECASE - 40;
const int HAUTEUR_AXE_GRAPHE = HAUTEUR * TAILLECASE - 40;
int QUANTITE_TOTALE_NOURRITURE_MAP = 0;

char listeMaps[4][10] = { "map0.ppm","map1.ppm","map2.ppm","map3.ppm" };

char nomMap[10];
int idMap = -1;
const int NBFOURMI_ECLAIREUSE = 100;
const int NBFOURMI_TRANSPORTEUSE = 100;
int typeMouvement = -1;

int QUANTITE_TOTALE_NOURRITURE = 0;


SDL_Texture* Orientation[8];

typedef struct Case {
	int X;
	int Y;
	int typeCase; //0 = normale; 1 = nourriture; 2 = obstacle;
	int quantiteNourriture; // 0 si typeCase != 1
	int R;
	int G;
	int B;
} Case;

typedef Case Map[HAUTEUR][LARGEUR];

typedef struct Fourmi {
	int X;
	int Y;
	int etat; // 0 = Normal, 1 = Retour fourmillière
	int idOrientation;// NO=0, N=1, NE=2, E=3, SE=4, S=5, SO=6, O=7
	int quantiteNourriture;
	int type; //0=eclaireurs, 1=transporteurs
} Fourmi;

typedef Fourmi Colonie[NBFOURMI_ECLAIREUSE +NBFOURMI_TRANSPORTEUSE];

typedef struct Pheromone {
	bool active;
} Pheromone;

typedef Pheromone ListePheromone[HAUTEUR][LARGEUR];

// ==========/\/\/\/\/========== GRAPHE

int INSTANT_TMP = 0;
int VALEUR_TMP = 0;


// ============== EDM ============

const int nbCase_EDM = 120;
const int TAILLE_CASE_EDM = 5;

typedef struct Case_EDM {
	int x;
	int y;
	int typeCase; //0 = normale; 1 = nourriture; 2 = obstacle;
	int quantiteNourriture; // 0 si typeCase != 1
	int R;
	int G;
	int B;
} Case_EDM;

typedef Case_EDM Map_EDM[nbCase_EDM][nbCase_EDM];
