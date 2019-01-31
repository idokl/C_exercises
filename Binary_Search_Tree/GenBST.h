/******************************************
* Ido Klein
* 203392964
* 8911004
* Exercise 6
******************************************/

#ifndef GENBST_H
#define GENBST_H  

#include <stdio.h>
#include <stdlib.h>  

typedef void* Elm; 
typedef void* BST; 
typedef void* BST_ROOT;  


typedef enum {SUCCESS, OUT_OF_MEM, BAD_ARGS, FAILURE, NULL_TREE } Result;  

BST_ROOT BSTCreate( Elm root_val,
					Elm    (*create_elm)(),        
					void   (*cpy_elm) (Elm,Elm),        
					int    (*cmp_elm) (Elm, Elm),        
					void   (*free_elm)(Elm),
					void   (*print_elm)(Elm) ); 
void      BSTDestroy       (BST_ROOT root); 
Result    BSTAddElement    (BST_ROOT root, Elm node); 
Result    BSTRemoveElement (BST_ROOT root, Elm node); 
Elm       BSTFindElement   (BST_ROOT root, Elm node);

void	  BSTPrint			   (BST_ROOT root);
#endif 

