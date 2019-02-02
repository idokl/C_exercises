
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "DynamicArray.h"

void main() {
	int i;
	DynamicArray arr1 = NewArr(23);
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;
	int e = 5;
	((int*)((*arr1).base))[0] = a;
	((int*)(arr1->base))[1] = b;
	((int*)(arr1->base))[2] = c;
	((int*)(arr1->base))[3] = d;
	((int*)(arr1->base))[4] = e;
	
	for (i = 0; i < 21; i++) {
		(((int *)(arr1->base))[i]) = 90;
		//(((int*)((*arr1).base))[i]) = 90;
	}
	
	//scanf(" %d",&((int*)arr1)[0]);
	
	//printf("%d",((int*)arr1)[0]);

	getchar();
	getchar();
	
}
