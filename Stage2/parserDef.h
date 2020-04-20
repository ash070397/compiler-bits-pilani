/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#include "lexer.h"

int first_done[NUM_NT];
unsigned long long int First[NUM_NT];
unsigned long long int Follow[NUM_NT];

//Structure for the rhs of a production rule
struct rhs_node{
    Symbol s;
    int is_terminal;
    struct rhs_node *next;
    struct rhs_node *prev;
};

//Just rename it
typedef struct rhs_node RHS_NODE;

//Stores the grammar production rules (line by line)
typedef struct{
    NonTerminals sym;
    RHS_NODE *head[MAX_BRANCH];
    RHS_NODE *tail[MAX_BRANCH];
    int num_of_branch; // IMP: If there is only 1 branch in Grammar NT_Rule then num_of_branch = 0 ie it is zero indexed
}Grammar[MAX_RULES];

typedef struct{
    int NT_rule ,branch_no ,isnonempty,count;
} PARSE_TABLE[NUM_NT][NUM_T];

struct tree_node{
	struct  tree_node *next;//for sibling 
	struct tree_node *parent;
	struct tree_node *child;//corresponding to first child
	int branch_no;//to store the branch of the corresponding rule.
	int level;
	int line_number;
	int enum_value;
	char str[30];
	bool is_terminal;
};

typedef struct tree_node tree_node;

struct stack_node{
	tree_node* data;
	struct stack_node* next;
};

typedef struct stack_node stack_node;

struct parseTree{
	 tree_node *root;
	 tree_node *current;
};

typedef struct parseTree parseTree;

Grammar g;
PARSE_TABLE tb;