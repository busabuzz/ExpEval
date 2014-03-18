/* prefixExp.h, Gerard Renardel, 29 January 2014 */

#ifndef PREFIXEXP_H
#define PREFIXEXP_H

/* Here the definition of the type tree of binary trees with nodes containing tokens.
 */

typedef struct ExpTreeNode *ExpTree;
  
typedef struct ExpTreeNode {
  TokenType tt;
  Token t;
  ExpTree left;
  ExpTree right;
} ExpTreeNode;

void differentiate(ExpTree *tp, char *identifier);
ExpTree makeTreeCopy(ExpTree *tp);
void simplify(ExpTree *tree);
int valueNumber(List *lp, double *wp);
int simplifyLevel(ExpTree *tree);
int valueExpression(List *tempList, ExpTree *tp);
int valueTerm(List *tempList, ExpTree *tp);
int valueFactor(List *tempList, ExpTree *tp);
ExpTree newExpTreeNode(TokenType tt, Token t, ExpTree tL, ExpTree tR);
int valueIdentifier(List *lp, char **sp);
int isNumerical(ExpTree tr);
double valueExpTree(ExpTree tr);
void printExpTreeInfix(ExpTree tr);
void infixExpTrees();

#endif
