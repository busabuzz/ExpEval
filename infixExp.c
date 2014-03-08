/* 
 * <expression>   ::= <term> { '+' <term> | '-' <term> }
 *              
 * <term>      ::= <factor> { '*' <factor> | '/' <factor> }
 *
 * <factor> ::= <natnum> { <identifier> | '('<expression>')' }
 *
 * Starting point is the token list obtained from the scanner (in scanner.c). 
 */

#include <stdio.h>  /* printf */
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include "scanner.h"
#include "recognizeExp.h"
#include "evalExp.h"
#include "prefixExp.h"

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
  return ( c == '+' || c == '-' || c == '*' || c == '/');
}

int valueOperator(List *lp, char *cp) {
  if (*lp != NULL && (*lp)->tt == Symbol && isOperator(((*lp)->t).symbol) ) {
    *cp = ((*lp)->t).symbol;
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
	if ( !valueTerm ( templist, tp ) ) {
		return 0;
	} 
	
	while(templist != NULL) {
		if( valueOperator(templist, &c) ) {
			if ( valueTerm(lp, &w) ) {
					
			} else {
				return 0;
			} 
		} else {
			return 0;
		}
	}
	return 1;
}

int valueTerm(List *tempList, ExpTree *tp) {
	char c;
	if ( !valueFactor(templist, tp) ) {
		return 0;
	}
	
	while(templist != NULL) {
		if ( valueOperator( tempList, &c ) ) {
			if ( !valueFactor( templist, tp ) ) {
				return 0;
			} 
			
		} else {
			return 0;
		}
	}
	return 1;
}

int valueFactor(List *tempList, ExpTree *tp) {
	double w;
	char *s;
	char c;
	Token t;
	if ( valueNumber(templist, &w) ) {
		t.number = (int)w;
		*tp = newExpTreeNode(Number, t, NULL, NULL);
		return 1;
	}
	
	if ( valueIdentifier(templist, &s) ) {
		t.identifier = s;
		*tp = newExpTreeNode(Identifier, t, NULL, NULL);
		return 1;
	}
	
	if ( valueExpression(templist, tp) ) {
		return 1;
	}
	return 0;
}

int treeInfixExpression(List *lp, ExpTree *tp) { 
	double w;
	char *s;
	char c;
	List *tempList;
	templist = lp;
	Token t;
	ExpTree tL, tR;
	t.number = (int)w;
	if ( !valueTerm(templist,&w) ) {
		return 0;
	} 
	
	while(templist != NULL) {
		if ( valueOperator(templist,&c) ) {
			if ( valueTerm(lp,&w) ) {
					
			} else {
				return 0;
			} 
		} else {
			return 0;
		}
	}
	return 1;
}
	
		tr = treeInfixExpression(lp,&tl);
		*tp = newExpTreeNode(Symbol, t, tL, tR);
	} else {
		tl = treeInfixExpression(lp,&tl);
		*tp = newExpTreeNode(Number, t, NULL, NULL);
	}
	
	
	
	
	p,&s) ) {
		t.identifier = s;
		*tp = newExpTreeNode(Identifier, t, NULL, NULL);
		return 1;
	}
	if ( treeInfixExpression(lp,&tL) ) {
		if ( valueOperator(lp,&c) ) {
			if ( treeInfixExpression(lp,&tR) ) {
				t.symbol = c;
				*tp = newExpTreeNode(Symbol, t, tL, tR);
				return 1;
			} else {
				freeExpTree(tL);
				return 0;
			}
		} else {
			return 1;
		}
	}
	return 0;
}

/* The function printExpTreeInfix does what its name suggests.
 */

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

/* the function prefExpressionExpTrees performs a dialogue with the user and tries
 * to recognize the input as a prefix expression. When it is a numerical prefix 
 * expression, its value is computed and printed.
 */ 

void prefExpTrees() {
  char *ar;
  List tl, tl1;  
  ExpTree t = NULL; 
  printf("give a prefix expression: ");
  ar = readInput();
  while (ar[0] != '!') {
    tl = tokenList(ar); 
    printf("the token list is ");
    printList(tl);
    tl1 = tl;
    if ( treeInfixExpression(&tl1,&t) && tl1 == NULL ) { 
         /* there should be no tokens left */
      printf("in infix notation: ");
      printExpTreeInfix(t);
      printf("\n");
      if ( isNumerical(t) ) {
        printf("the value is %g\n",valueExpTree(t));
      } else {
        printf("this is not a numerical prefix expression\n");
      }
    } else {
      printf("this is not a prefix expression\n"); 
    }
    freeExpTree(t);
    t = NULL; 
    freeTokenList(tl);  
    free(ar);
    printf("\ngive a prefix expression: ");
    ar = readInput();
  }
  free(ar);
  printf("good bye\n");
}
