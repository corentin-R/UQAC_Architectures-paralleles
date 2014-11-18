
/////////////////////////////// 8INF854 - ARCHITECTURES PARRALLELES - DEVOIR #3 ////////////////////////////////////////
///////////////////////////// tri tableau MPI - Corentin RAOULT - Adrien Cambillau /////////////////////////////////////

//http://www.cac.cornell.edu/vw/MPIoneSided/exercise.aspx

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

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
int TD_put(struct tableau T, int i, int x);
int TD_somme(struct tableau T); // Retourne la somme des  éléments de T .
void TD_afficher(struct tableau T,int i, int j); // Affiche dans l’ordre les  éléments i (i ≤ j) du tableau T .
void afficherNomMachine();

int partition(int * a, int p, int r);
void quicksort(int * a, int p, int r);



///////////////////// MAIN ///////////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv) 
{
	int n = atoi(argv[1]);

	int rank, i,p;

	double time, max_time;
	time = -MPI_Wtime();

    //Start up MPI...
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);	
	MPI_Group comm_group, group;
	comm = MPI_COMM_WORLD;


	//if (!rank)
		

	struct tableau T;
	T.tab = calloc(n, sizeof(int));//Initialize all buffers to 0

	if (rank == 0) {
        /* Only rank 0 has a nonzero buffer at start */
		T = TD_init(n);

	}
	else {
        /* Others only retrieve, so these windows can be size 0 */
		MPI_Win_create(T.tab,n*sizeof(int),sizeof(int),MPI_INFO_NULL,comm,&win);
	}
	printf("Before Get on %d:	\n",rank);
	for(i=0;i<n;i++)printf("%d    ", T.tab[i]);
		printf("\n\n");



	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
	if(rank == 1)
				TD_put(T, 0, 777);
	
	MPI_Win_fence(MPI_MODE_NOSUCCEED,win);
	MPI_Barrier(MPI_COMM_WORLD);



    /* No local operations prior to this epoch, so give an assertion */
	MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
    /* Inside the fence, ranks make RMA calls to GET from rank 0 */
	if (rank != 0) {
		//MPI_Get(T.tab,n,MPI_INT,0,0,n,MPI_INT,win);
		TD_get(T, 0, &T.tab[10]);
		int *vale = malloc(sizeof(int));
		TD_get(T, 0, vale);
		printf("val=%d\n",*vale );	
	}



    /* Complete the epoch - this blocks until the MPI_Get is complete -
       all done with the window, so tell MPI there are no more epochs */
	MPI_Win_fence(MPI_MODE_NOSUCCEED,win);



	printf("After Get on %d:\n",rank);
	for(i=0;i<n;i++)printf("%d    ", T.tab[i]);
		printf("\n\n");
    /* Free up our window */
	MPI_Win_free(&win);
	MPI_Barrier(MPI_COMM_WORLD);

	time += MPI_Wtime();
	MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (!rank)
		printf ("tri: processus: %d, secondes: %6.2f \n",p, max_time);
	MPI_Barrier(MPI_COMM_WORLD);

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
		T.tab[i] = rand()%T.taille; //limité par unsigned long long int
}

struct tableau TD_init(int n)
{
	struct tableau T;
	T.tab =  malloc(n*sizeof(int));
	T.taille=n;

	remplirTABrand(T);

	/* Everyone will retrieve from the buffer on root */
	MPI_Win_create(T.tab,sizeof(int)*n,sizeof(int),MPI_INFO_NULL,comm,&win);

	printf("Tableau original\n");fflush(stdout);
	TD_afficher(T,0, T.taille);fflush(stdout);

	return T;
}


int TD_get(struct tableau T, int i, int *x)
{
	if(i < T.taille)
	{
		//MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
		MPI_Get(x,1,MPI_INT,0,i,1,MPI_INT,win);
		//MPI_Win_fence(MPI_MODE_NOSUCCEED,win);
		printf("x=%d\n", *x);
		return 1;
	}
	else
		return 0;
}

int TD_put(struct tableau T, int i, int x)
{
	if(i < T.taille)
	{
		int val = x;
		T.tab[i]=x;
		MPI_Put(&val,1,MPI_INT,0,i,1,MPI_INT,win);
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
