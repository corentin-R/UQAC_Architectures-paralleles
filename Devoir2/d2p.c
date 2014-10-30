
///////////////////////////  8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #2 ///////////////////////////////////
///////////////////////////// d2p.c - Corentin RAOULT  - Adrien Cambillau  /////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


//////////////////////  déclaration des fonctions  /////////////////////////

int* remplirTAB(int n);
void afficherTAB(int* TAB, int n);
void triFusionParallele(int * TAB, int n);
void fusion(int * U, int taille_U, int * V, int taille_V , int * T);
void fonctionTriFusionParallele(int n);


/////////////////////  MAIN  ////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	//printf("Tri fusion parallèle\n");
	if(argc == 2)
	{
		char* buffer = argv[1];
		int num = atoi(buffer); 
		fonctionTriFusionParallele(num);
	}
	else
	{
		int n=2;
		printf("entrez un entier n (il sera multiplié 2)\n");
		scanf("%d",&n);
		fonctionTriFusionParallele(n);
	}

	return EXIT_SUCCESS;
}


///////////////////  développement des fonctions  /////////////////////////////////



void fonctionTriFusionParallele(int n)
{
	int * T = remplirTAB(n);
		//afficherTAB(T,2*n);


	double fin;
	double debut = omp_get_wtime();//--> encore un pb, mesure le temps sur un thread

	#pragma omp parallel
	{
		triFusionParallele(T, 2*n);
	}


	fin = omp_get_wtime();

	//afficherTAB(T, 2*n);
	//printf("n = %d  \tdurée = %lf \n", 2*n,fin -debut);
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

void triFusionParallele(int * TAB, int n)
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
     	#pragma omp single nowait
		{
         	   #pragma omp task
			triFusionParallele(U,n/2);
	           	#pragma omp task
			triFusionParallele(V,n/2);
				#pragma omp taskwait
			fusion(U,n/2,V,n/2,TAB);
		}
	}	
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
