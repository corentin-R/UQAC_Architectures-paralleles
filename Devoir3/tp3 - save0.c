
/////////////////////////////// 8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #3 ////////////////////////////////////////
///////////////////////////// tri tableau MPI - Corentin RAOULT - Adrien Cambillau /////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) \
(BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
(BLOCK_LOW((id)+1,p,n)-BLOCK_LOW(id,p,n))
#define BLOCK_OWNER(index,p,n) \
(((p)*(index)+1)-1)/(n))
struct tableau
{
    int * tab;
    int taille;
};
////////////////////// déclaration des fonctions ////////////////////////////////////////////////////////////////////
int demandeNombre();
void remplirTABrand( struct tableau T);
struct tableau TD_init(int n);
int TD_get(struct tableau T, int i, int *x);
int TD_put(struct tableau T, int i, int x);
int TD_somme(struct tableau T); // Retourne la somme des éléments de T .
void TD_afficher(struct tableau T,int i, int j); // Affiche dans l’ordre les éléments i (i ≤ j) du tableau T .
void afficherNomMachine();
int partition(int * a, int p, int r);
void quicksort(int * a, int p, int r);
///////////////////// MAIN ///////////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
    double time, max_time;

    ///////création et remplissage du tableau//////////////
    int n = atoi(argv[1]);
    struct tableau T = TD_init(n);
    struct tableau T_trie = TD_init(n);
    int pointeur=0;
    //////Initialisation de MPI////////////////////////////
    int id;
    int p;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    //afficherNomMachine();
    time = -MPI_Wtime();
    /********** Divide the array in equal-sized chunks **********/
    /*	struct tableau sub_array;
    sub_array.taille = BLOCK_SIZE (id,p,n);
    sub_array.tab = malloc(size * sizeof(int));*/
    int low_value = BLOCK_LOW(id,p,n);
    int high_value = BLOCK_HIGH(id,p,n);
    quicksort(T.tab, low_value, high_value);
    printf ("trié à moitié\n");
    TD_afficher(T, 0, n);
    int j,a;
    int plus_petit=1;
    for(j= 0; j<BLOCK_SIZE(id,p,n);j++)
    {
        for(a=0; a<p;a++)
        {
            if(a!=id)
            {
                if(T.tab[id+j]>T.tab[a+j])
                {
                    plus_petit=0;
                }
            }
        }
        if(plus_petit==1)
        {
            T_trie.tab[pointeur]=T.tab[id+j];
            pointeur++;
            printf ("pinteur incrémenté =%d\n",pointeur);
            MPI_Bcast(&pointeur, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else
            plus_petit=1;
    }
    time += MPI_Wtime();
    MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (!id)
        printf ("Floyd, matrix size %d, %d processes: %6.2f seconds\n",n, p, max_time);
    TD_afficher(T_trie, 0, n);
    /********** Finalize MPI **********/
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

/////////////////// développement des fonctions /////////////////////////////////////////////////////////////
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
void afficherNomMachine()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
        printf("%s\n", hostname);
    else
        fprintf(stderr, "La fonction gethostname a echoue.\n");
}
void remplirTABrand(struct tableau T)
{
    int i;
    srand(time(NULL));
    for(i=0;i<T.taille;i++)
T.tab[i] = rand()%T.taille; //limité par unsigned long long int
}
struct tableau TD_init(int n)
{
    struct tableau T;
    T.tab = malloc(n*sizeof(int));
    T.taille=n;
    remplirTABrand(T);
    printf("Tableau original\n");
    TD_afficher(T,0, T.taille);
    return T;
}
int TD_get(struct tableau T, int i, int *x)
{
    if(i < T.taille)
    {
        *x = *(T.tab + i);
        return 1;
    }
    else
    {
        return 0;
    }
}
int TD_put(struct tableau T, int i, int x)
{
    if(i < T.taille)
    {
        T.tab[i] = x;
        return 1;
    }
    else
    {
        return 0;
    }
}
int TD_somme(struct tableau T)
{
    int i;
    int somme=0;
    for(i=0;i<T.taille;i++)
    {
        somme+=T.tab[i];
    }
    return somme;
}
void TD_afficher(struct tableau T,int i, int j)
{
    int c;
    printf("tab: {");
    for(c=i;c<j;c++)
    {
        printf(" %d ", T.tab[c]);
    }
    printf("}\n");
}
int partition(int * a, int p, int r)
{
    int lt[r-p];
    int gt[r-p];
    int i;
    int j;
    int key = a[r];
    int lt_n = 0;
    int gt_n = 0;
#pragma omp parallel for
    for(i = p; i < r; i++){
        if(a[i] < a[r]){
            lt[lt_n++] = a[i];
        }else{
            gt[gt_n++] = a[i];
        }
    }
    for(i = 0; i < lt_n; i++){
        a[p + i] = lt[i];
    }
    a[p + lt_n] = key;
    for(j = 0; j < gt_n; j++){
        a[p + lt_n + j + 1] = gt[j];
    }
    return p + lt_n;
}
void quicksort(int * a, int p, int r)
{
    int div;
    if(p < r){
        div = partition(a, p, r);
#pragma omp parallel sections
        {
#pragma omp section
            quicksort(a, p, div - 1);
#pragma omp section
            quicksort(a, div + 1, r);
        }
    }
}



