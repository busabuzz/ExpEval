/* 
 * <expression>   ::= <term> { '+' <term> | '-' <term> }
 *              
 * <term>      ::= <factor> { '*' <factor> | '/' <factor> }
 *
 * <factor> ::= <natnum> | <identifier> | '('<expression>')' 
 *
 * Starting point is the token list obtained from the scanner (in scanner.c). 
 */

#include <stdio.h>  /* printf */
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include "scanner.h"
#include "recognizeExp.h"
#include "infixExp.h"
#include <string.h>

/* The function newExpTreeNode creates a new node for an expression tree.
 */

ExpTree newExpTreeNode(TokenType tt, Token t, ExpTree tL, ExpTree tR) {
  ExpTree new = malloc(sizeof(ExpTreeNode));
  assert (new!=NULL);
  new->tt = tt;
  new->t = t;
  new->left = tL;
  new->right = tR;
  return new;
}

/* The function valueIdentifier recognizes an identifier in a token list and
 * makes the second parameter point to it.
 */

int valueIdentifier(List *lp, char **sp) {
  if (*lp != NULL && (*lp)->tt == Identifier ) {
    *sp = ((*lp)->t).identifier;
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

/* The function valueOperator recognizes an arithmetic operator in a token list
 * and makes the second parameter point to it.
 * Here the auxiliary function isOperator is used.
 */

int isOperator(char c) {
	if ( c == '+' || c == '-' ) return 1;
	if ( c == '*' || c == '/' ) return 2;
	return 0;
}

int valueOperator(List *lp, char *cp) {
	if ( *lp != NULL && (*lp)->tt == Symbol ) {
		switch ( isOperator(((*lp)->t).symbol) ) {
			case 1:
				*cp = ((*lp)->t).symbol;
				return 1;
			case 2:
				*cp = ((*lp)->t).symbol;
				return 2;
			case 0:
				return 0;
		}
	}
	return 0;
}

int valueNumber(List *lp, double *wp) {
	if (*lp != NULL && (*lp)->tt == Number ) {
		*wp = ((*lp)->t).number;
		*lp = (*lp)->next;
		return 1;
	}
	return 0;
}

/* De functie freeExpTree frees the memory of the nodes in the expression tree.
 * Observe that here, unlike in freeList, the strings in indentifier nodes
 * are not freed. The reason is that the function newExpTree does not allocate
 * memory for strings in nodes, but only a pointer to a string in a node
 * in the token list.
 */

void freeExpTree(ExpTree tr) {
  if (tr==NULL) {
    return;
  }
  freeExpTree(tr->left);
  freeExpTree(tr->right);
  free(tr);
}

/* The function treeExpression tries to build a tree from the tokens in the token list 
 * (its first argument) and makes its second argument point to the tree.
 * The return value indicates whether the action is successful.
 * Observe that we use ordinary recursion, not mutual recursion.
 */

int valueExpression(List *tempList, ExpTree *tp) {
	Token t;
	ExpTree tR;
	char c;
	if ( !valueTerm(tempList, tp) ) {
		return 0;
	} 
	while ( valueOperator(tempList, &c) == 1 ) {
		(*tempList) = (*tempList)->next;
		t.symbol = c;
		if(valueTerm(tempList, &tR)) {		
			*tp = newExpTreeNode(Symbol, t, *tp, tR);
		} else {	
			return 0;
		}
	} 
	return 1;	
}

int valueTerm(List *tempList, ExpTree *tp) {
	Token t;
	ExpTree  tL, tR;
	char c;
	if ( !valueFactor(tempList, tp) ) {
		return 0;
	} 	
	while ( valueOperator(tempList, &c) == 2 ) {
		(*tempList) = (*tempList)->next;
		t.symbol = c;
		if(valueFactor(tempList, &tR)) {
			tL = *tp;
			*tp = newExpTreeNode(Symbol, t, tL, tR);
		} else {		
			return 0;
		}
	} 
	return 1;
}

int valueFactor(List *tempList, ExpTree *tp) {
	double w;
	char *s;
	Token t;
	
	if ( valueNumber(tempList, &w) ) {
		t.number = (int)w;
		*tp = newExpTreeNode(Number, t, NULL, NULL);
		return 1;
	}
	
	if ( valueIdentifier(tempList, &s) ) {
		t.identifier = s;
		*tp = newExpTreeNode(Identifier, t, NULL, NULL);
		return 1;
	}
	
	if ( acceptCharacter(tempList,'(') && valueExpression(tempList, tp) && acceptCharacter(tempList,')') ) {	
		return 1;
	}
	return 0;
}

void printExpTreeInfix(ExpTree tr) {
  if (tr == NULL) {
    return;
  }
  switch (tr->tt) {
  case Number: 
    printf("%d",(tr->t).number);
   break;
  case Identifier: 
    printf("%s",(tr->t).identifier);
    break;
  case Symbol: 
    printf("(");
    printExpTreeInfix(tr->left);
    printf(" %c ",(tr->t).symbol);
    printExpTreeInfix(tr->right);
    printf(")");
    break;
  }
}

/* The function isNumerical checks for an expression tree whether it represents 
 * a numerical expression, i.e. without identifiers.
 */

int isNumerical(ExpTree tr) {
  assert(tr!=NULL);
  if (tr->tt==Number) {
    return 1;
  }
  if (tr->tt==Identifier) {
    return 0;
  }
  return (isNumerical(tr->left) && isNumerical(tr->right));
}

/* The function valueExpTree computes the value of an expression tree that represents a
 * numerical expression.
 */

double valueExpTree(ExpTree tr) {  /* precondition: isNumerical(tr)) */
  double lval, rval;
  assert(tr!=NULL);
  if (tr->tt==Number) {
    return (tr->t).number;
  }
  lval = valueExpTree(tr->left);
  rval = valueExpTree(tr->right);
  switch ((tr->t).symbol) {
  case '+':
    return (lval + rval);
  case '-':
    return (lval - rval);
  case '*':
    return (lval * rval);
  case '/':
    assert( rval!=0 );
    return (lval / rval);
  default:
    abort();
  }
}

void simplify(ExpTree *tree) {	
	if(simplifyLevel(tree)) {
		simplify(tree);
	}
}

int simplifyLevel(ExpTree *tree) {
	ExpTree tempTree;
	Token tempToken;
	/* multiply  */
	if ( *tree == NULL ) return 0;
	if ( (*tree)->tt != Symbol ) return 0;
	if ( ((*tree)->t.symbol == '*') ) {
		/* 0 * E, E * 0 = 0 */
		if ( ((((*tree)->left)->tt == Number) && ((((*tree)->left)->t).number == 0)) || ((((*tree)->right)->tt == Number) && ((((*tree)->right)->t).number == 0)) ) {
			freeExpTree((*tree));
			tempToken.number = 0;
			*tree = newExpTreeNode(Number, tempToken , NULL, NULL);
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
		/* 1 * E = E*/
		if ( (((*tree)->left)->tt == Number) && ((((*tree)->left)->t).number == 1) ) {
			tempTree = newExpTreeNode(((*tree)->right)->tt, ((*tree)->right)->t, ((*tree)->right)->left, ((*tree)->right)->right);
			(*tree)->right->left = NULL;
			(*tree)->right->right = NULL;
			freeExpTree((*tree));
			*tree = tempTree;
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
		/* E * 1 = E */
		if ( (((*tree)->right)->tt == Number) && ((((*tree)->right)->t).number == 1) ) {
			tempTree = newExpTreeNode(((*tree)->left)->tt, (((*tree)->left)->t), ((*tree)->left)->left, ((*tree)->left)->right );
			(*tree)->left->left = NULL;
			(*tree)->left->right = NULL;
			freeExpTree((*tree));
			*tree = tempTree;
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
	}
	/* divide */		
	if ((*tree)->t.symbol == '/') {
		/* 0 / E = 0 */
		if ( (((*tree)->left)->tt == Number) && (((*tree)->left)->t.number == 0) ) {
			freeExpTree((*tree));
			tempToken.number = 0;
			*tree = newExpTreeNode(Number, tempToken, NULL, NULL);
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
		/* E / 1  = E */
		if ( (((*tree)->right)->tt == Number) && (((*tree)->right)->t.number == 1) ) {
			tempTree = newExpTreeNode(((*tree)->left)->tt, ((*tree)->left)->t, ((*tree)->left)->left, ((*tree)->left)->right);
			(*tree)->left->left = NULL;
			(*tree)->left->right = NULL;
			freeExpTree((*tree));
			*tree = tempTree;
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
	}
	/* plus */
	if ( ((*tree)->t.symbol == '+') ) {
		/* 0 + E = E */
		if (((*tree)->left)->tt == Number && ((*tree)->left)->t.number == 0) {
			tempTree = newExpTreeNode(((*tree)->right)->tt, (((*tree)->right)->t), ((*tree)->right)->left, ((*tree)->right)->right);
			(*tree)->right->left = NULL;
			(*tree)->right->right = NULL;
			freeExpTree(*tree);
			*tree = tempTree;
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
		/* E + 0 = E */
		if (((*tree)->right)->tt == Number && (((*tree)->right)->t).number == 0) {
			tempTree = newExpTreeNode(((*tree)->left)->tt, (((*tree)->left)->t), ((*tree)->left)->left, ((*tree)->left)->right);
			(*tree)->left->left = NULL;
			(*tree)->left->right = NULL;
			freeExpTree(*tree);
			*tree = tempTree;
			if(simplifyLevel(tree)) simplify(tree);
			return 1;
		}
	}
	/* minus, so E - 0 = E */
	if ( (((*tree)->t).symbol == '-') && (((*tree)->right)->tt == Number) && (((*tree)->right)->t.number == 0) ) {
		tempTree = newExpTreeNode(((*tree)->left)->tt, (((*tree)->left)->t), ((*tree)->left)->left, ((*tree)->left)->right);
		(*tree)->left->left = NULL;
		(*tree)->left->right = NULL;
		freeExpTree(*tree);
		*tree = tempTree;
		if(simplifyLevel(tree)) simplify(tree);
		return 1;
	}
	if ( simplifyLevel(&((*tree)->left)) ) return 1;
	if ( simplifyLevel(&((*tree)->right)) ) return 1;
	return 0;
}

/* the function prefExpressionExpTrees performs a dialogue with the user and tries
 * to recognize the input as a prefix expression. When it is a numerical prefix 
 * expression, its value is computed and printed.
 */ 
 
ExpTree makeTreeCopy(ExpTree *tp) {	
	ExpTree tempTree = NULL;
	if ( *tp != NULL ) {
		tempTree = newExpTreeNode((*tp)->tt, (*tp)->t, makeTreeCopy(&(*tp)->left), makeTreeCopy(&(*tp)->right));
	}
	return tempTree;
}

int differentiate(ExpTree *tp, char *identifier) {
	ExpTree copyLeft = NULL, copyRight = NULL, diffCopyLeft = NULL, diffCopyRight = NULL;
	Token t;
	if ( (*tp)->tt == Number ) {
		((*tp)->t).number = 0;
	}
	if ( ((*tp)->tt == Identifier) && (strcmp((*tp)->t.identifier, identifier)==0) ) {
		freeExpTree(*tp);
		t.number = 1;
		(*tp) = newExpTreeNode(Number, t, NULL, NULL);
	}
	if ( (*tp)->tt == Symbol ) {
		switch ((*tp)->t.symbol) {
			case '+':
				differentiate(&(*tp)->left, identifier);
				differentiate(&(*tp)->right, identifier);
				break;
			
			case '-':
				differentiate(&(*tp)->left, identifier);
				differentiate(&(*tp)->right, identifier);
				break;
			
			case '*':
				copyLeft = makeTreeCopy(&(*tp)->left);
				copyRight = makeTreeCopy(&(*tp)->right);
				diffCopyLeft = makeTreeCopy(&copyLeft);
				diffCopyRight = makeTreeCopy(&copyRight);
				differentiate(&diffCopyLeft, identifier);
				differentiate(&diffCopyRight, identifier);
				freeExpTree(*tp);
				t.symbol = '+';
				(*tp) = newExpTreeNode(Symbol, t, NULL, NULL);
				t.symbol = '*';
				(*tp)->left = newExpTreeNode(Symbol, t, makeTreeCopy(&diffCopyLeft), makeTreeCopy(&copyRight));
				(*tp)->right = newExpTreeNode(Symbol, t, makeTreeCopy(&copyLeft), makeTreeCopy(&copyRight));
				freeExpTree(copyLeft);
				freeExpTree(copyRight);
				freeExpTree(diffCopyLeft);
				freeExpTree(diffCopyRight);
				break;
			
			case '/':
				copyLeft = makeTreeCopy(&(*tp)->left);
				copyRight = makeTreeCopy(&(*tp)->right);
				diffCopyLeft = makeTreeCopy(&copyLeft);
				diffCopyRight = makeTreeCopy(&copyRight);
				differentiate(&diffCopyLeft, identifier);
				differentiate(&diffCopyRight, identifier);
				t.symbol = '/';
				freeExpTree(*tp);
				(*tp) = newExpTreeNode(Symbol, t, NULL, NULL);
				t.symbol = '-';
				(*tp)->left = newExpTreeNode(Symbol, t, NULL, NULL);		
				t.symbol = '*';
				((*tp)->left)->left = newExpTreeNode(Symbol, t, makeTreeCopy(&diffCopyLeft), makeTreeCopy(&copyRight));
				((*tp)->left)->right = newExpTreeNode(Symbol, t, makeTreeCopy(&copyLeft), makeTreeCopy(&diffCopyRight));
				(*tp)->right = newExpTreeNode(Symbol, t, makeTreeCopy(&copyRight), makeTreeCopy(&copyRight));
				freeExpTree(copyLeft);
				freeExpTree(copyRight);
				freeExpTree(diffCopyLeft);
				freeExpTree(diffCopyRight);
				break;
		}
	}
	simplify(tp);
	return 0;
}

void infixExpTrees() {
	char *ar;
	List tl, tl1;  
	ExpTree t = NULL; 
	printf("give an expression: ");
	ar = readInput();
	while (ar[0] != '!') {
		tl = tokenList(ar); 
		printList(tl);
		tl1 = tl;
		if ( valueExpression(&tl1,&t) && tl1 == NULL ) { 
			/* there should be no tokens left */
			printf("in infix notation: ");
			printExpTreeInfix(t);
			printf("\n");
			if ( isNumerical(t) ) {
				printf("the value is %g\n",valueExpTree(t));
			} else {
				printf("this is not a numerical expression\n");
			}
			simplify(&t);
			printf("simplified: ");
			printExpTreeInfix(t);
			printf("\nderivative to x: ");
			differentiate(&t,"x\0");
			simplify(&t);
			printExpTreeInfix(t);
			printf("\n");
			
			
		} else {
			printf("this is not an expression\n"); 
		}
		freeExpTree(t);
		t = NULL; 
		freeTokenList(tl);  
		free(ar);
		printf("\ngive an expression: ");
		ar = readInput();
	}
	free(ar);
	printf("good bye\n");
}
