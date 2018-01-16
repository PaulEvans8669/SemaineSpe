#ifndef FONCTIONSBD_H
#define FONCTIONSBD_H

#include "sqlite3.h"
#include<iostream>
using namespace std;

typedef struct liste {
	int argc ;
	char ** argv ;
	struct liste * next ;
} Liste ;


void bd_addElement(int argc , char ** argv);
static int bd_doNothing(void * notused , int argc , char ** argv , char ** azColName){return 0;}
static int bd_completeResult( void* liste , int argc , char ** argv , char ** azColName ){bd_addElement (argc ,argv);return 0;}
bool bd_erreur(int rc , char * zErrMsg , const char * query );
int bd_tailleListe(Liste * l);
void bd_detruireListe(Liste*&l);

sqlite3* bd_ouvrir(char* nomBD);//ouvre une base
void bd_fermer(sqlite3* bd);//ferme la bd

void bd_requeteSelect(sqlite3* bd, const char* requete, char*** &tabResultat, int &nbLignes, int &nbColonnes); //exécute une requete de type SELECT
bool bd_requeteUpdate(sqlite3* bd, const char* requete); //exécute une requete de type INSERT, CREATE,... (mise à jour)

void bd_detruireTab(char***&tab, int&nbLignes,int&nbColonnes);//détruit un tableau 2D de chaines

#endif