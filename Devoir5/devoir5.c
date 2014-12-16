
/////////////////////////////// 8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #5 ////////////////////////////////////////
//////////////////////////////// recherche cycle hamitonien - Corentin RAOULT //////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <omp.h>
#include <mpi.h>

//cstes couleurs
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_STYLE_BOLD    "\033[1m"


////////////déclaration des fonctions//////////////////////////////////////////////////////////////////////

long int factorielle(int n);

int** findCombinaisons(int taille, int elements) ;
void afficherCombinaisons(int** combinaisons, int taille, int elements);

void afficherMatrice(int * mat, int taille);
int* readFile(char* pathFile, int* n_elements);
void generateRandomMatrice(int * pathDistanceMatrix, int n_elements);

int testOneCycle(int * combinaison, int * pathDistanceMatrix, int n);
void afficherOneCombinaison(int* combinaison, int taille);

int min(int * tab, int taille, int* indice);


////////////Main/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[])
{

    //Start up MPI...
    int rank,p;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p); 
    printf("\n");

    time_t t1, t2;
 
   t1 = time(NULL);
   if (t1 == (time_t)-1)
   {
      perror("time");
      exit(EXIT_FAILURE);
   }
  // (void)printf("début %ld secondes, %s\n", t1, ctime(&t1));

    size_t datasize;
    int n_elements=10;
    int* pathDistanceMatrix;
    int nbreCombNonRedondantes;
    int nbreCombMax= pow(n_elements, n_elements);
    int** combinaisons = NULL;
    int* tabIndicesMin= malloc(p*sizeof(int));
    int* tabMin= malloc(p*sizeof(int));


    if (rank==0)
    {
        if(argc == 2)
        {
            printf("fichier lu = %s\n",argv[1]);
            pathDistanceMatrix = malloc(sizeof(int)*n_elements*n_elements);
            pathDistanceMatrix = readFile(argv[1], &n_elements);
            datasize=sizeof(int)*n_elements*n_elements;
        }
        else
        {
            datasize = sizeof(int)*n_elements*n_elements;
            generateRandomMatrice(pathDistanceMatrix, n_elements);
        }

        printf(ANSI_COLOR_YELLOW);
        afficherMatrice(pathDistanceMatrix, n_elements);
        printf(ANSI_COLOR_RESET);
        nbreCombNonRedondantes = factorielle(n_elements);

        combinaisons = (int**)malloc(sizeof(int*)*nbreCombNonRedondantes);
        for (int i = 0 ; i < nbreCombNonRedondantes ; i++) 
            combinaisons[i] = (int*)malloc(sizeof(int)*n_elements);

        //algo
        printf("nbre combinaisons Non Redondantes = %d (%d!)\n", factorielle(n_elements), n_elements);

        combinaisons = findCombinaisons(n_elements, n_elements); 
    }

    MPI_Bcast( &n_elements, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &nbreCombNonRedondantes, 1, MPI_INT, 0, MPI_COMM_WORLD );
    //MPI_Barrier(MPI_COMM_WORLD);

    if(rank != 0)
    {
        combinaisons = (int**)malloc(sizeof(int*)*nbreCombNonRedondantes);
        for (int i = 0 ; i < nbreCombNonRedondantes ; i++)  
            combinaisons[i] = (int*)malloc(sizeof(int)*n_elements);

        pathDistanceMatrix = malloc(sizeof(int)*n_elements*n_elements);
    }

    MPI_Bcast( pathDistanceMatrix, n_elements*n_elements, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0 ; i < nbreCombNonRedondantes ; i++)  
        MPI_Bcast( combinaisons[i], n_elements, MPI_INT, 0, MPI_COMM_WORLD);

    //  afficherCombinaisons(combinaisons,nbreCombNonRedondantes, n_elements);
    //  afficherMatrice(pathDistanceMatrix, n_elements);
    //  printf("[%d] tnbreCombNonRedondantes=%d n_elements=%d \n",rank, nbreCombNonRedondantes, n_elements);

   // printf("\n--------\n");

    int tailleBlock=nbreCombNonRedondantes/p;
    int debut = (nbreCombNonRedondantes/p)*(rank-1);

    if(nbreCombNonRedondantes%p != 0 && rank ==0)
    {
        tailleBlock += nbreCombNonRedondantes%p;
    }
    if(rank==0)
    {
        debut = (nbreCombNonRedondantes/p)*(p-1);
    }

    int fin = debut + tailleBlock;

    int resultatsCycles[nbreCombNonRedondantes];

    #pragma omp parallel for
    for(int i= debut; i< fin; i++)
    {
       // afficherOneCombinaison(combinaisons[i], n_elements);
      //  printf("i=%d\n",i );
        resultatsCycles[i]=testOneCycle(combinaisons[i], pathDistanceMatrix, n_elements);
    }

    int indiceMinNoeud;
    tabMin[rank]=min(resultatsCycles+debut,tailleBlock, &indiceMinNoeud);
    tabIndicesMin[rank] = indiceMinNoeud+debut;
    

    printf(ANSI_COLOR_CYAN);
    printf("proc[%d] => cycle minimum: chemin n°%d taille=> %d\n", rank, tabIndicesMin[rank], tabMin[rank]);
    afficherOneCombinaison(combinaisons[tabIndicesMin[rank]], n_elements);
    printf("\n");
    printf(ANSI_COLOR_RESET);


   // MPI_Barrier(MPI_COMM_WORLD);

    for(int o=0; o<p; o++)         
    {
        MPI_Bcast( &tabIndicesMin[o], 1, MPI_INT, o, MPI_COMM_WORLD );
        MPI_Bcast( &tabMin[o], 1, MPI_INT, o, MPI_COMM_WORLD );
    }

   // for(int j=0; j<p; j++)         
   // {
    //    printf("------------->tabIndicesMin[%d]=%d\n",j, tabIndicesMin[j];  );
    //}

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank==0)
    {

        int indiceMinGlobal;
        int minGlobal =  min(tabMin,p, &indiceMinGlobal);
        printf(ANSI_COLOR_GREEN);
        printf(ANSI_STYLE_BOLD);
        printf("cycle minimum global: chemin n°%d taille=> %d\n", tabIndicesMin[indiceMinGlobal], minGlobal);
        afficherOneCombinaison(combinaisons[tabIndicesMin[indiceMinGlobal]], n_elements);
        printf(ANSI_COLOR_RESET);
        t2 = time(NULL);
        if (t2 == (time_t)-1)
        {
            perror("time");
             exit(EXIT_FAILURE);
        }
        printf("temps d'execution = %ld secondes\n(on ne peut pas être plus précis que la seconde avec la fonction `time()`)\n", t2-t1);
        
    }


    //libération des pointeurs
    for(int i = 0 ; i < nbreCombNonRedondantes ; i++) 
        free(combinaisons[i]);
    free(combinaisons);
    
    //Shut down...
    MPI_Finalize();




    return 0;
}	

