#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include<time.h>

#define MAX_RULES 65
#define HASH_TABLE_SIZE 181 
#define MAX_LEN 30
#define MAX_BRANCH 10
#define NUM_NT 57
#define NUM_T 59
#define NUM_KEYWORD 30
#define KEYWORD_HASH_TABLE_SIZE 59
#define SYMBOL_TABLE_SIZE 30


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

typedef struct {
    int level;
	int line_number;
	int enum_value;
	char str[30];
	bool is_terminal;
}ASTdata_info;

struct ast{
     
    struct ast *left_child;
    struct ast *right_child;
    struct ast *forward;
    struct ast *parent;
    ASTdata_info data;
    
};

typedef struct ast ASTNode;


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



int first_done[NUM_NT];
unsigned long long int First[NUM_NT];
unsigned long long int Follow[NUM_NT];
int eps_enum = 56;



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
    int branch_no;
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
	// int no_of_level;
};

typedef struct parseTree parseTree;

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


Grammar g;
PARSE_TABLE tb;
stack_node *head=NULL;
int counter=0;

unsigned long hash(char *sym);
unsigned long keyword_hash(char *sym);
void populate_table();
void populate_keyword_table();
NODE string_to_enum(char* input);
NODE is_keyword(char *input);
tokenInfo* tokenize(char *str);
void getstream();
tokenInfo* getNextToken();
void removeComments(char *testcaseFile, char *cleanFile);

void populate_grammar(char * file_name);
void disp_forward();
void disp_backward();
void first(int nt_enum);
//void calculate_first();
unsigned long long int follow_set();
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
void declaration_varst(var_st *table,FILE*f);
int check_operator(ASTNode *ast_node);
void make_code(ASTNode* ast_node , var_st* table , FILE *f);
void operator_processor(ASTNode *ast_node , var_st *table , FILE *f, int operator);
void expr_code_writer(ASTNode *ast_node , var_st *table , FILE *f , int child_flag , int operator);
void terminals_handler(ASTNode *ast_node , var_st *table , FILE *f);
int is_left_child(ASTNode *node);
char *lexeme_generator(char * lexeme , int lno);


void printParseTree(tree_node *curr_node, FILE *outfile);

char buffer1[256];
char buffer2[256];
char lexeme_buffer[20]; //lexeme that is passed to tokenizer
int eof_buffer = 0; //buffer that has EOF
int eof_marker = -1; //position of EOF in the buffer
int curr_buffer = 1; //active buffer
int buffer_itr = 0; //buffer iterator
bool lex_error=false;
bool comment_flag=false; //true means that comment is ongoing
bool comment_flag_flag=false;
bool buffer1_empty = true;
bool buffer2_empty = true;
int lineNumber = 1;
extern FILE *fp;
bool delimiter_flag = false;
bool error_long_id = false;
bool error_long_id_print = false;
bool file_finished=false;
int err_count = 0;
datatype type_flag; 
//type_flag update.
int cmp_count = 0;
//have to make it global as it gets locally chutiyapa
char temp_lexeme[50];


char *terminal_map[NUM_T] = {
    "RNUM","NUM","ID","INTEGER","REAL","BOOLEAN","OF","ARRAY","START","END","DECLARE","MODULE","DRIVER",
	"PROGRAM","GET_VALUE","PRINT","USE","WITH","PARAMETERS","TRUE","FALSE","TAKES","INPUT",
	"RETURNS","AND","OR","FOR","IN","SWITCH","CASE","BREAK","DEFAULT","WHILE",
	"PLUS","MINUS","DIV","MUL","LT","LE","GE","GT","EQ","NE","DEF","COLON","RANGEOP",
	"SEMICOL","COMMA","ASSIGNOP","SQBO","SQBC","BO","BC","COMMENTMARK","DRIVERDEF","DRIVERENDDEF",
     "EPS", "DOLLAR", "ENDDEF"
};

char *non_terminal_map[NUM_NT] = {
    "program", "moduleDeclarations", "moduleDeclaration", "otherModules", "driverModule", "module",
    "ret", "input_plist", "N1", "output_plist", "N2", "dataType", "range_arrays", "type", "moduleDef",
    "statements", "statement", "ioStmt", "var", "var_id_num", "boolconstt", "whichID",
    "simpleStmt", "assignmentStmt", "whichStmt", "lvalueIDStmt", "lvalueARRStmt", "index", 
    "moduleReuseStmt", "optional", "idList", "N3", "expression", "u1", "new_NT", "unary_op", "arithmeticOrBooleanExpr",
    "N7", "AnyTerm", "N8", "arithmeticExpr", "N4", "op1", "term", "N5", "op2", "factor", "logicalOp", "relationalOp",
    "declareStmt", "conditionalStmt", "caseStmts", "N9", "value", "default", "iterativeStmt", "range"
};

char *keyword_map[NUM_KEYWORD] = {
    "integer", "real", "boolean", "of", "array", "start", "end", "declare", "module", "driver", "program",
    "get_value", "print", "use", "with", "parameters", "true", "false", "takes", "input",
    "returns", "AND", "OR", "for", "in", "switch", "case", "break", "default", "while"
    
};
int ptree_nodes = 0, ast_nodes = 0;

unsigned long hash(char *sym){
    const int p = 53; 
    unsigned long hash = 0;
    unsigned long power = 1;
    int i = 0;
    while(sym[i] != '\0')
    {
        hash = (hash + (sym[i] - 'a' +1) * power) % HASH_TABLE_SIZE;
        power= (power *p) % HASH_TABLE_SIZE;
        i++;
    }
    return hash;
    
}

unsigned long keyword_hash(char *sym){
    const int p = 53; // because both lower and uppercase letters are present
    unsigned long hash = 0;
    unsigned long power = 1;
    int i = 0;
    while(sym[i] != '\0')
    {
        hash = (hash + (sym[i] - 'a' +1) * power) % KEYWORD_HASH_TABLE_SIZE;
        power= (power *p) % KEYWORD_HASH_TABLE_SIZE;
        i++;
    }
    return hash;
} 
void populate_table(){
    char temp1[MAX_LEN], temp2[MAX_LEN];
  
    //Inserting terminals into hash table
    int ct1 = 0, ct2 = 0;

    for(int term = 0; term < NUM_T ; term++)
    {
        unsigned long pos_in_table = hash(terminal_map[term]);
        NODE *n = (NODE *)malloc(sizeof(NODE));       
        n->is_terminal = 1;
        n->next = m[pos_in_table].head;
        (n->s).t = ct1;

        m[pos_in_table].head = n;
        m[pos_in_table].size++;
        ct1++;   
    }

    for(int non_term = 0; non_term < NUM_NT ; non_term++)
    {
        unsigned long pos_in_table = hash(non_terminal_map[non_term]);
        NODE *n = (NODE *)malloc(sizeof(NODE));
        n->is_terminal = 0;
        n->next = m[pos_in_table].head;
        (n->s).nt = (NonTerminals)ct2;

        m[pos_in_table].head = n;
        m[pos_in_table].size++;
        ct2++;
    }
}

void populate_keyword_table(){
    for(int keyword_no = 0; keyword_no < NUM_KEYWORD; keyword_no ++){
        unsigned long pos_in_table = keyword_hash(keyword_map[keyword_no]);
        KEYWORD_NODE *the_keyword = (KEYWORD_NODE *)malloc(sizeof(KEYWORD_NODE));
        the_keyword->keyword_pos = keyword_no;
        the_keyword->next = keys[pos_in_table].head;
        
        keys[pos_in_table].head = the_keyword;
        keys[pos_in_table].size++;
    }
}


NODE string_to_enum(char* input)
{
    Symbol s1;
    NODE h1;
    unsigned long pos = hash(input);
    NODE *h = m[pos].head;
    while(h!=NULL) //loop over all collisions
    {
        if(h->is_terminal)
        {
            if(strcmp(input ,terminal_map[(h->s).t]) == 0)
            {
                h1.is_terminal = 1;
                h1.s = h->s;
                return h1;
            }
        }
        else
        {
            if(strcmp(input ,non_terminal_map[(h->s).nt]) == 0)
            {
                h1.is_terminal = 0;
                h1.s = h->s;
                return h1;
            }
        }
        h = h->next;
    }
    return h1;
}


NODE is_keyword(char *input){
    char getting_token[30];
    unsigned long pos = keyword_hash(input);
    KEYWORD_NODE *h = keys[pos].head;
    NODE ans;
    int found = 0;
    while(h != NULL){
        if(strcmp(input, keyword_map[h->keyword_pos]) == 0){
           int i = 0;
           while(input[i] != '\0'){
               getting_token[i] = input[i];
               if(input[i] >= 'a' && input[i] <= 'z'){
                   getting_token[i] = (input[i] - 32);
               }
               i++;
           }
           getting_token[i] = '\0';
           ans = string_to_enum(getting_token); 
           return ans;
        }
        h = h->next;
    }
    ans.is_terminal = -1;
    return ans;
}


