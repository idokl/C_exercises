/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 6
******************************************/

#include <math.h>
#include "GenBST.h"

#define RADIUS(X,Y) (sqrt((double)(X*X+Y*Y)))

typedef struct
{
	int x;
	int y;
} Point;

/***************************
FUNCTIONS FOR INT MANAGEMENT
****************************/
/************************************************************************
* function name: CreateInt
* The Input: none
* The output: pInputValue - pointer to memory that was allocated for
							int value in order to retain input from the user.
* The Function operation: allocate memory and return pointer to it, if succeed
*************************************************************************/
static int* CreateInt()
{
	int* pInputValue = (int*) malloc(sizeof(int));
		return pInputValue;
}

/************************************************************************
* function name: CmpInt
* The Input: firstPInt & secondPInt - pointers to integers for comparison.
* The output: 0 - if the integers are equal
*			  positive number - if the first int greater than the second
			  negative number - if the second int greater than the first
* The Function operation: compare between integers.
*************************************************************************/
static int CmpInt(int* firstPInt ,int* secondPInt)
{
	return *firstPInt - *secondPInt;
}

/************************************************************************
* function name: CpyInt
* The Input: pDestination & pSource - pointers to integers.
* The output: none
* The Function operation: copy *pSource to *pDestination.
*************************************************************************/
static void CpyInt(int* pDestination, int* pSource)
{
	*pDestination = *pSource;
}

/************************************************************************
* function name: FreeInt
* The Input: pInt - pointer to integer.
* The output: none
* The Function operation: free allocated memory.
*************************************************************************/
static void FreeInt(int* pInt)
{
	free(pInt);
}

/************************************************************************
* function name: PrintInt
* The Input: pInt - pointer to integer that we want to print.
* The output: none
* The Function operation: print integer by 5 characters.
*************************************************************************/
static void PrintInt(int* pInt)
{
	int integer = *pInt;
	printf("%05d",integer);
}

/************************************************************************
* function name: ReadInt
* The Input: pInt - pointer to integer that will be read.
* The output: none
* The Function operation: read integer
*************************************************************************/
static void ReadInt(void *pInt)
{
	scanf("%d",(int*)pInt);
}


/*****************************
FUNCTIONS FOR POINT MANAGEMENT
******************************/
/************************************************************************
* function name: CreatePoint
* The Input: none
* The output: pInputValue - pointer to memory that was allocated for
							point value (in order to retain input from user).
* The Function operation: allocate memory and return pointer to it, if succeed
*************************************************************************/
static Point* CreatePoint()
{
	Point* pInputValue = (Point*) malloc(sizeof(Point));
		return pInputValue;
}

/************************************************************************
* function name: CmpPoint
* The Input: firstPoint & secondPoint - pointers to points for comparison.
* The output: 0 - if the points are equal
*			  -1 - if the points' have the same radius but they're different.
*			  positive number - if the first point's radius (=distance from
*			  the origin (0,0)) greater than the second point's radius.
* 			  negative number - if the second point's radius greater than
* 			  the first point's radius
* The Function operation: compare between points.
*************************************************************************/
static int CmpPoint(Point* firstPoint ,Point* secondPoint)
{
	double difference = RADIUS(firstPoint->x ,firstPoint->y) - 
						RADIUS(secondPoint->x, secondPoint->y);
	if (difference == 0)
	{
		if ( (firstPoint->x == secondPoint->x) &&
			(firstPoint->y == secondPoint->y) )
			return 0;
		else
			// I assume that no 2 points with the same RADIUS
			return -1;
	}
	else if (difference < 0)
		return -1;
	else if (difference > 0)
		return 1;
}

/************************************************************************
* function name: CpyPoint
* The Input: pDestination & pSource - pointers to points.
* The output: none
* The Function operation: copy "Source" point to "Destination" point.
*************************************************************************/
static void CpyPoint(Point* pDestination, Point* pSource)
{
	pDestination->x = pSource->x;
	pDestination->y = pSource->y;
}

