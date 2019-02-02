
#ifndef  DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

typedef enum Result {FAIL, SUCCESS} Result;
#define ERROR -1;
typedef struct DynamicArr {
	void* base;
	int size;
} DynamicArr;
typedef DynamicArr* DynamicArray;

DynamicArray NewArr(int size);
int Size(DynamicArray arr);
void FreeArr(DynamicArray arr);
Result ChangeArrSize(DynamicArray arr, int newSize);
Result IncreaseArr(DynamicArray arr, int increment);
Result DecreaseArr(DynamicArray arr, int decrement);

#endif
