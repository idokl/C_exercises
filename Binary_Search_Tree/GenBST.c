/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 6
******************************************/

#include "GenBST.h"

typedef enum Side {LEFT, RIGHT, NONE, BOTH} Side;

typedef struct BST_S 
{      
	Elm val;      
	struct BST_S* left;      
	struct BST_S* right;   

	// OOP in C - not the best kind, but in this case, it isn't my fault...
	void   (*cpy_elm) (Elm, Elm);    
	int    (*cmp_elm)(Elm, Elm);    
	void   (*free_elm) (Elm);    
	Elm    (*create_elm)();
	void   (*print_elm)(Elm);
	//These fields are fixed before printing
	//number of tabs (or other 5 charactes) to print before the element
	int indent;
	//level in the BST hirarchy
	int depth;
} BST_T;  

//decleration of static functions
static void BSTRemoveElementPart2 (BST_ROOT root, Elm node);
static Side CheckSonSide(BST_T* root);
static void FreeNode(BST_T* node);
static Result DeleteFirstRoot(BST_T* root);
static void DeleteSon(BST_T* father, BST_T* son, Side sonSide);
static void DeleteSonFor1Grandson(BST_T* root, BST_T* sonToDelete,
								  Side sonSide, Side grandsonSide);
static void DeleteFatherOf2Sons(BST_T* root);
static const BST_T* FindMinScion(const BST_T* const root,
								 BST_T* *pMinScionFather);
static void FixIndentAndDepth(BST_T* root, int *pIndentDeg, int depth);
static void OrganizeBSTInRows(BST_T* root, BST_T* BSTRows[15][15],
							  int sizesOfRows[]);
static void PrintTree(BST_T* BSTRows[][15], int sizesOfRows[], int currentRow);
static void PrintRow(BST_T* row[15], int* pFormerIndent);
static void PrintTab(int counter);


/************************************************************************
* function name: BSTCreate
* The Input:
- root_val - pointer to the element of the current vertex (of the BST)
  5 POINTERS TO FUNCTIONS THAT SUITABLE TO THE ELEMENT'S TYPE:
- create_elm - function that allocate memory to new element (with same type).
- cpy_elm - function that copy element's content (=root_val) to another element
- cmp_elm - function that compare 2 "root_val"s
  (return int: 0 - if equal, positive number - if the first greater,
  negative number - if the second greater).
- free_elm - free allocated memory of the element
- print_elm - print value of the element by 5 characters.
* The output:
- BST_ROOT - pointer to BST's vertex that maintain root_val element.
* The Function operation:
  allocate memory for BST_T struct and for the element.
  initialize fields of this struct by the suitable values.
*************************************************************************/
extern BST_ROOT BSTCreate (Elm root_val, Elm  (*create_elm)(), void  (*cpy_elm)(Elm,Elm),                       
					int  (*cmp_elm)(Elm, Elm), void  (*free_elm)(Elm),
					void   (*print_elm)(Elm) ) 
{  
	BST_T *root = (BST_T*) malloc(sizeof(struct BST_S));
	if ( NULL != root ) 
	{   
		root->val = (Elm)create_elm();
		cpy_elm(root->val, root_val);  

		root->left       = NULL;   
		root->right      = NULL;   
		root->cpy_elm     = cpy_elm;    
		root->cmp_elm    = cmp_elm;    
		root->free_elm     = free_elm;   
		root->create_elm = create_elm;
		root->print_elm = print_elm;
	}
		else
			return NULL;
	return root; 
}


/************************************************************************
* function name: BSTDestroy
* The Input: root (of the tree that we want to delete)
* The output: none
* The Function operation: free allocated memory of all nodes of the
  tree, recursively.
*************************************************************************/
extern void BSTDestroy (BST_ROOT root)
{
	//free the left sub-tree
	if (NULL != ((BST_T*)root)->left)
		BSTDestroy(((BST_T*)root)->left);
	//free the right sub-tree
	if (NULL != ((BST_T*)root)->right)
		BSTDestroy(((BST_T*)root)->right);
	//free the current node (both element and its BST_T struct)
	((BST_T*)root)->free_elm(((BST_T*)root)->val);
	free(root);
}