/************************************************************************
* function name: FreePoint
* The Input: pInt - pointer to point.
* The output: none
* The Function operation: free allocated memory.
*************************************************************************/
static void FreePoint(Point* pPoint)
{
	free(pPoint);
}

/************************************************************************
* function name: PrintPoint
* The Input: pInt - pointer to point that we want to print.
* The output: none
* The Function operation: print point by 5 characters according to the
* pattern xx|yy .
*************************************************************************/
static void PrintPoint(Point* point)
{
	int x = point->x;
	int y =  point->y;
	
	printf( "%02d", x);
	printf("|");
	printf( "%02d", y);
}

/************************************************************************
* function name: ReadPoint
* The Input: pInt - pointer to point that will be read.
* The output: none
* The Function operation: read point values in the pattern (x,y)
*************************************************************************/
static void ReadPoint(void *pPoint)
{
	scanf(" (%d,%d)",&(((Point*)pPoint)->x), &(((Point*)pPoint)->y));
}


/************************************************************************
* function name: TreeManage
* The Input: root - root of tree to manage
* The output: none
* The Function operation: read sub-option letter and execute actions on the
* tree accordingly (a - Add element, s - Search element, d - Delete element,
* p - Print tree, e - Erase tree and Exit).
*************************************************************************/
static void TreeManage(BST_ROOT root, void (*ReadElm)(void* pElm), void *pElm)
{
	//variable to maintain return value of the function "BSTFindElement"
	Elm foundElm;
	//variable to maintain return value of the function "RemoveElement"
	Result result;
	char subOption;
	
	scanf(" %c",&subOption);
	switch(subOption)
	{
	case 'a':
		ReadElm(pElm);
		BSTAddElement(root, pElm);
		TreeManage(root, ReadElm, pElm);
		break;
	case 's':
		ReadElm(pElm);
		foundElm = BSTFindElement(root,pElm);
		if (NULL == foundElm)
			printf("FALSE\n");
		else
			printf("TRUE\n");
		TreeManage(root, ReadElm, pElm);
		break;
	case 'd':
		ReadElm(pElm);
		result = BSTRemoveElement (root, pElm);
		switch (result)
		{
		case NULL_TREE:
			return;
		case FAILURE:
			printf("FALSE\n");
			break;
		case SUCCESS:
			break;
		}
		TreeManage(root, ReadElm, pElm);
		break;
	case 'p':
		BSTPrint(root);
		TreeManage(root, ReadElm, pElm);
		break;
	case 'e':
		BSTDestroy(root);
		return;
	}
}





/************************************************************************
* function name: main
* The Input: none
* The output: none
* The Function operation: read option number 0 or 1:
* for 0 - create BST of integers elements.
* for 1 - create BST of points elements.
* call TreeManage function to enable the user to execute actions on the tree.
*************************************************************************/
void main()
{
	char option;
	BST_ROOT root;
	
	void *pElm;
	void (*ReadElm)(void *pElm);
	int rootInt;
	Point rootPoint;

	scanf(" %c",&option);
	switch(option)
	{
	case '0':
		//TREE OF INTEGERS;
		pElm = &rootInt;
		ReadElm = ReadInt;

		ReadElm(pElm);
		root = BSTCreate(pElm, (Elm(*)())CreateInt,
			   (void(*)(Elm,Elm))CpyInt, (int(*)(Elm,Elm))CmpInt,
			   (void(*)(Elm))FreeInt, (void(*)(Elm))PrintInt);
		break;
	
	case '1':
		//TREE OF POINTS;
		pElm = &rootPoint;
		ReadElm = ReadPoint;

		ReadElm(pElm);
		root = BSTCreate(pElm, (Elm(*)())CreatePoint,
			   (void(*)(Elm,Elm))CpyPoint, (int(*)(Elm,Elm))CmpPoint,
			   (void(*)(Elm))FreePoint, (void(*)(Elm))PrintPoint);
		break;
	}
	if (NULL == root)
		exit(1);
	TreeManage(root, ReadElm, pElm);
}



