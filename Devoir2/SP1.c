	
///////////////////////////  8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #2 ///////////////////////////////////
///////////////////////////// SP1.c - Corentin RAOULT  - Adrien Cambillau  /////////////////////////////////////

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


//////////////////////  déclaration des fonctions  /////////////////////////

int digit_to_int(char d);
void remplirTABrand(int* TAB, int n);
void afficherTAB(int* TAB, int n);
void SP1(int* T, int n);


/////////////////////  MAIN  ////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	int n;
	n = 1024;

	int* T = malloc(n*sizeof(int)); 
	remplirTABrand(T,n);
	afficherTAB(T,n);

	double fin;
	double debut = omp_get_wtime();//--> encore un pb, mesure le temps sur un thread

	SP1(T,n);

	fin = omp_get_wtime();

	afficherTAB(T,n);
	printf("durée = %lf\n", fin -debut);

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

void SP1(int* T, int n)
{
	int i;
	int* S = malloc((n/2)*sizeof(int));

	if (n==1) return;

	#pragma omp parallel for 
	for(i=0; i<= n/2 -1; i++)
	{
		S[i]=T[2*i] + T[2*i+1];
	}

	SP1(S, n/2);

	#pragma omp parallel for 
	for(i=0; i<= n/2 -1; i++)
	{
		T[2*i+1]=S[i];
		T[2*i+2]=S[i]+T[2*i+2];
	}
}

