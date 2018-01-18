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

// ======= Utilitaire ============


//===============================================|
//================ Gestion Map / SQLITE==================|

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
			QUANTITE_TOTALE_NOURRITURE_MAP += qttNourr;
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

void ajouterDonneesSQLITE(Map m, int tempstotalrecolte) {
	sqlite3* bd;
	char nomBase[] = "fourmis.sqlite";
	bd = bd_ouvrir(nomBase);
	ostringstream req;
	req.str("");
	req << "insert into Simulation values (NULL" << 
		"," << idMap <<
		",'" << nomMap <<
		"'," << NBFOURMI_ECLAIREUSE <<
		"," << NBFOURMI_TRANSPORTEUSE <<
		"," << QUANTITE_TOTALE_NOURRITURE <<
		"," << tempstotalrecolte << ")";
	cout << req.str().c_str() << endl;
	bool test = bd_requeteUpdate(bd, req.str().c_str());
	cout << test << endl;

}

//================ Gestion Map / SQLITE ==================|
//===============================================|
//=================== SDL =======================|

void initialisationSDL() {


	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << " Echec à l’ouverture ";
	}

	TTF_Init();
	fontLarge = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 48);
	fontSmall = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 18);


}

void initImagesFourmis() {
	char chemin[20];
	strcpy_s(chemin, "Images/FourmiNO.png");
	Orientation[0] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiN.png");
	Orientation[1] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiNE.png");
	Orientation[2] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiE.png");
	Orientation[3] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiSE.png");
	Orientation[4] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiS.png");
	Orientation[5] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiSO.png");
	Orientation[6] = loadImage(ecranSim, chemin);
	strcpy_s(chemin, "Images/FourmiO.png");
	Orientation[7] = loadImage(ecranSim, chemin);
}

void afficheMenu() {
	//Affichage de la fenêtre
	windowMenu = SDL_CreateWindow("Fourmi simulator 2K18", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);
	if (windowMenu == NULL) {
		cout << "erreur ouverture fenetre";
	}
	ecranMenu = SDL_CreateRenderer(windowMenu, -1, SDL_RENDERER_ACCELERATED);

	//Affichage du contenu de la fenêtre

	SDL_Rect fond;
	fond.x = fond.y = 0;
	fond.w = fond.h = 600;
	SDL_SetRenderDrawColor(ecranMenu, 0, 128, 0, 255);
	SDL_RenderFillRect(ecranMenu, &fond);

	SDL_Rect boutonSimul;
	boutonSimul.x = 150;
	boutonSimul.y = 200;
	boutonSimul.h = 62;
	boutonSimul.w = 300;
	SDL_SetRenderDrawColor(ecranMenu, 0, 0, 0, 255);
	SDL_RenderFillRect(ecranMenu, &boutonSimul);
	
	SDL_Rect texteBoutonSimul;
	texteBoutonSimul.x = 200;
	texteBoutonSimul.y = 210;
	SDL_Color blanc = { 255,255,255 };
	char texteSim[] = "Simulation";
	SDL_Texture *texte_TextureSimul = loadText(ecranMenu, texteSim, blanc, fontLarge);
	SDL_QueryTexture(texte_TextureSimul, NULL, NULL, &texteBoutonSimul.w, &texteBoutonSimul.h);
	SDL_RenderCopy(ecranMenu, texte_TextureSimul, NULL, &texteBoutonSimul);


	SDL_Rect boutonEdit;
	boutonEdit.x = 150;
	boutonEdit.y = 362;
	boutonEdit.h = 62;
	boutonEdit.w = 300;
	SDL_SetRenderDrawColor(ecranMenu, 0, 0, 0, 255);
	SDL_RenderFillRect(ecranMenu, &boutonEdit);

	SDL_Rect texteBoutonEdit;
	texteBoutonEdit.x = 150;
	texteBoutonEdit.y = 372;
	char texteEdit[] = "Editeur de Map";
	SDL_Texture *texte_TextureEdit = loadText(ecranMenu, texteEdit, blanc, fontLarge);
	SDL_QueryTexture(texte_TextureEdit, NULL, NULL, &texteBoutonEdit.w, &texteBoutonEdit.h);
	SDL_RenderCopy(ecranMenu, texte_TextureEdit, NULL, &texteBoutonEdit);


	SDL_RenderPresent(ecranMenu);

}

