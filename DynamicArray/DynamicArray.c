
#include "DynamicArray.h"
#include <malloc.h>
#include <stdio.h>



//return DynamicArray (or 0 if failed)
DynamicArray NewArr(int size) {
	/*
	int debug = sizeof(void*);
	int debug2 = sizeof(int*);
	int debug3 = sizeof(int);
	int debug4 = sizeof(DynamicArr);
	*/
	void* pointerToDynamicArrBase = malloc(size * sizeof(void*));
	DynamicArr* pDynamicArr = (DynamicArr*) malloc(sizeof(DynamicArr));
	if (pointerToDynamicArrBase == 0 || pDynamicArr == 0) {
		return 0;
	}
	(*pDynamicArr).base = pointerToDynamicArrBase;
	(*pDynamicArr).size = size;
	return pDynamicArr;
}

int Size(DynamicArray arr) {
	if (arr == 0) {
		printf("ERROR (of the function 'Size'): the argument 'arr' is not a valid DynamicArray");
		return 0;
	}
	return (*(DynamicArr*)arr).size;
}

void FreeArr(DynamicArray arr) {
	if (arr == 0) {
		printf("ERROR (of the function 'FreeArr'): the argument 'arr' is not a valid DynamicArray");
		return;
	}
	free((*(DynamicArr*)arr).base);
	free(arr);
}

#if 0
Result IncreaseArr(DynamicArray arr, int increment) {
	void* oldBase = (*(DynamicArr*)arr).base;
	int newSize = Size(arr) + increment;
	void* pointerToNewDynamicArrBase = realloc(oldBase, newSize * sizeof(void*));
	if (pointerToNewDynamicArrBase == 0) {
		return FAIL;
	}
	//if succeed, update the fields of the DynamicArray:
	(*(DynamicArr*)arr).base = pointerToNewDynamicArrBase;
	(*(DynamicArr*)arr).size = newSize;
	return SUCCESS;
}


Result DecreaseArr(DynamicArray arr, int decrement) {
	void* oldBase = (*(DynamicArr*)arr).base;
	int newSize = Size(arr) - decrement;
	void* pointerToNewDynamicArrBase = realloc(oldBase, newSize * sizeof(void*));
	if (pointerToNewDynamicArrBase == 0) {
		return FAIL;
	}
	//if succeed, update the fields of the DynamicArray:
	(*(DynamicArr*)arr).base = pointerToNewDynamicArrBase;
	(*(DynamicArr*)arr).size = newSize;
	return SUCCESS;
}
#endif

Result IncreaseArr(DynamicArray arr, int increment) {
	int newSize = Size(arr) + increment;
	return(ChangeArrSize(arr, newSize));
}

Result DecreaseArr(DynamicArray arr, int decrement) {
	int newSize = Size(arr) - decrement;
	return(ChangeArrSize(arr, newSize));
}

Result ChangeArrSize(DynamicArray arr, int newSize) {
	void* oldBase = (*(DynamicArr*)arr).base;
	void* pointerToNewDynamicArrBase = realloc(oldBase, newSize * sizeof(void*));
	if (pointerToNewDynamicArrBase == 0) {
		return FAIL;
	}
	//if succeed, update the fields of the DynamicArray:
	(*(DynamicArr*)arr).base = pointerToNewDynamicArrBase;
	(*(DynamicArr*)arr).size = newSize;
	return SUCCESS;
}