/************************************************************************
* function name: BSTFindElement
* The Input: root (of the Binary Serach Tree that we want to search in)
			 val - element with value that we want to find in the tree.
* The output: the found element (if found) or NULL (if not found).
* The Function operation: compare val to values of nodes in the tree
  that may contain this value, recursively.
*************************************************************************/
extern Elm BSTFindElement(BST_ROOT root, Elm val)  
{  
	int cmp;
	//if not found at all.
	if (NULL == root)
		return NULL;
	//comparison
	cmp = ((BST_T *)root)->cmp_elm(val, ((BST_T *)root)->val);
	//if found
	if (0 == cmp)
		return ((BST_T*)root)->val;
	//if not found so far, continue to search.
	else if (cmp < 0)
		return BSTFindElement( ((BST_T *)root)->left, val );
	else if (cmp > 0)             
		return BSTFindElement( ((BST_T *)root)->right, val );
	//We have already covered all options.
	printf("BSTFindElement: I think we will never arrive to here");
	return NULL; 
}


/************************************************************************
* function name: BSTAddElement
* The Input: root (of the Binary Serach Tree that we want to increase)
			 node - element that we want to add to the tree.
* The output: result of the adding try:
			  SUCCESS - if the adding succeed
			  OUT_OF_MEM - if the adding didn't execute because lack of memory
* The Function operation: find the suitable place to enter the new leaf
						  and add it, if there is enough memory.
*************************************************************************/
extern Result BSTAddElement (BST_ROOT root, Elm node)
{
	BST_T* pNode;
	int cmp = ((BST_T*)root)->cmp_elm(node, ((BST_T*)root)->val);
	if (cmp < 0)
	{
		if (((BST_T*)root)->left == NULL)
		{
			pNode = (BST_T*)BSTCreate(node, ((BST_T*)root)->create_elm,
				    ((BST_T*)root)->cpy_elm, ((BST_T*)root)->cmp_elm,
					((BST_T*)root)->free_elm, ((BST_T*)root)->print_elm);
			if (NULL == pNode)
				return OUT_OF_MEM;
			else if (NULL == pNode->val)
			{
				((BST_T *)root)->free_elm(((BST_T *)root)->val);
				return OUT_OF_MEM;
			}
			((BST_T*)root)->left = pNode;
			return SUCCESS;
		}
		else
			return ( BSTAddElement (((BST_T*)root)->left, node) );
	}
	else if (cmp > 0)
	{
		if (((BST_T*)root)->right == NULL)
		{
			pNode = (BST_T*)BSTCreate(node, ((BST_T*)root)->create_elm, 
					((BST_T*)root)->cpy_elm, ((BST_T*)root)->cmp_elm, 
					((BST_T*)root)->free_elm, ((BST_T*)root)->print_elm);
			if (NULL == pNode)
				return OUT_OF_MEM;
			else if (NULL == pNode->val)
			{
				((BST_T*)root)->free_elm(((BST_T*)root)->val);
				return OUT_OF_MEM;
			}
			((BST_T*)root)->right = pNode;
			return SUCCESS;
		}
		else
			return ( BSTAddElement (((BST_T *)root)->right, node) );
	}
	//printf("BSTAddElement: This Node has already exists in the tree\n");
	return BAD_ARGS;
} 

