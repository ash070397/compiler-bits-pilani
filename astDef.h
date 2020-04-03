#include "parser.h"

struct ASTdata_info{
    int level;
	int line_number;
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

