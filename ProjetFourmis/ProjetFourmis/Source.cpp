#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include "fonctionsBD.h"
#include "sqlite3.h"
#include "config_sdl.h"
#include "Header.h"

using namespace std;

//===============================================|
//================ Gestion Map ==================|

void initCouleursMap(Map m, char* nomMap) {
	char line[1000];
	ifstream inFile(nomMap); //ouverture du fichier nomMap
	inFile.getline(line, 1000);
	int larg, haut;
	inFile >> larg >> haut;
	inFile.getline(line, 1000);
	inFile.getline(line, 1000);//on passe les lignes qui nous sont innutiles
	for (int i = 0; i < HAUTEUR; i++) {
		for (int j = 0; j < LARGEUR; j++) {
			//On attribue des valeurs aux champs RGB des structures Case de la Map m
			int R, G, B;
			inFile >> R >> G >> B;
			m[i][j].R = R;
			m[i][j].G = G;
			m[i][j].B = B;
		}
	}
}

void initTypeCasesMap(Map m, int idMap) {
	//on ouvre la BD

	for (int i = 0; i < LARGEUR; i++) {
		for (int j = 0; j < HAUTEUR; j++) {
			m[i][j].X = j;
			m[i][j].Y = i;
			m[i][j].typeCase = 0;
			m[i][j].quantiteNourriture = 0;
		}
	}

	sqlite3*bd;
	char chemin[] = "fourmis.sqlite";
	bd = bd_ouvrir(chemin);
	char ***t = NULL;
	int L, C;
	bd_requeteSelect(bd, "select * from Nourriture", t, L, C);
	for (int i = 0; i < L; i++) {
		int X = stoi(t[i][2]);
		int Y = stoi(t[i][3]);
		if (*t[i][1] - '0' == idMap) {
			int qttNourr = *t[i][4] - '0';
			m[Y][X].typeCase = 1;
			m[Y][X].quantiteNourriture = qttNourr;
		}
	}
	bd_detruireTab(t, L, C);

	bd_requeteSelect(bd, "select * from Obstacle", t, L, C);
	for (int i = 0; i < L; i++) {
		int X = stoi(t[i][2]);
		int Y = stoi(t[i][3]);
		if (*t[i][1] - '0' == idMap) {
			m[Y][X].typeCase = 2;
		}
	}
	bd_detruireTab(t, L, C);

	bd_fermer(bd);

}

void initMap(Map m, char* nomMap, int idMap) {
	initCouleursMap(m, nomMap);
	initTypeCasesMap(m, idMap);
}


//================ Gestion Map ==================|
//===============================================|
//=================== SDL =======================|

void initialisationSDL() {


	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << " Echec à l’ouverture ";
	}

	TTF_Init();
	font = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 25);


	window = SDL_CreateWindow("Simulation Fourmis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR*TAILLECASE*2, HAUTEUR*TAILLECASE, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		cout << "erreur ouverture fenetre";
	}

	ecran = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	char chemin[20];
	strcpy_s(chemin, "Images/FourmiNO.png");
	Orientation[0] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiN.png");
	Orientation[1] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiNE.png");
	Orientation[2] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiE.png");
	Orientation[3] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiSE.png");
	Orientation[4] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiS.png");
	Orientation[5] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiSO.png");
	Orientation[6] = loadImage(ecran, chemin);
	strcpy_s(chemin, "Images/FourmiO.png");
	Orientation[7] = loadImage(ecran, chemin);


}

void destructionSDL() {

	SDL_DestroyRenderer(ecran);
	SDL_DestroyWindow(window);   //equivalent du delete

	TTF_CloseFont(font); //on ferme la font
	TTF_Quit(); //on quitte la TTF

				//fermeture de la SDL
	SDL_Quit();
}

void dessinCase(Case&c) {


	SDL_Rect fondCase;
	fondCase.h = TAILLECASE;
	fondCase.w = TAILLECASE;
	fondCase.x = c.X * TAILLECASE;
	fondCase.y = c.Y * TAILLECASE;
	int R = c.R;
	int G = c.G;
	int B = c.B;
	//DEBUG pour afficher les différents types de cases
	/*
	if (c.typeCase == 0) {
	R = G = B = 255;
	}
	else if(c.typeCase == 1){
	R = G = B = 127;
	}
	else {
	R = G = B = 0;
	}*/
	SDL_SetRenderDrawColor(ecran, R, G, B, 255);
	SDL_RenderFillRect(ecran, &fondCase);

}