/************************************************************************
* function name: BSTRemoveElement
* The Input: root (of tree that we want to delete node from)
			 node - element that we want to delete from the tree
* The output: FAILURE- the node doesn't exist in the tree
			  NULL_TREE- if the last node in the tree was deleted.
			  SUCCESS- the node was deleted and other nodes remain in the tree.
* The Function operation:
  find node in the tree and call to function to delete it.
  This part checks whether the node exists.
  If exists, it checks whether the node is the first root of the tree
  and calls to suitable function to delete it accordingly.
*************************************************************************/
extern Result BSTRemoveElement (BST_ROOT root, Elm node)
{  
	int cmp;
	//checks whether the node exists. if not - return FAILURE
	if (NULL == BSTFindElement(root, node))
		return FAILURE;
	//checks whether the node is the first root. If yes, call DeleteFirstRoot
	cmp = ((BST_T *)root)->cmp_elm(node, ((BST_T *)root)->val);
	if (0 == cmp)
		return( DeleteFirstRoot((BST_T*)root) );
	//else, call to BSTRemoveElementPart2 to find the node and delete it.
	else
	{
		BSTRemoveElementPart2(root,node);
		return SUCCESS;
	}
}


/************************************************************************
* function name: BSTRemoveElementPart2
* The Input: "root" (of tree that we want to delete node from)
			 "node" - element that we want to delete from the tree,
					It isn't the element of "root".
* The output: none
* The Function operation: Find node in the tree (follow the node's father).
  Then, call to function to delete it.
*************************************************************************/
static void BSTRemoveElementPart2 (BST_ROOT root, Elm node)
{  
	//variables
	int cmp, cmpSon;
	Side sonSide;
	BST_T* son;

	/*
	PART ONE:
	determine which side of the tree contains the son that we want to delete.
	*/
	cmp = ((BST_T *)root)->cmp_elm(node, ((BST_T *)root)->val);
	if (cmp < 0)
	{
		sonSide = LEFT;
		son = ((BST_T *)root)->left;
	}
	else if (cmp > 0)
	{
		sonSide = RIGHT;
		son = ((BST_T *)root)->right;
	}
	
	/*
	PART TWO:
	if son of the current root is the node that we want to delete - delete it,
	else - continue to search.
	*/
	cmpSon = son->cmp_elm(node, son->val);	
	if (0 == cmpSon)
		DeleteSon((BST_T*)root, son, sonSide);
	else
		BSTRemoveElement (son, node);
}


/************************************************************************
* function name: CheckSonSide
* The Input: root (BST node)
* The output: number of sons of "root", side of its son (NONE/LEFT/RIGHT/BOTH)
* The Function operation: check existence of root->left and root->right
*************************************************************************/
static Side CheckSonSide(BST_T* root)
{
	Side sonSide;

	//CASE 1: root doesn't have sons.
	if ( (NULL == (root->left)) && (NULL == (root->right)) )
		sonSide = NONE;
	//CASE 2: root has only one son.
	else if ( (NULL == (root->left)) && (NULL != (root->right)) )
		sonSide = RIGHT;
	else if ( (NULL != (root->left)) && (NULL == (root->right)) )
		sonSide = LEFT;
	//CASE 3: root has two sons.
	else if( (NULL != (root->left)) && (NULL != (root->right)) )
		sonSide = BOTH;
	
	return sonSide;
}


/************************************************************************
* function name: FreeNode
* The Input: node (pointer to BST_T struct!)
* The output: none
* The Function operation: free element of "node" and free its struct
*************************************************************************/
static void FreeNode(BST_T* node)
{
	node->free_elm(node->val);
	free(node);
}


/************************************************************************
* function name: DeleteFirstRoot
* The Input: root of the entire tree
* The output: result: SUCCESS - if other nodes remain in tree after deletion.
					  NULL_TREE - if the last node in the tree was deleted.
* The Function operation: check how many sons does "root" have and delete root
accordingly: 0 sons - just free root.
			 1 son - copy son's values to root and free the son.
			 2 sons - call DeleteFatherOf2Sons function.
*************************************************************************/
static Result DeleteFirstRoot(BST_T* root)
{
	//variables
	Side sonSide;
	BST_T* son;
	//******PART ONE: CHECK SON SIDE******
	sonSide = CheckSonSide(root);
	//******PART TWO: DELETE ROOT******
	switch (sonSide)
	{
	//CASE 1: there isn't sons (free root).
	case NONE:
		FreeNode(root);
		//return value of CASE 1:
		return NULL_TREE;
	//CASE 2: there is 1 son (copy son's values to root and free son).
	case RIGHT:
	case LEFT:
		if (RIGHT == sonSide)
			son = root->right;
		else if (LEFT == sonSide)
			son = root->left;
		root->cpy_elm(root->val,son->val);
		root->left = son->left;
		root->right = son->right;
		FreeNode(son);
		break;
	//CASE 3: there are 2 sons (call DeleteFatherOf2Sons function).
	case BOTH:
		DeleteFatherOf2Sons(root);
		break;
	default:
		printf("mistake in DeleteFirstRoot function");
		break;
	}
	//return value of CASE 2 and CASE 3:
	return SUCCESS;
}


