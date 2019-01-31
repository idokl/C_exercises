/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 3
******************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

/************************************************************************
* function name: GetArray
* The Input: inputArray[]
* The output: arraySize - number of the cells that the user enter their value.
* The Function operation: ask the user to enter array of numbers and its size.
* scan the numbers to the cells of the array. return the array size.
*************************************************************************/  
int GetArray(int inputArray[])
{
	//size of the the input array (according to the user choice)
	int arraySize;
	//counter
	int i;
		
	//read the size of the array
	printf("Please enter your array length and your array:\n");
	scanf ("%d",&arraySize);
	
	//read the array values
	for (i=0; i < arraySize; i++)
	{
		scanf("%d",&inputArray[i]);
	}
	return arraySize;
}
