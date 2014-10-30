
///////////////////////////  8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #2 ///////////////////////////////////
///////////////////////////// d2s.c - Corentin RAOULT  - Adrien Cambillau  /////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


//////////////////////  déclaration des fonctions  /////////////////////////

int* remplirTAB(int n);
void afficherTAB(int* TAB, int n);
void triFusion(int * TAB, int n);
void fusion(int * U, int taille_U, int * V, int taille_V , int * T);
int choixMode();
void fonctionTriFusion(int n);

/////////////////////  MAIN  ////////////////////////////////////////////////

int main(int argc, char* argv[])
{

	//printf ("Nombre d'argument : %d\n", argc);
	if(argc == 2)
	{
		char* buffer = argv[1];
		int num = atoi(buffer); 
		fonctionTriFusion(num);
	}
	else
	{
		int n=2;
		printf("entrez un entier n (il sera multiplié 2)\n");
		scanf("%d",&n);
		fonctionTriFusion(n);
	}

	return EXIT_SUCCESS;
}


///////////////////  développement des fonctions  /////////////////////////////////

void fonctionTriFusion(int n)
{
	int * T = remplirTAB(n);
	//afficherTAB(T,2*n);

	double fin;
	double debut = omp_get_wtime();//--> encore un pb, mesure le temps sur un thread

	triFusion(T, 2*n);

	fin = omp_get_wtime();

	//afficherTAB(T, 2*n);
	//printf("n = %d\tdurée = %lf \n", 2*n,fin -debut);
	printf("\t %lf \n",fin -debut);
	free(T);

}

int* remplirTAB(int n)
{
	int i;
	int* T = malloc(2*n*sizeof(int)); 
	int k=0, l=n;
	for(i=0;i<2*n;i++)
	{
		if(i%2 == 0)
		{
			T[k]=i;
			k++;
		}
		else
		{
			T[l]=i;
			l++;
		}
	}

	return T;
}

void afficherTAB(int* TAB, int n)
{
	int j;
	printf("TAB : { ");

	for(j = 0; j < n; j++)
	{
		printf(" [%d] ",TAB[j]);
	}		

	printf(" }\n");	
}

void triFusion(int * TAB, int n)
{

	int i;
	int * U = malloc((n/2)*sizeof(int));
	int * V = malloc((n/2)*sizeof(int));
	for(i=0; i<n/2;i++)
	{
		U[i]=TAB[i];
		V[i]=TAB[i+(n/2)];
	}

	if(n>=2)//si n==1 pas besoin de trier les tableaux
	{
		triFusion(U,n/2);
		triFusion(V,n/2);
	}
	fusion(U,n/2,V,n/2,TAB);

	free(U);
	free(V);

}

void fusion(int * U, int taille_U, int * V, int taille_V , int * T)
{
	int i=0,j=0;
	int k;
	for(k=0; k<(taille_U+taille_V);k++)
	{
		if (U[i]<V[j] && i<taille_U)
			T[k]=U[i++];
		else
			T[k]=V[j++];
	}
}
