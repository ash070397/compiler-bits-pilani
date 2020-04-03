/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#ifndef parser
#define parser
#include "parserDef.h"
#endif

void populate_grammar(char * file_name);
void disp_forward();
void disp_backward();
void first(int nt_enum);
//void calculate_first();
int follow_set();
//void calculate_follow_set();
void ComputeFirstAndFollowSets();
void disp_first();
void disp_follow();
//void fill_parse_table();
void createParseTable();
void print_parse_table();
tree_node*  initialize();
void push(stack_node* temp_node);
tree_node* peek();
tree_node* pop();
void display_stack();
//parseTree* create_tree();
parseTree* parseInputSourceCode();
void printParseTree(tree_node *curr_node, FILE *outfile);