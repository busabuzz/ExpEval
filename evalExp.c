#include <stdio.h>  /* getchar, printf */
#include <stdlib.h> /* NULL */
#include "scanner.h"
#include "recognizeExp.h"
#include "evalExp.h"
#include <math.h>

int valueNumber(List *lp, double *wp) {
  if (*lp != NULL && (*lp)->tt == Number ) {
    *wp = ((*lp)->t).number;
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

double cCuriosity(double x) {
	if( x <= 0 && -0.0005 <= x ) x = 0.000;
	return x;
}

void calculateEquation(double a, double b, double c) {
	double answer,answer2, disc;
	if(a==0) {
		if(b==0) {
			printf("no solution\n");
		} else {
			answer = ((-c)/b);
			printf("solution: %.3f\n", cCuriosity(answer));
		}
	} else {
		disc = pow(b,2)-4*a*c;
		if (disc<0) {
			printf("no solution\n");
		}
		if (disc==0) {
			answer = (-b)/(2*a);
			printf("solution: %.3f\n", cCuriosity(answer));
		} 
		if (disc>0) {
			answer = ((-b)+sqrt(disc))/(2*a);
			answer2 = ((-b)-sqrt(disc))/(2*a);
			if(answer>answer2) {
				printf("solutions: %.3f %.3f\n", cCuriosity(answer), cCuriosity(answer2));
			} else {
				printf("solutions: %.3f %.3f\n", cCuriosity(answer2), cCuriosity(answer));
			}
		}
	}
}

void parseEquation(List *li) {
	int d=1,e=1,temp;
	double a=0,b=0,c=0;
	List liCopy;
	while (*li != NULL) {
		if(acceptCharacter(li,'=')) {
			e=-1;
			d=1;
		}
		if(acceptCharacter(li,'-')) {
			d=-1;
		}
		if(acceptCharacter(li,'+')) {
			d=1;
		}
		liCopy = *li;
		temp = acceptTerm(li);
		if(temp==2) {
			a += (d * e * (liCopy->t.number));
		}
		if(temp==1) {
			b += (d * e * (liCopy->t.number));
		}
		if(temp==0) {
			c += (d * e * (liCopy->t.number));
		}
	}	
	calculateEquation(a,b,c);
}