void afficheNomMap() {
	SDL_Rect fondRectTexteInputNFM;
	fondRectTexteInputNFM.x = 290;
	fondRectTexteInputNFM.y = 50;
	fondRectTexteInputNFM.h = 25;
	fondRectTexteInputNFM.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 255, 255, 255, 255);
	SDL_SetTextInputRect(&fondRectTexteInputNFM);
	SDL_RenderFillRect(ecranParam, &fondRectTexteInputNFM);

	SDL_Rect rectTexteInputNFM;
	rectTexteInputNFM.x = 290;
	rectTexteInputNFM.y = 50;
	rectTexteInputNFM.h = 25;
	rectTexteInputNFM.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 0, 0, 0, 255);
	SDL_SetTextInputRect(&rectTexteInputNFM);
	SDL_RenderDrawRect(ecranParam, &rectTexteInputNFM);
	

	SDL_Rect contenuTexteInputNFM;
	contenuTexteInputNFM.x = 295;
	contenuTexteInputNFM.y = 55;
	char texte_boutonNFM[10];
	SDL_Texture *texte_TextureNFM = loadText(ecranParam, nomMap, { 0,0,0 }, fontSmall);
	SDL_QueryTexture(texte_TextureNFM, NULL, NULL, &contenuTexteInputNFM.w, &contenuTexteInputNFM.h);
	SDL_RenderCopy(ecranParam, texte_TextureNFM, NULL, &contenuTexteInputNFM);

	SDL_RenderPresent(ecranParam);
}

void afficheIdMap() {
	SDL_Rect fondRectTexteInputIDM;
	fondRectTexteInputIDM.x = 290;
	fondRectTexteInputIDM.y = 100;
	fondRectTexteInputIDM.h = 25;
	fondRectTexteInputIDM.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 255, 255, 255, 255);
	SDL_SetTextInputRect(&fondRectTexteInputIDM);
	SDL_RenderFillRect(ecranParam, &fondRectTexteInputIDM);

	SDL_Rect rectTexteInputIDM;
	rectTexteInputIDM.x = 290;
	rectTexteInputIDM.y = 100;
	rectTexteInputIDM.h = 25;
	rectTexteInputIDM.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 0, 0, 0, 255);
	SDL_SetTextInputRect(&rectTexteInputIDM);
	SDL_RenderDrawRect(ecranParam, &rectTexteInputIDM);

	if (idMap != -1) {
		SDL_Rect contenuTexteInputIDM;
		contenuTexteInputIDM.x = 295;
		contenuTexteInputIDM.y = 105;
		char texte_boutonIDM[10];
		_itoa_s(idMap, texte_boutonIDM, 10);
		SDL_Texture *texte_TextureIDM = loadText(ecranParam, texte_boutonIDM, { 0,0,0 }, fontSmall);
		SDL_QueryTexture(texte_TextureIDM, NULL, NULL, &contenuTexteInputIDM.w, &contenuTexteInputIDM.h);
		SDL_RenderCopy(ecranParam, texte_TextureIDM, NULL, &contenuTexteInputIDM);
	}

	SDL_RenderPresent(ecranParam);
}

void afficheTypeMouvement() {
	SDL_Rect fondRectTexteInputTM;
	fondRectTexteInputTM.x = 290;
	fondRectTexteInputTM.y = 250;
	fondRectTexteInputTM.h = 25;
	fondRectTexteInputTM.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 255, 255, 255, 255);
	SDL_SetTextInputRect(&fondRectTexteInputTM);
	SDL_RenderFillRect(ecranParam, &fondRectTexteInputTM);

	SDL_Rect rectTexteInputTM;
	rectTexteInputTM.x = 290;
	rectTexteInputTM.y = 250;
	rectTexteInputTM.h = 25;
	rectTexteInputTM.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 0, 0, 0, 255);
	SDL_SetTextInputRect(&rectTexteInputTM);
	SDL_RenderDrawRect(ecranParam, &rectTexteInputTM);

	if (typeMouvement != -1) {
		SDL_Rect contenuTexteInputTM;
		contenuTexteInputTM.x = 295;
		contenuTexteInputTM.y = 255;
		char texte_boutonTM[10];
		_itoa_s(typeMouvement, texte_boutonTM, 10);
		SDL_Texture *texte_TextureTM = loadText(ecranParam, texte_boutonTM, { 0,0,0 }, fontSmall);
		SDL_QueryTexture(texte_TextureTM, NULL, NULL, &contenuTexteInputTM.w, &contenuTexteInputTM.h);
		SDL_RenderCopy(ecranParam, texte_TextureTM, NULL, &contenuTexteInputTM);
	}

	SDL_RenderPresent(ecranParam);
}

