/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 3
******************************************/
#include <stdio.h>
#include "GetArray.h"

#define MAX_ARRAY_SIZE 50

//function that gets array and returns 1 if it's palindrome or 0 if not.
int IsPalindrome(int a[], int size);

/************************************************************************
* function name: Palindrome
* The Input: none
* The output: 0
* The Function operation: gets an array and print whether it is a palindrome.
* the function uses IsPalindrome function in order to check if it's palindrome.
*************************************************************************/
int Palindrome()
{
	int arraySize;
	int inputArray[MAX_ARRAY_SIZE];

	/*
	call to GetArray function that gets values of inputArray from the user and
	returns the number of the filled cells into the variable ArraySize.
	*/
	arraySize = GetArray(inputArray);

	/*
	execute IsPalindrome function on inputArray and print massage to the user
	according to its output.
	*/
	switch (IsPalindrome(inputArray, arraySize))
		{
			case 1:
				printf ("Your array is palindrome.\n");
				break;
			case 0:
				printf ("Your array is not palindrome.\n");
				break;
			default:
				printf ("I hope this line won't appear in running time\n");
		}

	return 0;
}


/************************************************************************
* function name: IsPalindrome 
* The Input: "a[]" - an array of integers, "size" (int) - the array's size
* The output: if the array is palindrome - 1. else - 0.
* The Function operation:
* the function compares the values of the first half of the array's cells to
* the values in the second half to decide whether it's a palindrome.
*************************************************************************/  
int IsPalindrome(int a[], int size)
{
	enum Boolean {NO, YES};
	enum Boolean palindromeFlag;
	//counters in the loop (i counts from the beginning, j - from the end)
	int i, j;
	
	//initializtion
	palindromeFlag = YES;

	/*
	The code is based on lec4.ppt (although Avinatan show a different way).
	if one of the values in the first half of the array is different from
	his parallel value, change palindromeFlag to "NO".
	*/
	for (i=0, j=size-1 ; i+1 <= size/2; i++, j--)
	{
		if (a[i] != a[j])
		{
			palindromeFlag = NO;
			continue;
		}

	}
	
	if (palindromeFlag == YES)
		return 1;
	else 
		return 0;
}
