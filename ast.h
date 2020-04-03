#ifndef astDef
#define astDef
#include "astDef.h"
#endif

ASTNode* make_node(tree_node* pt_node);
ASTNode *postorder(tree_node *pTree, ASTNode *inh);
void fill_ast_parent(ASTNode *current);
void printTree(ASTNode *current);