tokenInfo* tokenize(char *str){//str denotes the line for which i have to give the token
	int i=0;//important variable 
        // i has to be maintained for next token 
	char ch;//parsing the string and used in switch
        lex_error=false; //true when lexecal error is encountered
        comment_flag_flag = false; //true only when the lexeme is "**"
        delimiter_flag = false;
	tokenInfo *ans = (tokenInfo*)malloc(sizeof(tokenInfo));//the one I will be returning in get_Token()
        
        int k=0; //initialsing lexeme to null
        for(k=0; k<30;k++){
                ans->lexeme[k] = '\0';
        }

	ch=str[0];
        ans->line_number=lineNumber;
        while(ch!='\0'){
                if(ch=='\n'){
                        lineNumber++;
                        ans->line_number=lineNumber;
                        i++;
                        ch = str[i];
                        delimiter_flag=true;
                        error_long_id = false;
                        break;
                }
                else if(ch==' ' || ch=='\t'){
                        i++;
                        ch = str[i];
                        delimiter_flag=true;
                        error_long_id = false;
                        break;
                }
                else if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (error_long_id == true && ((ch == '_' ) || (ch>='0' && ch<='9')))){
                        char temp_lex[30];
                        int k;
                        for(k=0;k<30;k++)
                            temp_lex[k]='\0';
                        temp_lex[0]=ch;
                        int j=1;
                        i++;
                        ch=str[i];
                        while((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') || (ch == '_')){
                                temp_lex[j]=ch;
                                j++;
                                if(j==21){
                                        error_long_id = true;
                                        j=20;
                                        break;
                                }
                                i++;
                                ch=str[i];
                        }
                        for(k=0;k<j;k++){
                                ans->lexeme[k]=temp_lex[k];
                        }
                        ans->lexeme[k]='\0';

                        if(error_long_id){
                                lex_error=true;
                                break;
                        }
                        if(j>20){
                                strcpy(ans->lexeme,temp_lex);
                                lex_error=true;
                        }
                        
                        NODE keyword_info;
                        keyword_info = is_keyword(ans->lexeme);

                        if(keyword_info.is_terminal!=-1){
                                ans->Token = keyword_info.s.t;
                        }
                        else{
                                ans->Token=T_ID;
                        }
                }
                else if(ch>='0' && ch<='9'){
                        char temp_lex[30];
                        int k;
                        for(k=0;k<30;k++)
                            temp_lex[k]='\0';
                        temp_lex[0]=ch;
                        int j=1;
                        i++;
                        ch=str[i];
                        while(ch>='0' && ch<='9'){
                                temp_lex[j]=ch;
                                j++;
                                i++;
                                ch=str[i];  
                        }
                        if(ch=='.'){
                                temp_lex[j]=ch;
                                i++;
                                j++;
                                ch=str[i];
                                if(ch>='0' && ch<='9'){
                                        temp_lex[j]=ch;
                                        j++;
                                        i++;
                                        ch=str[i];
                                        while(ch>='0' && ch<='9'){
                                                temp_lex[j]=ch;
                                                i++;
                                                j++;
                                                ch=str[i];
                                        }
                                        if(ch=='e' || ch=='E'){
                                                temp_lex[j]=ch;
                                                j++;
                                                i++;
                                                ch=str[i];
                                                if(ch>='0' && ch<='9'){
                                                        temp_lex[j]=ch;
                                                        j++;i++;
                                                        ch=str[i];
                                                
                                                        while(ch>='0' && ch<='9'){
                                                                temp_lex[j]=ch;
                                                                j++;i++;
                                                                ch=str[i];
                                                        }
                                                        strcpy(ans->lexeme,temp_lex);
                                                        ans->Token=T_RNUM;
                                                }
                                                else{
                                                        //+-
                                                        if(ch=='+' || ch=='-'){
                                                                temp_lex[j]=ch;
                                                                j++;i++;
                                                                ch=str[i];
                                                                if(ch>='0' && ch<='9'){
                                                                        temp_lex[j]=ch;
                                                                        j++;i++;
                                                                        ch=str[i];
                                                                        while(ch>='0' && ch<='9'){
                                                                                temp_lex[j]=ch;
                                                                                j++;i++;
                                                                                ch=str[i]; 
                                                                        }
                                                                        strcpy(ans->lexeme,temp_lex);
                                                                        ans->Token=T_RNUM;
                                                                }
                                                                else{
                                                                        strcpy(ans->lexeme,temp_lex);
                                                                        lex_error=true;
                                                                }
                                                        }
                                                        else{
                                                                strcpy(ans->lexeme,temp_lex);
                                                                lex_error=true;
                                                        }
                                                }                                                        

                                        }
                                        else{
                                                strcpy(ans->lexeme,temp_lex);
                                                ans->Token=T_RNUM;
                                        }
                                }
                                else{
                                        if(ch=='.'){
                                            int k=0;
                                            for(k=0;k<j-1;k++)
                                                ans->lexeme[k]=temp_lex[k];
                                                ans->Token=T_NUM;
                                                i=i-1;    
                                        }
                                        else{
                                            strcpy(ans->lexeme,temp_lex);
                                            lex_error=true;
                                        }    
                                }
                        }
                        else{
                                strcpy(ans->lexeme,temp_lex);
                                ans->Token=T_NUM;
                        }
                }
                else if(!((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9'))){
                        error_long_id = false;
                        switch(ch){     
                                case '+':{
                                        strcpy(ans->lexeme,"+");
                                        ans->Token=T_PLUS;
                                        i+=1;
                                        break;
                                }
                                case '-':{
                                        strcpy(ans->lexeme,"-");
                                        ans->Token=T_MINUS;
                                        i+=1;
                                        break;
                                }
                                case ';':{
                                        strcpy(ans->lexeme,";");
                                        ans->Token=T_SEMICOL;
                                        i+=1;
                                        break;
                                }
                                case ',':{
                                        strcpy(ans->lexeme,",");
                                        ans->Token=T_COMMA;
                                        i+=1;
                                        break;
                                }
                                case '=':{
                                        if(i+1<strlen(str) && str[i+1]=='='){
                                                strcpy(ans->lexeme,"==");
                                                ans->Token=T_EQ;
                                                i+=2;
                                        }
                                        else
                                        {
                                                strcpy(ans->lexeme,"=");
                                                lex_error=true;
                                                i+=1;
                                        }                                                        
                                        break;
                                }
                                case '<':{
                                        if(i+1<strlen(str) && str[i+1]=='<'){
                                                if(i+2<strlen(str) && str[i+2]=='<'){
                                                        strcpy(ans->lexeme,"<<<");
                                                        ans->Token=T_DRIVERDEF;
                                                        i+=3;
                                                }
                                                else{
                                                        strcpy(ans->lexeme,"<<");
                                                        ans->Token=T_DEF;
                                                        i+=2;
                                                }
                                        }
                                        else if(i+1<strlen(str) && str[i+1]=='='){
                                                        strcpy(ans->lexeme,"<=");
                                                        ans->Token=T_LE;
                                                        i+=2;
                                        }       
                                        else{
                                                strcpy(ans->lexeme,"<");
                                                ans->Token=T_LT;
                                                i+=1;
                                        }  
                                        break;
                                }
                                case '>':{
                                        if(i+1<strlen(str) && str[i+1]=='>'){
                                                if(i+2<strlen(str) && str[i+2]=='>'){
                                                        strcpy(ans->lexeme,">>>");
                                                        ans->Token=T_DRIVERENDDEF;
                                                        i+=3;
                                                }
                                                else{
                                                        strcpy(ans->lexeme,">>");
                                                        ans->Token=T_ENDDEF;
                                                        i+=2;
                                                }
                                        }
                                        else if(i+1<strlen(str) && str[i+1]=='='){
                                                        strcpy(ans->lexeme,">=");
                                                        ans->Token=T_GE;
                                                        i+=2;
                                        }       
                                        else{
                                                strcpy(ans->lexeme,">");
                                                ans->Token=T_GT;
                                                i+=1;
                                        }  
                                        break;
                                }
                                case '*':{
                                        if(i+1<strlen(str) && str[i+1]!='*'){
                                                strcpy(ans->lexeme,"*");
                                                ans->Token=T_MUL;
                                                i+=1;
                                        }
                                        else if(i+1<strlen(str) && str[i+1]=='*'){
                                            
                                                i+=2;
                                                comment_flag_flag = true;
                                                if(comment_flag==false){
                                                        comment_flag = true;
                                                }
                                                else{
                                                        comment_flag = false;
                                                }

                                        }
                                        break;
                                        
                                }
                                case ':':{
                                        if(i+1<strlen(str) && str[i+1]=='='){
                                                strcpy(ans->lexeme,":=");
                                                ans->Token=T_ASSIGNOP;
                                                i+=2;
                                        }
                                        else{
                                                strcpy(ans->lexeme,":");
                                                ans->Token=T_COLON;
                                                i+=1;    
                                        }
                                        break;
                                }
                                case '.':{
                                        if(i+1<strlen(str) && str[i+1]=='.'){
                                                strcpy(ans->lexeme,"..");
                                                ans->Token=T_RANGEOP;
                                                i+=2;        
                                        }
                                        else{
                                                strcpy(ans->lexeme,".");
                                                lex_error=true;
                                                i+=1;
                                        }        
                                        break;                                
                                }
                                case '!':{
                                        if(i+1<strlen(str) && str[i+1]=='='){
                                                strcpy(ans->lexeme,"!=");
                                                ans->Token=T_NE;
                                                i+=2;        
                                        }
                                        else{
                                                strcpy(ans->lexeme,"!");
                                                lex_error=true;
                                                i+=1;
                                        }
                                        break;                                
                                }
                                case ']':{
                                        strcpy(ans->lexeme,"]");
                                        ans->Token=T_SQBC;
                                        i+=1;
                                        break;
                                }
                                case '[':{
                                        strcpy(ans->lexeme,"[");
                                        ans->Token=T_SQBO;
                                        i+=1;
                                        break;
                                }
                                case '(':{
                                        strcpy(ans->lexeme,"(");
                                        ans->Token=T_BO;
                                        i+=1;
                                        break;
                                }
                                case ')':{
                                        strcpy(ans->lexeme,")");
                                        ans->Token=T_BC;
                                        i+=1;
                                        break;
                                }
                                case '/':{
                                        strcpy(ans->lexeme,"/");
                                        ans->Token=T_DIV;
                                        i+=1;
                                        break;
                                } 
                                default:{
                                        sprintf(ans->lexeme,"%c",ch);
                                        lex_error=true;
                                        i+=1;
                                        break;
                                }       
	                }
                }
                break;
        }
        buffer_itr += i;
        return(ans);       
}


void getstream(){
        char ch;
        if(buffer1_empty == true  && eof_buffer==0){
                int itr = 0;
                while (itr<256 && (ch = fgetc(fp)) != EOF)
                {
                        buffer1[itr] = ch;
                        itr++;
                }
                if(itr!=256){
                        eof_marker = itr;
                        eof_buffer = 1;
                }
                buffer1_empty = false;
        }
        if(buffer2_empty == true && eof_buffer==0){
                int itr = 0;
                while (itr<256 && (ch = fgetc(fp)) != EOF)
                {
                        buffer2[itr] = ch;
                        itr++;
                }
                if(itr!=256){
                        eof_marker = itr;
                        eof_buffer = 2;
                }
                buffer2_empty = false;
        }

        if(buffer_itr > 256-1){
                buffer_itr-=256;
                if(curr_buffer == 1){
                        curr_buffer = 2;
                        buffer1_empty = true;
                }
                else if(curr_buffer == 2){
                        curr_buffer =1;
                        buffer2_empty = true;
                }
        }

        if(curr_buffer == 1){
                int i=0;
                while(i<21){
                        if((eof_buffer == 1 && (buffer_itr+i>=eof_marker)) || (eof_buffer == 2 && (buffer_itr+i-256>=eof_marker))){
                                lexeme_buffer[i] = '\0';
                        }
                        else if(buffer_itr+i<256){
                                lexeme_buffer[i] = buffer1[buffer_itr+i];
                        }
                        else{
                                lexeme_buffer[i] = buffer2[buffer_itr+i-256];
                        }
                        i++;
                }
        }
        else if(curr_buffer == 2){
                int i=0;
                while(i<21){
                        if((eof_buffer == 2 && (buffer_itr+i>=eof_marker)) || (eof_buffer == 1 && (buffer_itr+i-256>=eof_marker))){
                                lexeme_buffer[i] = '\0';
                        }
                        else if(buffer_itr+i<256){
                                lexeme_buffer[i] = buffer2[buffer_itr+i];
                        }
                        else{
                                lexeme_buffer[i] = buffer1[buffer_itr+i-256];
                        }
                        i++;
                }
        }
}

tokenInfo* getNextToken(){
        tokenInfo *a = (tokenInfo *)malloc(sizeof(tokenInfo));
        if(curr_buffer == eof_buffer  && buffer_itr>=eof_marker){
                if(comment_flag==true){
                        printf("\nLexical Error because end of comment not found");
                }
                else{
                        printf("\nEOF reached");
                }
                printf("\nTerminating Lexer\n");
                file_finished = true;
                a->Token = T_DOLLAR;
                strcpy(a->lexeme, "DOLLAR");
        }
        else{
                getstream();
                a = tokenize(lexeme_buffer); //NOTE: line number will be taken care of in anubhav
                if(comment_flag==true || comment_flag_flag==true || delimiter_flag==true){
                        a = getNextToken();
                }
                else if(lex_error){
                        if(error_long_id==true){
                                if(error_long_id_print == false){
                                        error_long_id_print = true;
                                        printf("\nLine Number %d Lexeme Error Lexeme %s",a->line_number,a->lexeme);
                                        err_count+=1;
                                }
                                else{
                                        printf("%s",a->lexeme);
                                }
                        }
                        else{
                                error_long_id_print = false;
                                printf("\nLine Number %d Lexeme Error Lexeme %s",a->line_number,a->lexeme);
                                err_count+=1;
                        }
                        a = getNextToken();
                }
                else{
                        return a;
                }
        }
        return a;
}


void removeComments(char *testcaseFile, char *cleanFile){
	FILE *fp_test = fopen(testcaseFile, "r");
	FILE *fp_clean = fopen(cleanFile, "w");
        if (fp_test == NULL)
        {
                printf("File is not available \n");
        }
	int match = 0;
	char ch1, ch2;
	while(1){
		ch1 = fgetc(fp_test);
		if(ch1 == EOF)
			break;
		//if I find a new line or I am outside a comment
		if(ch1 == '\n' || (ch1 != '*' && match == 0))
			fputc(ch1, fp_clean);
		//if character I find is a '*'
		else if(ch1 == '*'){
			ch2 = fgetc(fp_test);
			if(ch2 != '*' && match == 0)
				fputc(ch1, fp_clean);
			if(ch2 == EOF)
				break;
			//I get a new line and am outside a comment 
			if(ch2 == '\n' || (ch2 != '*' && match == 0))
				fputc(ch2, fp_clean);
			else if(ch2 == '*' && match == 0)
				match = 1;
			else if(ch2 == '*' && match == 1)
				match = 0;
		}
	}
        fclose(fp_test);
        fclose(fp_clean);
}

void printTokens(char *filename){
        populate_table();
        populate_keyword_table();

        fp = fopen(filename, "r");
        if (fp == NULL)
        {
                printf("File is not available \n");
        }
        
        tokenInfo *a;
        while(1){
                a = getNextToken();
                if(file_finished!=true){
                        printf("\n\n%d ",a->line_number);
                        printf("%s ", a->lexeme);
                        printf("%s", terminal_map[a->Token]);
                }
                else{
                        break;
                }
        }

        fclose(fp);
}
FILE *fp;

void push(stack_node* temp_node){
	
	temp_node->next=head;
	head=temp_node;
    return;
}

//Peek function for Stack ADT
tree_node* peek(){
    tree_node* data;
	if(head == NULL){
        return NULL; 
    }
    else{
        data = head->data;
    }
    return data;
}

//Pop function for Stack ADT
tree_node* pop(){ 
	tree_node* data;
    if(head->next == NULL){
        return NULL;  //returns null if stack empty
    }
    else{
        data = head->data;
        head=head->next;
    }
    return data;
}

//Creating a doubly linked list of grammar rules.
void populate_grammar(char * file_name)
{
	FILE *fp1 = fopen(file_name, "r");
	if(fp1 == NULL)
		printf("ERROR IN OPENING GRAMMAR FILE\n");
	char buff[50];
	char *buff_ptr = buff;
	int nt_number = 0;
	while(fscanf(fp1, "%s", buff) != EOF){
	
		// I have read the non terminal over here
		g[nt_number].sym = string_to_enum(buff).s.nt;

		int rule_no = 0;
		while(fscanf(fp1, "%s", buff) != EOF){
			if(buff[0] == '?'){
				
				
				g[nt_number].num_of_branch = rule_no;
				nt_number++;
				break;
			}
			if(buff[0] == '|'){
			
				rule_no++;
				continue;
			}

			RHS_NODE *put = (RHS_NODE *)malloc(sizeof(RHS_NODE));
			NODE n = string_to_enum(buff);
			
			put->s = n.s;
			put->is_terminal = n.is_terminal;
			put->next = NULL;
			put->prev = (g[nt_number].tail)[rule_no];
			if((g[nt_number].head)[rule_no] == NULL)
				(g[nt_number].head)[rule_no] = put;
			if((g[nt_number].tail)[rule_no] != NULL)
				(g[nt_number].tail)[rule_no]->next = put;
			(g[nt_number].tail)[rule_no] = put;
		}
	}
	fclose(fp1);
				
}

//Helper function to print the production rules in forward rules
void disp_forward(){

	for(int nt_number = 0 ; nt_number< NUM_NT ; nt_number++){
		int branches = g[nt_number].num_of_branch;
		for(int branch = 0 ; branch<=branches ; branch++){
			RHS_NODE *head = (g[nt_number].head)[branch];
			printf("%s -> ",non_terminal_map[nt_number]);
			while(head != NULL){
				if(head->is_terminal){
					printf("%s ",terminal_map[(int)(head->s).t]);
				}
				else{
					printf("%s ", non_terminal_map[(int)(head->s).nt]);
				}
				head = head->next;			
			}
			printf("\n");
		}
	} 
}
//Helper function to print the production rules in reverse order
void disp_backward(){

	for(int nt_number = 0 ; nt_number< NUM_NT ; nt_number++){
		int branches = g[nt_number].num_of_branch;
		for(int branch = 0 ; branch<=branches ; branch++){
			RHS_NODE *head = (g[nt_number].tail)[branch];
			printf("%s -> ",non_terminal_map[nt_number]);
			while(head != NULL){
				if(head->is_terminal){
					printf("%s ",terminal_map[(int)(head->s).t]);
				}
				else{
					printf("%s ", non_terminal_map[(int)(head->s).nt]);
				}
				head = head->prev;			
			}
			printf("\n");
		}
	} 
}

//Computation of first sets
void first(int nt_enum){
    int num_branches = g[nt_enum].num_of_branch;

    for(int branch = 0 ; branch <= num_branches ; branch++ ){
        RHS_NODE *head = g[nt_enum].head[branch];

        while(head != NULL){
            Symbol sym = head->s;
            if(head->is_terminal){ //next element is a terminal
                First[nt_enum] = First[nt_enum] | (((unsigned long long int)1)<<((int)sym.t));
                break;
            }
            else{ // next element is a non terminal

                if(first_done[sym.nt] == 0){ /// calculating first of next non terminal
                    first(sym.nt);
                }
                if(head->next != NULL && (First[sym.nt] & (((unsigned long long int)1)<<eps_enum) )){ ////checking if epsilon is set or not and if it is the last non terminal
                    unsigned long long int eps_free = (First[sym.nt] & (~(((unsigned long long int)1)<<eps_enum))); ///// removing epsilon
                    First[nt_enum] = First[nt_enum] | eps_free;
                }
                else {
                    First[nt_enum] = First[nt_enum] | First[sym.nt];
                    break;
                }
            }
            head = head->next;
        }
    }
    first_done[nt_enum] = 1;
}

//Computation of follow sets
unsigned long long follow_set(){
    //iterating backwards and initilazing dollar as follow of program
    int change = 0;
    Follow[NT_program] = (((unsigned long long int) 1) << (T_DOLLAR));

    for(int nt_number = NUM_NT - 1; nt_number>=0; nt_number--){
        int branches = g[nt_number].num_of_branch;
        for(int branch = 0; branch <= branches; branch++){
            unsigned long long int run;
            RHS_NODE *last = (g[nt_number].tail)[branch];
            run = Follow[nt_number];
            //If the last symbol is a non temrinal then Follow of RHS
            //last sym must include Follow of the LHS
            
            if(last->is_terminal == 0){
                unsigned long long int old = Follow[(last->s).nt];
                
                Follow[(last->s).nt] |=  run;
                unsigned long long int now = Follow[(last->s).nt];;
                if(old != now)
                    change = 1;

            }
            while(last->prev != NULL){
                RHS_NODE *second_last = last->prev;

                //only if second last is a NT there is meaning in going ahead
                if(second_last->is_terminal == 0){
                    //checking if first the last sym is a terminal
                    //or doesn't have eps in both cases I need to make 
                    //run as 0
                    //If it goes to the second or condition means we have 
                    //a non terminal in which case indexing into the First
                    //array makes sense
                    if(last->is_terminal){
                        unsigned long long int old =  Follow[(second_last->s).nt];
                        if((last->s).nt == 24)
                            printf(" ");
                        run = (((unsigned long long int) 1) <<((last->s).t));
                        Follow[(second_last->s).nt] |=  run;
                        unsigned long long int now = Follow[(second_last->s).nt];
                        if(old != now)
                            change = 1;
                
                    }
                    else if(((First[(last->s).nt] & (((unsigned long long int)1)<<eps_enum))) == 0){
                        unsigned long long int old = Follow[(second_last->s).nt];
                        if((last->s).nt == 24)
                            printf(" ");
                        run = First[(last->s).nt];
                        Follow[(second_last->s).nt] |= run;
                        unsigned long long int now = Follow[(second_last->s).nt];
                        if(old != now)
                            change = 1;
                    }
        
                    else{
                        //I am a Non terminal, have a non terminal after me and
                        //that contains an EPS

                        unsigned long long int old = Follow[(second_last->s).nt];
                        unsigned long long int eps_free_first = First[(last->s).nt] & (~(((unsigned long long int)1)<<eps_enum));
                        run = run | eps_free_first;
                        if((last->s).nt == 24)
                            printf(" ");
                        Follow[(second_last->s).nt] |= run;
                        unsigned long long int now = Follow[(second_last->s).nt];
                        if(old != now)
                            change =1;
                        
                    }
                }
                last = last->prev;
            }
            
        }
    }
    return change;
}

//First and follow computation.
void ComputeFirstAndFollowSets(){
    for(int i = 0 ; i<NUM_NT ; i++){
        first_done[i] = 0;
        First[i] = 0;
    }
    //printf("hello\n");
    for(int nt_enum = 0 ; nt_enum<NUM_NT ; nt_enum++){
        if(first_done[nt_enum] == 0){
            first(nt_enum);
        }
    }

    int ct = 0;
    while(1){
        if(follow_set() == 0)
            break;
        else{
            follow_set();
        }
        ct++;
    }

}

//Helper function to display first sets
void disp_first(){
    unsigned long long int temp = 0;
    for(int i = 0 ; i<NUM_NT ; i++){

        printf("%s = ",non_terminal_map[i]);
        for(int j = 0 ; j<64 ; j++){
            temp = First[i]>>j;
            if(temp%2 == 1){
                printf("%s, ",terminal_map[j]);
            }
        }
        printf("\n");
    }
}

//Helper function to display follow sets
void disp_follow(){
    unsigned long long int temp = 0;
    for(int i=0;i<NUM_NT; i++){
        printf("%s = ", non_terminal_map[i]);
        for(int j=0; j<64; j++){
            temp = Follow[i]>>j;
            if(temp %2 == 1){
                printf("%s, ", terminal_map[j]);
            }
        }
        printf("\n");
    }
    
}

//creation of Parse Table using grammar rules 
void createParseTable()
{
   for(int nt_index = 0 ; nt_index < NUM_NT ; nt_index++)
   {
       int branches = g[nt_index].num_of_branch + 1;
       unsigned long long int current_follow = Follow[g[nt_index].sym];
       for (int branch_index = 0; branch_index < branches; branch_index++)
       {
            RHS_NODE *head = (g[nt_index].head)[branch_index];
            RHS_NODE *nt_head = (g[nt_index].head)[branch_index];
            unsigned long long int  current_first = 0;

            //code for calculating the first of grammar rule
            while(head!= NULL)
            {
                if(head -> is_terminal)
                {
                    current_first =  current_first | ((unsigned long long int)1)<<((int)((head->s).t));
                    break;
                }
                else
                {
                    current_first = current_first | First[(head->s).nt];
                    if(!(current_first & (((unsigned long long int)1)<<eps_enum))) break; // checking for not epsilon
                    else
                    {
                        if(head->next != NULL) 
                            current_first = current_first & (~(((unsigned long long int)1)<<eps_enum)); // removing epsilon and continuing
                    }
                }
                head = head ->next;
            }

            if(current_first & (((unsigned long long int)1)<<eps_enum))
            {
                current_first = current_first & (~(((unsigned long long int)1)<<eps_enum)); // removing epsilon
                current_first = current_first | current_follow;
            }
            //subcode end

            for(int terminal =0 ; terminal < NUM_T ; terminal++)
            {
                if(current_first & (((unsigned long long int)1)<<terminal))
                {
                    tb[nt_index][terminal].branch_no = branch_index;
                    tb[nt_index][terminal].NT_rule = nt_index;
                    tb[nt_index][terminal].isnonempty = 1;
                    tb[nt_index][terminal].count++;
                    
                    // we have to initialize all isnonempty to 0.
                }
                if(current_follow & (((unsigned long long int)1)<<terminal))
                {
                    if(tb[nt_index][terminal].isnonempty == 0) tb[nt_index][terminal].isnonempty = -1;
                }
            }
       }    
   }
}

//Helper function to print Parse Table
void print_parse_table()
{
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < NUM_T; j++)
        {
            
            RHS_NODE *rule_head = (g[i].head)[tb[i][j].branch_no];
            if(tb[i][j].isnonempty)
            {
                printf("THE NON TERMINAL IS  -->  %s and TERMINAL IS --> %s\n",non_terminal_map[i], terminal_map[j]);
                while(rule_head!= NULL){
                if(rule_head->is_terminal){
                    printf(" %s ", terminal_map[(rule_head->s).t]);
                }
                else{
                    printf(" %s ", non_terminal_map[(rule_head->s).nt]);
                }
                rule_head = rule_head->next;
                }
                printf("\n\n");
            }
        }
    }
}