/************************************************************************
* function name: DeleteSon
* The Input: father - father of "son".
			 son - node (BST_T*) that we want to delete
* The output: none
* The Function operation: delete the BST_T* of "son" from the tree.
*************************************************************************/
static void DeleteSon(BST_T* father, BST_T* son, Side sonSide)
{
	Side grandsonSide;

	//******PART ONE: CHECK GRANDSON SIDE******
	grandsonSide = CheckSonSide(son);

	//******PART TWO: DELETE SON******
	switch (grandsonSide)
	{
	//CASES 1-2: there is 0 or 1 grandson from the son.
	case NONE:
	case RIGHT:
	case LEFT:
		DeleteSonFor1Grandson(father, son, sonSide, grandsonSide);
		break;
	//CASE 3: there are 2 grandsons from the son.
	case BOTH:
		DeleteFatherOf2Sons(son);
		break;
	default:
		printf("mistake in DeleteSon function");
		break;
	}
}


/************************************************************************
* function name: DeleteSonFor1Grandson
* The Input: root - father of "son".
			 sonToDelete - node (BST_T*) that we want to delete
			 sonSide - is "son" the LEFT son of root or the RIGHT son of it?
			 grandsonSide - does the struct "son" have LEFT son, RIGHT son
							or neither?
* The output: none
* The Function operation: delete the item "son" from he tree.
  This function suitable to cases that the deleted item has only 1 son
  at the most (suitable also to case that the deleted item doesn't have sons).
  Before the deletion, this function save the addresss of the son.
  It links the father of the deleted item ("root") to the son of the deleted
  item ("grandson").
*************************************************************************/
static void DeleteSonFor1Grandson(BST_T* root, BST_T* sonToDelete, Side sonSide,
								  Side grandsonSide)
{
	BST_T* grandson;
	//part 1: set grandson according to grandsonSide value.
	switch(grandsonSide)
	{
	case LEFT:
		grandson = sonToDelete->left;
		break;
	case RIGHT:
		grandson = sonToDelete->right;
		break;
	case NONE:
		grandson = NULL;
		break;
	}
	//part 2: delete son
	FreeNode(sonToDelete);
	//part 3: link "root" to its grandson
	if (LEFT == sonSide)
		root->left = grandson;
	else if (RIGHT == sonSide)
		root->right = grandson;	
}


/************************************************************************
* function name: DeleteFatherOf2Sons
* The Input: root - item that we want to delete from its Binary Search Tree.
* The output: none
* The Function operation: change root's value to "Right-Subtree Minimum"
  value, and delete the "Right-Subtree Minimum" (because there is a duplicate)
*************************************************************************/
static void DeleteFatherOf2Sons(BST_T* root)
{
	//father of Minimal value in the Right Subtree
	BST_T* RightSubtreeMinFather = root;
	BST_T* *pRightSubtreeMinFather = &RightSubtreeMinFather;
	//side of Minimum in Right Subtree
	//(whether it's the right son or the left son of its father)
	Side rightSubtreeMinSide;
	//Minimum in Right Subtree (find it and its father)
	BST_T* rightSubtreeMin = (BST_T*)FindMinScion
							 ((root->right), pRightSubtreeMinFather);
	//copy the value of "Right-Subtree Minimum" instead of the value of root
	root->cpy_elm(root->val, rightSubtreeMin->val);
	//check whether "Right-Subtree Minimum" is RIGHT/LEFT son of its father
	if ((*pRightSubtreeMinFather)->right == rightSubtreeMin)
		rightSubtreeMinSide = RIGHT;
	else if ((*pRightSubtreeMinFather)->left == rightSubtreeMin)
		rightSubtreeMinSide = LEFT;
	//remove "Right-Subtree Minimum" (duplicate) from the right subtree.
	DeleteSon(*pRightSubtreeMinFather, rightSubtreeMin, rightSubtreeMinSide);
}


