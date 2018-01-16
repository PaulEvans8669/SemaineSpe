#include"fonctionsBD.h"

using namespace std;

Liste* liste=NULL;
Liste* current=NULL;

bool bd_erreur(int rc , char * zErrMsg , const char * query ) {
	if( rc != SQLITE_OK ){
		sqlite3_free ( zErrMsg );
		return true;
	}
	return false;
}

int bd_tailleListe(Liste * l){
	int n=0;	
	while (l!= NULL ) {
		l = l-> next ;
		n++;
	} 
	return n;
}

void bd_detruireListe(Liste*&l){
	if(l!=NULL){
		Liste*p;
		Liste*q;
		int i;
		p=l;
		while(p!=NULL){
			q=p->next;
			for(i=0;i<p->argc;i++)
				delete[]p->argv[i];
			delete[]p->argv;
			delete p;
			p=q;
		}
		l=NULL;
	}
}

void bd_addElement(int argc , char ** argv) {
	Liste * n;
	int i;
	n = new Liste;
	n-> argc = argc ;
	n-> argv = new char*[argc];
	for (i=0;i< argc ;i++) {
		if ( argv [i]!= NULL ){
			n-> argv [i] = new char[strlen(argv[i])+1];
			strcpy_s(n-> argv[i],strlen(argv[i])+1,argv[i]);
		}else
			n-> argv [i] = NULL ;
	}
	n-> next = NULL ;
	if (liste== NULL ) {
		liste = n;
		current=liste;
		return ;
	}else{
		current->next=n;
		current=n;
	}
}

sqlite3* bd_ouvrir(char* nomBD){//ouvre une base
	int rc;
	sqlite3 * db;
	rc = sqlite3_open (nomBD , &db );
	if (rc) {
		cout<<"Erreur a l'ouverture de la base de donnees : "<<sqlite3_errmsg (db)<<endl;
		sqlite3_close (db);
		exit (1);
	}
	else
		return db;
}

void bd_fermer(sqlite3* bd){//ferme la bd
	if(bd!=NULL)
		sqlite3_close (bd);
}

void bd_requeteSelect(sqlite3* bd, const char* requete, char*** &tabResultat, int &nbLignes, int &nbColonnes){
	char* zErrMsg=0;
	int rc = sqlite3_exec (bd , requete , bd_completeResult , NULL , & zErrMsg );
	if(rc!=SQLITE_OK){
		tabResultat=NULL;
		nbLignes=nbColonnes=0;
	}
	bd_erreur (rc , zErrMsg , requete );
	nbLignes=bd_tailleListe(liste);
	if(nbLignes>0){
		tabResultat=new char**[nbLignes];
		int i=0,j;
		Liste*L=liste;
		while(L!=NULL){
			nbColonnes=L->argc;
			tabResultat[i]=new char*[L->argc];
			for(j=0;j<L->argc;j++){
				if(L->argv[j]!=NULL){
					tabResultat[i][j]=new char[strlen(L->argv[j])+1];
					strcpy_s(tabResultat[i][j],strlen(L->argv[j])+1,L->argv[j]);
				}else{
					tabResultat[i][j]=NULL;
				}
			}
			i++;
			L=L->next;
		}
		bd_detruireListe(liste);
	}else{
		tabResultat=NULL;
		nbLignes=nbColonnes=0;
	}
}

bool bd_requeteUpdate(sqlite3* bd, const char* requete){
	char* zErrMsg=0;
	int rc = sqlite3_exec (bd , requete , bd_doNothing , NULL , & zErrMsg );
	return (!bd_erreur(rc , zErrMsg , requete ));
}

void bd_detruireTab(char***&tab, int&nbLignes, int&nbColonnes){
	int i,j;
	for(i=0;i<nbLignes;i++){
		for(j=0;j<nbColonnes;j++){
				delete[]tab[i][j];
		}
		delete[]tab[i];
	}
	if(tab!=NULL)
		delete[]tab;
	tab=NULL;
	nbLignes=nbColonnes=0;
}