//Function to parse user's code
parseTree* parseInputSourceCode(){

    int curr_line_no = -1;

    parseTree *p_tree=(parseTree*)malloc(sizeof(parseTree));
    //create root n''ode of the tree
    tree_node* r_node=initialize();
    p_tree->root=r_node;
    p_tree->current=r_node;
    r_node->enum_value=NT_program;//start symbol
    tree_node* dollar=initialize();
    dollar->enum_value=T_DOLLAR;
    dollar->is_terminal=true;
    stack_node *stack_dollar=(stack_node*)malloc(sizeof(stack_node));
    stack_dollar->data=dollar;
    stack_dollar->next=NULL;
    stack_node *stack_rnode=(stack_node*)malloc(sizeof(stack_node));
    stack_rnode->data=r_node;
    stack_rnode->next=NULL;
    push(stack_dollar);//pushing dollar on stack
    push(stack_rnode);
    
    tree_node *emp = peek();
    tokenInfo *temp = getNextToken();
    
    //printf("VALUE OF -->1 %s\n", temp->lexeme);
    tree_node *make_new;

    while(!(emp->enum_value == T_DOLLAR && emp->is_terminal == 1)){

        make_new = initialize();
        if(emp->is_terminal){
            if(temp->Token == emp->enum_value && emp->is_terminal == 1){
                tree_node *mc = pop();

                strcpy(mc->str, temp->lexeme);
               
                temp = getNextToken();
            }
            else{
                //terminals dont match
                //Recovery done by popping TOS

                if(curr_line_no != temp->line_number){
                    printf("\nLine Number %d Syntactic Error ", temp->line_number);
                    curr_line_no = temp->line_number;
                    err_count++;
                }
                pop();
            }
        }
        else{
            //non terminal is there on tos

            if(tb[emp->enum_value][temp->Token].isnonempty == 0){
                //Recovery not possible using follow sets so fetching next token using getNextToken().

                if(temp->Token == T_DOLLAR){
                    return p_tree;
                }
                temp = getNextToken();
                
                if(curr_line_no != temp->line_number){
                    printf("\nLine Number %d Syntactic Error ", temp->line_number);
                    curr_line_no = temp->line_number;
                    err_count++;
                }
            }
            else if(tb[emp->enum_value][temp->Token].isnonempty == -1){

                //Recovet done using Follow sets.                
                pop();
                if(curr_line_no != temp->line_number){
                    printf("\nLine Number %d Syntactic Error", temp->line_number);
                    curr_line_no = temp->line_number;
                    err_count++;
                }
            }
            
            else if(tb[emp->enum_value][temp->Token].isnonempty == 1){
                //Normal case when stack top yields something
                pop();
                

                int rule_no = tb[emp->enum_value][temp->Token].NT_rule;
                int branch = tb[emp->enum_value][temp->Token].branch_no;
                tree_node *last_tree = NULL;
                RHS_NODE *last = NULL;
                RHS_NODE *second_last = (g[rule_no].tail)[branch];
                
                emp->branch_no = branch;//updating the non terminal with the corresponding branch to expand with..

                while(second_last != NULL){
                    
                    make_new = initialize();
                    make_new->is_terminal = second_last->is_terminal;
                    if(second_last->is_terminal){
                        make_new->enum_value = (second_last->s).t;
                        make_new->line_number = temp->line_number;
                    }
                    else{
                        make_new->enum_value = (second_last->s).nt;
                        make_new->line_number = -1;
                    }

                    //stack start
                    stack_node *to_push = (stack_node *)malloc(sizeof(stack_node));
                    
                    make_new->next = last_tree;
                    make_new->parent = emp;

                    to_push->data = make_new;
                    to_push->next = NULL;
                    if(!(make_new->enum_value == eps_enum &&make_new->is_terminal == 1))
                        push(to_push);

                    last_tree = make_new;
                    last = second_last;
                    second_last = second_last->prev;
              
                }
            }
        }
        //Creating a parent link if the parent is a non terminal and avoiding creation of link with T_DOLLAR.
        if(emp->is_terminal == 0 && make_new->enum_value != T_DOLLAR){
            emp->child = make_new;
            
            
            if(make_new->enum_value == T_EPS){
                strcpy(make_new->str, " ");
            }
            // //tester
            // printf("Parent is %s and rule no is %d --> ", non_terminal_map[emp->enum_value], emp->branch_no);
            // tree_node *temp = emp->child;
            // while(temp!= NULL){
            //     if(temp->is_terminal == 1){
            //         printf(" %s ", terminal_map[temp->enum_value]);
            //     }
            //     else{
            //         printf(" %s ", non_terminal_map[temp->enum_value]);
            //     }
            //     temp = temp->next;
            // }
            // printf("\n");
        }
            
        emp = peek();        
    }
    if(head->data->enum_value == T_DOLLAR && temp->Token == T_DOLLAR && err_count == 0){
        printf("SUCCESSSFUL PARSING IS DONE !!!!!!\n");
    }
    
    return p_tree;

}


tree_node*  initialize(){
        ptree_nodes++;
		tree_node *node=(tree_node*)malloc(sizeof(tree_node));
		node->next=NULL;
		node->parent=NULL;
		node->child=NULL;
		node->is_terminal=false;
		node->level=0;
		node->line_number=0;
		return node;
}

void display_stack(){
	if(head==NULL)
			printf("not\n");
	stack_node *temp2=head;
	while(temp2!= NULL){
		if(temp2->data->is_terminal){
			printf("%s\n",terminal_map[temp2->data->enum_value]);
		}
		else{
			printf("%s\n",non_terminal_map[temp2->data->enum_value]);
		}
		temp2= temp2->next;
	}
}

void printParseTree(tree_node *curr_node, FILE *fp){
        
    if(curr_node!=NULL && curr_node->parent!=NULL && curr_node->parent->child == curr_node){
        printParseTree(curr_node->child, fp);	
        if(curr_node->is_terminal){
                    if(curr_node->enum_value == 0){
                        //RNUM
                        double lex_rnum;
                        sscanf(curr_node->str, "%lf", &lex_rnum);
                        fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%lf\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n",curr_node->str,curr_node->line_number,terminal_map[curr_node->enum_value],lex_rnum,non_terminal_map[curr_node->parent->enum_value],"TRUE","terminal");
                    }
                    else if (curr_node->enum_value == 1){
                        //NUM
                        int lex_num = atoi(curr_node->str);
                        fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%d\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n",curr_node->str,curr_node->line_number,terminal_map[curr_node->enum_value],lex_num,non_terminal_map[curr_node->parent->enum_value],"TRUE","terminal");
                    }
                    else{
                        fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%s\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n",curr_node->str,curr_node->line_number,terminal_map[curr_node->enum_value],"N/A",non_terminal_map[curr_node->parent->enum_value],"TRUE","terminal");
                    }
        }       
        else{
            fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%s\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n","----",curr_node->line_number,non_terminal_map[curr_node->enum_value],"N/A",non_terminal_map[curr_node->parent->enum_value],"FALSE",non_terminal_map[curr_node->enum_value]);
        }
        
        if(curr_node->child != NULL){
            printParseTree(curr_node->child->next, fp);
        }
        curr_node = curr_node->next;
    }
    else {
        while(curr_node!=NULL){
	printParseTree(curr_node->child, fp);	    
if(curr_node->is_terminal){
            if(curr_node->enum_value == 0){
                //RNUM
                double lex_rnum;
                sscanf(curr_node->str, "%lf", &lex_rnum);
                fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%lf\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n",curr_node->str,curr_node->line_number,terminal_map[curr_node->enum_value],lex_rnum,non_terminal_map[curr_node->parent->enum_value],"TRUE","terminal");
            }
            else if (curr_node->enum_value == 1){
                //NUM
                int lex_num = atoi(curr_node->str);
                fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%d\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n",curr_node->str,curr_node->line_number,terminal_map[curr_node->enum_value],lex_num,non_terminal_map[curr_node->parent->enum_value],"TRUE","terminal");
            }
            else{
                fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%s\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n",curr_node->str,curr_node->line_number,terminal_map[curr_node->enum_value],"N/A",non_terminal_map[curr_node->parent->enum_value],"TRUE","terminal");
            }
        }
        else{
            if(curr_node->parent != NULL){fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%s\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n","----",curr_node->line_number,non_terminal_map[curr_node->enum_value],"N/A",non_terminal_map[curr_node->parent->enum_value],"FALSE",non_terminal_map[curr_node->enum_value]);}
        
            else {fprintf(fp,"%s\t\t%d\t\t%s\t\t\t%s\t\t\t%s\t\t\t\t%s\t\t\t\t%s\n\n","----",curr_node->line_number,non_terminal_map[curr_node->enum_value],"N/A","No Parent (root)","FALSE",non_terminal_map[curr_node->enum_value]);}}

            if(curr_node->child != NULL){
                printParseTree(curr_node->child->next, fp);
            }
        curr_node = curr_node->next;
        }
    }
    return ;
}

void time_taken(char* filename){
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();
                                    // invoke your lexer and parser here
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("File is not available \n");
    }
    parseTree *main_tree = parseInputSourceCode();

    end_time = clock();
    total_CPU_time  =  (double) (end_time - start_time);
    total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;
    printf("Total CPU Time %f\n",total_CPU_time);
    printf("Total CPU Time In Seconds %f\n",total_CPU_time_in_seconds);
    fclose(fp);
}

ASTNode* make_node(tree_node* pt_node){

    ASTNode* temp=(ASTNode*)malloc(sizeof(ASTNode));
    temp->parent = temp->forward=temp->left_child=temp->right_child=NULL;
    temp->data.level = pt_node->level;
	temp->data.line_number = pt_node->line_number;
	temp->data.enum_value = pt_node->enum_value;
	strcpy(temp->data.str,pt_node->str);
	temp->data.is_terminal = pt_node -> is_terminal; 
    //Exceptions
    if(temp->data.enum_value == NT_optional && !(temp->data.is_terminal))
    {
        temp->data.enum_value = T_ASSIGNOP;
        temp->data.is_terminal = 1;
    }
    if(temp->data.enum_value == NT_ret && !(temp->data.is_terminal))
    {
        temp->data.enum_value = T_RETURNS;
        temp->data.is_terminal = 1;
    }
    if(temp->data.enum_value == NT_default && !(temp->data.is_terminal))
    {
        temp->data.enum_value = T_DEFAULT;
        temp->data.is_terminal = 1;
    }
    return temp;
}



