#include <iostream>
#include <ctime>
#include "config_sdl.h"

using namespace std;


const int nbCase = 120;
const int TAILLE_CASE = 5;
const int LARGEUR_FENETRE = nbCase * TAILLE_CASE / 2 * 3; //largeur = 900 

int typePinceau;

SDL_Window*window = NULL;
SDL_Renderer*rendu = NULL;
TTF_Font*font = NULL;

typedef struct Case {
	int x;
	int y;
	int typeCase;
	int quantiteNourriture;
	int R;
	int G;
	int B;
}Case;


typedef Case Grille[nbCase][nbCase];


void couleur_pinceau(int c) { // 0=vert 1=rouge 2=noir
	if (c == 0)
		SDL_SetRenderDrawColor(rendu, 0, 128, 0, 255);
	else if (c == 1)
		SDL_SetRenderDrawColor(rendu, 32, 128, 128, 255);
	else if (c == 2)
		SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	
}


void initialisationSDL() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << " Echec à l’ouverture ";
	}

	TTF_Init();
	font = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 25);


	window = SDL_CreateWindow("Editeur de map",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		LARGEUR_FENETRE,
		nbCase*	TAILLE_CASE,
		SDL_WINDOW_SHOWN);

	if (window == NULL) {
		cout << "erreur ouverture fenetre";
	}

	rendu = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderPresent(rendu);

}

void destructionSDL() {

	SDL_DestroyRenderer(rendu);
	SDL_DestroyWindow(window);   //equivalent du delete

	TTF_CloseFont(font); //on ferme la font
	TTF_Quit(); //on quitte la TTF

				//fermeture de la SDL
	SDL_Quit();
}

void fond_droite(SDL_Renderer *rendu) {
			SDL_Rect FD;
			FD.x = nbCase * TAILLE_CASE;
			FD.y = 0;
			FD.w = 300;
			FD.h = nbCase * TAILLE_CASE;
			SDL_SetRenderDrawColor(rendu, 255,255,255, 255);
			SDL_RenderFillRect(rendu, &FD);
}

void bouton_terrain(SDL_Renderer *rendu) {
			SDL_Rect BT;
			BT.x = nbCase * TAILLE_CASE;
			BT.y = 200;
			BT.w = 300;
			BT.h = 75;
			SDL_SetRenderDrawColor(rendu, 0, 255, 0, 255);
			SDL_RenderFillRect(rendu, &BT);

			SDL_Rect text_button_terrain;
			text_button_terrain.x = 700;
			text_button_terrain.y = 210;
			SDL_Color blanc = { 255,255,255 };
			char tab[] = "Terrain";
			SDL_Texture *texte_TextureSimul = loadText(rendu, tab, blanc, font);
			SDL_QueryTexture(texte_TextureSimul, NULL, NULL, &text_button_terrain.w, &text_button_terrain.h);
			SDL_RenderCopy(rendu, texte_TextureSimul, NULL, &text_button_terrain);
}

void bouton_nourriture(SDL_Renderer *rendu) {
			SDL_Rect BN;
			BN.x = nbCase * TAILLE_CASE;
			BN.y = 300;
			BN.w = 300;
			BN.h = 75;//
			SDL_SetRenderDrawColor(rendu, 255, 0, 0, 255);
			SDL_RenderFillRect(rendu, &BN);


			SDL_Rect text_button_nourriture;
			text_button_nourriture.x = 700;
			text_button_nourriture.y = 310;
			SDL_Color blanc = { 255,255,255 };
			char tab[] = "Nourriture";
			SDL_Texture *texte_TextureSimul = loadText(rendu, tab, blanc, font);
			SDL_QueryTexture(texte_TextureSimul, NULL, NULL, &text_button_nourriture.w, &text_button_nourriture.h);
			SDL_RenderCopy(rendu, texte_TextureSimul, NULL, &text_button_nourriture);
}


void bouton_obstacle(SDL_Renderer *rendu) {
			SDL_Rect BO;
			BO.x = nbCase * TAILLE_CASE;
			BO.y = 400;
			BO.w = 300;
			BO.h = 75;
			SDL_SetRenderDrawColor(rendu, 0, 0, 255, 255);
			SDL_RenderFillRect(rendu, &BO);


			SDL_Rect text_button_obstacle;
			text_button_obstacle.x = 700;
			text_button_obstacle.y = 410;
			SDL_Color blanc = { 255,255,255 };
			char tab[] = "Obstacle";
			SDL_Texture *texte_TextureSimul = loadText(rendu, tab, blanc, font);
			SDL_QueryTexture(texte_TextureSimul, NULL, NULL, &text_button_obstacle.w, &text_button_obstacle.h);
			SDL_RenderCopy(rendu, texte_TextureSimul, NULL, &text_button_obstacle);
}

