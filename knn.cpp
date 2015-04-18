#include "knn.h"
#include "myvector.h"
#include <iostream>
#include <cmath>

using namespace std;

void Knn::init(){
    for (int i = 0; i <= K; i++)
	{
		knnlist[i] = -1;
		distlist[i] = -1;
	}
	sqrtbound = 1000000;
}

//linear scan the dataset and put the result in knnlist (order is not maintained)
void Knn::linear_scan(float data[][D], float querypoint[])
{
    init();
	for (int i = 0; i < datasize; i++)
	{
		addvertex(data, i, querypoint);
	}
}

// check if data forcheck is a knn of querypoint, if yes maintain a new knn list
void Knn::addvertex(float data[][D], int forcheck, float querypoint[])
{
	float dist;
	dist = MyVector::distancel2sq(D,data[forcheck], querypoint,0);
	if (knnlist[K - 1] == -1)
	{
		for (int i = 0; i < K; i++)
		{
			if (knnlist[i] == -1)
			{
				knnlist[i] = forcheck;// towrite here is a label for the datapoint
				distlist[i] = dist;
				if (i == K - 1) computebound();
				return;
			}
		}
	}
	if (dist >= bound) return;
	knnlist[tochange] = forcheck;
	distlist[tochange] = dist;
	computebound();
}



//compute the bound : current largest knn distance and its index: tochange
void Knn::computebound()
{
	bound = distlist[0];
	tochange = 0;
	for (int i = 1; i < K; i++)
	{
		if (distlist[i] > bound)
		{
			bound = distlist[i];
			tochange = i;
		}
	}
	sqrtbound = sqrt(bound);
}
