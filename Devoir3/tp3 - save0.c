
/////////////////////////////// 8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #3 ////////////////////////////////////////
///////////////////////////// tri tableau MPI - Corentin RAOULT - Adrien Cambillau /////////////////////////////////////

//http://www.cac.cornell.edu/vw/MPIoneSided/exercise.aspx

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <limits.h>

#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))

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

//variable globale sale accessible de partout
MPI_Win win;
MPI_Comm comm;

////////////////////// déclaration des fonctions ////////////////////////////////////////////////////////////////////

int demandeNombre();
void remplirTABrand( struct tableau T);

struct tableau TD_init(int n);
int TD_get(struct tableau T, int i, int *x);
int TD_put(struct tableau T, int i, int *x);
int TD_somme(struct tableau T); // Retourne la somme des  éléments de T .
void TD_afficher(struct tableau T,int i, int j); // Affiche dans l’ordre les  éléments i (i ≤ j) du tableau T .
void afficherNomMachine();

void triFusionParallele(int * TAB, int n);
void fusion(int * U, int taille_U, int * V, int taille_V , int * T);
void afficherTAB(int* TAB, int n);

void getPartTab(struct tableau T, int debut, int fin);

///////////////////// MAIN ///////////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv) 
{
    int n = atoi(argv[1]);

    int rank, i,p;

    //double time, max_time;
    //time = -MPI_Wtime();

    //Start up MPI...
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);  
    MPI_Group comm_group, group;
    comm = MPI_COMM_WORLD;

    struct tableau T;

    if (rank == 0) {
        /* Only rank 0 has a nonzero buffer at start */
        T = TD_init(n);

    }
    else {
        T.tab = calloc(n, sizeof(int));//Initialize all buffers to 0
        /* Others only retrieve, so these windows can be size 0 */
        MPI_Win_create(T.tab,n*sizeof(int)+5,sizeof(int),MPI_INFO_NULL,comm,&win);
    }

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////


    getPartTab(T,BLOCK_LOW(rank-1,4,n),BLOCK_HIGH(rank-1,4,n)+1);
    

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////

    if(rank!=0)
    {
        //#pragma omp parallel
        triFusionParallele(T.tab+(rank-1)*(n/4), n/4);
    }

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////


    MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
    if(rank!=0){
        for(i=BLOCK_LOW(rank-1,4,n);i<BLOCK_HIGH(rank-1,4,n)+1;i++)
        {
        //printf("%d ",T.tab[i] );
            TD_put(T, i, &T.tab[i]);

        }
        
    }
    MPI_Win_fence(MPI_MODE_NOSUCCEED,win);

    getPartTab(T,0,n);
    
    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////

    if(rank==0 || rank==1)
    {
        int * U = malloc((n/4+1)*sizeof(int));
        int * V = malloc((n/4+1)*sizeof(int));
        for(i=0; i<n/4;i++)
        {
            U[i]=T.tab[i+rank*(n/2)];
            V[i]=T.tab[i+(n/4)+rank*(n/2)];
        }
        //*(U+n/2)=*(V+n/2)=INT_MAX;
        fusion(U,n/4,V,n/4,T.tab+rank*(n/2));
        //afficherTAB(T.tab, n);
        free(U);
        free(V);
    }  

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////


    MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
    if(rank==0 || rank==1)
    {
        for(i=rank*(n/2);i<(rank+1)*(n/2);i++)
        {           
            TD_put(T, i, &T.tab[i]);            
        }

    }
    MPI_Win_fence(MPI_MODE_NOSUCCEED,win);

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////

    getPartTab(T,0,n);

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////

    if(rank==0){
        int * leftTab = malloc((n/2)*sizeof(int));
        int * rightTab = malloc((n/2)*sizeof(int));

        for(i=0; i<n/2;i++)
        {
            leftTab[i]=T.tab[i];
            rightTab[i]=T.tab[i+(n/2)];
        }

        fusion(leftTab, n/2, rightTab, n/2, T.tab);

        free(leftTab);
        free(rightTab);
    }
    

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////

    MPI_Win_free(&win);

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////

    if(rank == 0)
    {
        TD_afficher(T,0,n);
    }

    //time += MPI_Wtime();
    //MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    //if (!rank)
    //  printf ("tri: processus: %d, secondes: %6.2f \n",p, max_time);

    MPI_Barrier(MPI_COMM_WORLD);///////////////////////////////barrier///////////////////////////////////////////////////////////////////////////////
    
    free(T.tab);

    //Shut down...
    MPI_Finalize();

    return(0);
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
    {
        printf("%s\n", hostname);fflush(stdout);
    }
    else 
        fprintf(stderr, "La fonction gethostname a echoue.\n");
}

void remplirTABrand(struct tableau T)
{
    int i;
    srand(time(NULL));
    for(i=0;i<T.taille;i++)
        T.tab[i] = rand()%T.taille; 
}
struct tableau TD_init(int n)
{
    struct tableau T;
    T.tab =  malloc(n*sizeof(int));
    T.taille=n;

    remplirTABrand(T);

    /* Everyone will retrieve from the buffer on root */
    MPI_Win_create(T.tab,n*sizeof(int),sizeof(int),MPI_INFO_NULL,comm,&win);

    printf("Tableau original\n");fflush(stdout);
    TD_afficher(T,0, T.taille);fflush(stdout);

    return T;
}


int TD_get(struct tableau T, int i, int *x)
{
    if(i < T.taille)
    {
        MPI_Get(x,1,MPI_INT,0,i,1,MPI_INT,win);
        return 1;
    }
    else
        return 0;
}

int TD_put(struct tableau T, int i, int *x)
{
    if(i < T.taille)
    {
        MPI_Put(x,1,MPI_INT,0,i,1,MPI_INT,win);
        return 1;
    }
    else
        return 0;
}

int TD_somme(struct tableau T)
{
    int i;
    int somme=0;
    for(i=0;i<T.taille;i++)
        somme+=T.tab[i];
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
        //#pragma omp single nowait
        //{
        //#pragma omp task
        triFusionParallele(U,n/2);
        //#pragma omp task
        triFusionParallele(V,n/2);
        //#pragma omp taskwait
        fusion(U,n/2,V,n/2,TAB);
        //}
    }
    free(U);
    free(V);
}
void fusion(int * U, int taille_U, int * V, int taille_V , int * T)
{
    int i=0,j=0;
    int k;
    *(U+taille_U)=INT_MAX;
    *(V+taille_V)=INT_MAX;

    for(k=0; k<(taille_U+taille_V);k++)
    {
        if (U[i]<V[j])
            T[k]=U[i++];
        else 
            T[k]=V[j++];
    }
}

void afficherTAB(int* TAB, int n)
{
    int j;
    printf("TAB : { ");
    for(j = 0; j < n; j++)
        printf(" [%d] ",TAB[j]);
    
    printf(" }\n");
}

void getPartTab(struct tableau T, int debut, int fin)
{
    MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
    int i;
    for(i=debut;i<fin;i++)
        TD_get(T, i, &T.tab[i]);

    MPI_Win_fence(MPI_MODE_NOSUCCEED,win);
}