void afficheParam() {
	windowParam = SDL_CreateWindow("Parametres simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, SDL_WINDOW_SHOWN);
	if (windowParam == NULL) {
		cout << "erreur ouverture fenetre";
	}
	ecranParam = SDL_CreateRenderer(windowParam, -1, SDL_RENDERER_ACCELERATED);
	SDL_StartTextInput();

	SDL_Rect fond;
	fond.x = fond.y = 0;
	fond.w = fond.h = 400;
	SDL_SetRenderDrawColor(ecranParam, 255, 255, 255, 255);
	SDL_RenderFillRect(ecranParam, &fond);

	SDL_Color Noir = { 0,0,0 };
	SDL_Color Blanc = { 255,255,255 };

	// ======== NomMap ===========
	SDL_Rect rectTexteNFM;
	rectTexteNFM.x = 25;
	rectTexteNFM.y = 50;
	char texteNFM[] = "Nom du fichier map: ";
	SDL_Texture *texte_TextureNFM = loadText(ecranParam, texteNFM, Noir, fontSmall);
	SDL_QueryTexture(texte_TextureNFM, NULL, NULL, &rectTexteNFM.w, &rectTexteNFM.h);
	SDL_RenderCopy(ecranParam, texte_TextureNFM, NULL, &rectTexteNFM);
	
	afficheNomMap();
	// ======== NomMap ===========


	// ======== idMap ===========
	SDL_Rect rectTexteIDM;
	rectTexteIDM.x = 25;
	rectTexteIDM.y = 100;
	char texteIDM[] = "id map: ";
	SDL_Texture *texte_TextureIDM = loadText(ecranParam, texteIDM, Noir, fontSmall);
	SDL_QueryTexture(texte_TextureIDM, NULL, NULL, &rectTexteIDM.w, &rectTexteIDM.h);
	SDL_RenderCopy(ecranParam, texte_TextureIDM, NULL, &rectTexteIDM);

	afficheIdMap();
	// ======== idMap ===========


	// ======== nb fourmis eclaireuses ===========
	SDL_Rect rectTexteNBFE;
	rectTexteNBFE.x = 25;
	rectTexteNBFE.y = 150;
	char texteNBFE[] = "Nombre fourmis eclaireuses: ";
	SDL_Texture *texte_TextureNBFE = loadText(ecranParam, texteNBFE, Noir, fontSmall);
	SDL_QueryTexture(texte_TextureNBFE, NULL, NULL, &rectTexteNBFE.w, &rectTexteNBFE.h);
	SDL_RenderCopy(ecranParam, texte_TextureNBFE, NULL, &rectTexteNBFE);

	SDL_Rect rectFondTexteInputNBFE;
	rectFondTexteInputNBFE.x = 290;
	rectFondTexteInputNBFE.y = 150;
	rectFondTexteInputNBFE.h = 25;
	rectFondTexteInputNBFE.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 128, 128, 128, 255);
	SDL_SetTextInputRect(&rectFondTexteInputNBFE);
	SDL_RenderFillRect(ecranParam, &rectFondTexteInputNBFE);

	SDL_Rect rectTexteInputNBFE;
	rectTexteInputNBFE.x = 290;
	rectTexteInputNBFE.y = 150;
	rectTexteInputNBFE.h = 25;
	rectTexteInputNBFE.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 0, 0, 0, 255);
	SDL_SetTextInputRect(&rectTexteInputNBFE);
	SDL_RenderDrawRect(ecranParam, &rectTexteInputNBFE);

	SDL_Rect contenuTexteInputNBFE;
	contenuTexteInputNBFE.x = 295;
	contenuTexteInputNBFE.y = 155;
	char texte_boutonNBFE[10];
	_itoa_s(NBFOURMI_ECLAIREUSE, texte_boutonNBFE, 10);
	SDL_Texture *texte_nbTextureNBFE = loadText(ecranParam, texte_boutonNBFE, { 0,0,0 }, fontSmall);
	SDL_QueryTexture(texte_nbTextureNBFE, NULL, NULL, &contenuTexteInputNBFE.w, &contenuTexteInputNBFE.h);
	SDL_RenderCopy(ecranParam, texte_nbTextureNBFE, NULL, &contenuTexteInputNBFE);
	// ======== nb fourmis eclaireuses ===========

	// ======== nb fourmis transporteuses ===========
	SDL_Rect rectTexteNBFT;
	rectTexteNBFT.x = 25;
	rectTexteNBFT.y = 200;
	char texteNBFT[] = "Nombre fourmis transporteuses: ";
	SDL_Texture *texte_TextureNBFT = loadText(ecranParam, texteNBFT, Noir, fontSmall);
	SDL_QueryTexture(texte_TextureNBFT, NULL, NULL, &rectTexteNBFT.w, &rectTexteNBFT.h);
	SDL_RenderCopy(ecranParam, texte_TextureNBFT, NULL, &rectTexteNBFT);

	SDL_Rect rectFondTexteInputNBFT;
	rectFondTexteInputNBFT.x = 290;
	rectFondTexteInputNBFT.y = 200;
	rectFondTexteInputNBFT.h = 25;
	rectFondTexteInputNBFT.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 128, 128, 128, 255);
	SDL_SetTextInputRect(&rectFondTexteInputNBFT);
	SDL_RenderFillRect(ecranParam, &rectFondTexteInputNBFT);

	SDL_Rect rectTexteInputNBFT;
	rectTexteInputNBFT.x = 290;
	rectTexteInputNBFT.y = 200;
	rectTexteInputNBFT.h = 25;
	rectTexteInputNBFT.w = 85;
	SDL_SetRenderDrawColor(ecranParam, 0, 0, 0, 255);
	SDL_SetTextInputRect(&rectTexteInputNBFT);
	SDL_RenderDrawRect(ecranParam, &rectTexteInputNBFT);

	SDL_Rect contenuTexteInputNBFT;
	contenuTexteInputNBFT.x = 295;
	contenuTexteInputNBFT.y = 205;
	char texte_boutonNBFT[10];
	_itoa_s(NBFOURMI_ECLAIREUSE, texte_boutonNBFT, 10);
	SDL_Texture *texte_nbTextureNBFT = loadText(ecranParam, texte_boutonNBFT, { 0,0,0 }, fontSmall);
	SDL_QueryTexture(texte_nbTextureNBFT, NULL, NULL, &contenuTexteInputNBFT.w, &contenuTexteInputNBFT.h);
	SDL_RenderCopy(ecranParam, texte_nbTextureNBFT, NULL, &contenuTexteInputNBFT);

	// ======== nb fourmis transporteuses ===========

	SDL_Rect rectTexteTM;
	rectTexteTM.x = 25;
	rectTexteTM.y = 250;
	char texteTM[] = "Type mouvement: ";
	SDL_Texture *texte_TextureTM = loadText(ecranParam, texteTM, Noir, fontSmall);
	SDL_QueryTexture(texte_TextureTM, NULL, NULL, &rectTexteTM.w, &rectTexteTM.h);
	SDL_RenderCopy(ecranParam, texte_TextureTM, NULL, &rectTexteTM);

	afficheTypeMouvement();

	SDL_Rect boutonGo;
	boutonGo.x = 25;
	boutonGo.y = 300;
	boutonGo.h = 62;
	boutonGo.w = 350;
	SDL_SetRenderDrawColor(ecranParam, 0, 0, 0, 255);
	SDL_RenderFillRect(ecranParam, &boutonGo);

	SDL_Rect texteBoutonGo;
	texteBoutonGo.x = 75;
	texteBoutonGo.y = 310;
	char texteGo[] = "Simuler >";
	SDL_Texture *texte_TextureGo = loadText(ecranParam, texteGo, Blanc, fontLarge);
	SDL_QueryTexture(texte_TextureGo, NULL, NULL, &texteBoutonGo.w, &texteBoutonGo.h);
	SDL_RenderCopy(ecranParam, texte_TextureGo, NULL, &texteBoutonGo);

	SDL_RenderPresent(ecranParam);
}