////////////développement des fonctions//////////////////////////////////////////////////////////////////////

long int factorielle(int n)
{
    int i=0;
    long int temp=n;
    while(i<=n-2)
    {
        i=i+1;
        temp=temp*(n-i);
    }
    return temp;
}

int** findCombinaisons(int taille, int elements) 
{
    //déclaration des variables

    int i = 0;
    int j = 0;
    int nbreComb = pow(taille, elements);
    int indiceActuel = 0;
    int nbreCombNonRedondantes;

    //allocation des pointeurs 

    nbreComb = pow(taille, elements);
    int** allCombinaisons = (int**)malloc(sizeof(int*)*nbreComb);
    for (i = 0 ; i < nbreComb ; i++) 
    {
        allCombinaisons[i] = (int*)malloc(sizeof(int)*elements);
        for (j = 0 ; j < elements ; j++)
        {
            allCombinaisons[i][j] = j;
        }
    }


    int* liste = (int*)malloc(sizeof(int)*taille);
    for (i = 0 ; i < taille; i++) 
    {
        liste[i] = i;
    } 

    for (j = 0 ; j < elements ; j++) 
    {
        indiceActuel = 0;
        for (i = 0 ; i < nbreComb ; i++) 
        {
            allCombinaisons[i][elements - j - 1] = liste[indiceActuel];
            if ((i+1) % (int)pow(taille, j) == 0)
                indiceActuel++;
            if (indiceActuel >= taille)
                indiceActuel = 0;
        }
    }

    int** combinaisons = (int**)malloc(sizeof(int*)*nbreComb);
    for (i = 0 ; i < nbreComb ; i++) 
    {
        combinaisons[i] = (int*)malloc(sizeof(int)*elements);
        for (j = 0 ; j < elements+1 ; j++)
        {
            combinaisons[i][j] = j;
        }
    }

    // afficherCombinaisons(allCombinaisons, taille, elements);

    //vérification

    int tabVerif[elements];
    nbreCombNonRedondantes=0;
    int cpt=0;
    for (i = 0 ; i < nbreComb ; i++) 
    {

        for (j = 0 ; j < elements ; j++) 
            tabVerif[j]=0;             

        for (j = 0 ; j < elements ; j++) 
            tabVerif[allCombinaisons[i][j]]=1;


        cpt=0;
        for (int jj = 0 ; jj< elements ; jj++) 
            if(tabVerif[jj]==0)
                cpt++;                

            if(cpt==0)
            {
                combinaisons[(nbreCombNonRedondantes)]=allCombinaisons[i];
                (nbreCombNonRedondantes)++;
            }        
        }

        for(int i = 0 ; i < elements ; i++) 
            free(allCombinaisons[i]);
        free(allCombinaisons);
        free(liste);

        return combinaisons;
    }

    void afficherCombinaisons(int** combinaisons, int taille, int elements)
    {

     int i, j;
     for (i = 0 ; i < taille ; i++) 
     {
        printf("%i :\t", i);
        for (j = 0 ; j < elements; j++) 
          printf("%i\t", combinaisons[i][j]);

      printf("\n");
  }
}