ASTNode *postorder(tree_node *pTree, ASTNode *inh){
   //ANUBHAV PARTS STARTS
   if(pTree->is_terminal==0 && pTree->enum_value==NT_program){
        //program-> moduleDeclarations otherModules(1) driverModule otherModules(2) 
        tree_node *pt_moduleDeclarations=pTree->child;
        ASTNode *ast_moduleDeclarations=make_node(pt_moduleDeclarations);
        tree_node *pt_otherModules1=pt_moduleDeclarations->next;
        ASTNode *ast_otherModules1=make_node(pt_otherModules1);
        tree_node *pt_driverModule=pt_otherModules1->next;
        ASTNode* driverModule_syn=postorder(pt_driverModule,NULL);
        tree_node *pt_otherModules2=pt_driverModule->next;
        ASTNode *ast_otherModules2=make_node(pt_otherModules2);
        ast_moduleDeclarations->forward=ast_otherModules1;
        ast_otherModules1->forward=driverModule_syn;
        driverModule_syn->forward=ast_otherModules2;
        ast_moduleDeclarations->left_child=postorder(pt_moduleDeclarations,NULL);
        ast_otherModules1->left_child=postorder(pt_otherModules1,NULL);
        ast_otherModules2->left_child=postorder(pt_otherModules2,NULL);
        ASTNode* program_syn=ast_moduleDeclarations;
        return program_syn;
        
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_moduleDeclarations){
        //moduleDeclarations(1) -> moduleDeclaration moduleDeclarations(2)
        if(pTree->branch_no==0){
            tree_node *pt_moduleDeclaration=pTree->child;            
            ASTNode* moduleDeclaration_syn = postorder(pt_moduleDeclaration, NULL);
            ASTNode* moduleDeclarations2_syn = postorder(pt_moduleDeclaration->next,NULL);
            moduleDeclaration_syn->forward=moduleDeclarations2_syn;
            ASTNode* moduleDeclarations1_syn =  moduleDeclaration_syn;
            return moduleDeclarations1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_moduleDeclaration){
        tree_node* pt_Declare=pTree->child;
        tree_node* id=pt_Declare->next->next;
        ASTNode *ast_id=make_node(id);
        ASTNode* moduleDeclaration_syn=ast_id;
        return moduleDeclaration_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_otherModules){
        if(pTree->branch_no==0){
            //otherModules(1) module otherModules(2)
            tree_node *pt_module=pTree->child;
            ASTNode *module_syn=postorder(pt_module,NULL);
            ASTNode *otherModules2_syn=postorder(pt_module->next,NULL);
            module_syn->forward=otherModules2_syn;
            ASTNode *otherModules1_syn=module_syn;
            return otherModules1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }

    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_driverModule){
        //driverModule->DRIVERDEF DRIVER PROGRAM DRIVERENDDEF moduleDef 
        tree_node *pt_PROGRAM=pTree->child->next->next;
        ASTNode *ast_PROGRAM=make_node(pt_PROGRAM);
        tree_node *pt_moduleDef=pt_PROGRAM->next->next;
        ASTNode* moduleDef_syn=postorder(pt_moduleDef,NULL);
        ast_PROGRAM->left_child=moduleDef_syn;
        ASTNode *driverModule_syn=ast_PROGRAM;
        return driverModule_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_module){
        //module->DEF MODULE ID ENDDEF TAKES INPUT SQBO input_plist SQBC SEMICOL ret moduleDef
        tree_node *pt_MODULE=pTree->child->next;
        ASTNode *ast_MODULE=make_node(pt_MODULE);
        tree_node *pt_ID=pt_MODULE->next;
        ASTNode *ast_ID=make_node(pt_ID);
        ast_MODULE->left_child=ast_ID;
        tree_node *pt_input_plist=pt_ID->next->next->next->next->next;
        ASTNode *input_plist_syn=postorder(pt_input_plist,NULL);
        ast_ID->forward=input_plist_syn;
        //
        tree_node *pt_ret=pt_input_plist->next->next->next;
        ASTNode *AST_ret = make_node(pt_ret); //3
        ASTNode *ret_syn=postorder(pt_ret,NULL);
        AST_ret ->left_child = ret_syn; //4
        //
        ASTNode *moduleDef_syn=postorder(pt_ret->next,NULL);
        input_plist_syn->forward=AST_ret; //1 
        AST_ret->forward=moduleDef_syn; //2
        ASTNode *module_syn=ast_MODULE;
        return module_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_ret){
        if(pTree->branch_no==0){
            //ret->RETURNS SQBO output_plist SQBC SEMICOL
            tree_node *pt_RETURNS = pTree->child;
            tree_node *pt_output_plist=pt_RETURNS->next->next;
            ASTNode *output_plist_syn=postorder(pt_output_plist,NULL);
            ASTNode *ret_syn=output_plist_syn;
            return ret_syn;
        }
        else if(pTree->branch_no==1){
            //ret -> EPS
            return NULL;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_input_plist){
        //input_plist->ID COLON dataType N1
        ASTNode *ast_input_plist=make_node(pTree);
        tree_node *pt_ID=pTree->child;
        ASTNode *ast_ID=make_node(pt_ID);
        ast_input_plist->left_child=ast_ID;
        ASTNode *dataType_syn=postorder(pt_ID->next->next,NULL);
        ast_ID->left_child=dataType_syn;
        ASTNode *N1_syn=postorder(pt_ID->next->next->next,NULL);
        ast_ID->forward=N1_syn;
        ASTNode *input_plist_syn=ast_input_plist;
        return input_plist_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_N1){
        if(pTree->branch_no==0){
            //N1(1)->COMMA ID COLON dataType N1(2)
            tree_node *pt_ID=pTree->child->next;
            ASTNode *ast_ID=make_node(pt_ID);
            ASTNode *dataType_syn=postorder(pt_ID->next->next,NULL);
            ast_ID->left_child=dataType_syn;
            ASTNode *N1_2_syn=postorder(pt_ID->next->next->next,NULL);
            ast_ID->forward=N1_2_syn;
            ASTNode *N1_1_syn=ast_ID;
            return N1_1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }    
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_output_plist){
        //output_plist->ID COLON type N2 
        ASTNode *ast_output_plist=make_node(pTree);
        tree_node *pt_ID=pTree->child;
        ASTNode *ast_ID=make_node(pt_ID);
        ast_output_plist->left_child=ast_ID;
        ASTNode *dataType_syn=postorder(pt_ID->next->next,NULL);
        ast_ID->left_child=dataType_syn;
        ASTNode *N2_syn=postorder(pt_ID->next->next->next,NULL);
        ast_ID->forward=N2_syn;
        ASTNode *output_plist_syn=ast_output_plist;
        return output_plist_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_N2){
        if(pTree->branch_no==0){
            //N2(1)->COMMA ID COLON type N2(2)
            tree_node *pt_ID=pTree->child->next;
            ASTNode *ast_ID=make_node(pt_ID);
            ASTNode *type_syn=postorder(pt_ID->next->next,NULL);
            ast_ID->left_child=type_syn;
            ASTNode *N2_2_syn=postorder(pt_ID->next->next->next,NULL);
            ast_ID->forward=N2_2_syn;
            ASTNode *N2_1_syn=ast_ID;
            return N2_1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }    
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_dataType){
        if(pTree->branch_no==0 || pTree->branch_no==1 || pTree->branch_no==2){
            //dataType->INTEGER | REAL | BOOLEAN 
            tree_node *pt_child=pTree->child;
            ASTNode *ast_child=make_node(pt_child);
            ASTNode *dataType_syn=ast_child;
            return dataType_syn;
        }
        else if(pTree->branch_no==3){
            //dataType->ARRAY SQBO range_arrays SQBC OF type
            tree_node *pt_ARRAY=pTree->child;
            ASTNode *ast_ARRAY=make_node(pt_ARRAY);
            ASTNode *range_arrays_syn=postorder(pt_ARRAY->next->next,NULL);
            ast_ARRAY->left_child=range_arrays_syn;
            tree_node *pt_type=pt_ARRAY->next->next->next->next->next;
            ASTNode *Type_syn=postorder(pt_type,NULL);
            range_arrays_syn->forward = Type_syn;
            //ast_ARRAY->right_child=Type_syn;
            ASTNode *dataType_syn=ast_ARRAY;
            return dataType_syn;
        } 
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_range_arrays){
        //range_arrays->index(1) RANGEOP index(2)
        tree_node *pt_index1=pTree->child;
        ASTNode *AST_rangeop = make_node(pt_index1->next);
        
        ASTNode *index1_syn=postorder(pt_index1,NULL);
        ASTNode *index2_syn=postorder(pt_index1->next->next,NULL);
        AST_rangeop->left_child = index1_syn;
        index1_syn->forward=index2_syn;
        ASTNode *range_arrays_syn=AST_rangeop;
        return range_arrays_syn;

    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_type){
        //type->INTEGER | REAL | BOOLEAN 
        tree_node *type_child=pTree->child;
        ASTNode *ast_type_child=make_node(type_child);
        ASTNode *type_syn=ast_type_child;
        return type_syn; 

    }
    
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_moduleDef){
        //moduleDef->START statements END
        tree_node *pt_statements=pTree->child->next;
        ASTNode * ast_statements=make_node(pt_statements);
        ast_statements->left_child=postorder(pt_statements,NULL);
        ASTNode *moduleDef_syn=ast_statements;
        return moduleDef_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_statements){
        if(pTree->branch_no==0){
            //statements(1)->statement statements(2)
            tree_node *pt_statement=pTree->child;            
            ASTNode* statement_syn = postorder(pt_statement, NULL);
            ASTNode* statements2_syn = postorder(pt_statement->next,NULL);
            statement_syn->forward=statements2_syn;
            ASTNode* statements1_syn = statement_syn;
            return statements1_syn;

        }
        else if(pTree->branch_no==1){
            return NULL;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_statement){
        //statement ioStmt | simpleStmt | declareStmt | conditionalStmt | iterativeStmt
        tree_node *pt_stat_child=pTree->child;
        ASTNode *stat_child_syn=postorder(pt_stat_child,NULL);
        ASTNode *statement_syn=stat_child_syn;
        return statement_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_ioStmt){
        
        if(pTree->branch_no==0){
            //ioStmt->GET_VALUE BO ID BC SEMICOL
            tree_node *pt_GET_VALUE=pTree->child;
            ASTNode *ast_GET_VALUE=make_node(pt_GET_VALUE);
            tree_node *pt_ID=pt_GET_VALUE->next->next;
            ASTNode *ast_ID=make_node(pt_ID);
            ast_GET_VALUE->left_child=ast_ID;
            ASTNode *ioStmt_syn=ast_GET_VALUE;
            return ioStmt_syn;
        }
        else if(pTree->branch_no==1){
            //ioStmt->PRINT BO var BC SEMICOL
            tree_node *pt_PRINT=pTree->child;
            ASTNode *ast_PRINT=make_node(pt_PRINT);
            tree_node *pt_var=pt_PRINT->next->next;
            ASTNode *var_syn=postorder(pt_var,NULL);
            ast_PRINT->left_child=var_syn;
            ASTNode *ioStmt_syn= ast_PRINT;
            return ioStmt_syn;
        }
        
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_var){
        //var->var_id_num | boolconstt
        tree_node *pt_var_child=pTree->child;
        ASTNode *var_child_syn=postorder(pt_var_child,NULL);
        ASTNode *var_syn=var_child_syn;
        return var_syn; 
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_var_id_num){
        if(pTree->branch_no==0){
            //var_id_num->ID whichID 
            tree_node *pt_ID=pTree->child;
            ASTNode *ast_ID=make_node(pt_ID);
            ASTNode *whichID_syn=postorder(pt_ID->next,NULL);
            ast_ID->forward=whichID_syn;
            ASTNode *var_id_num_syn=ast_ID;
            return var_id_num_syn;
        }
        else if(pTree->branch_no==1 || pTree->branch_no==2){
            //var_id_num->NUM | RNUM
            tree_node *pt_child=pTree->child;
            ASTNode *ast_child=make_node(pt_child);
            ASTNode *var_id_num_syn=ast_child;
            return var_id_num_syn;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_boolconstt){
        //boolconstt->TRUE|FALSE
        //no need of branching in this case;
        tree_node *bool_child=pTree->child;
        ASTNode *ast_bool_child=make_node(bool_child);
        ASTNode *boolconstt_syn=ast_bool_child;
        return boolconstt_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_whichID){
        if(pTree->branch_no==0){
            //whichID->SQBO index SQBC
            tree_node *pt_index=pTree->child->next;
            ASTNode *index_syn=postorder(pt_index,NULL);
            ASTNode *whichID_syn=index_syn;
            return whichID_syn;
        }
        else if(pTree->branch_no==1){
            //whichID->EPS
            /*ASTNode *whichID_inh=inh;
            ASTNode *whichID_syn=whichID_inh;
            return whichID_syn;*/
            return NULL;
        }    
    }
   //ANUBHAV PART ENDS

   //ASHISH PART-1 STARTS
   if(pTree->is_terminal == 0 && pTree->enum_value == NT_simpleStmt && pTree->branch_no == 0 )
    {
        //simpleStmt -> assignmentStmt
        ASTNode *AST_assignmentStmt , *simpleStmt_syn;
        AST_assignmentStmt = make_node(pTree->child);
        AST_assignmentStmt->left_child = postorder(pTree->child,NULL);
        simpleStmt_syn = AST_assignmentStmt;
        return simpleStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_simpleStmt && pTree->branch_no == 1 )
    {
        //simpleStmt->moduleReuseStmt
        ASTNode *AST_moduleReuseStmt , *simpleStmt_syn;
        AST_moduleReuseStmt = make_node(pTree->child);
        AST_moduleReuseStmt->left_child = postorder(pTree->child,NULL);
        simpleStmt_syn = AST_moduleReuseStmt;
        return simpleStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_assignmentStmt && pTree->branch_no == 0 )
    {
        //assignmentStmt -> ID whichStmt
        ASTNode *AST_ID = make_node(pTree->child);
        ASTNode *whichStmt_inh = AST_ID;
        ASTNode *whichStmt_syn =  postorder(pTree->child->next,whichStmt_inh);
        ASTNode *assignmentStmt_syn = whichStmt_syn;
        return assignmentStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_whichStmt && pTree->branch_no == 0 )
    {
        //whichStmt -> lvalueIDStmt
        ASTNode *lvalueIDStmt_inh = inh;
        ASTNode *whichStmt_syn = postorder(pTree->child,lvalueIDStmt_inh);
        return whichStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_whichStmt && pTree->branch_no == 1)
    {
        //whichStmt -> lvalueARRStmt
        ASTNode *lvalueARRStmt_inh = inh;
        ASTNode *whichStmt_syn = postorder(pTree->child,lvalueARRStmt_inh);
        return whichStmt_syn;
    }
    
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_lvalueIDStmt && pTree->branch_no == 0)
    {
        //lvalueIDStmt -> ASSIGNOP expression SEMICOL
        ASTNode *AST_ASSIGNOP = make_node(pTree->child);
        tree_node *PT_expression = pTree->child->next;
        AST_ASSIGNOP -> left_child = inh;
        AST_ASSIGNOP -> right_child = postorder(PT_expression,NULL);
        return AST_ASSIGNOP;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_lvalueARRStmt && pTree->branch_no == 0)
    {
        //lvalueARRStmt-> SQBO index SQBC ASSIGNOP expression SEMICOL
        ASTNode *index_syn = postorder(pTree->child->next,NULL);
        tree_node *PT_ASSIGNOP = pTree -> child ->next -> next-> next;
        ASTNode *AST_ASSIGNOP = make_node(PT_ASSIGNOP);
        tree_node *PT_expression = PT_ASSIGNOP->next;
        AST_ASSIGNOP -> left_child = inh;
        inh -> forward = index_syn;
        AST_ASSIGNOP -> right_child = postorder(PT_expression,NULL);
        return AST_ASSIGNOP;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_index && pTree->branch_no == 0)
    {
        //index->NUM
        ASTNode * AST_NUM = make_node(pTree -> child);
        return AST_NUM;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_index && pTree->branch_no == 1)
    {
        //index->ID
        ASTNode * AST_ID = make_node(pTree -> child);
        return AST_ID;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_moduleReuseStmt && pTree->branch_no == 0)
    {
        //moduleReuseStmt -> optional USE MODULE ID WITH PARAMETERS idList SEMICOL
        ASTNode *AST_Optional = make_node(pTree->child);
        ASTNode * optional_syn = postorder(pTree->child, NULL);
        tree_node * PT_ID = pTree->child->next->next->next;
        ASTNode *AST_ID = make_node(PT_ID);
        AST_Optional->left_child = optional_syn;
        AST_Optional->right_child = AST_ID;
        tree_node *PT_idList = PT_ID->next->next->next;
        AST_ID->forward = postorder(PT_idList,NULL);
        ASTNode *moduleReuseStmt_syn = AST_Optional;
        return moduleReuseStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_optional && pTree->branch_no == 0)
    {
        //optional -> SQBO idList SQBC ASSIGNOP
        ASTNode * optional_syn = postorder(pTree->child->next,NULL);
        return optional_syn; 
    }
    
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_optional && pTree->branch_no == 1)
    {
        //optional -> EPS
        ASTNode * optional_syn = NULL;
        return optional_syn;
        
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_idList && pTree->branch_no == 0)
    {
        //idList -> ID N3
        ASTNode * AST_idList = make_node(pTree);
        ASTNode * AST_ID = make_node(pTree->child);
        ASTNode * N3_inh = AST_ID;
        AST_idList->left_child = postorder(pTree->child->next,N3_inh);
        return AST_idList;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N3 && pTree->branch_no == 0)
    {
        //N3(1) -> COMMA ID N3(2)
        tree_node *PT_ID = pTree->child->next;
        tree_node *PT_N3 = PT_ID->next;
        ASTNode * AST_ID = make_node(PT_ID);
        ASTNode *N3_2_inh = AST_ID;
        ASTNode *N3_1_inh = inh;
        N3_1_inh -> forward = postorder(PT_N3,N3_2_inh);
        return N3_1_inh;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N3 && pTree->branch_no == 1)
    {
        //N3->EPS
        return inh;
    }
   //ASHISH PART-1 ENDS

    //ANIRUDH PART STARTS    
    //1. Expression -> arithmeticOrBooleanExpr
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_expression){
        if(pTree->branch_no == 0){
            //Expression.syn = arithmeticOrBooleanExpr.syn
            tree_node *PT_arithmeticOrBooleanExpr = pTree->child;
            ASTNode *Expression_syn = postorder(PT_arithmeticOrBooleanExpr, NULL);
            return Expression_syn;

        }
        else if(pTree->branch_no == 1){
            //Expression.syn = u1.syn
            tree_node *PT_u1 = pTree->child;
            ASTNode *Expression_syn = postorder(PT_u1, NULL);
            return Expression_syn;

        }
    }

    //2. u1 -> unary_op new_NT
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_u1){
        if(pTree->branch_no == 0){
            // new_NT.inh=uniary_op.syn
            // U1.syn = new_NT.syn

            tree_node *PT_unary_op = pTree->child;
            ASTNode *unary_op_syn = postorder(PT_unary_op, NULL);
            tree_node *PT_new_NT = PT_unary_op->next;
            //new_NT will inherit AST_unary_op
            ASTNode *new_NT_syn = postorder(PT_new_NT, unary_op_syn);
            ASTNode *u1_syn = new_NT_syn;
            return u1_syn;
        }
    }

    //3. new_NT -> BO arithmeticExpr BC | var_id_num
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_new_NT){
        if(pTree->branch_no == 0){
            //new_NT.inh->left_child = arithmeticExpr.syn
            //new_NT.syn = new_NT.inh
            tree_node *PT_arithmeticExpr = pTree->child->next;
            ASTNode *arithmeticExpr_syn = postorder(PT_arithmeticExpr, NULL);
            //here I inherit the unary op
            ASTNode *New_NT_inh = inh;
            New_NT_inh->left_child = arithmeticExpr_syn;
            ASTNode *New_NT_syn = New_NT_inh;
            return New_NT_syn;
        }
        else if(pTree->branch_no == 1){
            //new_NT.inh->left_child = var_id_num.syn
            //new_NT.syn = new_NT.inh
            tree_node *PT_var_id_num = pTree->child;
            ASTNode *var_id_syn = postorder(PT_var_id_num, NULL);
            //I inherit unary_op
            ASTNode *New_NT_inh = inh;
            New_NT_inh->left_child = var_id_syn;
            ASTNode *New_NT_syn = New_NT_inh;
            return New_NT_syn;

        }
    }

    //4. unary_op -> PLUS | MINUS
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_unary_op){
        if(pTree->branch_no == 0){
            // node=make_node(‘PLUS’,type=’UniaryOp’)
            // Unary_op.syn=node
            tree_node *PT_plus = pTree->child;
            ASTNode *AST_plus = make_node(PT_plus);
            ASTNode *Unary_op_syn = AST_plus;
            return Unary_op_syn;

        }
        else if(pTree->branch_no == 1){
            //node=make_node(‘MINUS’,type=’UniaryOp’)
            //Unary_op.syn=node
            tree_node *PT_minus = pTree->child;
            ASTNode *AST_minus = make_node(PT_minus);
            ASTNode *Unary_op_syn = AST_minus;
            return Unary_op_syn;
        }
    }

    //5. arithmeticOrBooleanExpr AnyTerm N7 ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_arithmeticOrBooleanExpr){
        if(pTree->branch_no == 0){
            //N7.inh = AnyTerm.syn
            //arithmeticOrBooleanExpr.syn = N7.syn
            tree_node *PT_anyterm = pTree->child;
            tree_node *PT_n7 = PT_anyterm->next;
            ASTNode *Anyterm_syn = postorder(PT_anyterm, NULL);
            ASTNode *N7_syn = postorder(PT_n7, Anyterm_syn);
            ASTNode *ArithmeticOrBooleanExpr_syn = N7_syn;
            return ArithmeticOrBooleanExpr_syn;
        }
    }

    //6. N7 logicalOp AnyTerm N7 | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N7){
        if(pTree->branch_no == 0){
            // N7(2).inh = AnyTerm.syn
            // logicalOp.syn-> left_child = N7(1).inh
            // logicalOp.syn -> right_child = N7(2).syn
            // N7(1).syn = logicalOp.syn
            tree_node *PT_logical_op = pTree->child;
            tree_node *PT_anyterm = PT_logical_op->next;
            tree_node *PT_n7_2 = PT_anyterm->next;

            ASTNode *Logical_op_syn = postorder(PT_logical_op, NULL);
            ASTNode *Anyterm_syn = postorder(PT_anyterm, NULL);
            ASTNode *N7_2_syn = postorder(PT_anyterm, Anyterm_syn);

            //filling left and right operands
            ASTNode *N7_1_inh = inh;
            Logical_op_syn->left_child = N7_1_inh;
            Logical_op_syn->right_child = N7_2_syn;

            ASTNode *N7_1_syn = Logical_op_syn;
            return N7_1_syn;
        }
        else if(pTree->branch_no == 1){
            //N7.syn = N7.inh
            ASTNode *N7_inh = inh;
            ASTNode *N7_syn = N7_inh;
            return N7_syn;

        }
    }

    //7. AnyTerm arithmeticExpr N8 | boolconstt ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_AnyTerm){
        if(pTree->branch_no == 0){
            //N8.inh = arithmeticExpr.syn
            //AnyTerm.syn = N8.syn
            tree_node *PT_arithmeticExpr = pTree->child;
            tree_node *PT_n8 = PT_arithmeticExpr->next;

            ASTNode *ArithmeticExpr_syn = postorder(PT_arithmeticExpr, NULL);
            ASTNode *N8_syn = postorder(PT_n8, ArithmeticExpr_syn);

            ASTNode *AnyTerm_syn = N8_syn;
            return AnyTerm_syn;

    
        }
        else if(pTree->branch_no == 1){
            //AnyTerm.syn = boolconstt.syn
            tree_node *PT_boolconstt = pTree->child;
            ASTNode *Boolconstt_syn = postorder(PT_boolconstt, NULL);
            
            ASTNode *Anyterm_syn = Boolconstt_syn;
            return Anyterm_syn;
        }
    }

    //8. N8 relationalOp arithmeticExpr | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N8){
        if(pTree->branch_no == 0){
            //relationalOp.syn->left_child = N8.inh
            //relationalOp.syn->right_child = arithmeticExpr.syn
            //N8.syn = realtionalOp.syn
            tree_node *PT_relationalOp = pTree->child;
            tree_node *PT_arithmeticExpr = PT_relationalOp->next;
            
            ASTNode *Relational_op_syn = postorder(PT_relationalOp, NULL);
            ASTNode *ArithmeticExpr_syn = postorder(PT_arithmeticExpr, NULL);
            //inherited left operand
            ASTNode *N8_inh = inh;
            //filling operand
            Relational_op_syn->left_child = N8_inh;
            Relational_op_syn->right_child = ArithmeticExpr_syn;

            ASTNode *N8_syn = Relational_op_syn;
            return N8_syn;
        }
        else if(pTree->branch_no == 1){
            //N8.syn = N8.inh
            ASTNode *N8_inh = inh;
            ASTNode *N8_syn = N8_inh;
            return N8_syn;
        }
    }

    //9. arithmeticExpr term N4 ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_arithmeticExpr){
        if(pTree->branch_no == 0){
            //N4.inh = term.syn
            //arithmeticExpr.syn = N4.syn
            tree_node *PT_term = pTree->child;
            tree_node *PT_n4 = PT_term->next;

            ASTNode *Term_syn = postorder(PT_term, NULL);
            ASTNode *N4_syn = postorder(PT_n4, Term_syn);

            ASTNode *ArithmeticExpr_syn = N4_syn;
            return ArithmeticExpr_syn;
        }
    }

    //10. N4 op1 term N4 | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N4){
        if(pTree->branch_no == 0){
            // N4(2).inh = term.syn
            // op1.syn->left_child = N4(1).inh
            // op1.syn->right_child = N4(2).syn
            // N4(1).syn = op1.syn
            tree_node *PT_op1 = pTree->child;
            tree_node *PT_term = PT_op1->next;
            tree_node *PT_n4_2 = PT_term->next;

            ASTNode *Op1_syn = postorder(PT_op1, NULL);
            ASTNode *Term_syn = postorder(PT_term, NULL);
            ASTNode *N4_2_syn = postorder(PT_n4_2, Term_syn);

            //left operand inherit
            ASTNode *N4_1_inh = inh;
            //filling operands
            Op1_syn->left_child = N4_1_inh;
            Op1_syn->right_child = N4_2_syn;

            ASTNode *N4_1_syn = Op1_syn;
            return N4_1_syn;
        }
        else if(pTree->branch_no == 1){
            //N4.syn = N4.inh
            ASTNode *N4_inh = inh;
            ASTNode *N4_syn = N4_inh;
            return N4_syn;

        }
    }

    //11. op1 PLUS | MINUS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_op1){
        if(pTree->branch_no == 0){
            //temp = make_node(PLUS)
            //op1.syn = temp
            tree_node *PT_plus = pTree->child;
            ASTNode *AST_plus = make_node(PT_plus);
            ASTNode *Op1_syn = AST_plus;
            return Op1_syn;



        }
        else if(pTree->branch_no == 1){
            //temp = make_node(MINUS)
            //op1.syn = temp
            tree_node *PT_minus = pTree->child;
            ASTNode *AST_minus = make_node(PT_minus);
            ASTNode *Op1_syn = AST_minus;
            return Op1_syn;



        }
    }

    //12. term factor N5 ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_term){
        if(pTree->branch_no == 0){
            //N5.inh = factor.syn
            //term.syn = N5.syn
            tree_node *PT_factor = pTree->child;
            tree_node *PT_n5 = PT_factor->next;

            ASTNode *Factor_syn = postorder(PT_factor, NULL);
            ASTNode *N5_syn = postorder(PT_n5, Factor_syn);

            ASTNode *Term_syn = N5_syn;
            return Term_syn;
        }
    }

    //13. N5 op2 factor N5 | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N5){
        if(pTree->branch_no == 0){
            // N5(2).inh = factor.syn
            // op2.syn->left_child = N5(1).inh
            // op2.syn->right_child = N5(2).syn
            // N5(1).syn = op2.syn
            tree_node *PT_op2 = pTree->child;
            tree_node *PT_factor = PT_op2->next;
            tree_node *PT_N5_2 = PT_factor->next;

            ASTNode *Op2_syn = postorder(PT_op2, NULL);
            ASTNode *Factor_syn = postorder(PT_factor, NULL);
            ASTNode *N5_2_syn = postorder(PT_N5_2, Factor_syn);

            //getting left operand
            ASTNode *N5_1_inh = inh;
            Op2_syn->left_child = N5_1_inh;
            Op2_syn->right_child = N5_2_syn;

            ASTNode *N5_1_syn = Op2_syn;
            return N5_1_syn;     
        }
        else if(pTree->branch_no == 1){
            //N5.syn = N5.inh
            ASTNode *N5_inh = inh;
            ASTNode *N5_syn = N5_inh;
            return N5_syn;
        }
    }

    //14. op2 -> MUL|DIV
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_op2){
        if(pTree->branch_no == 0){
            // temp = make_node(MUL)
            // Op2.syn = temp

            tree_node *PT_mul = pTree->child;
            ASTNode *AST_mul = make_node(PT_mul);
            ASTNode *Op2_syn = AST_mul;
            return Op2_syn;

        }
        else if(pTree->branch_no == 1){
            // temp = make_node(DIV)
            // Op2.syn = temp

            tree_node *PT_div = pTree->child;
            ASTNode *AST_div = make_node(PT_div);
            ASTNode *Op2_syn = AST_div;
            return Op2_syn;

        }
    }

    //15. factor -> BO arithmeticOrBooleanExpr BC | var_id_num
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_factor){
        if(pTree->branch_no == 0){
            //factor.syn = arithmeticOrBooleanExpr.syn
            tree_node *PT_arithmeticOrBooleanExpr = pTree->child->next;
            ASTNode *ArithmeticOrBooleanExpr_syn = postorder(PT_arithmeticOrBooleanExpr, NULL);
            ASTNode *Factor_syn = ArithmeticOrBooleanExpr_syn;
            return Factor_syn;

        }
        else if(pTree->branch_no == 1){
            //factor.syn = var_id_num.syn
            tree_node *PT_var_id_num = pTree->child;
            ASTNode *Var_id_num_syn = postorder(PT_var_id_num, NULL);
            ASTNode *Factor_syn = Var_id_num_syn;
            return Factor_syn;
        }
    }

    //16. logicalOp AND | OR ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_logicalOp){
        if(pTree->branch_no == 0){
            //temp = make_node(AND)
            //logicalOp.syn = temp
            tree_node *PT_and = pTree->child;
            ASTNode *AST_and = make_node(PT_and);
            ASTNode *Logicalop_syn = AST_and;
            return Logicalop_syn;

        }
        else if(pTree->branch_no == 1){
            //temp = make_node(AND)
            //logicalOp.syn = temp
            tree_node *PT_or = pTree->child;
            ASTNode *AST_or = make_node(PT_or);
            ASTNode *Logicalop_syn = AST_or;
            return Logicalop_syn;
        }

    }

    //17. relationalOp LT | LE | GT | GE | EQ | NE ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_relationalOp){
        //temp = make_node(‘the relop’)
        //relationalOp.syn = temp

        if(pTree->branch_no == 0){
            tree_node *PT_lt = pTree->child;
            ASTNode *AST_lt = make_node(PT_lt);
            ASTNode *Relationalop_syn = AST_lt;
            return Relationalop_syn;
        }
        else if(pTree->branch_no == 1){
            tree_node *PT_le = pTree->child;
            ASTNode *AST_le = make_node(PT_le);
            ASTNode *Relationalop_syn = AST_le;
            return Relationalop_syn;

        }
        else if(pTree->branch_no == 2){
            tree_node *PT_gt = pTree->child;
            ASTNode *AST_gt = make_node(PT_gt);
            ASTNode *Relationalop_syn = AST_gt;
            return Relationalop_syn;
            
        }
        else if(pTree->branch_no == 3){
            tree_node *PT_ge = pTree->child;
            ASTNode *AST_ge = make_node(PT_ge);
            ASTNode *Relationalop_syn = AST_ge;
            return Relationalop_syn;
            
        }
        else if(pTree->branch_no == 4){
            tree_node *PT_eq = pTree->child;
            ASTNode *AST_eq = make_node(PT_eq);
            ASTNode *Relationalop_syn = AST_eq;
            return Relationalop_syn;
            
        }
        else if(pTree->branch_no == 5){
            tree_node *PT_ne = pTree->child;
            ASTNode *AST_ne = make_node(PT_ne);
            ASTNode *Relationalop_syn = AST_ne;
            return Relationalop_syn;
            
        }

    }
    //ANIRUDH PART ENDS    

    //ASHISH PART -2 STARTS
     if(pTree->is_terminal == 0 && pTree->enum_value == NT_declareStmt && pTree->branch_no == 0)
    {
        //declareStmt -> DECLARE idList COLON dataType SEMICOL
        ASTNode *AST_Declare = make_node(pTree->child);
        tree_node * PT_idList = pTree->child->next;
        tree_node * PT_dataType = PT_idList->next->next;
        ASTNode *idList_syn = postorder(PT_idList,NULL);
        AST_Declare->left_child = idList_syn;
        idList_syn->forward = postorder(PT_dataType,NULL);
        return AST_Declare;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_conditionalStmt && pTree->branch_no == 0)
    {
        //conditionalStmt -> SWITCH BO ID BC START caseStmts default END
        ASTNode *AST_SWITCH = make_node(pTree->child);
        tree_node * PT_ID = pTree->child->next->next;
        tree_node * PT_caseStmts = PT_ID->next->next->next;
        tree_node * PT_default = PT_caseStmts->next;
        ASTNode *AST_ID = make_node(PT_ID);
        ASTNode *caseStmts_syn = postorder(PT_caseStmts,NULL);
        ASTNode *temp = caseStmts_syn;
        ASTNode *AST_default = make_node(PT_default);//passing a non terminal
        ASTNode *default_syn = postorder(PT_default,NULL); 
        AST_default ->left_child = default_syn;
        AST_SWITCH->left_child = AST_ID;
        AST_ID -> forward = caseStmts_syn;
        while(temp->forward!=NULL)
        {
            temp= temp->forward;
        }
        temp->forward = AST_default;

        return AST_SWITCH;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_caseStmts && pTree->branch_no == 0)
    {
        //caseStmts -> CASE value COLON statements BREAK SEMICOL N9
        tree_node * PT_value = pTree->child->next;
        tree_node * PT_statements = PT_value -> next -> next;
        tree_node * PT_N9 = PT_statements ->next->next->next;
        ASTNode * value_syn = postorder(PT_value,NULL);
        ASTNode * statements_syn = postorder(PT_statements,NULL);
        value_syn->left_child = statements_syn;
        ASTNode * N9_syn = postorder(PT_N9,value_syn);
        return N9_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N9 && pTree->branch_no == 0)
    {
        //N9(1) ->CASE value COLON statements BREAK SEMICOL N9(2)
        tree_node * PT_value = pTree->child->next;
        tree_node * PT_statements = PT_value -> next -> next;
        tree_node * PT_N9 = PT_statements ->next->next->next;
        ASTNode * value_syn = postorder(PT_value,NULL);
        ASTNode * statements_syn = postorder(PT_statements,NULL);
        value_syn->left_child = statements_syn;
        ASTNode * N9_1_inh = inh;
        ASTNode * N9_2_syn = postorder(PT_N9,value_syn);
        N9_1_inh ->forward = N9_2_syn;  
        return N9_1_inh;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N9 && pTree->branch_no == 1)
    {
        //N9 ->EPS
        return inh;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_value && pTree->branch_no == 0)
    {
        //value -> NUM
        return make_node(pTree->child);
    }
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_value && pTree->branch_no == 1)
    {
        //value -> TRUE
        return make_node(pTree->child);
    }
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_value && pTree->branch_no == 2)
    {
        //value -> FALSE
        return make_node(pTree->child);
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_default && pTree->branch_no == 0)
    {
        //default -> DEFAULT COLON statements BREAK SEMICOL
        tree_node *PT_statements = pTree->child->next->next;
        ASTNode *AST_statements = postorder(PT_statements,NULL);
        return AST_statements;
    }
    
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_default && pTree->branch_no == 1)
    {
        return NULL; // in case of DEFAULT it may be NULL as well.
        //Need to create a Default node in parent is a solution as in ModuleDeclarations or is this fine ??? Change appropriately.
    }


    if(pTree->is_terminal == 0 && pTree->enum_value == NT_iterativeStmt && pTree->branch_no == 0)
    {
        //iterativeStmt -> FOR BO ID IN range BC START statements END
        ASTNode *AST_FOR = make_node(pTree->child);
        tree_node *PT_ID = pTree->child->next->next;
        tree_node *PT_range = PT_ID ->next->next;
        tree_node *PT_statements = PT_range ->next->next->next;
        ASTNode * AST_ID = make_node(PT_ID);
        ASTNode * AST_range = postorder(PT_range,NULL);
        ASTNode * AST_statements = postorder(PT_statements,NULL);
        AST_FOR -> left_child = AST_ID;
        AST_ID->forward = AST_range;
        AST_range -> forward = AST_statements;
        return AST_FOR;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_iterativeStmt && pTree->branch_no == 1)
    {
        //iterativeStmt -> WHILE BO arithmeticOrBooleanExpr BC START statements END
        ASTNode *AST_WHILE = make_node(pTree->child);
        tree_node *PT_AOB = pTree->child->next->next;
        tree_node *PT_statements = PT_AOB ->next->next->next;
        ASTNode * AST_AOB = postorder(PT_AOB,NULL);
        ASTNode * AST_statements = postorder(PT_statements,NULL);
        AST_WHILE -> left_child = AST_AOB;
        AST_AOB -> forward = AST_statements;
        return AST_WHILE;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_range && pTree->branch_no == 0)
    {
        //range -> NUM(1) RANGEOP NUM(2)
        ASTNode *AST_NUM1 = make_node(pTree->child);
        ASTNode *AST_RANGEOP = make_node(pTree->child->next);
        ASTNode *AST_NUM2 = make_node(pTree->child->next->next);
        AST_RANGEOP->left_child = AST_NUM1;
        AST_NUM1->forward = AST_NUM2;
        return AST_RANGEOP;
        //See yourself why RangeOP is needed and there cant be a forward pointer b/w NUM1 and NUM2 w/o RangeOp as was discussed, otherwise ask.
    }
    // ASHISH PART -2 ENDS
    return NULL;
}