void dessinMap(Map&m) {

	for (int i = 0; i < HAUTEUR; i++) {
		for (int j = 0; j < LARGEUR; j++) {
			dessinCase(m[i][j]);
		}
	}
}

void afficheDonneesCase(Case&c) {
	cout << "X: " << c.X << "; Y: " << c.Y << endl;
	cout << "typeCase" << c.typeCase << endl;

}

void afficheFourmi(Fourmi &f) {
	SDL_Rect tuile;
	tuile.h = TAILLECASE;
	tuile.w = TAILLECASE;
	tuile.x = f.X*TAILLECASE;
	tuile.y = f.Y*TAILLECASE;
	SDL_RenderCopy(ecran, Orientation[f.idOrientation], NULL, &tuile);
}

void afficheBaseGraphe() {
	SDL_Rect fond;
	fond.x = LARGEUR * TAILLECASE;
	fond.y = 0;
	fond.h = HAUTEUR*TAILLECASE;
	fond.w = LARGEUR*TAILLECASE;
	SDL_SetRenderDrawColor(ecran, 255, 255, 255, 255);
	SDL_RenderFillRect(ecran, &fond);

	int XBaseRepere = LARGEUR * TAILLECASE + 20;
	int YBaseRepere = HAUTEUR * TAILLECASE - 20;

	SDL_SetRenderDrawColor(ecran, 0, 0, 0, 255);
	SDL_RenderDrawLine(ecran, XBaseRepere, YBaseRepere, XBaseRepere + LONGEUR_AXE_GRAPHE, YBaseRepere);
	SDL_RenderDrawLine(ecran, XBaseRepere, YBaseRepere, XBaseRepere, YBaseRepere - HAUTEUR_AXE_GRAPHE);


}

void ajouterValeurGraphe(int instant, int valeur) {
	int val1X = INSTANT_TMP * LONGEUR_AXE_GRAPHE / 4000 + LARGEUR * TAILLECASE + 20;
	int val2X = instant * LONGEUR_AXE_GRAPHE / 4000 + LARGEUR * TAILLECASE + 20;
	SDL_SetRenderDrawColor(ecran, 255, 0, 0, 255);
	SDL_RenderDrawLine(ecran, val1X, HAUTEUR_AXE_GRAPHE+20-VALEUR_TMP, val2X, HAUTEUR_AXE_GRAPHE+20-valeur);
	INSTANT_TMP = instant;
	VALEUR_TMP = valeur;
}

void affichePheromones(ListePheromone lp) {
	for (int i = 0; i < HAUTEUR; i++)
	{
		for (int j = 0; j < LARGEUR; j++)
		{
			if (lp[i][j].active) {
				SDL_Rect pheromone;
				pheromone.x = j * TAILLECASE;
				pheromone.y = i * TAILLECASE;
				pheromone.h = TAILLECASE;
				pheromone.w = TAILLECASE;
				SDL_SetRenderDrawColor(ecran, 128, 128, 128, 100);
				SDL_RenderFillRect(ecran, &pheromone);
			}
		}
	}
}


//=================== SDL =======================|
//===============================================|
//============== Logique Fourmi =================|


bool caseValide(Map m, int X, int Y) {
	if (0 < X && X < LARGEUR && 0 < Y && Y < HAUTEUR && m[Y][X].typeCase != 2) {
		return true;
	}
	return false;
}

int xCaseEnFaceFourmi(Fourmi f) {
	if (f.idOrientation == 0 || f.idOrientation == 7 || f.idOrientation == 6) {
		return f.X-1;
	}
	if (f.idOrientation == 2 || f.idOrientation == 3 || f.idOrientation == 4) {
		return f.X+1;
	}
	return f.X;
}

int yCaseEnFaceFourmi(Fourmi f) {
	if (f.idOrientation == 0 || f.idOrientation == 1 || f.idOrientation == 2) {
		return f.Y-1;
	}
	if (f.idOrientation == 4 || f.idOrientation == 5 || f.idOrientation == 6) {
		return f.Y+1;
	}
	return f.Y;
}

void tournerFourmiDroite(Fourmi& f) {
	if (f.idOrientation == 7) {
		f.idOrientation = 0;
	}
	else {
		f.idOrientation++;
	}
}

void tournerFourmiGauche(Fourmi& f) {
	if (f.idOrientation == 0) {
		f.idOrientation = 7;
	}
	else {
		f.idOrientation--;
	}
}


