#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "scanner.h"
#include "evalExp.h"
#include "recognizeExp.h"

int main(int argc, char *argv[]) {
	char *ar;
	List tl, tl1; 
	int degree; 
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
				degree = equationDegree(&tl1);
				printf("this is an equation in 1 variable of degree %d\n", degree);
				if (degree == 1 || degree == 2) {
					tl1 = tl;
					parseEquation(&tl1);
				}
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
	return 0;
}