void fill_ast_parent(ASTNode *current){
    if(current == NULL)
        return;
    
    if(current->right_child == NULL){
        if(current->left_child != NULL){
            current->left_child->parent = current;
        }
        fill_ast_parent(current->left_child);

        if(current->forward != NULL){
            current->forward->parent = current->parent;
        }
        fill_ast_parent(current->forward);
    }

    else{
        if(current->left_child != NULL){
            current->left_child->parent = current;
        }
        fill_ast_parent(current->left_child);
        
        if(current->right_child != NULL){
            current->right_child->parent = current;
        }
        fill_ast_parent(current->right_child);

        if(current->forward != NULL){
            current->forward->parent = current->parent;
        }
        fill_ast_parent(current->forward);
    }
}


void printTree(ASTNode *current){
    if(current == NULL){
        return;
    }
    
    if(current->right_child == NULL){
        if(current->data.is_terminal == 1){
            printf(" %s \n",terminal_map[current->data.enum_value] );
        }
        else{
            printf(" %s \n",non_terminal_map[current->data.enum_value] );
        }

        printTree(current->left_child);
        printTree(current->forward);
    }

    else{
        printTree(current->left_child);
        
        if(current->data.is_terminal == 1){
            printf(" %s \n",terminal_map[current->data.enum_value] );
        }
        else{
            printf(" %s \n",non_terminal_map[current->data.enum_value]);
        }

        printTree(current->right_child);
        printTree(current->forward);
    }
}