void bouton_save(SDL_Renderer *rendu) {
			SDL_Rect BS;
			BS.x = nbCase * TAILLE_CASE;
			BS.y = 500;
			BS.w = 500;
			BS.h = 75;
			SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
			SDL_RenderFillRect(rendu, &BS);


			SDL_Rect text_button_save;
			text_button_save.x = 700;
			text_button_save.y = 510;
			SDL_Color blanc = { 255,255,255 };
			char tab[] = "Sauvegarder";
			SDL_Texture *texte_TextureSimul = loadText(rendu, tab, blanc, font);
			SDL_QueryTexture(texte_TextureSimul, NULL, NULL, &text_button_save.w, &text_button_save.h);
			SDL_RenderCopy(rendu, texte_TextureSimul, NULL, &text_button_save);
}

	 


void Grille_Edit(Grille g) {
	for (int i = 0; i < nbCase; i++) {
		for (int j = 0; j < nbCase; j++) {
			g[i][j].x = j;
			g[i][j].y = i;
			g[i][j].typeCase = 0;
			g[i][j].quantiteNourriture = 0;
			g[i][j].R = 0;
			g[i][j].G = 128;
			g[i][j].B = 0;
		}
	}
}

void afficheCase(Case c) {
	SDL_Rect rect;
	rect.x = c.x * TAILLE_CASE;
	rect.y = c.y * TAILLE_CASE;
	rect.w = rect.h = TAILLE_CASE - 1;
	SDL_SetRenderDrawColor(rendu, c.R, c.G, c.B, 255);
	SDL_RenderFillRect(rendu, &rect);
}

void Afficher_Grille(SDL_Renderer* rendu, Grille g) {
	for (int i = 0; i < nbCase; i++) {
		for (int j = 0; j < nbCase; j++) {
			afficheCase(g[i][j]);
		}
	}
}



int main(int argn, char* argv[]) {//entête imposée
								  //ouverture de la SDL
	srand(time(NULL));
	Case c;
	Grille g;

	initialisationSDL(); 
	Grille_Edit(g);
	Afficher_Grille(rendu, g);
	fond_droite(rendu);
	bouton_terrain(rendu);
	bouton_nourriture(rendu);
	bouton_obstacle(rendu);
	bouton_save(rendu);
	

	bool continuer = true; //booléen fin de programme
	SDL_Event event;//gestion des évènements souris/clavier,
					//SDL_Event est de type struct
	while (continuer)
	{
		SDL_RenderPresent(rendu);
		SDL_WaitEvent(&event);//attente d’un évènement
		switch (event.type) //test du type d’évènement
		{
		case SDL_QUIT: //clic sur la croix de fermeture
						//on peut enlever SDL_Delay
			continuer = false;
			break;
			
			
		case SDL_MOUSEBUTTONDOWN://appui souris
			if (event.button.button == SDL_BUTTON_LEFT) {//si on clique bouton gauche
				int posXclic = event.button.x;
				int posYclic = event.button.y;
				//cout << "(" << posYclic << ";" << posXclic << ")" << endl;
				if (posXclic > 600 && posXclic<900){//clic dans la partie de droite
					if (posYclic > 200 && posYclic < 275) { //clic bouton terrain
						typePinceau = 0;
						cout << "terrain" << endl;
					}
					else if (posYclic > 300 && posYclic < 375) {//clic bouton nourriture
						typePinceau = 1;
						cout << "nourriture" << endl;
					}
					else if (posYclic > 400 && posYclic < 475) {//clic bouton obstacle
						typePinceau = 2;
						cout << "obstacle" << endl;
					}
					else if (posYclic > 500 && posYclic < 575) {//clic bouton save
						cout << "save" << endl;
					}
				}
				else {//clic dans la partie de la grille
					cout << "grille" << endl;
					int posCaseX = posXclic / TAILLE_CASE;
					int posCaseY = posYclic / TAILLE_CASE;
					cout << "(" << posCaseY << ";" << posCaseX << ")" << endl;
					if (typePinceau == 0) {
						g[posCaseY][posCaseX].typeCase = 0;
						g[posCaseY][posCaseX].R = 0;
						g[posCaseY][posCaseX].G = 128;
						g[posCaseY][posCaseX].B = 0;
					}
					else if (typePinceau == 1) {
						g[posCaseY][posCaseX].typeCase = 1;
						g[posCaseY][posCaseX].R = 255;
						g[posCaseY][posCaseX].G = 255;
						g[posCaseY][posCaseX].B = 0;
						g[posCaseY][posCaseX].quantiteNourriture = (rand() % 10) + 1;
					
					}
					else if (typePinceau == 2) {
						g[posCaseY][posCaseX].typeCase = 2;
						g[posCaseY][posCaseX].R = 0;
						g[posCaseY][posCaseX].G = 0;
						g[posCaseY][posCaseX].B = 0;
					}
					
					afficheCase(g[posCaseY][posCaseX]);

				}
			}
			break;
				

			
		}
	}
	
	//fermeture de la SDL
	destructionSDL();
	return 0;
}