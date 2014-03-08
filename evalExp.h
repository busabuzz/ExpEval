/* evalExp.h, Gerard Renardel, 29 January 2014 */

#ifndef EVALEXP_H
#define EVALEXP_H

int valueNumber(List *lp, double *wp);
double cCuriosity(double x);
void calculateEquation(double a, double b, double c);
void parseEquation(List *li);
/*void evaluateEquation(List *li);*/

#endif
