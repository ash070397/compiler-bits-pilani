/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
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
int check_var(char* lex, var_st* table);
var_hash_entry* find_var(char* lex, int lno, var_st* table);
int extract_while_exp_var(ASTNode *current,var_st *current_var_st,char while_vars[10][21], int index);
void fill_symbol_table(ASTNode *current, var_st *current_var_st,char for_string[10][21],int size_for_string);
struct struct_type expr_typechecker(ASTNode *current,var_st *current_var_st);
var_st* semantic_check(ASTNode *current, var_st *current_var_st, var_st *prev_child_var_st);
void printSTEntry(var_st* table);
void printSymbolTable();
void printActRecords();
void printArrays();
void printArrayEntry(var_st* table);
// void print_var_st();
// void print_st();