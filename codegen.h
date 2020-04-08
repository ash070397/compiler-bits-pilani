#ifndef symbolTable
#define symbolTable
#include "symbolTable.h"
#endif
void declaration_maker();
void declaration_varst(var_st *table,FILE*f);
int check_operator(ASTNode *ast_node);
void make_code(ASTNode* ast_node , var_st* table , FILE *f);
void operator_processor(ASTNode *ast_node , var_st *table , FILE *f, int operator);
void expr_code_writer(ASTNode *ast_node , var_st *table , FILE *f , int child_flag , int operator);
void terminals_handler(ASTNode *ast_node , var_st *table , FILE *f);
int is_left_child(ASTNode *node);
char *lexeme_generator(char * lexeme , int lno);