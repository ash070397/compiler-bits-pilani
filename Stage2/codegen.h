/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
#ifndef symbolTable
#define symbolTable
#include "symbolTable.h"
#endif

void declaration_maker(FILE *f1);
void neg_printer(FILE *f, int array_flag);
void declaration_varst(var_st *table,FILE*f);
void declaration_arr(ASTNode *declare_node, var_st *curr_scope, var_st *prev_scope, FILE *f1);
void assignment_stmt(ASTNode *assign_node, var_st *curr_scope, var_st *prev_scope, FILE *f1);
void bound_checker(int is_const_indx, char *indx_name, FILE *f1, char *arr_name, ASTNode *curr);
void calc_offset_assign(char *arr_name, type type_of_arr, FILE *f1);
void module_input_copy(func_hash_entry *module_st, ASTNode *module_reuse_node, var_st *curr_scope, var_st *prev_scope, FILE *temp_file);
void input_gen(ASTNode *get_input_node, var_st *curr_scope, var_st *prev_scope, FILE *temp_file);
void print_gen(ASTNode *print_node, var_st *curr_scope, var_st *prev_scope ,FILE *temp_file);
char *lexeme_generator(char * lexeme , int lno, var_st *table);
int is_left_child(ASTNode *node);
void terminals_handler(ASTNode *ast_node , var_st *table , FILE *f);
void expr_code_writer(ASTNode *ast_node , var_st *table , FILE *f , int child_flag , int operator);
void operator_processor(ASTNode *ast_node , var_st *table , FILE *f, int operator);
int check_operator(ASTNode *ast_node);
void make_code(ASTNode* ast_node , var_st* curr_scope , FILE *f1);
void module_return_gen(ASTNode *module_reuse_node, var_st *curr_scope, var_st *prev_scope, FILE *f1);
void for_gen(ASTNode *rfor, var_st *curr_scope, var_st *prev_scope, FILE *f1);
void while_gen(ASTNode *rwhile, var_st *curr_scope, var_st *prev_scope, FILE *f1);
void switch_gen(ASTNode *rswitch, var_st *curr_scope, var_st *prev_scope, FILE *f1);
void entire_code_gen(ASTNode *moduledec, FILE *f1);
void float_printer(FILE *f1);
void float_printer_arr(FILE *f1);
void driverprogram_gen(ASTNode *driver_program, FILE *f1);
void other_modules_gen(ASTNode *othermodules, FILE *f1);
void module_body_gen(ASTNode *modules_node, FILE *f1);
var_st * statements_gen(ASTNode *stmts, var_st *curr, var_st* prev, FILE *f1);