#include "symbolTable.h"
extern char *terminal_map;
extern char *non_terminal_map;

int getBytes(ASTNode *node)
{
	if(node->data.enum_value == T_ARRAY){
        return getBytes(node->left_child->forward);
    }
    else if(node->data.enum_value == T_INTEGER){
        return 2;
    }
    else if(node->data.enum_value == T_REAL){
        return 4;
    }
    else if(node->data.enum_value == T_BOOLEAN){
        return 1;
    }
    return -1;
}

/// works for both datatype and type
type getType(ASTNode *node){
    type node_type;
    if(node->data.enum_value == T_ARRAY){
        struct array a;
        type temp = getType(node->left_child->forward);
        a.dt = temp.t.v.dt;
        if(node->left_child->left_child->data.enum_value == T_NUM){
            a.s_idx = atoi(node->left_child->left_child->data.str);
            a.is_static = true;
        }
        else {a.s_idx = -1;a.is_static = false;}
        if(node->left_child->left_child->forward->data.enum_value == T_NUM){
            a.e_idx = atoi(node->left_child->left_child->forward->data.str);
        }
        else {a.e_idx = -1;a.is_static = false;}
        node_type.t.a =a;
        node_type.is_array = 1;
        return node_type;
    }
    else if(node->data.enum_value == T_INTEGER){
        struct variable var;
        var.dt = INTEGER;
        node_type.t.v = var;
        node_type.is_array = 0;
        return node_type;
    }
    else if(node->data.enum_value == T_REAL){
        struct variable var;
        var.dt = REAL;
        node_type.t.v = var;
        node_type.is_array = 0;
        return node_type;
    }
    else if(node->data.enum_value == T_BOOLEAN){
        struct variable var;
        var.dt = BOOLEAN;
        node_type.t.v = var;
        node_type.is_array = 0;
        return node_type;
    }
}

void initialise_var_hash_table(struct var_hash_table_struct *table)
{
    int i;
    for(i = 0; i<SYMBOL_TABLE_SIZE; i++){
        table[i].size = 0;
        table[i].head = NULL;
    }
}

void initialise_func_st_root(){
    int i;
    for(i = 0; i<SYMBOL_TABLE_SIZE; i++){
        func_st_root[i].size = 0;
        func_st_root[i].head = NULL;
    }
}

unsigned long id_hash(char *sym){
    const int p = 67; // because numbers, lower & uppercase letters and underscore are present (also needs to be prime)
    unsigned long hash = 0;
    unsigned long power = 1;
    int i = 0;
    while(sym[i] != '\0')
    {
        hash = (hash + (sym[i] - '0' +1) * power) % SYMBOL_TABLE_SIZE;
        power= (power * p) % SYMBOL_TABLE_SIZE;
        i++;
    }
    return hash;
}

void insert_var_hash(var_hash_entry* entry, var_st* table){
    unsigned long key = id_hash(entry->lexeme);
    entry->next = table->var_hash_table[key].head;
    table->var_hash_table[key].head = entry;
    table->var_hash_table[key].size++;
}

void insert_func_hash(func_hash_entry* entry){
    unsigned long key = id_hash(entry->lexeme);
    entry->next = func_st_root[key].head; //func_st_root (global variable) has all the function names
    func_st_root[key].head = entry;
    func_st_root[key].size++;
}

func_hash_entry* find_func_hash(char* lex){
    unsigned long key = id_hash(lex);
    func_hash_entry *head = func_st_root[key].head;
    while(head!= NULL){
        if(strcmp(lex,head->lexeme) == 0){
            return head;
        }
    }
    return head;
}

/////////returns flag value i.e.  -1: error,  2: you can add,  3: add with flag 3
int check_var(char* lex, var_st* table){ ///// used while filling the symbol table
    unsigned long key = id_hash(lex);
    /////////////////check in root_var_id
    var_hash_entry *root_var_entry = table->root_var_st->var_hash_table[key].head;
    while(root_var_entry!= NULL){
        if(strcmp(lex,root_var_entry->lexeme) == 0){
            if(root_var_entry->flag == 1){
                ///////////////////////////do something
                printf("Semantic error");
                return -1;
            }
            else{
                break;
            }
        }
		root_var_entry = root_var_entry->next;
    }
    /////////////////////check in current var_st
    if(table == table->root_var_st){
        ////////////////// if present is root_var_st
        var_hash_entry *head = table->var_hash_table[key].head;
        while(head!= NULL){
            if(strcmp(lex,head->lexeme) == 0){
                
                if(head->flag == 1){
                ///////////////////////////do something for error
                    printf("semantic error");
                    return -1;
                }
                else if(head->flag == 0){
                    ////////////////////////add somthing to tell that you are allowed to declare using flag = 3
                    return 3;
                }
                else{
                ///////////////////////////do something for error
                    return -1;
                    printf("error");
                }
            }
			head = head->next;
        }
    }
    else{
        //////////////////if present is not root_var_st
        var_hash_entry *head = table->var_hash_table[key].head;
        while(head!= NULL){
            if(strcmp(lex,head->lexeme) == 0){
                ///////////////////////////do something for error
                return -1;
                printf("error");
            }
			head = head->next;
        }
    }
    return 2;
    //////////////add a return statement here for the condition when nothing is found and you can using flag = 2
}


////////////////////////this is function needs to be done
var_hash_entry* find_var(char* lex, int lno, var_st* table){
    if(table == NULL || lex == NULL){
        return NULL;
    }
    unsigned long key = id_hash(lex);
    while (1)
    {
        var_hash_entry *head = table->var_hash_table[key].head;
        while(head!= NULL){

            if(strcmp(lex,head->lexeme) == 0 && lno > head->lno){
                return head;
            }
            head = head->next;
        }
        if(table->tag == 0){ 
            break;
        }
        table = table->parent.v_parent;
    }
    return NULL; //////////return null if found nothing
}


void fill_symbol_table(ASTNode *current, var_st *current_var_st,char for_string[10][21],int size_for_string){
	var_st *next_var_st = current_var_st;
	if(current == NULL){
		return;
    }
    
    if(current->right_child == NULL){
        //module reuse statement
        //Semantic Rules 11,12,14 checked
        if((current->data.is_terminal == 1 && current->data.enum_value == T_ID)
                && (current->parent->data.is_terminal == 1 && current->parent->data.enum_value == T_ASSIGNOP)
                && (current->parent->parent->data.is_terminal != 1 && current->parent->parent->data.enum_value == NT_moduleReuseStmt)){

            //Function cannot be invoked recursively (14)
            func_hash_entry *func_entry = current_var_st->root_var_st->parent.f_parent;
            if(strcmp(func_entry->lexeme,current->data.str)==0){
                printf("\nSemantic Error: %s module cannot be invoked recursively (line number: %d)",current->data.str, current->data.line_number);
            }

            //Semantic rule 11 & 12 i.e. status of function declaration and defination before invoking it
            else{
                func_hash_entry *found_entry = find_func_hash(current->data.str);
                if(found_entry == NULL){
                    printf("\nSemantic Error: %s module neither declared nor defined (line number: %d)",current->data.str, current->data.line_number);
                }
                else if(found_entry->called == 0){
                    if(found_entry->declare_lno!=-1 && found_entry->declare_lno<current->data.line_number){
                        if(found_entry->define_lno!=-1 && found_entry->define_lno<current->data.line_number){
                            printf("\nSemantic Error: %s module both declared and defined before invoking (line number: %d)",current->data.str, current->data.line_number);
                        }
                        else if(found_entry->define_lno==-1 || found_entry->define_lno>current->data.line_number){
                            found_entry->called = 1;
                        }
                    }
                    else if(found_entry->declare_lno==-1){
                        if(found_entry->define_lno!=-1 && found_entry->define_lno<current->data.line_number){
                            found_entry->called = 1;
                        }
                        else if(found_entry->define_lno==-1 || found_entry->define_lno>current->data.line_number){
                            printf("\nSemantic Error: %s module not defined (line number: %d)",current->data.str, current->data.line_number);
                        }
                    }
                }
            }
        }

        //Filling var_st and func_st
        else if(current->data.is_terminal == 1 && (current->data.enum_value == T_ID || current->data.enum_value == T_PROGRAM)){
            //Module Defination
            if(current->data.enum_value == T_PROGRAM || current->parent->data.enum_value == T_MODULE){
                func_hash_entry *found_entry = find_func_hash(current->data.str);
                if(found_entry == NULL){
                    func_hash_entry *func_entry = (func_hash_entry *) malloc(sizeof(func_hash_entry));
                    strcpy(func_entry->lexeme,current->data.str);
                    func_entry->define_lno = current->data.line_number;
                    func_entry->declare_lno = -1;
                    func_entry->called = 0;
                    func_entry->inputList = NULL;
                    func_entry->outputList = NULL;
                    func_entry->next = NULL;
                    func_entry->child = (var_st *) malloc(sizeof(var_st));
                    func_entry->child->lno = func_entry->define_lno;
                    func_entry->child->depth = 1;
                    func_entry->child->next = NULL;
                    func_entry->child->child = NULL;
                    func_entry->child->tag = 0;
                    func_entry->child->parent.f_parent = func_entry;
                    func_entry->child->root_var_st = func_entry->child;
                    initialise_var_hash_table(func_entry->child->var_hash_table);
                    insert_func_hash(func_entry);
                    next_var_st = func_entry->child;
                    current_offset = 0;
                }
                else{
                    if(found_entry->define_lno != -1){
                        printf("Error: %s module already defined", current->data.str);
                    }
                    else if(found_entry->define_lno == -1 && found_entry->declare_lno != -1){
                        found_entry->define_lno = current->data.line_number;
                        found_entry->child = (var_st *) malloc(sizeof(var_st));
                        found_entry->child->lno = current->data.line_number;
                        found_entry->child->depth = 1;
                        found_entry->child->next = NULL;
                        found_entry->child->child = NULL;
                        found_entry->child->tag = 0;
                        found_entry->child->parent.f_parent = found_entry;
                        found_entry->child->root_var_st = found_entry->child;
                        initialise_var_hash_table(found_entry->child->var_hash_table);
                        next_var_st = found_entry->child;
                        current_offset = 0;
                    }
                }
            }

            //Module Declaration
            else if(current->parent->data.enum_value == NT_moduleDeclarations && current->parent->data.is_terminal==0){
                func_hash_entry *found_entry = find_func_hash(current->data.str);
                if(found_entry == NULL){
                    func_hash_entry *func_entry = (func_hash_entry *) malloc(sizeof(func_hash_entry));
                    strcpy(func_entry->lexeme,current->data.str);
                    func_entry->declare_lno = current->data.line_number;
                    func_entry->define_lno = -1;
                    func_entry->called = 0;
                    func_entry->inputList = NULL;
                    func_entry->outputList = NULL;
                    func_entry->next = NULL;
                    func_entry->child = NULL;
                    insert_func_hash(func_entry);
                    next_var_st = func_entry->child;
                }
                else{
                    printf("Error: %s module already declared", current->data.str);
                }
            }

            //Input Parameters
            else if(current->parent->data.enum_value == NT_input_plist && current->parent->data.is_terminal==0){
                int flag_val = check_var(current->data.str,current_var_st);
                if(flag_val == -1 || flag_val==3){
                    printf("Error in module parameters");
                }
                else if(flag_val == 2){
                    var_hash_entry *var_entry = (var_hash_entry *) malloc(sizeof(var_hash_entry));
                    strcpy(var_entry->lexeme,current->data.str);
                    var_entry->lno = current->data.line_number;
                    var_entry->flag = 0;
                    var_entry->width = getBytes(current->left_child);
                    var_entry->offset = current_offset;
                    current_offset = current_offset + var_entry->width;
                    var_entry->type = getType(current->left_child);
                    var_entry->next = NULL;
                    insert_var_hash(var_entry,current_var_st);
                    
                    // adding type to parameter list
                    parameter *p = (parameter *) malloc(sizeof(parameter));
                    p->next = NULL;
                    p->type = getType(current->left_child);
                    parameter *temp = current_var_st->parent.f_parent->inputList;
                    if(temp == NULL){
                        current_var_st->parent.f_parent->inputList = p;
                    }
                    else{
                        while(temp->next != NULL){
                            temp = temp->next;
                        }
                        temp->next = p;
                    }
                }
            }

            //Output Parameters
            else if(current->parent->data.enum_value == NT_output_plist && current->parent->data.is_terminal==0){
                int flag_val = check_var(current->data.str,current_var_st);
                if(flag_val == -1 || flag_val==3){
                    printf("Error in module parameters");
                }
                else if(flag_val == 2){
                    var_hash_entry *var_entry = (var_hash_entry *) malloc(sizeof(var_hash_entry));
                    strcpy(var_entry->lexeme,current->data.str);
                    var_entry->lno = current->data.line_number;
                    var_entry->flag = 1;
                    var_entry->width = getBytes(current->left_child);
                    var_entry->offset = current_offset;
                    current_offset = current_offset + var_entry->width;
                    var_entry->type = getType(current->left_child);
                    var_entry->next = NULL;
                    insert_var_hash(var_entry,current_var_st);

                    // adding type to parameter list
                    parameter *p = (parameter *) malloc(sizeof(parameter));
                    p->next = NULL;
                    p->type = getType(current->left_child);
                    parameter *temp = current_var_st->parent.f_parent->outputList;
                    if(temp == NULL){
                        current_var_st->parent.f_parent->outputList = p;
                    }
                    else{
                        while(temp->next != NULL){
                            temp = temp->next;
                        }
                        temp->next = p;
                    }
                }
            }

            //identifier declaration
            else if(current->parent->parent->data.enum_value == T_DECLARE && current->parent->parent->data.is_terminal==1){
				int flag = check_var(current->data.str,current_var_st);
				int for_flag = 1;
				///////////// check for identifier in for
				for(int i = 0 ; i<size_for_string ; i++){
					if(strcmp(for_string[i],current->data.str) == 0) for_flag = 0;
				}
                if(flag == -1){
                    printf("Semantic error: in Identifier declaration. (line number: %d)\n", current->data.line_number);
                }
				else if(for_flag == 0){
					printf("Semantic Error: The identifier is a for identifier\n");
				}
                else{
                    var_hash_entry *var_entry = (var_hash_entry *) malloc(sizeof(var_hash_entry));
                    strcpy(var_entry->lexeme,current->data.str);
                    var_entry->lno = current->data.line_number;
                    var_entry->flag = flag;
                    var_entry->width = getBytes(current->parent->forward);
                    var_entry->offset = current_offset;
                    current_offset = current_offset + var_entry->width;
                    var_entry->type = getType(current->parent->forward);       
                    var_entry->next = NULL;
                    insert_var_hash(var_entry,current_var_st);
                }
            }   

            fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
            fill_symbol_table(current->forward, next_var_st,for_string,size_for_string);
        }

        //Identify Blocks (for, while, switch)..create var_st..
        //Semantic Rules 7,8,9,13 checked
        //Semantic check that the for and switch variable is declared before use
        else if(current->data.is_terminal == 1 && 
                (current->data.enum_value == T_FOR 
                || current->data.enum_value == T_WHILE 
                || current->data.enum_value == T_SWITCH)){
            var_st *block_entry = (var_st *) malloc(sizeof(var_st));
            block_entry->lno = current->data.line_number;
            block_entry->depth = current_var_st->depth+1;
            block_entry->next = NULL;
            block_entry->child = NULL;
            block_entry->tag = 1;
            block_entry->parent.v_parent = current_var_st;
            block_entry->root_var_st = current_var_st->root_var_st;
            initialise_var_hash_table(block_entry->var_hash_table);
            
            if(current_var_st->child == NULL){
                current_var_st->child = block_entry;
            }
            else{
                var_st *temp = current_var_st->child;
                while(temp->next != NULL){
                    temp = temp->next;
                }
                temp->next = block_entry;
            }

			next_var_st = block_entry;
            int offset_val = current_offset;
            current_offset = 0;
            
            /////////////semantics related to switch
            if(current->data.is_terminal == 1 && current->data.enum_value == T_SWITCH){
                var_hash_entry* id = find_var(current->left_child->data.str, current->left_child->data.line_number, current_var_st);
                if(id == NULL){
                    printf("\nSemantic Error: the identifier (%s) is not declared  (line number: %d)\n",current->left_child->data.str,current->left_child->data.line_number);
                }
                else{
                    if(id->type.is_array){
                        printf("\nSemantic Error: Switch variable cannot be array type  (line number: %d)\n",current->left_child->data.line_number);
					}
                    else{
                        if(id->type.t.v.dt == BOOLEAN){
                            ASTNode* val = current->left_child->forward;
                            while(!(val->data.is_terminal == 1 && val->data.enum_value == T_DEFAULT)){
                                if(!(val->data.is_terminal == 1 && (val->data.enum_value == T_FALSE  || val->data.enum_value == T_TRUE))) printf("\nSemantic Error: case value is not BOOLEAN  (line number: %d)\n",val->data.line_number);
                                val = val->forward;
                            }
                            if(val->left_child != NULL){
                                printf("\nSemantic Error: Default should not be present\n");
                            }
                        }
                        else if(id->type.t.v.dt == INTEGER){
                            ASTNode* val = current->left_child->forward;
                            while(!(val->data.is_terminal == 1 && val->data.enum_value == T_DEFAULT)){
                                if(!(val->data.is_terminal == 1 && val->data.enum_value == T_NUM)) printf("\nSemantic Error: case value is not integer (line number: %d)\n",val->data.line_number);
                                val = val->forward;
                            }
                            if(val->left_child == NULL){
                                printf("\nSemantic Error: Default is missing \n");
                            }
                        }
                        else{
                            printf("\nSemantic Error: Switch variable cannot be real (line number: %d)",current->left_child->data.line_number);
                        }
                    }
                }
            }
            
            // for variables must be used declared and cannot be declared again in the for
            if(current->data.is_terminal == 1 && current->data.enum_value == T_FOR){
				var_hash_entry* id = find_var(current->left_child->data.str, current->left_child->data.line_number, current_var_st);
                if(id == NULL){
                    printf("Semantic Error: Variable no declared  (line number: %d)",current->left_child->data.line_number);
                }
                else{
                    strcpy(for_string[size_for_string],current->left_child->data.str);
				    size_for_string++;
				    if(size_for_string > 9){
                        printf("\nnesting limit exeeded\n");
                    }
				    fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
				    current_offset = offset_val;
				    next_var_st = current_var_st;
				    fill_symbol_table(current->forward, next_var_st,for_string,size_for_string--);
			    }
            }
			else{
				fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
				current_offset = offset_val;
				next_var_st = current_var_st;
				fill_symbol_table(current->forward, next_var_st,for_string,size_for_string);
			}
        }

        else{
            fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
            fill_symbol_table(current->forward, next_var_st,for_string,size_for_string);
        }
    }

    else{ //right child not NULL
        fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
        fill_symbol_table(current->right_child, next_var_st,for_string,size_for_string);
        fill_symbol_table(current->forward, next_var_st,for_string,size_for_string);
    }
}

