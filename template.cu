#include "CRtree.h"

__global__ void OddEvenSortPhaseKernel(float* d_arrayIn, int n, bool phase)
{
	int id = blockDim.x * blockIdx.x + threadIdx.x;
	int subsetSize = 4;
	int indexBegin;

	if(phase) // FASE PAR
		indexBegin = id * (subsetSize*2);
	else // FASE IMPAR
		indexBegin = id * ((subsetSize*2)-subsetSize);

	if(indexBegin < n)
	{
		int indexEnd = indexBegin + (subsetSize*2);
		int i, temp;

		for(i = indexBegin; i < (indexEnd-1); i++)
		{
			if( i+1 < n &&
				d_arrayIn[i] > d_arrayIn[i+1] )
			{
				temp = d_arrayIn[i];
				d_arrayIn[i] = d_arrayIn[i+1];
				d_arrayIn[i+1] = temp;
			}
		}
	}
}

void OddEvenSortPhase(float* arrayIn, int n, bool phase)
{
	int size = n * sizeof(float);
	float *d_arrayIn;

	cudaMalloc((void **) &d_arrayIn, size);
	cudaMemcpy(d_arrayIn, arrayIn, size, cudaMemcpyHostToDevice);

	dim3 DimGrid(ceil(n/256.0), 1, 1);
	dim3 DimBlock(256, 1, 1);
	OddEvenSortPhaseKernel<<< DimGrid, DimBlock >>>(d_arrayIn, n, phase);

	cudaMemcpy(arrayIn, d_arrayIn, size, cudaMemcpyDeviceToHost);

	cudaFree(d_arrayIn);
}

int main()
{
	printf("Begin ==========\n");

	int numEntries = 4;
	int numFeatures = 64;
	CRtree rtree(numEntries, numFeatures);

	// INSERTANDO DATOS ==================================================
	float* bufferIn;
	bufferIn = (float*) malloc(numFeatures * sizeof(float));

	for(int i = 0; i < numFeatures; i++)
		bufferIn[i] = 0.0;

	//printf("root: %d\n", rtree.m_root->m_typeNode);

	rtree.InsertData(bufferIn);
	rtree.InsertData(bufferIn);
	rtree.InsertData(bufferIn);
	rtree.InsertData(bufferIn);
	rtree.InsertData(bufferIn);

	//printf("root: %d\n", rtree.m_root->m_typeNode);

	printf("End ============\n");

	return 0;
}
