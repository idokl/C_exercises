/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 3
******************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

/************************************************************************
* function name: Calculator
* The Input: none
* The output: 0 (in error case - 1)
* The Function operation:
* the function gets a simple math exercise with simple fractions and prints
* the result.
*************************************************************************/  
int Calculator()
{
	//numerator of the result
	int resultNumer = 0;
	//denominator of the result
	int resultDenom = 1;
	//numerator and denominator of the input
	int inputNumer;
	int inputDenom;
	//operator (+, -, *, \)
	char operat;
	//variable that is supposed to contain the "/" sign of the fractions
	char divideSign;
	//counter of the loop
	int i;

	printf("Please enter your equation:\n");

	//initialization
	scanf("%d",&resultNumer);
	scanf("%c",&divideSign);
	scanf("%d",&resultDenom);
	
	//error case - invalid sign between numerator and denominator
	if (divideSign != '/')
	{
		printf ("error");
		return 1;
	}

	//empty exercise. the result: 0/1
	if (resultNumer == 0 && resultDenom == 0)
	{
		printf("Result 0/1.\n");
		return 0;
	}

	while (resultNumer != 0 || resultDenom != 0)
	{
		//read mathematical operator and a fraction
		scanf("%c",&operat);
		scanf("%d",&inputNumer);
		scanf("%c",&divideSign);
		scanf("%d",&inputDenom);
		
		//error case - invalid sign between numerator and denominator
		if (divideSign != '/')
		{
			printf ("error");
			return 1;
		}

		if (inputNumer != 0 || inputDenom != 0)
		{
			switch (operat)
			{
			
			/*
			calculating addition or subtraction:
			The common denominator of 2 fractions is the denominators' product
			we multiply the numerators accordingly and add/subtract the input
			numerator.
			*/
			case '+':
				resultNumer *= inputDenom;
				inputNumer *= resultDenom; 
				resultDenom *= inputDenom;
				resultNumer += inputNumer;
				break;
			
			case '-':
				resultNumer *= inputDenom;
				inputNumer *= resultDenom; 
				resultDenom *= inputDenom;
				resultNumer -= inputNumer;
				break;
			
			/*
			multiplying:
			result denominator - the denominators' product.
			result numerator - the numerators' product.
			*/
			case '*':
				resultNumer *= inputNumer;
				resultDenom *= inputDenom;
				break;
			
			/*
			division:
			result numerator - multiply first numerator * second denominator
			result denominator - multiply first denominator * second numerator
			*/
			case '/':
				resultNumer *= inputDenom;
				resultDenom *= inputNumer;
				break;
			
			//if invalid operator was entered
			default:
				printf ("undefined sign was entered\n");
			} //end of switch
		} //end of if
		else
		{
			
			/*
			diminution to the result fraction:
			if both thenumerator and denominator can be devided by any number
			between 2 to the number of the denominator, devide them accordingly
			*/
			for (i=2; i<=resultDenom; i++)
			{
				while (resultNumer % i == 0 && resultDenom % i == 0)
				{
					resultNumer /= i;
					resultDenom /= i;
				}
				
			}
			printf("Result %d/%d.\n",resultNumer,resultDenom);
			return 0;
		}
	}	//end of while
	return 0;
}