/* ///FIll Symbol Table Structure
module defination //create new scope
	current(PROGRAM)
	or
	current(ID)->parent(MODULE)

declaration
	current(ID)->parent(moduleDeclarations)

input parameters
	current(ID)->parent(input_plist)
	current(ID)->left_child(datatype)

ouput parameteres
	current(ID)->parent(output_plist)
	current(ID)->left_child(datatype)

declare identifier
	current(ID)->parent(idList)->parent(DECLARE)
	current(ID)->parent->forward(datatype)

start/end block //create new scope
	- for
		current(FOR)
		current(ID)->parent(FOR)
        exit: right child = NULL
	- while
		current(WHILE)
        exit: right child = NULL
	- case
		current(NUM|TRUE|FALSE)->parent(SWITCH)
        exit: right child = NULL
	- default
		current(DEFAULT)
        exit: right child = NULL
*/


void semantic_check(ASTNode *current, var_st *current_var_st,var_st *prev_child_var_st){
    if(current == NULL){
        return;
    }
    // printf("%s , term: %d , enum: %d , lno: %d\n",current->data.str,current->data.is_terminal,current->data.enum_value,current->data.line_number);
    
    if(current->right_child == NULL){
		//Semantic Check for Rules 3,5,6
        if(current->data.is_terminal==0 && current->data.enum_value == NT_moduleReuseStmt){
            func_hash_entry *func = find_func_hash(current->left_child->right_child->data.str);
            
            //checking type and number of assigned and returned parameters
            ASTNode *assign_id_list = current->left_child->left_child;
            if(assign_id_list == NULL){
                if(func->outputList != NULL){
                    printf("Semantic Error: %s has return parameters that must be assigned\n", current->left_child->right_child->data.str);
                }
            }
            else{
                if(func->outputList == NULL){
                    printf("Semantic Error: %s does not return any parameters\n", current->left_child->right_child->data.str);
                }
                else{
                    ASTNode *assign_id = assign_id_list->left_child;
                    parameter *ret_id = func->outputList;
                    while(assign_id!=NULL && ret_id!=NULL){
                        var_hash_entry* assign_var = find_var(assign_id->data.str, assign_id->data.line_number, current_var_st);
                        if(assign_var == NULL){
                            printf("Semantic Error: %s is not defined\n", assign_id->data.str);
                        }
                        else if(assign_var->type.is_array == ret_id->type.is_array){
                            if(assign_var->type.is_array){
                                if(assign_var->type.t.a.dt != ret_id->type.t.a.dt){//datatype of array don't match
                                    printf("Semantic Error: Type mismatch in return type and assign type\n");
                                }
                            }
                            else{
                                if(assign_var->type.t.v.dt != ret_id->type.t.v.dt){//datatype of variable don't match
                                    printf("Semantic Error: Type mismatch in return type and assign type\n");
                                }
                            }
                        }
                        else{//one is array and other is variable
                            printf("Semantic Error: Type mismatch in return type and assign type\n");
                        }
                        assign_id = assign_id->forward;
                        ret_id = ret_id->next;
                    }
                    if(assign_id!=NULL || ret_id!=NULL){
                        printf("Semantic Error: Mismatch in number of return parameters and assigned variables\n");
                    }
                }
            }

            //checking type and number of passed and input parameters
            ASTNode *passed_id_list = current->left_child->right_child->forward;
            if(passed_id_list == NULL){
                if(func->inputList != NULL){
                    printf("Semantic Error: input parameters required to invoke %s\n", current->left_child->right_child->data.str);
                }
            }
            else{
                if(func->inputList == NULL){
                    printf("Semantic Error: %s does not take any input parameters\n", current->left_child->right_child->data.str);
                }
                else{
                    ASTNode *passed_id = passed_id_list->left_child;
                    parameter *input_id = func->inputList;
                    while(passed_id!=NULL && input_id!=NULL){
                        var_hash_entry* passed_var = find_var(passed_id->data.str, passed_id->data.line_number, current_var_st);
                        if(passed_var == NULL){
                            printf("Semantic Error: %s is not defined\n", passed_id->data.str);
                        }
                        else if(passed_var->type.is_array == input_id->type.is_array){
                            if(passed_var->type.is_array){
                                if(passed_var->type.t.a.dt != input_id->type.t.a.dt){//datatype of array don't match
                                    printf("Semantic Error: Type mismatch in invoking variable type and module input parameter type\n");
                                }
                            }
                            else{
                                if(passed_var->type.t.v.dt != input_id->type.t.v.dt){//datatype of variable don't match
                                    printf("Semantic Error: Type mismatch in invoking variable type and module input parameter type\n");
                                }
                            }
                        }
                        else{//one is array and other is variable
                            printf("Semantic Error: Type mismatch in invoking variable type and module input parameter type\n");
                        }
                        passed_id = passed_id->forward;
                        input_id = input_id->next;
                    }
                    if(passed_id!=NULL || input_id!=NULL){
                        printf("Semantic Error: Mismatch in number of invoking variables and module input parameters\n");
                    }
                }
            }

            semantic_check(current->left_child,current_var_st,NULL);
			semantic_check(current->forward,current_var_st,prev_child_var_st);
        }

        //WARNING: This part is for traversal of the semantic_check
        //Add your semantic Rule above this only!!
		/*****************************yahan pe else aana tha??????????????????*******************************/
		if(current->parent != NULL && current->parent->data.is_terminal == 0 && current->parent->data.enum_value == NT_statements){
			if(current->data.is_terminal == 1 && 
                (current->data.enum_value == T_FOR 
                || current->data.enum_value == T_WHILE 
                || current->data.enum_value == T_SWITCH)){
					if(prev_child_var_st!=NULL){
						semantic_check(current->left_child,prev_child_var_st->next,NULL);
						semantic_check(current->forward,current_var_st,prev_child_var_st->next);
					}
					else {
						semantic_check(current->left_child,current_var_st->child,NULL);
						semantic_check(current->forward,current_var_st,current_var_st->child);
					}
				}
			else if(current->data.is_terminal == 1 && (current->data.enum_value == T_PRINT || current->data.enum_value == T_GET_VALUE)){
					ASTNode *var = current->left_child;
					var_hash_entry* id = find_var(var->data.str, var->data.line_number, current_var_st);
					if(id == NULL){
						printf("Semantic Error: Variable no declared  (line number: %d)",var->data.line_number);
					}
					semantic_check(current->forward,current_var_st,prev_child_var_st);
					///////////////need to optimise by bypassing left child traversal
				}
			else{
				semantic_check(current->left_child,current_var_st,NULL);
				semantic_check(current->forward,current_var_st,prev_child_var_st);
			}
		}
		else if(current->data.is_terminal == 1 && current->data.enum_value == T_ID && current->parent->data.enum_value == T_MODULE){
			func_hash_entry *found_entry = find_func_hash(current->data.str);
			semantic_check(current->left_child,found_entry->child,NULL);
			semantic_check(current->forward,found_entry->child,NULL);
		}
		else if(current->data.is_terminal == 1 && current->data.enum_value == T_PROGRAM){
			func_hash_entry *found_entry = find_func_hash(current->data.str);
			semantic_check(current->left_child,found_entry->child,NULL);
			semantic_check(current->forward,NULL,NULL);
		}

		else{
			semantic_check(current->left_child,current_var_st,NULL);
			semantic_check(current->forward,current_var_st,NULL);
		}
    }

    else{
		semantic_check(current->left_child,current_var_st,NULL);
        semantic_check(current->right_child,current_var_st,NULL);
		if(current->parent->data.is_terminal == 0 && current->parent->data.enum_value == NT_statements){
			semantic_check(current->forward,current_var_st,prev_child_var_st);
		}
		else{
			semantic_check(current->forward,current_var_st,NULL);
		}
    }
}


////////printing the symbol table
void print_var_st(var_st* table){
    if(table == NULL) return;
    else{
        printf("\nVariable Symbol Table:\n");
        printf("lno: %d, depth: %d, tag= %d\n",table->lno,table->depth,table->tag);
        printf("entries\n");
        for(int i = 0 ;i<SYMBOL_TABLE_SIZE; i++){
            var_hash_entry* temp = table->var_hash_table[i].head;
            while(temp != NULL){
                if(temp->type.is_array == 0){
                    printf("\t%s %d %d %d %d %d\n",temp->lexeme, temp->lno, temp->flag, temp->width, temp->offset, (int)temp->type.t.v.dt);
                }
                else{
                    printf("\t%s %d %d %d %d %d %d %d\n",temp->lexeme, temp->lno, temp->flag, temp->width, temp->offset, (int)temp->type.t.a.dt, temp->type.t.a.s_idx, temp->type.t.a.e_idx);
                }
                temp = temp->next;
            }
        }
        printf("%s","Printing Child Table \n");
        print_var_st(table->child);
        printf("%s","Printing Sibling Table \n");
        print_var_st(table->next);
    }
}

void print_st(){
    for(int i = 0 ; i<SYMBOL_TABLE_SIZE; i++){
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            printf("\nFUNCTION\n");
            printf("%s\t%d\t%d",f_entry->lexeme, f_entry->declare_lno, f_entry->define_lno);
            print_var_st(f_entry->child);
            f_entry = f_entry->next;
        }
    }
}

void declaration_maker()
{
    FILE *f1 = fopen("bss_section.asm","w");
    fprintf(f1,"SECTION .bss \n");
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            declaration_varst(f_entry->child,f1);
            f_entry = f_entry->next;
        }
    }
    fprintf(f1,"sys_fleft : resb 4");
    fprintf(f1,"sys_fright : resb 4");
    fclose(f1);
}

void declaration_varst(var_st *table,FILE*f)
{
    if(table == NULL) return;
    else
    {
        for(int i = 0 ;i<SYMBOL_TABLE_SIZE; i++)
        {
            var_hash_entry* temp = table->var_hash_table[i].head;
            while(temp != NULL){
                //printf("\t%s %d %d %d %d %d\n",temp->lexeme, temp->lno, temp->flag, temp->width, temp->offset, (int)temp->type.t.v.dt);
                char temp_lexeme[21];
                char temp_buffer[21];
                sprintf(temp_buffer,"%d",temp->lno);
                //itoa(temp->lno,temp_buffer,10);
                strcpy(temp_lexeme,temp->lexeme);
                strcat(temp_lexeme,"_");
                strcat(temp_lexeme,temp_buffer);
                if(temp->type.is_array == 0){
                    
                    fprintf(f,"%s: resb %d \n",temp_lexeme,temp->width);
                }
                else{
                    // in all cases address is stored (so allocating 4 bytes)
                    fprintf(f,"%s: resb %d \n",temp_lexeme,4);
                }
                temp = temp->next;
            }
        }
        declaration_varst(table->child,f);
        declaration_varst(table->next,f);
    }
    
}

char *lexeme_generator(char * lexeme , int lno)
{
    char temp_buffer[20]; // i am taking this as a bound for variable name + line_number +1 .. See if it does not fit.
    sprintf(temp_buffer,"%d",lno);
    //itoa(temp->lno,temp_buffer,10);
    strcpy(temp_lexeme,lexeme);
    strcat(temp_lexeme,"_");
    strcat(temp_lexeme,temp_buffer);
    return temp_lexeme;
}

int is_left_child(ASTNode *node)
{
    if(node == node->parent ->left_child)
    return 1;
    else return 0;
}