void afficheSim() {
	windowSim = SDL_CreateWindow("Simulation Fourmis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR*TAILLECASE * 2, HAUTEUR*TAILLECASE, SDL_WINDOW_SHOWN);
	if (windowSim == NULL) {
		cout << "erreur ouverture fenetre";
	}
	ecranSim = SDL_CreateRenderer(windowSim, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(ecranSim, SDL_BLENDMODE_BLEND);
}

void afficheEdit(){
	windowEdit = SDL_CreateWindow("Editeur de map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, nbCase_EDM*TAILLE_CASE_EDM+300, nbCase_EDM*TAILLE_CASE_EDM, SDL_WINDOW_SHOWN);
	if (windowEdit == NULL) {
		cout << "erreur ouverture fenetre";
	}
	ecranEdit = SDL_CreateRenderer(windowEdit, -1, SDL_RENDERER_ACCELERATED);
}




void destructionMenu() {
	SDL_DestroyRenderer(ecranMenu);
	SDL_DestroyWindow(windowMenu);
}

void destructionParam() {
	SDL_DestroyRenderer(ecranParam);
	SDL_DestroyWindow(windowParam);
	SDL_StopTextInput();
}

void destructionSim() {
	SDL_DestroyRenderer(ecranSim);
	SDL_DestroyWindow(windowSim);
}

void destructionEdit() {
	SDL_DestroyRenderer(ecranEdit);
	SDL_DestroyWindow(windowEdit);
}

void destructionSDL() {

  //equivalent du delete

	TTF_CloseFont(fontLarge); //on ferme la font
	TTF_Quit(); //on quitte la TTF

	destructionEdit();
	destructionMenu();
	destructionParam();
	destructionSim();
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
	SDL_SetRenderDrawColor(ecranSim, R, G, B, 255);
	SDL_RenderFillRect(ecranSim, &fondCase);

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
	SDL_RenderCopy(ecranSim, Orientation[f.idOrientation], NULL, &tuile);
}

void afficheBaseGraphe() {
	SDL_Rect fond;
	fond.x = LARGEUR * TAILLECASE;
	fond.y = 0;
	fond.h = HAUTEUR*TAILLECASE;
	fond.w = LARGEUR*TAILLECASE;
	SDL_SetRenderDrawColor(ecranSim, 255, 255, 255, 255);
	SDL_RenderFillRect(ecranSim, &fond);

	int XBaseRepere = LARGEUR * TAILLECASE + 20;
	int YBaseRepere = HAUTEUR * TAILLECASE - 20;

	SDL_SetRenderDrawColor(ecranSim, 0, 0, 0, 255);
	SDL_RenderDrawLine(ecranSim, XBaseRepere, YBaseRepere, XBaseRepere + LONGEUR_AXE_GRAPHE, YBaseRepere);
	SDL_RenderDrawLine(ecranSim, XBaseRepere, YBaseRepere, XBaseRepere, YBaseRepere - HAUTEUR_AXE_GRAPHE);


}

void ajouterValeurGraphe(int instant, int valeur) {
	int val1X = INSTANT_TMP * LONGEUR_AXE_GRAPHE / 4000 + LARGEUR * TAILLECASE + 20;
	int val2X = instant * LONGEUR_AXE_GRAPHE / 4000 + LARGEUR * TAILLECASE + 20;
	SDL_SetRenderDrawColor(ecranSim, 255, 0, 0, 255);
	SDL_RenderDrawLine(ecranSim, val1X, HAUTEUR_AXE_GRAPHE+20-VALEUR_TMP, val2X, HAUTEUR_AXE_GRAPHE+20-valeur);
	INSTANT_TMP = instant;
	VALEUR_TMP = valeur;
}

void affichePheromones(ListePheromone lp) {
	if (typeMouvement == 4) {
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
					SDL_SetRenderDrawColor(ecranSim, 128, 128, 128, 100);
					SDL_RenderFillRect(ecranSim, &pheromone);
				}
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
		if (f.etat == 1 && !lp[f.Y][f.X].active) {
			lp[f.Y][f.X].active = true;
		}
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
	else if (aleaRotation >= 16 && aleaRotation <= 17) {
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
	else if (aleaRotation == 18) {
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
	int aleaRotation = rand() % 16;


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
	bool nourriturePresente = false;
	for (int i = 0; i < 8; i++) {
		tournerFourmiDroite(f);
		if (m[yCaseEnFaceFourmi(f)][xCaseEnFaceFourmi(f)].typeCase == 1){// si il y a de la nourriture en face de la fourmie
			nourriturePresente = true;
			break;//on la laisse dans cette direction
		}
	}
	if (!nourriturePresente) {//si il n'y a pas de nourriture, on cherche autour pour voir si il y a des phéromones

		/*Revoir ici en prenant en compte 
		la distance entre les =/= cases 
		autour de la fourmie et la fourmillière
		afin de chosir la plus éloignée
		
		idées:	abs (diff x et y en mm temps)
				difference x et y séparément
		*/



		if (!lp[yCaseEnFaceFourmi(f)][xCaseEnFaceFourmi(f)].active) {//si la case en face de la fourmi n'est pas une phéromone
			for (int i = 0; i < 8; i++) {// on tourne à droite jusquà 8 fois tant que la fourmie se tourne vers une pheromone
				tournerFourmiDroite(f);
				if (lp[yCaseEnFaceFourmi(f)][xCaseEnFaceFourmi(f)].active) {//si il y a une pheromone
					pheromonePresente = true;
					break;//on la laisse dans cette direction
				}
			}
		}


		if (!pheromonePresente) {//si aucune pheromone n'est trouvée autour
			deplacementDistribution3(f);//déplacement de type 3
		}
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


void initFourmi(Fourmi &f, int type ) {
	f.X = LARGEUR / 2;
	f.Y = HAUTEUR / 2;
	f.etat = 0;
	f.idOrientation = rand() % 8;
	f.type = type;
}

void initColonie(Colonie &c) {
	for (int i = 0; i < NBFOURMI_ECLAIREUSE; i++) {
		initFourmi(c[i], 0);
	}
	for (int i = NBFOURMI_ECLAIREUSE; i < NBFOURMI_ECLAIREUSE + NBFOURMI_TRANSPORTEUSE; i++) {
		initFourmi(c[i], 1);
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
				if (f.type) {
					f.etat = 1;
					f.quantiteNourriture = 1;
					m[f.Y][f.X].quantiteNourriture--;
				}
				else {
					f.etat = 1;
				}
				//cout << "Nourriture recoltee." << endl;
				if (m[f.Y][f.X].quantiteNourriture == 0) {
					m[f.Y][f.X].typeCase = 0;
					m[f.Y][f.X].R = 0;
					m[f.Y][f.X].G = 128;
					m[f.Y][f.X].B = 0;
					//cout << "Nourriture Epuisee en (" << f.Y << ";" << f.X << ")." << endl;
				}
			}
		}
		else if (f.X == XPOSFOURMILLIERE && f.Y == YPOSFOURMILLIERE) {
			f.etat = 0;
			f.quantiteNourriture = 0;
			QUANTITE_TOTALE_NOURRITURE++;
			//cout << "Nourriture Deposee." << endl;
			//cout << "Quantite totale: " << QUANTITE_TOTALE_NOURRITURE << endl;
		}
}



//============== Logique Fourmi =================|
//===============================================|


// ==============================================
// ==================== EDM =====================


void init_MapEDM_SDL(Map_EDM m_EDM) {
	for (int i = 0; i < nbCase_EDM; i++) {
		for (int j = 0; j < nbCase_EDM; j++) {
			m_EDM[i][j].x = j;
			m_EDM[i][j].y = i;
			m_EDM[i][j].R = 0;
			m_EDM[i][j].G = 128;
			m_EDM[i][j].B = 0;
		}
	}
}

void afficheCase_EDM(Case_EDM c_EDM) {
	SDL_Rect c;
	c.x = c_EDM.x * TAILLE_CASE_EDM;
	c.y = c_EDM.y * TAILLE_CASE_EDM;
	c.h = c.w = TAILLE_CASE_EDM -1;
	SDL_SetRenderDrawColor(ecranEdit, c_EDM.R, c_EDM.G, c_EDM.B, 255);
	SDL_RenderFillRect(ecranEdit, &c);
}

void afficheMap_EDM(Map_EDM m_EDM) {
	for (int i = 0; i < nbCase_EDM; i++) {
		for (int j = 0; j < nbCase_EDM; j++) {
			afficheCase_EDM(m_EDM[i][j]);
		}
	}
	SDL_RenderPresent(ecranEdit);
}

// ==================== EDM =====================
// ==============================================



int main(int argc, char *argv[]) {
	srand(time(NULL));
	
	initialisationSDL();


	bool continuerExec = true;
	bool continuerMenu = true;
	bool continuerParam = false;
	bool continuerSim = false;
	bool continuerEdit = false;

	bool simulationInitialisee;
	bool simuler;
	int tempsRecolte;
	bool recolteFinie;
	int instantT;

	bool editeurInitialise;

	afficheMenu();
	while (continuerExec) {

		Map map;
		Colonie c;
		ListePheromone lp;
		simulationInitialisee = false;
		simuler = false;
		editeurInitialise = false;

		Map_EDM map_EDM;

		while (continuerMenu) {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {//attente d’un évènement
				switch (event.type) //test du type d’évènement
				{
				case SDL_QUIT: //clic sur la croix de fermeture
							   //on peut enlever SDL_Delay
					continuerExec = false;
					continuerMenu = false;
					break;
				case SDL_MOUSEBUTTONUP:
					int posYClic = event.button.y;
					int posXClic = event.button.x;
					if (150 < posXClic && posXClic < 450 && 200 < posYClic && posYClic < 262) {
						continuerParam = true;
						continuerMenu = false;

						afficheParam();
						destructionMenu();
					}
					else if (150 < posXClic && posXClic < 450 && 362 < posYClic && posYClic < 424) {
						continuerEdit = true;
						continuerMenu = false;

						afficheEdit();
						destructionMenu();
					}
					break;
				}
			}
		}
		while (continuerParam) {

			SDL_Event event;
			while (SDL_PollEvent(&event)) {//attente d’un évènement
				switch (event.type) //test du type d’évènement
				{
				case SDL_QUIT: //clic sur la croix de fermeture
							   //on peut enlever SDL_Delay
					continuerParam = false;
					continuerMenu = true;
					destructionParam(); 
					afficheMenu();
					break;
				case SDL_MOUSEBUTTONUP:
					int posYClic = event.button.y;
					int posXClic = event.button.x;
					if (300 < posXClic && posXClic < 375 && 50 < posYClic && posYClic < 75) {
						cout << "Entrer un nom de fichier map: ";
						cin.getline(nomMap, 10);
						afficheNomMap();
					}
					if (300 < posXClic && posXClic < 375 && 100 < posYClic && posYClic < 125){
						cout << "Entrer un id de map: ";
						cin >> idMap;
						afficheIdMap();
					}
					if (300 < posXClic && posXClic < 375 && 250 < posYClic && posYClic < 275) {
						cout << "Entrer un type de movuement (0,1,2,3,4): ";
						cin >> typeMouvement;
						afficheTypeMouvement();
					}
					if (25 < posXClic && posXClic < 325 && 300 < posYClic && posYClic < 362) {
						bool mapExiste = false;
						for (int i = 0; i < 4; i++) {
							if (strcmp(nomMap, listeMaps[i])) {
								mapExiste = true;
								break;
							}
						}
						if (mapExiste && 0 <= idMap && idMap <= 4 && 0 <= typeMouvement && typeMouvement <= 4) {
						continuerParam = false;
						continuerSim = true;

						afficheSim();
						destructionParam();
						}
						else {
							cout << "Parametres incorrectes." << endl;
						}
					}
					break;
				}
			}
		}
		while (continuerSim) {
			if (!simulationInitialisee) {
				initImagesFourmis();
				initMap(map, nomMap, idMap);
				initColonie(c);
				initListePheromones(lp);
				afficheBaseGraphe();
				simulationInitialisee = true;
				simuler = true;
				recolteFinie = false;
				tempsRecolte = 4000;
				instantT = 0;
			}
			if (simuler) {
				dessinMap(map);
				affichePheromones(lp);
				for (int i = 0; i < NBFOURMI_ECLAIREUSE + NBFOURMI_TRANSPORTEUSE; i++) {
					orientationFourmi(c[i], lp, map, typeMouvement);
					afficheFourmi(c[i]);
					deplacerFourmi(c[i], lp, map);
					if (!c[i].type)
					{
						if (c[i].X == LARGEUR / 2 && c[i].Y == HAUTEUR / 2) {
							c[i].etat = 0;
						}
						orientationFourmi(c[i], lp, map, typeMouvement);
						afficheFourmi(c[i]);
						deplacerFourmi(c[i], lp, map);
						if (c[i].X == LARGEUR / 2 && c[i].Y == HAUTEUR / 2) {
							c[i].etat = 0;
						}
					}

				}
				for (int i = 0; i < NBFOURMI_ECLAIREUSE + NBFOURMI_TRANSPORTEUSE; i++) {
					actionNourriture(c[i], map);
				}
				SDL_Delay(0);
				ajouterValeurGraphe(instantT, QUANTITE_TOTALE_NOURRITURE);
				SDL_RenderPresent(ecranSim);
				instantT += 1;
				if (!recolteFinie && QUANTITE_TOTALE_NOURRITURE == QUANTITE_TOTALE_NOURRITURE_MAP) {
					tempsRecolte = instantT;
					cout << "Temps recolte: " << tempsRecolte << endl;
					cout << "Envoir de donnees." << endl;
					ajouterDonneesSQLITE(map, tempsRecolte);
					recolteFinie = true;
				}
				if (instantT == 4000) {
					simuler = false;
					continuerSim = false;
					continuerMenu = true;
					INSTANT_TMP = 0;
					VALEUR_TMP = 0;
					QUANTITE_TOTALE_NOURRITURE = 0;
					SDL_Delay(3000);
					destructionSim();
					afficheMenu();
				}
			}

			SDL_Event event;
			while (SDL_PollEvent(&event)) {//attente d’un évènement
				switch (event.type) //test du type d’évènement
				{
				case SDL_QUIT: //clic sur la croix de fermeture
							   //on peut enlever SDL_Delay
					continuerSim = false;
					continuerMenu = true;
					destructionSim();
					afficheMenu();
				}
			}
		}
		while (continuerEdit) {
			if (!editeurInitialise) {
				init_MapEDM_SDL(map_EDM);
				afficheMap_EDM(map_EDM);
				editeurInitialise = true;
			}

			SDL_Event event;
			while (SDL_PollEvent(&event)) {//attente d’un évènement
				switch (event.type) //test du type d’évènement
				{

				case SDL_QUIT: //clic sur la croix de fermeture
							   //on peut enlever SDL_Delay
					continuerEdit = false;
					continuerMenu = true;
					destructionEdit();
					afficheMenu();
					break;
				case SDL_MOUSEBUTTONDOWN:
					int xClic = event.button.x;
					int yClic = event.button.y;
					int xCase = xClic / TAILLE_CASE_EDM;
					int yCase = yClic / TAILLE_CASE_EDM;
						cout << "(" << yCase << ";" << xCase << ")" << endl;
						map_EDM[yCase][xCase].G = 0;
						map_EDM[yCase][xCase].R += 35;
						afficheMap_EDM(map_EDM);
					break;
				}
			}
		}
	}
	destructionSDL();

	return EXIT_SUCCESS;
}
