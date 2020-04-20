/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>

#define MAX_RULES 65
#define HASH_TABLE_SIZE 181 
#define MAX_LEN 30
#define MAX_BRANCH 10
#define NUM_NT 57
#define NUM_T 59
#define NUM_KEYWORD 30
#define KEYWORD_HASH_TABLE_SIZE 59

typedef enum
{
    T_RNUM, T_NUM,T_ID, T_INTEGER,T_REAL,T_BOOLEAN,T_OF,T_ARRAY,T_START,T_END,T_DECLARE,T_MODULE,T_DRIVER,
	T_PROGRAM,T_GET_VALUE,T_PRINT,T_USE,T_WITH,T_PARAMETERS,T_TRUE,T_FALSE,T_TAKES,T_INPUT,
	T_RETURNS,T_AND,T_OR,T_FOR,T_IN,T_SWITCH,T_CASE,T_BREAK,T_DEFAULT,T_WHILE,
	T_PLUS,T_MINUS,T_DIV,T_MUL,T_LT,T_LE,T_GE,T_GT,T_EQ,T_NE,T_DEF,T_COLON,T_RANGEOP,
	T_SEMICOL,T_COMMA,T_ASSIGNOP,T_SQBO,T_SQBC,T_BO,T_BC,T_COMMENTMARK,T_DRIVERDEF,
    T_DRIVERENDDEF, T_EPS, T_DOLLAR, T_ENDDEF
}tokenizer;

typedef enum{
    NT_program, NT_moduleDeclarations, NT_moduleDeclaration, NT_otherModules, NT_driverModule, NT_module,
    NT_ret, NT_input_plist, NT_N1, NT_output_plist, NT_N2, NT_dataType, NT_range_arrays, NT_type, NT_moduleDef,
    NT_statements, NT_statement, NT_ioStmt, NT_var, NT_var_id_num, NT_boolconstt, NT_whichID,
    NT_simpleStmt, NT_assignmentStmt, NT_whichStmt, NT_lvalueIDStmt, NT_lvalueARRStmt, NT_index, 
    NT_moduleReuseStmt, NT_optional, NT_idList, NT_N3, NT_expression, NT_u1, NT_new_NT, NT_unary_op, NT_arithmeticOrBooleanExpr,
    NT_N7, NT_AnyTerm, NT_N8, NT_arithmeticExpr, NT_N4, NT_op1, NT_term, NT_N5, NT_op2, NT_factor, NT_logicalOp, NT_relationalOp,
    NT_declareStmt, NT_conditionalStmt, NT_caseStmts, NT_N9, NT_value, NT_default, NT_iterativeStmt, NT_range
}NonTerminals;

typedef struct 
{
	char lexeme[30];
	tokenizer Token;
    int line_number;
	// type var_type;
}tokenInfo;

typedef union{
    tokenizer t;
    NonTerminals nt;

}Symbol;


struct node{
    Symbol s;
    int is_terminal; //-1 for not keyword
    struct node *next;
};
// have to add a boolean is_keyword.

typedef struct node NODE;

typedef struct{
    //stores the head of the hash table of that row
    struct node *head;
    //size of that row
    int size;   
}mappingTable[HASH_TABLE_SIZE];

mappingTable m;

struct keywordNode{
    int keyword_pos;
    struct keywordNode *next;
};

typedef struct keywordNode KEYWORD_NODE;

typedef struct{
    struct keywordNode *head;
    int size;
}keywordTable[KEYWORD_HASH_TABLE_SIZE];

keywordTable keys;