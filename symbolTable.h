#ifndef symolTableDef
#define symolTableDef
#include "symbolTableDef.h"
#endif

int getBytes(ASTNode *node);
type getType(ASTNode *node);
void initialise_var_hash_table(struct var_hash_table_struct* table);
void initialise_func_st_root();
unsigned long id_hash(char *sym);
void insert_var_hash(var_hash_entry* entry, var_st* table);
void insert_func_hash(func_hash_entry* entry);
func_hash_entry* find_func_hash(char* lex);
var_hash_entry* find_var(char* lex, int lno, var_st* table);
int check_var(char* lex, var_st* table);
void fill_symbol_table(ASTNode *current, var_st *current_var_st,char for_string[10][21],int size_for_string);
//void semantic_check(ASTNode *current, var_st *current_var_st);
//void TraverseTree_st(ASTNode *current, var_st *current_var_st,var_st *prev_child_var_st);
void semantic_check(ASTNode *current, var_st *current_var_st,var_st *prev_child_var_st);
void print_var_st();
void print_st();