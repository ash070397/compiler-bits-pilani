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
        }
        else a.s_idx = -1;
        if(node->left_child->left_child->forward->data.enum_value == T_NUM){
            a.e_idx = atoi(node->left_child->left_child->forward->data.str);
        }
        else a.e_idx = -1;
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
    var_hash_entry* result = NULL;
    while (table->tag !=0)
    {
        var_hash_entry *head = table->var_hash_table[key].head;
        while(head!= NULL){
            if(strcmp(lex,head->lexeme) == 0){
                break;
            }
        }
    }
    
    // var_hash_entry *head = table->var_hash_table[key].head;
    // while(head!= NULL){
    //     if(strcmp(lex,head->lexeme) == 0){

    //     }
    // }
    return result;
    // while(table->tag != 0){
    //     result = find_var_hash(lex, table->parent.v_parent);
    //     if(result != NULL && lno>result->lno){
    //         return result;
    //     }
    // }
}


//////////////////////////////////////////////////////////////////////////////////////////////
void fill_symbol_table(ASTNode *current, var_st *current_var_st,char for_string[10][21],int size_for_string){
	var_st *next_var_st = current_var_st;
	if(current == NULL){
		return;
    }
    if(current->right_child == NULL){
        if(current->data.is_terminal == 1 && (current->data.enum_value == T_ID || current->data.enum_value == T_PROGRAM)){
            //Module Defination
            if(current->data.enum_value == T_PROGRAM || current->parent->data.enum_value == T_MODULE){
                func_hash_entry *found_entry = find_func_hash(current->data.str);
                if(found_entry == NULL){
                    func_hash_entry *func_entry = (func_hash_entry *) malloc(sizeof(func_hash_entry));
                    strcpy(func_entry->lexeme,current->data.str);
                    func_entry->lno = current->data.line_number;
                    func_entry->status = defined;
                    func_entry->inputList = NULL;
                    func_entry->outputList = NULL;
                    func_entry->next = NULL;
                    func_entry->child = (var_st *) malloc(sizeof(var_st));
                    func_entry->child->lno = func_entry->lno;
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
                    if(found_entry->status == defined){
                        printf("Error: %s module already defined", current->data.str);
                    }
                    else if(found_entry->status == declared){
                        found_entry->status = defined;
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
                    func_entry->lno = current->data.line_number;
                    func_entry->status = declared;
                    func_entry->inputList = NULL;
                    func_entry->outputList = NULL;
                    func_entry->next = NULL;
                    func_entry->child = NULL;
                    insert_func_hash(func_entry);
                    next_var_st = func_entry->child;
                }
                else{
                    printf("Error: %s module already declared/defined", current->data.str);
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
				//printf("size: %d\t string: %s\n",size_for_string,for_string[0]);
				for(int i = 0 ; i<size_for_string ; i++){
				
					//printf("comparing: %s\t%s\n",for_string[i],current->data.str);
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

        //Identify Blocks (for, while, case, default)
        else if(current->data.is_terminal == 1 && 
                (current->data.enum_value == T_FOR 
                || current->data.enum_value == T_WHILE 
                || current->data.enum_value == T_DEFAULT
                || ((current->data.enum_value == T_NUM || current->data.enum_value == T_TRUE || current->data.enum_value == T_FALSE) 
                    && current->parent->data.is_terminal == 1 && current->parent->data.enum_value == T_SWITCH))){
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
            if(current->data.is_terminal == 1 &&current->data.enum_value == T_FOR){
				strcpy(for_string[size_for_string],current->left_child->data.str);
				size_for_string++;
				if(size_for_string > 9) printf("\nnesting limit exeeded\n");
				fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
				current_offset = offset_val;
				next_var_st = current_var_st;
				fill_symbol_table(current->forward, next_var_st,for_string,size_for_string--);
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

/////printing the symbol table
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
        print_var_st(table->child);
        print_var_st(table->next);
    }
}

void print_st(){
    for(int i = 0 ; i<SYMBOL_TABLE_SIZE; i++){
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            printf("\nFUNCTION\n");
            printf("%s\t%d\t%d",f_entry->lexeme, f_entry->lno, (int)f_entry->status);
            print_var_st(f_entry->child);
            f_entry = f_entry->next;
        }
    }
}

/*
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