/************************************************************************
* function name: FindMinScion
* The Input: root - root of BST,
*			 pMinScionFather - pointer to father of "root"
							   pointer to maintain the father of the output
* The output: minimal leaf (scion) of the BST
* The Function operation: change pMinScionFather to point on the father of
* the minimal leaf of the BST.
* (this function should not change other values).
*************************************************************************/
static const BST_T* FindMinScion(const BST_T* const root,
								 BST_T* *pMinScionFather)
{
	//if root hasn't left son, root is minimal scion
	if (NULL == root->left)
		return root;
	//if root has left son, find minimal scion of the left son
	else
	{
		*pMinScionFather = (BST_T*)root;
		return FindMinScion(root->left, pMinScionFather);
	}
}


/************************************************************************
* function name: BSTPrint
* The Input: root of Binary Search Tree
* The output: none
* The Function operation: prepare BST to printing and print it.
  Print style: the left subtree of every root (smaller elements) will be
  printed under it in its left side.
  Its right subtree (greater elements) will be printed under it in its right
  side.
*************************************************************************/
extern void BSTPrint(BST_ROOT root)
{
	/***************************VARIABLES*******************************/
	//indent degree - number of tabs (or other 5 charactes) before the node
	int indentDeg = 0;
	int *pIndentDeg = &indentDeg;
	//number of rows to print (according to the tree depth)
	int rowsNum = 0;
	/*
	memory allocation is not fun...
	in this array: *BSTRows = pointer to row,
				   **BSTRows = pointer to BST_T in the row.
	*/
	BST_T* BSTRows[15][15] = { {NULL} };
	//array that maintain the number of the elements in every line
	int sizesOfRows[15] = {0};
	
	/****************************COMMANDS*******************************/
	//Fix "indent" and "depth" values of every BST_T struct in the BST.
	FixIndentAndDepth((BST_T*)root, pIndentDeg, 0);
	//Organize arrays of pointers to BST_Ts that have the same depth.
	OrganizeBSTInRows((BST_T*)root, BSTRows, sizesOfRows);
	//Print these rows in format of tree (according to the correct indents).
	PrintTree(BSTRows, sizesOfRows, 0);
}


/************************************************************************
* function name: FixIndentAndDepth
* The Input: 
  root - of the BST.
  pIndentDeg - pointer to "indent degree" variable that was initialized to 0
  			   It will help us to fix "indent" values of the BST_Ts.
			   (indentDeg - number of tabs (or 5 characters) that we have to
				print in the row before printing the element).
  depth - depth of "root". number of ancestors (forefathers) of root.
		  It will help us to fix "depth" values of the BST_Ts
* The output: none
* The Function operation: Fix "indent" and "depth" values of every
  BST_T struct in the BST, recursively.
  logic of the function:
  If there are N BST_T structs in the tree, they will have N different
  "indent" values: 1, 2, 3, ... , N.
  For every root (i.e. BST_T*) the following is correct:
  The "indent" value of its left subtree smaller than its own.
  The "indent" value of its right subtree greater than its own.
  The "depth" value of its sons = its "depth" value + 1.
  Firstly, this function fix the "indent" value of the leftmost root and then
  it continues to the right in an orderly manner.
  gives to every BST_T in the tree its correct "indent" and "depth" values,
  recursively. I think it's cool.
*************************************************************************/
static void FixIndentAndDepth(BST_T* root, int *pIndentDeg, int depth)
{
	//fix "indent" and "depth" of the left subtree
	if (NULL != root->left)
		FixIndentAndDepth(root->left, pIndentDeg, depth + 1);
	
	//fix "depth" value of the current "root"
	root->depth = depth;
	//fix "indent" value of the current "root"
	root->indent = *pIndentDeg;
	//increase "indentDeg" before fixing "indent" value of the next root
	(*pIndentDeg)++;
	
	//fix "indent" and "depth" of the right subtree
	if (NULL != root->right)
		FixIndentAndDepth(root->right, pIndentDeg, depth + 1);
}