void deplacerFourmi(Fourmi& f, ListePheromone& lp, Map m) {
	if (caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
		if (f.idOrientation == 0) {
			f.X--;
			f.Y--;
		}
		else if (f.idOrientation == 1) {
			f.Y--;
		}
		else if (f.idOrientation == 2) {
			f.X++;
			f.Y--;
		}
		else if (f.idOrientation == 3) {
			f.X++;
		}
		else if (f.idOrientation == 4) {
			f.X++;
			f.Y++;
		}
		else if (f.idOrientation == 5) {
			f.Y++;
		}
		else if (f.idOrientation == 6) {
			f.Y++;
			f.X--;
		}
		else if (f.idOrientation == 7) {
			f.X--;
		}
	}

	if (f.etat == 1 && !lp[f.Y][f.X].active) {
		lp[f.Y][f.X].active = true;
	}


}

void deplacementDistribution1(Fourmi &f) {
	/* Distribution 1 */

	//plutôt pour les recherches


	int aleaRotation = rand() % 24;

	if (aleaRotation >= 5 && aleaRotation <= 12) {
		if (rand() % 2) {
			f.idOrientation++;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation--;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}

		}
	}
	else if (13 >= aleaRotation <= 18) {
		if (rand() % 2) {
			f.idOrientation += 2;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 2;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
	else if (aleaRotation >= 19 && aleaRotation <= 22) {
		if (rand() % 2) {
			f.idOrientation += 3;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 3;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
	else if (aleaRotation == 23) {
		if (rand() % 2) {
			f.idOrientation += 4;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 4;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
}

void deplacementDistribution2(Fourmi &f){
	int aleaRotation = rand() % 19;


	if (aleaRotation >= 12 && aleaRotation <= 15) {
		if (rand() % 2) {
			f.idOrientation += 1;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 1;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
	else if (aleaRotation >= 16 && aleaRotation <= 18) {
		if (rand() % 2) {
			f.idOrientation += 2;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 2;
			if (f.idOrientation <  0) {
				f.idOrientation += 8;
			}
		}
	}
	else if (aleaRotation == 19) {
		if (rand() % 2) {
			f.idOrientation += 3;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 3;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
}

void deplacementDistribution3(Fourmi &f) {
	int aleaRotation = rand() % 19;


	if (aleaRotation >= 12 && aleaRotation <= 13) {
		if (rand() % 2) {
			f.idOrientation += 1;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 1;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
	else if (aleaRotation == 14) {
		if (rand() % 2) {
			f.idOrientation += 2;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 2;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
	else if (aleaRotation == 15) {
		if (rand() % 2) {
			f.idOrientation += 3;
			if (f.idOrientation > 7) {
				f.idOrientation -= 8;
			}
		}
		else {
			f.idOrientation -= 3;
			if (f.idOrientation < 0) {
				f.idOrientation += 8;
			}
		}
	}
}

void deplacementDistribution4(Fourmi &f, ListePheromone& lp, Map m) {
	bool pheromonePresente = false;
	for (int i = 0; i < 4; i++) {
		if (lp[yCaseEnFaceFourmi(f)][xCaseEnFaceFourmi(f)].active) {
			pheromonePresente = true;
			deplacerFourmi(f, lp, m);
			break;
		}
	}
	if (!pheromonePresente) {
		if (m[yCaseEnFaceFourmi(f)][xCaseEnFaceFourmi(f)].typeCase == 1){
			deplacementDistribution3(f);
		}
	}
	else {
	}
}

void deplacementRetourFourmilliere(Fourmi &f, Map m) {
	if (f.X < XPOSFOURMILLIERE && f.Y == YPOSFOURMILLIERE) {
		f.idOrientation = 3;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 5;
		}
	}
	else if (f.X > XPOSFOURMILLIERE && f.Y == YPOSFOURMILLIERE) {
		f.idOrientation = 7;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 0;
		}
	}
	else if (f.X == XPOSFOURMILLIERE && f.Y < YPOSFOURMILLIERE) {
		f.idOrientation = 5;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 7;
		}
	}
	else if (f.X == XPOSFOURMILLIERE && f.Y > YPOSFOURMILLIERE) {
		f.idOrientation = 1;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 2;
		}
	}
	else if (f.X < XPOSFOURMILLIERE && f.Y < YPOSFOURMILLIERE) {
		f.idOrientation = 4;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 5;
		}
	}
	else if (f.X < XPOSFOURMILLIERE && f.Y > YPOSFOURMILLIERE) {
		f.idOrientation = 2;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 1;
		}
	}
	else if (f.X > XPOSFOURMILLIERE && f.Y < YPOSFOURMILLIERE) {
		f.idOrientation = 6;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 5;
		}
	}
	else if (f.X > XPOSFOURMILLIERE && f.Y > YPOSFOURMILLIERE) {
		f.idOrientation = 0;
		if (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
			f.idOrientation = 1;
		}
	}
	while (!caseValide(m, xCaseEnFaceFourmi(f), yCaseEnFaceFourmi(f))) {
		tournerFourmiDroite(f);
	}
};


void orientationFourmi(Fourmi&f, ListePheromone& lp, Map m, int typeMouvement) {
	if (f.etat == 0) {
		if (typeMouvement == 0) {
			f.idOrientation = rand() % 8;
		}
		else if (typeMouvement == 1) {
			deplacementDistribution1(f);
		}
		else if (typeMouvement == 2) {
			deplacementDistribution2(f);
		}
		else if (typeMouvement == 3) {
			deplacementDistribution3(f);
		}
		else if (typeMouvement == 4) {
			deplacementDistribution4(f, lp, m);
		}
	}
	else
	{
		deplacementRetourFourmilliere(f, m);
	}
}


void initFourmi(Fourmi &f) {
	f.X = LARGEUR / 2;
	f.Y = HAUTEUR / 2;
	f.etat = 0;
	f.idOrientation = rand() % 8;
}

void initColonie(Colonie &c) {
	for (int i = 0; i < NBFOURMI; i++) {
		initFourmi(c[i]);
	}
}

void initListePheromones(ListePheromone &lp) {
	for (int i = 0; i < HAUTEUR; i++) {
		for (int j = 0; j < LARGEUR; j++) {
			lp[i][j].active = false;
		}
	}
}

void actionNourriture(Fourmi& f, Map m) {
	if (f.etat != 1 || f.quantiteNourriture == 0) {
		if (m[f.Y][f.X].typeCase == 1 && m[f.Y][f.X].quantiteNourriture != 0) {
			f.etat = 1;
			f.quantiteNourriture = 1;
			m[f.Y][f.X].quantiteNourriture--;
			cout << "Nourriture recoltee." << endl;
			if (m[f.Y][f.X].quantiteNourriture == 0) {
				m[f.Y][f.X].typeCase = 0;
				m[f.Y][f.X].R = 0;
				m[f.Y][f.X].G = 128;
				m[f.Y][f.X].B = 0;
				cout << "Nourriture Epuisee en (" << f.Y << ";" << f.X << ")." << endl;
			}
		}
	}
	else if (f.X == XPOSFOURMILLIERE && f.Y == YPOSFOURMILLIERE) {
		f.etat = 0;
		f.quantiteNourriture = 0;
		QUANTITE_TOTALE_NOURRITURE++;
		cout << "Nourriture Deposee." << endl;
		cout << "Quantite totale: " << QUANTITE_TOTALE_NOURRITURE << endl;
	}
}

//============== Logique Fourmi =================|
//===============================================|




int main(int argc, char *argv[]) {
	srand(time(NULL));

	initialisationSDL();

	Map map;
	char nomMap[10] = "map3.ppm";
	int idMap = 3;
	initMap(map, nomMap, idMap);

	Colonie c;
	initColonie(c);

	ListePheromone lp;
	initListePheromones(lp);

	afficheBaseGraphe();


	bool continuer = true;
	bool simuler = true;
	int instantT = 0;

	while (continuer) {

		if (simuler) {
			dessinMap(map);
			affichePheromones(lp);
			for (int i = 0; i < NBFOURMI; i++) {
				orientationFourmi(c[i], lp, map, 4);
				afficheFourmi(c[i]); 
				deplacerFourmi(c[i], lp, map);
			}
			for (int i = 0; i < NBFOURMI; i++) {
				actionNourriture(c[i], map);
			}
			SDL_Delay(30);
			ajouterValeurGraphe(instantT, QUANTITE_TOTALE_NOURRITURE);
			SDL_RenderPresent(ecran);
			instantT += 5;
			if (instantT == 4000) {
				simuler = false;
			}
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {//attente d’un évènement
			switch (event.type) //test du type d’évènement
			{
			case SDL_QUIT: //clic sur la croix de fermeture
						   //on peut enlever SDL_Delay
				continuer = false;
				break;
			case SDL_MOUSEBUTTONUP:
				int posLig = event.button.y / TAILLECASE;
				int posCol = event.button.x / TAILLECASE;
				Case c = map[posLig][posCol];
				afficheDonneesCase(c);


				break;
			}
		}
	}

	destructionSDL();

	return EXIT_SUCCESS;
}
