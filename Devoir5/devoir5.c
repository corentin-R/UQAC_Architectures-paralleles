#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>


////////////déclaration des fonctions//////////////////////////////////////////////////////////////////////

long int factorielle(int n);

int** findCombinaisons(int taille, int elements) ;
void afficherCombinaisons(int** combinaisons, int taille, int elements);

void afficherMatrice(int * mat, int taille);
int* readFile(char* pathFile, int* n_elements);
void generateRandomMatrice(int * pathDistanceMatrix, int n_elements);

int testOneCycle(int * combinaison, int * pathDistanceMatrix, int n);
void afficherOneCombinaison(int* combinaison, int taille);


////////////Main/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[])
{
    size_t datasize;
    int n_elements=10;
    int* pathDistanceMatrix;
    int nbreCombNonRedondantes = factorielle(n_elements);


    if(argc == 2)
    {
        printf("fichier lu = %s\n",argv[1]);
        pathDistanceMatrix = readFile(argv[1], &n_elements);
        datasize=sizeof(int)*n_elements*n_elements;
    }
    else
    {
        datasize = sizeof(int)*n_elements*n_elements;
        generateRandomMatrice(pathDistanceMatrix, n_elements);
    }

    afficherMatrice(pathDistanceMatrix, n_elements);


    int tailleListe = n_elements;
    int nbreComb = pow(tailleListe, n_elements);


    int** combinaisons = (int**)malloc(sizeof(int*)*nbreComb);
    for (int i = 0 ; i < nbreComb ; i++) 
    {
        combinaisons[i] = (int*)malloc(sizeof(int)*n_elements);
        for (int j = 0 ; j < n_elements ; j++)
        {
            combinaisons[i][j] = j;
        }
    }


    printf("\n--------\n");

    //algo
    printf("nbreCombNonRedondantes %d\n", factorielle(n_elements));

    combinaisons = findCombinaisons(tailleListe, n_elements); 

    nbreCombNonRedondantes = factorielle(n_elements);

    //afficherCombinaisons(combinaisons, nbreCombNonRedondantes, n_elements);


    printf("\n--------\n");

    int resultatsCycles[nbreCombNonRedondantes];
    for(int i= 0; i< nbreCombNonRedondantes; i++)
    {
      //  afficherOneCombinaison(combinaisons[i],n_elements);
        resultatsCycles[i]=testOneCycle(combinaisons[i], pathDistanceMatrix, n_elements);
       // printf("%d\n",resultatsCycles[i]);
    }

    //recherche du min
    int min = INT_MAX;
    int indiceMin=0;
    for(int i= 0; i< nbreCombNonRedondantes; i++)
    {
        if(resultatsCycles[i]<=min)
        {
            min=resultatsCycles[i];
            indiceMin=i;
        }
    }
    printf("cycle minimum:\n");
    afficherOneCombinaison(combinaisons[indiceMin], n_elements);
    printf("taille minimale %d\n", min);




    //libération des pointeurs
    for(int i = 0 ; i < n_elements ; i++) 
        free(combinaisons[i]);
    free(combinaisons);
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


       // printf("tab verif %d =", i);
        cpt=0;
        for (int jj = 0 ; jj< elements ; jj++) 
        {
            //printf(" tab[%d]=%d", allCombinaisons[i][jj], tabVerif[allCombinaisons[i][jj]]);
          //  printf(" %d", tabVerif[jj]);
            if(tabVerif[jj]==0)
            {
                cpt++;                
            }
        }
       //printf(" \n\n");
        if(cpt==0){

            combinaisons[(nbreCombNonRedondantes)]=allCombinaisons[i];
           // combinaisons[i][0]=0;
          /*  printf("%d =", i);
            for (int k = 0 ; k < elements ; k++) 
            {
                printf(" %d", combinaisons[i][k]);
            }
            printf("\n");
            */
            (nbreCombNonRedondantes)++;
        }
        
    }
    return combinaisons;
}

void afficherCombinaisons(int** combinaisons, int taille, int elements)
{
   //int nbreComb = pow(taille, elements);
    int i, j;
    for (i = 0 ; i < taille ; i++) 
    {
        printf("%i :\t", i);
        for (j = 0 ; j < elements; j++) 
        {
            printf("%i\t", combinaisons[i][j]);
        }
        printf("\n");
    }

}


int* readFile(char* pathFile, int* n_elements)
{
    FILE* fichier = NULL;
    int* matrix=NULL;
    if(pathFile!=NULL)
        fichier = fopen(pathFile, "r");
        //printf("tail\n");
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
            //printf("c[%d]= %d \n",indice, matrix[indice]);
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
            {
                printf("%d ", mat[i+taille*j]);
            }
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
        //printf("temp[%d]=%d\n",indice,pathDistanceMatrix[indice]);
        somme+=pathDistanceMatrix[indice];
    }

    return somme;
}

void afficherOneCombinaison(int* combinaison, int taille)
{
    printf("combinaison= ");
    for(int j=0; j<taille;j++)
    {
        printf(" %d", combinaison[j]);
    }
    printf("\n");
}