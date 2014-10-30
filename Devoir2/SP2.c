	
///////////////////////////  8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #2 ///////////////////////////////////
///////////////////////////// SP1.c - Corentin RAOULT  - Adrien Cambillau  /////////////////////////////////////

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


//////////////////////  déclaration des fonctions  /////////////////////////

int digit_to_int(char d);
void remplirTABrand(int* TAB, int n);
void afficherTAB(int* TAB, int n);
int* SP2(int* T, int n);


/////////////////////  MAIN  ////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	int n;
	n = 102400;
	int* S;

	int* T = malloc(n*sizeof(int)); 
	remplirTABrand(T,n);
	afficherTAB(T,n);

	double fin;
	double debut = omp_get_wtime();//--> encore un pb, mesure le temps sur un thread

	S = SP2(T,n);

	fin = omp_get_wtime();

	afficherTAB(S,n);
	printf("durée = %lf\n", fin - debut);

	return EXIT_SUCCESS;
}


///////////////////  développement des fonctions  /////////////////////////////////

void remplirTABrand(int* TAB, int n)
{
	int i;
	srand(time(NULL));
	for(i=0;i<n;i++)
		TAB[i] = rand()%10000; //limité par unsigned long long int
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

int* SP2(int* T, int n)
{
	int * S = malloc((n)*sizeof(int));
	int i;
	int j;
	int somme;

	#pragma omp parallel for
	for(i = 0; i < n ; i++){
		somme = 0;

		#pragma omp parallel for reduction(+:somme)
		for(j = 0; j < i; j++){
			somme += T[j];
			S[i] = somme;
		}
	}

	return S;
}

