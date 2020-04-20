/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
#include "parser.h"

struct ASTdata_info{
    int start_line;
	int line_number;
    int end_line;    //only modified for scope for switch while and modules.
	int enum_value;
	char str[30];
	bool is_terminal;
};
typedef struct ASTdata_info ASTdata_info;

struct ast{
    struct ast *left_child;
    struct ast *right_child;
    struct ast *forward;
    struct ast *parent;
    ASTdata_info data;
};
typedef struct ast ASTNode;

