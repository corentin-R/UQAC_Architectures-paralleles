#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE* fichier = NULL;
	int nb_elements = 2048;
	if(argc == 2)
	{
		nb_elements = atoi(argv[1]);
	}

	fichier = fopen("graph", "w");

	if (fichier != NULL)
	{ 
		// On l'écrit dans le fichier
		fprintf(fichier, "%d ", nb_elements);
		// random initialisation of input
		srand((int)time(NULL));
		int i,j;
		for(i=0;i<nb_elements;i++)
		{
			for(j=0;j<nb_elements;j++)
			{			
				if(i==j)
					fprintf(fichier, "%d ", 0);
				else
					fprintf(fichier, "%d ",  rand()%20);
			}
		}
		fclose(fichier);
	}

	return 0;
}