int* readFile(char* pathFile, int* n_elements)
{
    FILE* fichier = NULL;
    int* matrix=NULL;
    if(pathFile!=NULL)
        fichier = fopen(pathFile, "r");

    if (fichier != NULL)
    {
        fscanf(fichier, "%d ", n_elements);
        printf("tailleGraphe : %d \n", *n_elements);
        matrix = (int *) malloc(sizeof(int)*(*n_elements)*(*n_elements));
        int j=0, indice=0;
        for(int i=0;i<(*n_elements)*(*n_elements);i++)
        {
            indice=i/(*n_elements)+(i%(*n_elements))*(*n_elements);
            fscanf(fichier, "%d ", &matrix[indice]);
        }
    }
    else
    {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier graphe");
    }
    return matrix;
}

void afficherMatrice(int * mat, int taille)
{
    if(taille<40)
    {
        for(int i=0;i<taille;i++)
        {
            for(int j=0;j<taille;j++)
                printf("%d ", mat[i+taille*j]);

            printf("\n");
        }
    }
    else
        printf("matrice trop grande pour être affichée\n");
}

void generateRandomMatrice(int * pathDistanceMatrix, int n_elements)
{
    // Compute the size of the data    
    pathDistanceMatrix = (int *) malloc(sizeof(int)*n_elements*n_elements);
    // random initialisation of input
    srand((int)time(NULL));
    for(int i=0;i<n_elements;i++)
    {
        for(int j=0;j<n_elements;j++)
        {
            if(i==j)
                pathDistanceMatrix[i+n_elements*j] = 0;
            else
                pathDistanceMatrix[i+n_elements*j] = rand()%20;
        }
    }
}

int testOneCycle(int * combinaison, int * pathDistanceMatrix, int n)
{
    int somme=0, indice;
    for(int j=0; j<n-1;j++)
    {
        indice = combinaison[j]*n+combinaison[j+1];
        somme+=pathDistanceMatrix[indice];
    }

    return somme;
}

void afficherOneCombinaison(int* combinaison, int taille)
{
    printf("chemin = ");
    for(int j=0; j<taille;j++)
        printf(" %d", combinaison[j]);

    printf("\n");
}

int min(int * tab, int taille, int* indice)
{
    //recherche du min
    int min = INT_MAX;
    int* indiceMin=malloc(sizeof(int));
    for(int i= 0; i< taille; i++)
    {
       // printf("tab[%d]=%d\n",i, tab[i] );
        if(tab[i]<=min)
        {
            min=tab[i];
            *indiceMin=i;
        }
    }
    //printf("indice mmin = %d\n",*indiceMin );
    *indice=*indiceMin;
    return min;
}