void terminals_handler(ASTNode *ast_node , var_st *table , FILE *f)
{
    //Handles NUM,RNUM, ID , ID[NUM] , ID[ID] , T_TRUE , T_FALSE
    if(ast_node->data.enum_value == T_NUM)
    {
        type_flag = INTEGER;
        if(is_left_child(ast_node)) fprintf(f,"xor eax, eax\nmov ax, %s\n",ast_node->data.str);
        else fprintf(f,"xor ebx,ebx\nmov bx, %s\n",ast_node->data.str);
    }
    else if(ast_node->data.enum_value == T_RNUM)
    {
        type_flag = REAL;
        if(is_left_child(ast_node))
        fprintf(f,"mov [sys_fleft], __float32__(%s)\n",ast_node->data.str); // not writing DWORD as it is implicit.
        else fprintf(f,"mov [sys_fright], __float32__(%s)\n",ast_node->data.str);
    }
    else if(ast_node->data.enum_value == T_TRUE)
    {
        type_flag = BOOLEAN;
        if(is_left_child(ast_node)) fprintf(f, "xor eax,eax\nmov al, 0x1\n");
        else fprintf(f,"xor ebx,ebx\nmov bl, 0x1\n");
    }
    else if(ast_node->data.enum_value == T_FALSE)
    {
        type_flag = BOOLEAN;
        if(is_left_child(ast_node)) fprintf(f, "xor eax,eax\nmov al, 0x0\n");
        else fprintf(f,"xor ebx,ebx\nmov bl, 0x0\n");
    }
    else
    {
        var_hash_entry *temp = find_var(ast_node->data.str,ast_node->data.line_number,table);
        if(temp->type.is_array)
        {
            if(!temp->type.t.a.is_static) // if variable is not a static array
            {
                //Dynamic bound check to be done here.
                fprintf(f,"push eax \npush ebx\n");
                if(ast_node->forward->data.enum_value == T_ID) 
                {
                    //moving value of id into ax; must be integer so ax
                    fprintf(f,"xor eax , eax\nmov ax , [%s]\n",lexeme_generator(ast_node->forward->data.str,ast_node->forward->data.line_number));
                }
                else
                {
                    fprintf(f,"xor eax , eax\nmov ax , %s\n",ast_node->forward->data.str);
                }
                //first the start_index is present
                fprintf(f,"xor ebx , ebx\nmov bx, [%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number)); // moving lower_index into bx
                fprintf(f, "cmp ax ,bx\n");
                fprintf(f,"jl abort1\n");
                //second the end_index is present
                fprintf(f,"xor ebx , ebx\nmov bx, [%s + 2]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number)); // moving lower_index into bx
                fprintf(f, "cmp ax ,bx\n");
                fprintf(f,"jg abort1\n");

                fprintf(f,"pop ebx \npop eax");
            }
            if(ast_node->forward->data.enum_value == T_ID)
            {
                fprintf(f,"push eax\npush ebx\npush ecx\n");
                fprintf(f,"xor eax,eax\n mov eax %s\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));
                fprintf(f,"xor ebx,ebx\nmov bx,word [%s]\n" , lexeme_generator(ast_node->forward->data.str,ast_node->forward->data.line_number));
                fprintf(f,"xor ecx,ecx\nlea ecx, [eax + 4 + (ebx - %d)*%d ]\n",temp->type.t.a.s_idx , temp->width);
                if(temp->type.t.a.dt == INTEGER)
                {
                    //remember things happen in 16 bit regs
                    type_flag = INTEGER;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov ax, word[ecx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bx, word[ecx]\n");
                    }
                    
                }
                else if(temp->type.t.a.dt == BOOLEAN)
                {
                    type_flag = BOOLEAN;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov al, byte[ecx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bl, byte[ecx]\n");
                    }
                }
                else
                {
                    type_flag = REAL;
                    //things happen in 32 bit regs
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov [sys_fleft], dword[ecx]\n");
                    }                 
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov [sys_fright], dword[ecx]\n");
                    }
                }
                fprintf(f,"pop ecx\npop ebx\npop eax\n");
                
            }
            else
            {
                fprintf(f,"push ecx\n");
                //fprintf(f,"xor eax,eax\n mov eax %s\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));
                //fprintf("xor ebx,ebx\nmov bx,word [%s]\n" , lexeme_generator(ast_node->forward->data.str,ast_node->forward->data.line_number));
                fprintf(f,"xor ecx,ecx\nlea ecx, [eax + 4 + (%s - %d)*%d ]\n",ast_node->forward->data.str, temp->type.t.a.s_idx , temp->width);
                if(temp->type.t.a.dt == INTEGER)
                {
                    //remember things happen in 16 bit regs
                    type_flag = INTEGER;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov ax, word[ecx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bx, word[ecx]\n");
                    }     
                }
                else if(temp->type.t.a.dt == BOOLEAN)
                {
                    type_flag = BOOLEAN;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov al, byte[ecx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bl, byte[ecx]\n");
                    }
                }
                else
                {
                    type_flag = REAL;
                    //things happen in 32 bit regs
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov [sys_fleft], dword[ecx]\n");
                    }                 
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov [sys_fright], dword[ecx]\n");
                    }
                }
                fprintf(f,"pop ecx\n");
            }
            
        }
        else
        {
            if(temp->type.t.v.dt == INTEGER)
            {
                type_flag = INTEGER;
                if(is_left_child(ast_node))
                {
                    fprintf(f,"xor eax,eax\nmov ax, word[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov bx, word[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));    
                }
                
            }
            else if(temp->type.t.v.dt == BOOLEAN)
            {
                type_flag = BOOLEAN;
                if(is_left_child(ast_node))
                {
                    fprintf(f,"xor eax,eax\nmov al, byte[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov bl, byte[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));    
                }
            }
            else
            {
                type_flag = REAL;
                if(is_left_child(ast_node))
                {
                    fprintf(f,"xor eax,eax\nmov eax, dword[%s]\nmov [sys_fleft] eax\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov ebx, dword[%s]\nmov [sys_fright] ebx\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));    
                }
            }
        }
    }
}

void expr_code_writer(ASTNode *ast_node , var_st *table , FILE *f , int child_flag , int operator)
{
    //child_flag: 0:BOTH OPS , 1: LEFT_CHILD is ID, 2:RIGHT_CHILD is ID ,3 :BOTH_CHILD are ID , 4:UNARY_OP
    // if((ast_node -> data).enum_value == T_PLUS) return 1;
    // else if((ast_node -> data).enum_value == T_MINUS) return 2;
    // else if((ast_node -> data).enum_value == T_AND)return 3;
    // else if((ast_node -> data).enum_value == T_OR)return 4;
    // else if((ast_node -> data).enum_value == T_LT)return 5;
    // else if((ast_node -> data).enum_value == T_LE)return 6;
    // else if((ast_node -> data).enum_value == T_GT)return 7;
    // else if((ast_node -> data).enum_value == T_GE)return 8;
    // else if((ast_node -> data).enum_value == T_EQ)return 9;
    // else if((ast_node -> data).enum_value == T_NE)return 10;
    if(operator == 1)
    {
        if(child_flag >=0 && child_flag <=3)
        {
            if(type_flag == INTEGER)
            {
                fprintf(f,"add ax , bx\n");
                if(!is_left_child) fprintf(f,"mov bx , ax\n");
            }
            else
            {
                fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfaddp\n");
                if(is_left_child) fprintf(f,"fstp dword[sys_fleft]");
                else fprintf(f,"fstp dword[sys_fright]");
            }  
        }
        else
        {
            //unary op '+' do nothing
        }
    }
    if(operator == 2)
    {
        if(child_flag >=0 && child_flag <=3)
        {
            if(type_flag == INTEGER)
            {
                fprintf(f,"sub ax , bx\n");
                if(!is_left_child) fprintf(f,"mov bx , ax\n");
            }
            else
            {
                fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfsubp st1,st0\n");
                if(is_left_child) fprintf(f,"fstp dword[sys_fleft]");
                else fprintf(f,"fstp dword[sys_fright]");
            }  
        }
        else
        {
            //unary op -
            if(type_flag ==INTEGER)
            {
                fprintf(f,"NEG AX\n");
            }
            else
            {
                fprintf(f,"fld dword[sys_fleft]\nfchs\n");
                fprintf(f,"fstp dword[sys_fleft]");
            }
        }
    }
    if(operator == 3)
    {
        if(type_flag == BOOLEAN)
        {
            fprintf(f,"and al , bl\n");
            if(!is_left_child) fprintf(f,"mov bl , al\n");
        }
    }
    if(operator == 4)
    {
        if(type_flag == BOOLEAN)
        {
            fprintf(f,"or al , bl\n");
            if(!is_left_child) fprintf(f,"mov bl , al\n");
        }
    }
    if(operator == 5)
    {
        //LT
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(!is_left_child) fprintf(f,"jl sys_true_ax%d\njg sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jl sys_true_bx%d\njg sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            if(!is_left_child)fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_true_ax%d\nlb_greater%d: jnz sys_false_ax%d\n",cmp_count,cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_true_bx%d\nlb_greater%d: jnz sys_false_bx%d\n",cmp_count,cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
    }
    if(operator == 6)
    {
        //LE
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(!is_left_child) fprintf(f,"jle sys_true_ax%d\njg sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jle sys_true_bx%d\njg sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            if(!is_left_child)fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njnz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njnz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        
    }
    if(operator == 7)
    {
        //GT
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(!is_left_child) fprintf(f,"jg sys_true_ax%d\njg sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jg sys_true_bx%d\njg sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            if(!is_left_child)fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_false_ax%d\nlb_greater%d: jnz sys_true_ax%d\n",cmp_count,cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_false_bx%d\nlb_greater%d: jnz sys_true_bx%d\n",cmp_count,cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
    }
    if(operator == 8)
    {
        //GE
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(!is_left_child) fprintf(f,"jge sys_true_ax%d\njg sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jge sys_true_bx%d\njg sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            if(!is_left_child)fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
    }
    if(operator == 9)
    {
        //EQ
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(!is_left_child) fprintf(f,"je sys_true_ax%d\njg sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"je sys_true_bx%d\njg sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            if(!is_left_child)fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x4000\njz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }

    }
    if(operator == 10)
    {
        //NE
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(!is_left_child) fprintf(f,"jne sys_true_ax%d\njg sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jne sys_true_bx%d\njg sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            if(!is_left_child)fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x4000\njnz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\njnz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
    }
    if(operator == 11)
    {
        //MUL
        if(type_flag == INTEGER)
        {
            fprintf(f,"imul ax , bx\n");
            if(!is_left_child) fprintf(f,"mov bx , ax\n");
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfsubp st1,st0\n");
            if(is_left_child) fprintf(f,"fstp dword[sys_fleft]");
            else fprintf(f,"fstp dword[sys_fright]");
        }
    }
    if(operator == 12)
    {
        //DIV
        if(type_flag == INTEGER)
        {
            fprintf(f,"xchg ax,bx\n idiv ax , bx\n");
            if(!is_left_child) fprintf(f,"mov bx , ax\n");
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfsubp st1,st0\n");
            if(is_left_child) fprintf(f,"fstp dword[sys_fleft]");
            else fprintf(f,"fstp dword[sys_fright]");
        }
    }
    if(operator >=5 && operator<=10)
    {
        //changing type in case of relational ops.
        type_flag = BOOLEAN;
    }
}

void operator_processor(ASTNode *ast_node , var_st *table , FILE *f, int operator)
{
    //flag: 0:BOTH OPS , 1: LEFT_CHILD, 2:RIGHT_CHILD ,3 :BOTH_CHILD , 4:UNARY_OP

    int flag = 0;
    if(ast_node -> left_child-> data.enum_value == T_ID || ast_node -> left_child-> data.enum_value == T_NUM ||ast_node -> left_child-> data.enum_value == T_RNUM ||ast_node -> left_child-> data.enum_value == T_TRUE ||ast_node -> left_child-> data.enum_value == T_FALSE )
    {
        flag=1;
    }
    if(ast_node ->right_child != NULL)
    {
        if (ast_node -> right_child-> data.enum_value == T_ID || ast_node -> right_child-> data.enum_value == T_NUM ||ast_node -> right_child-> data.enum_value == T_RNUM ||ast_node -> left_child-> data.enum_value == T_TRUE ||ast_node -> left_child-> data.enum_value == T_FALSE)
        {
            if(flag == 1) flag = 3;
            else flag = 2;
        }
    }
    else
    {
        flag =4; // in case of unary_op 4 will overwrite 1 in case its a TERMINAL/
    }  
    if (flag == 0)
    {
        make_code(ast_node->right_child,table,f);
        make_code(ast_node->left_child,table,f);
        expr_code_writer(ast_node,table,f,0,operator);
    } 
    else if (flag == 1)
    {
        make_code(ast_node->right_child,table,f);
        terminals_handler(ast_node->left_child,table,f);
        expr_code_writer(ast_node,table,f,1,operator);
    }
    else if (flag == 2)
    {
        make_code(ast_node->left_child,table,f);
        terminals_handler(ast_node->right_child,table,f);
        expr_code_writer(ast_node,table,f,2,operator);
    }
    else if (flag == 3)
    {
        terminals_handler(ast_node->left_child,table,f);
        terminals_handler(ast_node->right_child,table,f);
        expr_code_writer(ast_node,table,f,3,operator);

    }
    else
    {
        if(check_operator(ast_node->left_child))
        {
            make_code(ast_node->left_child,table,f);
        }
        else
        {
            terminals_handler(ast_node->left_child,table,f);
        }
        
        expr_code_writer(ast_node,table,f,4,operator);
    }
    
}


int check_operator(ASTNode *ast_node)
{
    if((ast_node -> data).enum_value == T_PLUS) return 1;
    else if((ast_node -> data).enum_value == T_MINUS) return 2;
    else if((ast_node -> data).enum_value == T_AND)return 3;
    else if((ast_node -> data).enum_value == T_OR)return 4;
    else if((ast_node -> data).enum_value == T_LT)return 5;
    else if((ast_node -> data).enum_value == T_LE)return 6;
    else if((ast_node -> data).enum_value == T_GT)return 7;
    else if((ast_node -> data).enum_value == T_GE)return 8;
    else if((ast_node -> data).enum_value == T_EQ)return 9;
    else if((ast_node -> data).enum_value == T_NE)return 10;
    else if((ast_node -> data).enum_value == T_MUL) return 11;
    else if((ast_node -> data).enum_value == T_DIV) return 12;
    else return 0;
}

void make_code(ASTNode* ast_node , var_st* table , FILE *f)
{
    int operator = check_operator(ast_node);
    if(operator)
    {
        operator_processor(ast_node,table,f,operator);
    }
    if(ast_node -> data.enum_value == T_ID || ast_node ->data.enum_value == T_NUM ||ast_node -> data.enum_value == T_RNUM ||ast_node ->data.enum_value == T_TRUE ||ast_node -> data.enum_value == T_FALSE )
    {
        terminals_handler(ast_node,table,f);
    }
}

int main(int argc,char *argv[]){
    printf("FIRST and FOLLOW are fully automated\n");
    printf("Lexer and Parser are working\n");
    printf("All 6 testcases working\n");
    printf("Parse Tree constructed\n");
    printf("Parse Tree printed in output file\n");
    populate_table();
    populate_keyword_table();
    populate_grammar("grammar.txt");
    // disp_forward();
    // calculate_first();
    // disp_first();
    ComputeFirstAndFollowSets();
    // disp_first();
    // disp_follow();
    createParseTable();

    if(argc!=3){
        printf("\nEnter a valid number of parameters\n");
        return 0;
    }

    FILE *fp_temp;
    fp_temp = fopen(argv[1],"r");
    if(fp_temp==NULL){
        printf("\nError in opening the test file\n");
        return 0;
    }
    fclose(fp_temp);

    int ip=-1;
    while(1){
        //resetting global variables
        int k=0;
        for(k=0; k<256;k++){
            buffer1[k] = '\0';
            buffer2[k] = '\0';
        }
        for(k=0; k<20; k++){
            lexeme_buffer[k] = '\0';
        }
        eof_buffer = 0; //buffer that has EOF
        eof_marker = -1; //position of EOF in the buffer
        curr_buffer = 1; //active buffer
        buffer_itr = 0; //buffer iterator
        lineNumber = 1;
        lex_error = false;
        comment_flag = false; //true means that comment is ongoing
        comment_flag_flag = false;
        buffer1_empty = true;
        buffer2_empty = true;
        delimiter_flag = false;
        error_long_id = false;
        error_long_id_print = false;
        file_finished = false;
        eps_enum = 56;
        head=NULL;
        err_count = 0;

        printf("\n\nEnter the case number:");
        scanf("%d",&ip);
        if(ip==0){
            printf("Exiting\n");
            exit(0);
        }
        
        else if(ip==1){
            removeComments(argv[1],"clean_file.txt");
            
            char buffer_print[512];
            FILE *fp_open;
            fp_open=fopen("clean_file.txt","r");
            int nread;
            
            if(fp_open){
                while((nread = fread(buffer_print, 1, sizeof buffer_print, fp_open)) > 0){
                    fwrite(buffer_print, 1, nread, stdout);
                }
            fclose(fp_open);
            }
        }

        else if(ip==2){
            printTokens(argv[1]);
        }

        else if(ip==3){
            fp = fopen(argv[1], "r");
            if (fp == NULL)
            {
                printf("File is not available \n");
            }
            parseTree *main_tree = parseInputSourceCode();
            // if(err_count==0){
                FILE *fp2 = fopen(argv[2], "w");
                fprintf(fp2, "LEXEME        LINE NO         TOKEN NAME             VALUE OF CONST                PARENT NODE                     IS LEAF                     CURRENT NODE\n");
                printParseTree(main_tree->root, fp2);
                fclose(fp2);
            // }
            // else{
                printf("\n\n%d ERRORS encountered\n",err_count);
            // }
            fclose(fp);
            tree_node *root_of_pTree = main_tree->root;
            ASTNode *ast = postorder(root_of_pTree,  NULL);

            fill_ast_parent(ast);
            printTree(ast);
            
            initialise_func_st_root();
			char for_string[10][21];
			int size_for_string = 0;
            fill_symbol_table(ast,NULL,for_string,size_for_string);
            //print_st();
            declaration_maker();
            printf("%d",ast->forward->forward->left_child->left_child->left_child->right_child->data.enum_value);
            func_hash_entry *main_entry = find_func_hash("program");
            var_st *main_st = main_entry->child;
            FILE *f = fopen("olo.asm", "a");
            make_code(ast->forward->forward->left_child->left_child->left_child->right_child,main_st,f);
        }

        else if(ip==4){
            time_taken(argv[1]);
        }
        else{
            printf("Enter a correct case number!");
        }
    }
}