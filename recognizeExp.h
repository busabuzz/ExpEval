#ifndef RECOGNIZEEXP_H
#define RECOGNIZEEXP_H

int acceptNumber(List *lp);
int acceptIdentifier(List *lp);
int acceptCharacter(List *lp, char c);
int acceptTerm(List *lp);
int acceptExpression(List *lp);
int equationDegree(List *lp);
int varCheck(List *lp);
int acceptEquation(List *lp);
void recognizeEquation();

#endif
