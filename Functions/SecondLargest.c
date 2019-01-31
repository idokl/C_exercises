/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 3
******************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
/************************************************************************
* function name: SecondLargest
* The Input: none
* The output: none
* The Function operation: find the second largest from group of input numbers
*************************************************************************/  
int SecondLargest()
{
	int largestNum = 0;
	int secondNum = 0;
	//new input number from the user
	int input;
	
	//ask the user to enter an array
	printf("Please enter your array:\n");
	
	//scan the first number
	scanf ("%d",&input);

	/*
	We compare every new input (number) to the secondNum (second largest
	so far). If the input is bigger, we compare it to largestNum.
	If the input is bigger than both of them, it will become to largeNum
	and largeNum will become to secondNum.
	If the input is only bigger than secondNum, it will become to second num.
	*/
	while (input != 0)
	{
		if (input > secondNum)
		{
			if (input > largestNum)
			{
				secondNum = largestNum;
				largestNum = input;
			}
			else if (input < largestNum)
			{
				secondNum = input;
			}
		}
		scanf ("%d",&input);
	}
	
	//print a massage to the user
	printf("The second largest value is %d.\n",secondNum);
	return 0;
}
