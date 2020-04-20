/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
#ifndef astDef
#define astDef
#include "astDef.h"
#endif

ASTNode* make_node(tree_node* pt_node);
ASTNode *postorder(tree_node *pTree, ASTNode *inh);
void fill_ast_parent(ASTNode *current);
void printTree(ASTNode *current);