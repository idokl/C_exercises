/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 3
******************************************/

#include <stdio.h>
#include "GetArray.h"

//The maximal size of array that the program enable the user to enter
#define MAX_ARRAY_SIZE 50

/************************************************************************
* function name: ShiftRight
* The Input: none
* The output: 0
* The Function operation:
* The function gets from the user an array of numbers and number of shifts.
* The function transfers every number some steps to the right according to the
* choice of the user.
* It prints both the original array and the new array.
* The function uses 2 arrays: it uses the "input array" to get the array
* from the user, and it uses the "output array" in order to reorder the array.
* The function go through the adresses of the input array cells and pass the
* value of every cell to higher address in the "output array" in a cyclic way
* that make the values of the array to "move".
*************************************************************************/
int ShiftRight()
{
	//size of the the input array (according to the user choice)
	int arraySize;
	//number of shifts to the right  (according to the user choice)
	int shiftsNum;
	//the array before the shift
	int inputArray[MAX_ARRAY_SIZE];
	//the array after the shift
	int outputArray[MAX_ARRAY_SIZE];
	//counter of the loop
	int i;
	
	/*
	call to GetArray function that gets values of inputArray from the user
	and returns the number of the filled cells into the variable arraySize.
	*/
	arraySize = GetArray(inputArray);
	
	//read the number of shifts the array have to shift
	printf("Please enter the number to shift right:\n");
	scanf ("%d",&shiftsNum);
	
	/*
	the loop run over the filled cells in the inputArray and transfer their
	value to another place in the outputArray according to shiftsNum.
	*/
	for (i=0; i < arraySize; i++)
	{
		outputArray[(i + shiftsNum) % arraySize] = inputArray[i];
	}
	
	//print the original array.
	printf ("Your array is:");
	for (i=0; i < arraySize; i++)
		printf(" %d",inputArray[i]);
	printf ("\n");
	
	//print the new array
	printf ("After %d shifts right is:",shiftsNum);
	for (i=0; i < arraySize; i++)
		printf(" %d",outputArray[i]);
	printf ("\n");
	
	return 0;
}
