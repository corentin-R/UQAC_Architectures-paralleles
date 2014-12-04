
__kernel 
void floydWarshallPass(__global int * pathDistanceBuffer,  
                       const unsigned int numNodes, 
                       const unsigned int pass     
					             )
{

    int x = get_global_id(0);
    int y = get_global_id(1);

    int k = pass;
    
    int temp = (pathDistanceBuffer[y  * numNodes+k] + pathDistanceBuffer[k* numNodes + x ]);

    if (temp < pathDistanceBuffer[y  * numNodes + x ])
    {
        pathDistanceBuffer[y  * numNodes + x ] = temp;
		//printf("test -> %d\n", outputBuffer[y  * numNodes + x ] );
    }

}

