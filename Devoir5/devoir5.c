#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


////////////déclaration des fonctions//////////////////////////////////////////////////////////////////////

void findCombinaisons(int* liste, int** combinaisons, int taille, int elements);
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
    int* outputMatrix;


    if(argc == 2)
    {
        printf("fichier lu = %s\n",argv[1]);
        pathDistanceMatrix = readFile(argv[1], &n_elements);
        datasize=sizeof(int)*n_elements*n_elements;
        outputMatrix = (int *) malloc(datasize);
    }
    else
    {
        datasize = sizeof(int)*n_elements*n_elements;
        generateRandomMatrice(pathDistanceMatrix, n_elements);
    }

    afficherMatrice(pathDistanceMatrix, n_elements);


    //déclaration des variables
    int* liste = NULL;
    int** combinaisons = NULL;
    int i = 0;
    int j = 0;
    int n = n_elements;
    int nbreComb = 0;
    int tailleListe = n;

    //allocation des pointeurs
    liste = (int*)malloc(sizeof(int)*tailleListe);
    for (i = 0 ; i < tailleListe ; i++) 
    {
        liste[i] = i;
    } 
    nbreComb = pow(tailleListe, n);
    combinaisons = (int**)malloc(sizeof(int*)*nbreComb);
    for (i = 0 ; i < nbreComb ; i++) 
    {
        combinaisons[i] = (int*)malloc(sizeof(int)*n);
        for (j = 0 ; j < n ; j++)
        {
            combinaisons[i][j] = j;
        }
    }


    printf("\n--------\n");

    //algo

    findCombinaisons(liste, combinaisons, tailleListe, n); 

    afficherCombinaisons(combinaisons, tailleListe, n);

    printf("\n--------\n");

    afficherOneCombinaison(combinaisons[5],n_elements);
    printf("%d\n",testOneCycle(combinaisons[5], pathDistanceMatrix, n_elements));

    //libération des pointeurs
    for(i = 0 ; i < nbreComb ; i++) 
        free(combinaisons[i]);

    free(combinaisons);
    free(liste);
    return 0;
}	

////////////développement des fonctions//////////////////////////////////////////////////////////////////////


void findCombinaisons(int* liste, int** combinaisons, int taille, int elements) 
{
    int i = 0;
    int j = 0;
    int nbreComb = pow(taille, elements);
    int indiceActuel = 0;

    for (j = 0 ; j < elements ; j++) 
    {
        indiceActuel = 0;
        for (i = 0 ; i < nbreComb ; i++) 
        {
            combinaisons[i][elements - j - 1] = liste[indiceActuel];
            if ((i+1) % (int)pow(taille, j) == 0)
                indiceActuel++;
            if (indiceActuel >= taille)
                indiceActuel = 0;
        }
    }
}

void afficherCombinaisons(int** combinaisons, int taille, int elements)
{
    int nbreComb = pow(taille, elements);
    int i, j;
    for (i = 0 ; i < nbreComb ; i++) 
    {
        printf("%i :\t", i+1);
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
        printf("temp[%d]=%d\n",indice,pathDistanceMatrix[indice]);
        somme+=pathDistanceMatrix[indice];
    }

    return somme;
}

void afficherOneCombinaison(int* combinaison, int taille)
{
    printf("combinaison=\t");
    for(int j=0; j<taille;j++)
    {
        printf(" %d", combinaison[j]);
    }
    printf("\n");
}