/************************************************************************
* function name: OrganizeBSTInRows
* The Input: 
  root - of the BST.
  BSTRows - two-dimensional array that will contain rows of pointers to BST_Ts.
  sizesOfRows - array that will maintain the size of every row.
* The output: none
* The Function operation: 
  The address of every BST_T in the tree (i.e. root) is entered to the
  suitable sub-array (i.e. row) according to its "depth" value.
  The leftmost root of the current depth will be the first root in the 
  suitable sub-array. If there are more roots with the same depth, they
  will be second, third, fourth etc. in the correct order.
*************************************************************************/
static void OrganizeBSTInRows(BST_T* root, BST_T* BSTRows[][15],
							  int sizesOfRows[])
{
	//Put root in the first free place in the row of its depth
	*(BSTRows[root->depth]+sizesOfRows[root->depth]) = root;
	//Increase the size of this row (because new root was entered)
	(sizesOfRows[root->depth])++;

	//If root has sons, organize them in their rows, too. The left son first.
	if (NULL != root->left)
		OrganizeBSTInRows(root->left, BSTRows, sizesOfRows);
	if (NULL != root->right)
		OrganizeBSTInRows(root->right, BSTRows, sizesOfRows);

}


/************************************************************************
* function name: PrintTree
* The Input: 
  BSTRows - two-dimensional array that contains the BST in rows
  sizesOfRows - array that maintain the number of the elements in every row
  currentRow - The first row that hasn't been printed yet (0 in the beginning)
* The output: none
* The Function operation: print the BST.
  Print style: the left subtree of every root (smaller elements) will be
  printed under it in its left side.
  Its right subtree (greater elements) will be printed under it in its right
  side.
*************************************************************************/
static void PrintTree(BST_T* BSTRows[][15], int sizesOfRows[], int currentRow)
{
	//How many tabs (or 5 characters) has been already printed in the row:
	int formerIndent = 0;
	//stop condition - the current row is empty
	if (NULL == *BSTRows[currentRow])
		return;
	else
	{
		//print the current row
		PrintRow(BSTRows[currentRow], &formerIndent);
		printf("\n");
		//print the next rows
		PrintTree(BSTRows, sizesOfRows, currentRow + 1);
	}
}


/************************************************************************
* function name: PrintRow
* The Input:
  row - array of pointers to roots that have to be printed in this row.
  pFormerIndent - pointer to variable that was initialized to 0.
				  It maintains the number of the former indents that has
				  been already printed.
* The output: none
* The Function operation: print row of the BST (and indent correctly).
*************************************************************************/
static void PrintRow(BST_T* row[15], int* pFormerIndent)
{
	
	if (NULL != row[0])
	{
		/*
		print tabs according to the "indent" value of the current BST_T and
		according to the number of indents that has been printed so far. 
		*/
		PrintTab(row[0]->indent - *pFormerIndent);
		//increase formerIndent (= "indent" before this node + this node)
		*pFormerIndent = row[0]->indent + 1;
		//print the current node
		row[0]->print_elm(row[0]->val);
		//print rest of the nodes in this row
		PrintRow(row + 1, pFormerIndent);
	}
	//stop condition - the current cell of the row array is empty.
	else
		return;
}


/************************************************************************
* function name: PrintTab
* The Input: counter - number of tabs that we want to print
* The output: none
* The Function operation: print 5 spaces ("tab") some times according to
  the counter.
*************************************************************************/
static void PrintTab(int counter)
{
	if (counter > 0)
	{
		printf("     ");
		counter --;
		PrintTab(counter);
	}
	else
		return;
}