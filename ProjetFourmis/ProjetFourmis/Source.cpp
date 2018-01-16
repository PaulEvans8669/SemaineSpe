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


	window = SDL_CreateWindow("Simulation Fourmis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR*TAILLECASE, HAUTEUR*TAILLECASE, SDL_WINDOW_SHOWN);
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
	//DEBUG
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
//dessine la map, i.e. l'ensemble des cases
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

//=================== SDL =======================|
//===============================================|
//============== Logique Fourmi =================|

void orientationAlea(Fourmi&f) {
	f.idOrientation = rand() % 8;
}

bool caseValide(Map m,int X, int Y) {
	if (0 < X && X < LARGEUR && 0 < Y && Y < HAUTEUR && m[Y][X].typeCase != 2) {
		return true;
	}
	return false;
}

void deplacerFourmi(Fourmi& f, Map m) {
	if (true) { //si état "normal" faire des mouvements aléatoires
		if (f.idOrientation == 0 && caseValide(m,f.X-1,f.Y-1)) {
		f.X --;
		f.Y --;
	}
		else if (f.idOrientation == 1 && caseValide(m, f.X - 1, f.Y - 1)) {
		f.Y --;
	}
		else if (f.idOrientation == 2 && caseValide(m, f.X + 1, f.Y - 1)) {
		f.X ++;
		f.Y --;
	}
		else if (f.idOrientation == 3 && caseValide(m, f.X + 1, f.Y)) {
		f.X ++;
	}
		else if (f.idOrientation == 4 && caseValide(m, f.X + 1, f.Y + 1)) {
		f.X ++;
		f.Y ++;
	}
		else if (f.idOrientation == 5 && caseValide(m, f.X, f.Y + 1)) {
		f.Y ++;
	}
		else if (f.idOrientation == 6 && caseValide(m, f.X - 1, f.Y + 1)) {
		f.Y ++;
		f.X --;
	}
		else if (f.idOrientation == 7 && caseValide(m, f.X - 1, f.Y)) {
			f.X --;
		}
	}
	else { //sinon, la fourmi est en mode retour à la fourmillière

	}
}

void initFourmi(Fourmi &f) {
	f.X = LARGEUR / 2;
	f.Y = HAUTEUR / 2;
	f.etat = 0;
}

void initColonie(Colonie &c) {
	for (int i = 0; i < NBFOURMI; i++) {
	initFourmi(c[i]);
	}
}

void actionNourriture(Fourmi& f,Map m) {
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
	else if (f.X == XPOSFOURMILLIERE && f.Y == YPOSFOURMILLIERE){
		f.etat = 0;
		f.quantiteNourriture = 0;
		QUANTITE_TOTALE_NOURRITURE++;
		cout << "Nourriture Deposee." << endl;
		cout << "Quantite totale: " << QUANTITE_TOTALE_NOURRITURE << endl;
	}
}

void Retour_Fourmilière(Fourmi f) {		
	for (int i = 0; i < LARGEUR; i++) {
		for (int j = 0; j < HAUTEUR; j++){		
			while (f.X!=XFOURMILIERE && f.Y!=YFOURMILIERE){
				if (f.X - XFOURMILIERE < 0)
					f.idOrientation = 3;
				if (f.X - XFOURMILIERE>0)
					f.idOrientation = 7;
				if (f.Y - YFOURMILIERE < 0)
					f.idOrientation=5;
				if (f.Y - YFOURMILIERE > 0)
					f.idOrientation = 1;
			}
		}
	}
}

//============== Logique Fourmi =================|
//===============================================|




int main(int argc, char *argv[]) {
	srand(time(NULL));

	initialisationSDL();	
	
	Map map;
	char nomMap[10] = "map0.ppm";
	int idMap = 0;

	initMap(map, nomMap, idMap);

	Colonie c;
	initColonie(c);


	while(true){
	dessinMap(map);
	for (int i = 0; i < NBFOURMI; i++) {
		orientationAlea(c[i]);
		afficheFourmi(c[i]);
		deplacerFourmi(c[i], map);
	}
	SDL_RenderPresent(ecran);	
	for (int i = 0; i < NBFOURMI; i++) {
		actionNourriture(c[i], map);
	}
	SDL_Delay(30);
	}



	bool continuer = true;

	while (continuer) {


		SDL_Event event;

		SDL_WaitEvent(&event);//attente d’un évènement
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



	/*
	sqlite3*bd;
	char chemin[] = "fourmis.sqlite";
	bd = bd_ouvrir(chemin);
	bd_requeteUpdate(bd, "delete from map");
	bd_requeteUpdate(bd, "insert into map values (1,'nomTest',50,50)");
	int x = 2, largeur = 20, hauteur = 30;
	char name[10] = "map";
	ostringstream req;
	req.str("");
	req << "insert into map values (" << x << ",'" << name << "'," << largeur << "," << hauteur << ")";
	bd_requeteUpdate(bd, req.str().c_str());
	int L, C;
	char ***t = NULL;
	bd_requeteSelect(bd, "select * from map", t, L, C);
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			cout << t[i][j] << "|";
		}
		cout << endl;
	}
	bd_detruireTab(t, L, C);
	bd_fermer;
	*/
	


	system("pause");
	destructionSDL();

	return EXIT_SUCCESS;
}
