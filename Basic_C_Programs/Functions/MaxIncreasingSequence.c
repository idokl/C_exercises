
/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 3
******************************************/
#include <stdio.h>
#include "GetArray.h"

#define MAX_ARRAY_SIZE 50

/************************************************************************
* function name: MaxIncreasingSequence
* The Input: none
* The output: 0
* The Function operation:
* Gets an array from the user and finds the maximal strictly monotonically
* increasing sequence that the array includes (in sequential cells).
*************************************************************************/  
int MaxIncreasingSequence()
{
	int inputArray[MAX_ARRAY_SIZE];
	//size of the the input array (according to the user choice)
	int inputArraySize;
	
	/*
	pointer to the beginning of the current increasing sequence
	(the sub-array that is checked in the current iterations of the for loop)
	(After writing of the code I saw there were no pointers until exercise 5.
	I thought we have to use pointers because *lecture 5* of Avinatan contained
	pointers.
	We can write a same code without pointers by using variable of the indexes
	of the appropriate cells).
	*/
	int *pBeginCurrentSeq = &inputArray[0];
	//pointer to the beginning of the maximal increasing sequence
	int *pBeginMaxSeq = &inputArray[0];
	//length of the maximal increasing sequence
	int maxSeqLength;
	//counter of the loop
	int i;

	/*
	call to GetArray function that gets values of inputArray from the user and
	returns the number of the filled cells into the variable inputArraySize.
	*/
	inputArraySize = GetArray(inputArray);
	
	//initialization of maxSeqLength
	if (inputArraySize == 0)
		maxSeqLength = 0;
	else
		maxSeqLength = 1;
	
	/*
	This loop have to find the beginning of the maximal-ascending-sub-array
	and its length.
	We run over all the cells of the array (from the second cell)
	and we use 2 pointers:
	pBeginCurrentSeq - points to the beginning of the last ascending sub-array.
	pBeginMaxSeq - points to the beginning of the maximal ascending sub-array
	that was found so far.
	In the end of the process pBeginMaxSeq will point to the beginning of the
	maximal ascending sub-array in the whole array.
	*/
	for (i=1; i < inputArraySize; i++)
	{
		
		/*
		if the value of the current cell isn't bigger than the previous value
		fix pBeginCurrentSeq to point on the current cell (because it starts
		a new ascending-sub-array).
		*/
		if (inputArray[i] <= inputArray[i-1])
		{
			pBeginCurrentSeq = &inputArray[i];
		}
		
		/*
		if the distance from the beginning of the current ascending-sub-array
		to the current cell is bigger than the maxSeqLength, fix pBeginMaxSeq
		and  maxSeqLength according to the current ascending-sub-array.
		*/
		if ((&inputArray[i] - pBeginCurrentSeq + 1) > maxSeqLength)
		{
			pBeginMaxSeq = pBeginCurrentSeq;
			maxSeqLength = &inputArray[i] - pBeginCurrentSeq + 1;
		}
	} //end of for loop
	
	//print the max ascending-sub-array and its length
	printf("The max ascending array is:");
	for (i=0; i < maxSeqLength; i++)
		printf (" %d",pBeginMaxSeq[i]);
	printf ("\nLength %d\n",maxSeqLength);
	
	return 0;
}
