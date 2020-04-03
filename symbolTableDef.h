#include "ast.h"

#define SYMBOL_TABLE_SIZE 30

// typedef enum{defined, declared} funcStatus;
typedef enum{INTEGER, BOOLEAN, REAL} datatype;

int current_offset;

struct variable
{
	datatype dt;
};

struct array 
{
	datatype dt;
        bool is_static;
	int s_idx, e_idx;
};

struct struct_type{
        int is_array; //0:variable, 1:array
        union
	{
		struct variable v;
		struct array a;
	}t;
};

//Function parameters
struct parameter
{
        struct struct_type type;
	struct parameter *next;
};

typedef struct variable variable;
typedef struct array array;
typedef struct struct_type type;
typedef struct parameter parameter;


//Function Symbol table
struct func_hash_entry{
        char lexeme[21];
        int declare_lno;
	int define_lno;
	bool called;
        // int lno; //First come first
        // funcStatus status;
        struct parameter *inputList;
	struct parameter *outputList;
        struct var_st *child;//link to variable symbol table
        struct func_hash_entry *next;
};

struct func_st{
        int size;
        struct func_hash_entry*head;
}func_st_root[SYMBOL_TABLE_SIZE];

typedef struct func_st_entry func_st_entry;
typedef struct func_hash_entry func_hash_entry;

//Variable Symbol table
struct var_hash_entry{
        char lexeme[21];
        int lno;
        int flag; //0: input, 1: output, 2:local, 3:second_declaration and in input_list
        int width;
        int offset;
        struct struct_type type;
        struct var_hash_entry *next;
};

struct var_st{
        int lno;
        int depth;
        struct var_hash_table_struct{
                int size;
                struct var_hash_entry*head;
        }var_hash_table[SYMBOL_TABLE_SIZE];
        struct var_st *next;
        struct var_st *child;

        int tag; //0: function ST, 1:Variable ST //Parent type
        union st_type{
                struct func_hash_entry *f_parent;
                struct var_st *v_parent;
        } parent;
        struct var_st *root_var_st;
};

typedef struct var_hash_entry var_hash_entry;
typedef struct var_st var_st;