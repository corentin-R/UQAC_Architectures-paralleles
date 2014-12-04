#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#include "CLUtil.hpp"



//
// Lecture du fichier source
//http://stackoverflow.com/questions/24428153/opencl-kernel-file-not-entirely-loading
//
static char* Read_Source_File(const char *filename)
{
	long int
		size = 0,
		res  = 0;

	char *src = NULL;

	FILE *file = fopen(filename, "rb");

	if (!file)  return NULL;

	if (fseek(file, 0, SEEK_END))
	{
		fclose(file);
		return NULL;
	}

	size = ftell(file);
	if (size == 0)
	{
		fclose(file);
		return NULL;
	}

	rewind(file);

	src = (char *)calloc(size + 1, sizeof(char));
	if (!src)
	{
		src = NULL;
		fclose(file);
		return src;
	}

	res = fread(src, 1, sizeof(char) * size, file);
	if (res != sizeof(char) * size)
	{
		fclose(file);
		free(src);

		return src;
	}

	src[size] = '\0'; /* NULL terminated */
	fclose(file);

	return src;
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


void afficherMatrice(int *  mat, int taille)
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

int main(int argc, char * argv[])
{

	size_t datasize;

	int n_elements=10;

	int* pathDistanceMatrix;
	int* outputMatrix;
	if(argc == 2)
	{
		printf("fichier lu = %s\n",argv[1]);
		//int* n_elements =(int*) malloc(sizeof(int));
		pathDistanceMatrix = readFile(argv[1], &n_elements);
		datasize=sizeof(int)*n_elements*n_elements;
		outputMatrix = (int *) malloc(datasize);
	}
	else
	{
		// Compute the size of the data
		datasize = sizeof(int)*n_elements*n_elements;

		pathDistanceMatrix = (int *) malloc(datasize);
		outputMatrix = (int *) malloc(datasize);
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


	cl_int status;



	// This code executes on the OpenCL host
	char* programSource=Read_Source_File("FloydWarshall_Kernels.cl");


	//on affiche la matrice d'adjacence
	afficherMatrice(pathDistanceMatrix, n_elements);



	//-----------------------------------------------------
	// STEP 1: Discover and initialize the platforms
	//-----------------------------------------------------

	cl_uint numPlatforms = 0;
	cl_platform_id *platforms = NULL;   

	// Calcul du nombre de plateformes
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms = %d\n", numPlatforms);

	// Allocation de l'espace
	platforms =(cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));

	// Trouver les plateformes
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);   

	char Name[1000];
	clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, sizeof(Name), Name, NULL);
	printf("Name of platform : %s\n", Name);
	fflush(stdout);



	//-----------------------------------------------------
	// STEP 2: Discover and initialize the devices
	//-----------------------------------------------------

	cl_uint numDevices = 0;
	cl_device_id *devicess = NULL;

	// calcul du nombre de périphériques
	status = clGetDeviceIDs(
		platforms[0],
		CL_DEVICE_TYPE_ALL,
		0,
		NULL,
		&numDevices);

	printf("Number of devices = %d\n", (int)numDevices);

	// Allocation de l'espace
	devicess = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));



	// Trouver les périphériques
	status = clGetDeviceIDs(
		platforms[0],
		CL_DEVICE_TYPE_ALL,
		numDevices,
		devicess,
		NULL);

	printf("\n");
	for (int i=0; i<(int)numDevices; i++)
	{
		clGetDeviceInfo(devicess[i], CL_DEVICE_NAME, sizeof(Name), Name, NULL);
		printf("Name of device %d: %s\n", i, Name);
	}
	printf("\n");



	//-----------------------------------------------------
	// STEP 3: Create a context
	//-----------------------------------------------------

	printf("Création du contexte\n");
	fflush(stdout);    

	cl_context context = NULL;   

	context = clCreateContext(
		NULL,
		numDevices,
		devicess,
		NULL,
		NULL,
		&status);


	//-----------------------------------------------------
	// STEP 4: Create a command queue
	//-----------------------------------------------------

	printf("Création de la file d'attente\n");
	fflush(stdout);

	cl_command_queue cmdQueue;   

	cmdQueue = clCreateCommandQueue(
		context,
		devicess[0],
		0,
		&status);

	//-----------------------------------------------------
	// STEP 5: Create device buffers
	//-----------------------------------------------------

	printf("Création des buffers\n");
	fflush(stdout);

	cl_mem pathDistanceBuffer = clCreateBuffer(
		context,
		CL_MEM_READ_WRITE,
		datasize,
		NULL,
		&status);




	//-----------------------------------------------------
	// STEP 6: Write host data to device buffers
	//----------------------------------------------------- 

	printf("Ecriture dans les buffers\n");
	fflush(stdout);

	status = clEnqueueWriteBuffer(
		cmdQueue,
		pathDistanceBuffer,
		CL_TRUE,
		0,
		datasize,
		pathDistanceMatrix,
		0,
		NULL,
		NULL);
	int i=0;



	//-----------------------------------------------------
	// STEP 7: Create and compile the program
	//-----------------------------------------------------   

	printf("CreateProgramWithSource\n");
	fflush(stdout);

	cl_program program = clCreateProgramWithSource(
		context,
		1,
		(const char**)&programSource,
		NULL,
		&status);
	// printf("%s\n",programSource);   

	printf("Compilation\n");
	fflush(stdout);

	status = clBuildProgram(
		program,
		numDevices,
		devicess,
		NULL,
		NULL,
		NULL);   

	if (status) printf("ERREUR A LA COMPILATION: %d\n", status);


	//http://stackoverflow.com/questions/9464190/error-code-11-what-are-all-possible-reasons-of-getting-error-cl-build-prog
	if (status == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, devicess[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, devicess[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);
	}

	//-----------------------------------------------------
	// STEP 8: Create the kernel
	//-----------------------------------------------------	   

	cl_kernel kernel = NULL;

	printf("Création du kernel\n");
	fflush(stdout);

	kernel = clCreateKernel(program, "floydWarshallPass", &status);
	//CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");


	//-----------------------------------------------------
	// STEP 9: Set the kernel arguments
	//-----------------------------------------------------   

	// Associate the input and output buffers with the
	// kernel
	// using clSetKernelArg()

	printf("Passage des paramètres\n");
	fflush(stdout);

	status = clSetKernelArg(kernel,
		0,
		sizeof(cl_mem),//datasize,
		(void*)&pathDistanceBuffer);

	// n_elements
	status = clSetKernelArg(kernel,
		1,
		sizeof(cl_int),
		(void*)&n_elements);

	//-----------------------------------------------------
	// STEP 10: Configure the work-item structure
	//-----------------------------------------------------

	// Define an index space (global work size) of work
	// items for
	// execution. A workgroup size (local work size) is not
	// required,
	// but can be used.

	size_t MaxGroup;
	clGetDeviceInfo(devicess[0],CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &MaxGroup, NULL);
	printf("CL_DEVICE_MAX_WORK_GROUP_SIZE = %d\n", (int) MaxGroup);

	size_t MaxItems[3];
	clGetDeviceInfo(devicess[0],CL_DEVICE_MAX_WORK_ITEM_SIZES, 3*sizeof(size_t), MaxItems, NULL);
	printf("CL_DEVICE_MAX_WORK_ITEM_SIZES = (%d, %d, %d)\n", (int) MaxItems[0], (int)MaxItems[1], (int)MaxItems[2]);

	size_t globalWorkSize[2]={n_elements, n_elements};

	size_t localWorkSize[3]={2,2};

	// There are 'n_elements' work-items



	//-----------------------------------------------------
	// STEP 11: Enqueue the kernel for execution
	//-----------------------------------------------------   

	// Execute the kernel by using
	// clEnqueueNDRangeKernel().
	// 'globalWorkSize' is the 1D dimension of the
	// work-items   

	printf("Debut des appels\n");

	for(int pass = 0; pass < n_elements; pass++)
	{
		status = clSetKernelArg(kernel,
			2,
			sizeof(int),
			(void*)&pass);


		// Enqueue a kernel run call.

		cl_event ndrEvt;

		status = clEnqueueNDRangeKernel(cmdQueue,
			kernel,
			2,
			NULL,
			globalWorkSize,
			//NULL,
			localWorkSize,
			0,
			NULL,
			&ndrEvt);

		status = clFlush(cmdQueue);
		if(n_elements<128)
			printf("Fin du %d appel: status=%d\n",pass, status);
	}





	//-----------------------------------------------------
	// STEP 12: Read the output buffer back to the host
	//-----------------------------------------------------

	//
	//Lecture de la matrice C
	//

	// Enqueue readBuffer
	cl_event readEvt2;
	status = clEnqueueReadBuffer(cmdQueue,
		pathDistanceBuffer,
		CL_TRUE,
		0,
		datasize,
		outputMatrix,
		0,
		NULL,
		&readEvt2);


	//on affiche la matrice d'adjacence
	afficherMatrice(outputMatrix, n_elements);


	//-----------------------------------------------------
	// STEP 13: Release OpenCL resources
	//-----------------------------------------------------   

	// Free OpenCL resources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(pathDistanceBuffer);
	clReleaseContext(context);


	// Free host resources
	free(pathDistanceMatrix);
	free(outputMatrix);
	free(platforms);
	free(devicess);



	return 0;
}
