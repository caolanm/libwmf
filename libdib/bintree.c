/*
Caolan.McNamara@ul.ie
http://www.csn.ul.ie/~caolan

Released under the GPL, see COPYING
*/

#include <stdio.h>
#include <stdlib.h>

#include "bintree.h"

void InitBintree(BintreeInfo *tree,int (*func1)(void *,void *),int (*func2)(void *,void *))
	{
	tree->Root = NULL;               /* root of binary tree */
	tree->no_in_tree=0;
	tree->CompLT = func1;
	tree->CompEQ = func2;
	}

Node *InsertNode(BintreeInfo *tree,void *Data) {
    Node *X, *Current, *Parent;

   /***********************************************
    *  allocate node for Data and insert in tree  *
    ***********************************************/

    /* find X's parent */
    Current = tree->Root;
    Parent = 0;
    while (Current) {
        if (tree->CompEQ(Data, Current->Data)) return (NULL);
        Parent = Current;
        Current = tree->CompLT(Data, Current->Data) ?
            Current->Left : Current->Right;
    }

    /* setup new node */
    if ((X = (Node *)malloc (sizeof(Node))) == 0) {
        fprintf (stderr, "insufficient memory (InsertNode)\n");
        exit(1);
    }
	tree->no_in_tree++;
    X->Data = Data;
    X->Parent = Parent;
    X->Left = NULL;
    X->Right = NULL;

    /* insert X in tree */
    if(Parent)
        if(tree->CompLT(Data, Parent->Data))
            Parent->Left = X;
        else
            Parent->Right = X;
    else
        tree->Root = X;

    return(X);
}


void DeleteNode(BintreeInfo *tree,Node *Z) 
	{
    Node *X, *Y;

   /*****************************
    *  delete node Z from tree  *
    *****************************/

    /* Y will be removed from the parent chain */
    if (!Z || Z == NULL) return;


	tree->no_in_tree--;

    /* find tree successor */
    if (Z->Left == NULL || Z->Right == NULL)
        Y = Z;
    else {
        Y = Z->Right;
        while (Y->Left != NULL) Y = Y->Left;
    }

    /* X is Y's only child */
    if (Y->Left != NULL)
        X = Y->Left;
    else
        X = Y->Right;

    /* remove Y from the parent chain */
    if (X) X->Parent = Y->Parent;
    if (Y->Parent)
        if (Y == Y->Parent->Left)
            Y->Parent->Left = X;
        else
            Y->Parent->Right = X;
    else
        tree->Root = X;

    /* Y is the node we're removing */
    /* Z is the data we're removing */
    /* if Z and Y are not the same, replace Z with Y. */
    if (Y != Z) {
        Y->Left = Z->Left;
        if (Y->Left) Y->Left->Parent = Y;
        Y->Right = Z->Right;
        if (Y->Right) Y->Right->Parent = Y;
        Y->Parent = Z->Parent;
        if (Z->Parent)
            if (Z == Z->Parent->Left)
                Z->Parent->Left = Y;
            else
                Z->Parent->Right = Y;
        else
            tree->Root = Y;
        free (Z);
    } else {
        free (Y);
    }
}

Node *FindNode(BintreeInfo *tree,void *Data) 
	{

   /*******************************
    *  find node containing Data  *
    *******************************/

    Node *Current = tree->Root;
    while(Current != NULL)
        if(tree->CompEQ(Data, Current->Data))
            return (Current);
        else
            Current = tree->CompLT (Data, Current->Data) ?
                Current->Left : Current->Right;
    return(0);
}


Node *NextNode(BintreeInfo *tree,Node *element)
	{
	Node *father_of_element;
	Node *father_of_forefather;

	if (!element)
		{
		father_of_element = tree->Root;
		if (father_of_element != NULL)
			while (father_of_element->Left != NULL)
				father_of_element = father_of_element->Left;
		}
	else
		{
		father_of_element = element;
		if (father_of_element->Right != NULL)
			{
			father_of_element = father_of_element->Right;
			while (father_of_element->Left != NULL)
				father_of_element = father_of_element->Left;
			}
		else
			{
            father_of_forefather = father_of_element->Parent;
            while (father_of_forefather && (father_of_forefather->Right == father_of_element))
                {
                father_of_element = father_of_forefather;
            	father_of_forefather = father_of_element->Parent;
        		}
            father_of_element = father_of_forefather;
			}
		}
	return father_of_element;
	}
