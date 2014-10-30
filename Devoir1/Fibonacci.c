	
/////////////////////////////  8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #1 ///////////////////////////
///////////////////////////// Fibonacci - Corentin RAOULT  - Adrien Cambillau  /////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <time.h>//pour la fonction rand()
#include <pthread.h>//pour les threads


//////////////////////  déclaration des fonctions  /////////////////////////

int choixMode();
int demandeNombre();
void remplirTABrand(int n, int* TAB);
void afficherTAB(int n, int* TAB);
unsigned long long int fib(int nombre);
void * calculPartTAB(void * arg);
float threadsCalcul(int n, int k);

//permet d'envoyer au thread une partie de TAB 
//et la taille de cette partie
struct threadParams 
{
	int * tab;
	int amplitude;
};


/////////////////////  MAIN  ////////////////////////////////////////////////

int main()
{
	printf("Fibonacci\n");

	int choix = choixMode();
	if(choix==1)
	{	
		printf("mode auto\n");
		int k_max = 64;
		int n_max = 100000000;
		int n, k;

		for(n=10;n<=n_max;n*=10)
		{
			printf("---------------taille du TAB = %d---------------\n",n);
			for(k=1024;k>=1;k/=2)
			{
				//printf("taille tab = %d, nb threads = %d, temps de traitement = %f\n\n", n, k, threadsCalcul(n,k));	
				printf(" %d \t %d \t %f\n\n", n, k, threadsCalcul(n,k));
			}	
		}		
	}
	else if(choix==0)
	{
		printf("mode manuel\n");
		printf("Entrez la taille du tableau n: ");
		int n=demandeNombre();

		printf("Entrez le nombre de threads k: ");
		int k=demandeNombre();

		threadsCalcul(n,k);
	}
	else
		printf("problème dans la fonction choixMode()\n");

	return EXIT_SUCCESS;
}


///////////////////  développement des fonctions  /////////////////////////////////

int choixMode()
{
	printf("voulez-vous lancer le programme en mode auto? [o/n] ");
	char C;
	C=getchar();
	if(C=='o'||C=='O'||C=='y'||C=='Y')
		return 1;
	else if (C=='n'||C=='N')
		return 0;
	else
		return choixMode();
}

int demandeNombre()
{
	int i; char buf[128] = {0};
	//tant que l'entrée n'est pas correcte on la redemande
	while(scanf("%d", &i) != 1)
	{
		scanf("%s", &buf);
		printf("Désolé, [%s] n'est pas un nombre, veuillez taper une valeur correcte : ", &buf);
	}
	return i;
}

void remplirTABrand(int n, int* TAB)
{
	int i;
	srand(time(NULL));
	for(i=0;i<n;i++)
		TAB[i] = rand()%47; //limité par unsigned long long int
}

void afficherTAB(int n, int* TAB)
{
	int i,j,k;
	printf("TAB :");

	//si l long longueure est inférieure à 10 on affiche tout le tableau
	if(n<=10)
	{
		for(k=0; k<n; k++)
			printf(" %d ",TAB[k]);
	}
	//sinon on affiche les 5 premiers et derniers
	else
	{
		for(i =0; i<5; i++)
		{
			printf(" %d ",TAB[i]);
		}
		printf(" ... ");
		for(j =n-5; j<n; j++)
		{
			printf(" %d ",TAB[j]);
		}		
	}
	printf("\n");	
}

unsigned long long int fib(int nombre)
{
	//FIXME : on est limité à 4 294 967 295 (0xFFFFFFFF) (unsigned long)
	//alors que normalement avec un unsigned long long int 
	//on devrait pouvoir aller jusu'à 18 446 744 073 709 551 615 (0xFFFFFFFFFFFFFFF)
	//on est donc limité à 48 car fib(48) > 4 294 967 295
	unsigned long long int  premier = 0, second = 1, resultat, c;

	for (c = 0; c < nombre; c++)
	{
		if ( c < 1 )
			resultat = c;
		else
		{
			resultat = premier + second;
			premier = second;
			second = resultat;
		}
	}
	return resultat;
} 

void * calculPartTAB(void * arg)
{
	struct threadParams* param = (struct threadParams*)arg;
	
	int i;
	for(i=0;i<param->amplitude;i++)
		param->tab[i]=fib(param->tab[i]);

	pthread_exit(NULL);
}

float threadsCalcul(int n, int k)
{
	//Allocation de n fois la taille d'un "int"
	int* TAB = malloc(n*sizeof(int)); 
	remplirTABrand(n, TAB);

	//affiche les 5 premiers et 5 deniers chiffres du tableau initial
	afficherTAB(n, TAB);

	//	clock_t debut_t, fin_t, ecart_t;
	///	debut_t = clock();

	struct timespec begin, end;
	double elapsed;



	//Allocation d'un tableau de k threads
	pthread_t* tab_threads = malloc(k*sizeof(pthread_t)) ;

	int  i, amplitude = n/k, origine = 0;

	//Allocation de k struct et de k tableaux d'entiers
	struct threadParams*  threadParamsTab = malloc(k*sizeof(struct threadParams)); 

	clock_gettime(CLOCK_MONOTONIC, &begin);

	for(i=0; i<k; i++)
	{
		//printf("lancement de k=%d\n", i);
		threadParamsTab[i].amplitude = amplitude;//utiliser fonction qui cherche longuer du tab plutot que crer struct

		//copie d'un bout de TAB dans un plus petit tableau qui dera envoyé au thread
		threadParamsTab[i].tab = TAB+origine;
		pthread_create(&tab_threads[i], NULL, &calculPartTAB,&threadParamsTab[i]);
		origine += amplitude;
		if(n%k != 0 && i==k-2)
		{
			amplitude += n%k;
			
		}		
	}

	//attend tous les threads
	for(i=0; i<k; i++)
	{
		pthread_join(tab_threads[i], NULL);
		//printf("récupération de k=%d\n", i);
	}


	//fin_t = clock();//mesure le temps d'arrivée
	//printf("fin du traitement: %d\n",fin_t);

	//réaffiche le tableaux (5 premiers et derniers)
	afficherTAB(n, TAB);
	
	//ecart_t = (fin_t-debut_t);
	//printf("temps de traitement: %f\n", ((float)ecart_t)/CLOCKS_PER_SEC) ;


	//clock() ne fonctionne pas avec plusieurs threads
	clock_gettime(CLOCK_MONOTONIC, &end);

	elapsed = end.tv_sec - begin.tv_sec;
	elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

	//libère les tableaux
	free(TAB);
	free(threadParamsTab);
	free(tab_threads);	

	return (float)elapsed;
}