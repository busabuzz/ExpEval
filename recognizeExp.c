#include <stdio.h>  /* getchar, printf */
#include <stdlib.h> /* NULL */
#include "scanner.h"
#include "recognizeExp.h"
#include <string.h>

int acceptNumber(List *lp) {
  if (*lp != NULL && (*lp)->tt == Number) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptIdentifier(List *lp) {
  if (*lp != NULL && (*lp)->tt == Identifier ) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptCharacter(List *lp, char c) {
  if (*lp != NULL && (*lp)->tt == Symbol && ((*lp)->t).symbol == c ) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptTerm(List *lp) {
	List q;
	if ( acceptNumber(lp) ) {
		if (acceptIdentifier(lp)) {
			if (acceptCharacter(lp,'^')) {
				q = *lp;
				if ( acceptNumber(lp) ) {
					return (q->t.number);
				}
			} else {
				return 1;
			}
		} else {
			return 0;
		}
	}
    return -1;
}

int acceptExpression(List *lp) {
	if ( acceptCharacter(lp, '-') ) {
		if ( acceptTerm(lp)==-1 ) {
			return 0; 
		}
	} else {
		if ( acceptTerm(lp) ==-1 ) {
			return 0; 
		}
	}
	while ( acceptCharacter(lp,'+') || acceptCharacter(lp,'-') ) {
		if ( acceptTerm(lp) ==-1 ) {
			return 0;
		}
	} 
	return 1;
}

int equationDegree(List *lp) {
	int degree = 0, tempDegree;
	while (*lp != NULL) {
		if(!acceptCharacter(lp,'+') && !acceptCharacter(lp,'-') && !acceptCharacter(lp,'=')) {
			tempDegree = acceptTerm(lp);
			if (tempDegree>degree) degree = tempDegree;
		}
	}
	return degree;
}

int varCheck(List *lp) {
	char *iden;
	while (*lp != NULL) {
		if ((*lp)->tt == Identifier) {
			iden = (*lp)->t.identifier;
			*lp = (*lp)->next;
			while (*lp != NULL) {
				if ((*lp)->tt == Identifier) {	
					if(strcmp(((*lp)->t.identifier), iden)!=0){
						return 0;
					}
				}
				*lp = (*lp)->next;
			}
			return 1;
		}
		*lp = (*lp)->next;
    }
    return 0;
}

int acceptEquation(List *lp) {
	if ( !acceptExpression(lp) ) {
		return 0;
	}
	if ( acceptCharacter(lp,'=') ) {
		if ( !acceptExpression(lp) ) {
			return 0;
		} else {
			if ( acceptCharacter(lp,'=') ) {
				return 0;
			}
		}
    } else {
		return 0;
	}
	return 1;
}

void recognizeEquation() {
	char *ar;
	List tl, tl1;  
	printf("give an equation: ");
	ar = readInput();
	while (ar[0] != '!') {
		tl = tokenList(ar); 
		printList(tl);
		tl1 = tl;
		if ( acceptEquation(&tl1) ) {
			tl1 = tl;
			if(varCheck(&tl1)!=0) {
				tl1 = tl;
				printf("this is an equation in 1 variable of degree %d\n", equationDegree(&tl1));
			} else {
				printf("this is an equation, but not in 1 variable\n");
			}
		} else {
			printf("this is not an equation\n"); 
		}
		free(ar);
		freeTokenList(tl);
		printf("\ngive an equation: ");
		ar = readInput();
	}
	free(ar);
	printf("good bye\n");
}


