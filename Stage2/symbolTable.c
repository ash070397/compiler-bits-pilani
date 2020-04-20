/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
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
            a.s_str[0] = '\0';
        }
        else{
            a.s_idx = -1;
            a.is_static = false;
            strcpy(a.s_str,node->left_child->left_child->data.str);
        }
        
        if(node->left_child->left_child->forward->data.enum_value == T_NUM){
            a.e_idx = atoi(node->left_child->left_child->forward->data.str);
            a.e_str[0] = '\0';
        }
        else{
            a.e_idx = -1;
            a.is_static = false;
            strcpy(a.e_str,node->left_child->left_child->forward->data.str);
        }
        node_type.t.a = a;
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
    return node_type;

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
        head = head->next;
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
                //printf("Semantic error");
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
                    //printf("semantic error");
                    return -1;
                }
                else if(head->flag == 0){
                    ////////////////////////add somthing to tell that you are allowed to declare using flag = 3
                    return 3;
                }
                else{
                    return -1;
                    //printf("error");
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
                return -1;
                //printf("error");
            }
			head = head->next;
        }
    }
    return 2;
    //////////////add a return statement here for the condition when nothing is found and you can using flag = 2
}


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

int extract_while_exp_var(ASTNode *current,var_st *current_var_st,char while_vars[10][21],int index){
    if(current == NULL){
        return index;
    }
    if(current->data.enum_value  == T_ID){
        var_hash_entry* var = find_var(current->data.str,current->data.line_number,current_var_st);
        if(var!= NULL){
            if(var->while_level != current_var_st->depth){
                var->while_level = current_var_st->depth;
                if(var->while_var_assign_level >= current_var_st->depth){
                    var->while_var_assign_level = current_var_st->depth-1;
                }
                strcpy(while_vars[index],current->data.str);
                index++;
                return index;
            }
        }
        return index;
    }
    index = extract_while_exp_var(current->left_child,current_var_st,while_vars,index);
    index = extract_while_exp_var(current->right_child,current_var_st,while_vars,index);
    return index;
}


void fill_symbol_table(ASTNode *current, var_st *current_var_st,char for_string[10][21],int size_for_string){
	var_st *next_var_st = current_var_st;
	if(current == NULL){
		return;
    }
    // printf("lno: %d, str: %s , isterm: %d , enum: %d\n",current->data.line_number,current->data.str,current->data.is_terminal,current->data.enum_value);
    if(current->right_child == NULL){
        //module reuse statement
        //Semantic Rules 11,12,14 checked
        if((current->data.is_terminal == 1 && current->data.enum_value == T_ID)
                && (current->parent->data.is_terminal == 1 && current->parent->data.enum_value == T_ASSIGNOP)
                && (current->parent->parent->data.is_terminal != 1 && current->parent->parent->data.enum_value == NT_moduleReuseStmt)){
            //Function cannot be invoked recursively (14)
            func_hash_entry *func_entry = current_var_st->root_var_st->parent.f_parent;
            if(strcmp(func_entry->lexeme,current->data.str)==0){
                printf("Line %d: (Semantic Error) %s module cannot be invoked recursively\n", current->data.line_number, current->data.str);
                semantic_error += 1;
            }

            //Semantic rule 11 & 12 i.e. status of function declaration and defination before invoking it
            else{
                func_hash_entry *found_entry = find_func_hash(current->data.str);
                if(found_entry == NULL){
                    printf("Line %d: (Semantic Error) %s module neither declared nor defined\n", current->data.line_number, current->data.str);
                    semantic_error += 1;
                }
                else if(found_entry->called == 0){
                    if(found_entry->declare_lno!=-1 && found_entry->declare_lno<current->data.line_number){
                        if(found_entry->define_lno!=-1 && found_entry->define_lno<current->data.line_number){
                            printf("Line %d: (Semantic Error) %s module both declared and defined before invoking\n", found_entry->define_lno, current->data.str);
                            semantic_error += 1;
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
                            printf("Line %d: (Semantic Error) %s module not defined\n", current->data.line_number, current->data.str);
                            semantic_error += 1;
                        }
                    }
                }
            }
        }

        //Filling var_st and func_st
        else if(current->data.is_terminal == 1 && (current->data.enum_value == T_ID || current->data.enum_value == T_PROGRAM)){
            bool is_func_invalid = 0; //when a function is overload we want to bypass it

            //Module Defination
            if(current->data.enum_value == T_PROGRAM || (current->parent != NULL && current->parent->data.enum_value == T_MODULE)){
                func_hash_entry *found_entry = find_func_hash(current->data.str);
                if(found_entry == NULL){
					func_hash_entry *func_entry = (func_hash_entry *) malloc(sizeof(func_hash_entry));
                    strcpy(func_entry->lexeme,current->data.str);
                    func_entry->define_lno = current->data.line_number;
                    func_entry->declare_lno = -1;
                    func_entry->called = 0;
                    func_entry->total_func_memory = 0;
                    func_entry->inputList = NULL;
                    func_entry->outputList = NULL;
                    func_entry->next = NULL;
                    func_entry->child = (var_st *) malloc(sizeof(var_st));
                    func_entry->child->lno = func_entry->define_lno;
                    if(current->data.enum_value == T_PROGRAM) {
                        func_entry->child->end_lno = current->data.end_line;
                        func_entry->child->start_lno = current->data.start_line;
                    }
                    else {
                        func_entry->child->end_lno = current->parent->data.end_line;
                        func_entry->child->start_lno = current->parent->data.start_line;
                    }
                    func_entry->child->depth = 1;
                    func_entry->child->next = NULL;
                    func_entry->child->child = NULL;
                    func_entry->child->tag = 0;
                    func_entry->child->parent.f_parent = func_entry;
                    func_entry->child->root_var_st = func_entry->child;
                    initialise_var_hash_table(func_entry->child->var_hash_table);
                    insert_func_hash(func_entry);
                    next_var_st = func_entry->child;
                    
                    //if(current->data.enum_value == T_PROGRAM) func_entry->child->end_lno = current->data.end_line;
                    //else func_entry->child->end_lno = current->parent->data.end_line;
                    current_offset = 0;
                    parameter_offset = 0;
                }
                else{
                    if(found_entry->define_lno != -1){
                        printf("Line %d: (Semantic Error) %s module already defined (cannot be overloaded)\n", current->data.line_number, current->data.str);
                        semantic_error += 1;
                        is_func_invalid = 1;
                    }
                    else if(found_entry->define_lno == -1 && found_entry->declare_lno != -1){
                        found_entry->define_lno = current->data.line_number;
                        found_entry->child = (var_st *) malloc(sizeof(var_st));
                        found_entry->child->lno = current->data.line_number;
                        if(current->data.enum_value == T_PROGRAM) {
                            found_entry->child->end_lno = current->data.end_line;
                            found_entry->child->start_lno = current->data.start_line;
                        }
                        else {
                            found_entry->child->end_lno = current->parent->data.end_line;
                            found_entry->child->start_lno = current->parent->data.start_line;
                        }
                        found_entry->child->depth = 1;
                        found_entry->child->next = NULL;
                        found_entry->child->child = NULL;
                        found_entry->child->tag = 0;
                        found_entry->child->parent.f_parent = found_entry;
                        found_entry->child->root_var_st = found_entry->child;
                        //if(current->data.enum_value == T_PROGRAM) found_entry->child->end_lno = current->data.end_line;
                        //else found_entry->child->end_lno = current->parent->data.end_line;
                        initialise_var_hash_table(found_entry->child->var_hash_table);
                        next_var_st = found_entry->child;
                        current_offset = 0;
                        parameter_offset = 0;
                    }
                }
                output_counter = 0;
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
                    func_entry->total_func_memory = 0;
                    func_entry->inputList = NULL;
                    func_entry->outputList = NULL;
                    func_entry->next = NULL;
                    func_entry->child = NULL;
                    insert_func_hash(func_entry);
                    next_var_st = func_entry->child;
                }
                else{
                    printf("Line %d: (Semantic Error) %s module already declared\n",current->data.line_number, current->data.str);
                    semantic_error += 1;
                }
            }

            //Input Parameters
            else if(current->parent->data.enum_value == NT_input_plist && current->parent->data.is_terminal==0){
                int flag_val = check_var(current->data.str,current_var_st);
                if(flag_val == -1 || flag_val==3){
                    printf("Line %d: Semantic Error in module input parameter %s\n",current->data.line_number, current->data.str);
                    semantic_error += 1;
                }
                else if(flag_val == 2){
                    var_hash_entry *var_entry = (var_hash_entry *) malloc(sizeof(var_hash_entry));
                    strcpy(var_entry->lexeme,current->data.str);
                    var_entry->lno = current->data.line_number;
                    var_entry->flag = 0;
					var_entry->assigned = 0;
                    var_entry->while_level = -1;
                    var_entry->while_var_assign_level = -1;
                    var_entry->offset = current_offset;
                    var_entry->type = getType(current->left_child);
                    var_entry->next = NULL;
                    var_entry->width = getBytes(current->left_child);
                    if(var_entry->type.is_array == 1){
                        var_entry->width = 5;
                    }
                    current_offset = current_offset + var_entry->width;
                    parameter_offset = current_offset;
                    insert_var_hash(var_entry,current_var_st);
                    
                    // adding parameter info to parameter list
                    parameter *p = (parameter *) malloc(sizeof(parameter));
                    p->next = NULL;
                    strcpy(p->lexeme,current->data.str);
                    p->lno = current->data.line_number;
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
                    printf("Line %d: Semantic Error in module output parameter %s\n",current->data.line_number, current->data.str);
                    semantic_error += 1;
                }
                else if(flag_val == 2){
                    var_hash_entry *var_entry = (var_hash_entry *) malloc(sizeof(var_hash_entry));
                    strcpy(var_entry->lexeme,current->data.str);
                    var_entry->lno = current->data.line_number;
                    var_entry->flag = 1;
					var_entry->assigned = 0;
                    var_entry->while_level = -1;
                    var_entry->while_var_assign_level = -1;
                    var_entry->offset = current_offset;
                    var_entry->type = getType(current->left_child);
                    var_entry->next = NULL;
                    var_entry->width = getBytes(current->left_child);
                    if(var_entry->type.is_array == 1){
                        printf("Line %d: Semantic Error as output parameter cannot be an array", var_entry->lno);
                        semantic_error += 1;
                        var_entry->width = 5;
                    }
                    current_offset = current_offset + var_entry->width;
                    parameter_offset = current_offset;
                    insert_var_hash(var_entry,current_var_st);

                    // adding parameter info to parameter list
                    parameter *p = (parameter *) malloc(sizeof(parameter));
                    p->next = NULL;
                    strcpy(p->lexeme,current->data.str);
                    p->lno = current->data.line_number;
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
				///////////// check for identifier is not declared
				for(int i = 0 ; i<size_for_string ; i++){
					if(strcmp(for_string[i],current->data.str) == 0){
                        for_flag = 0;
                    }
				}
                if(flag == -1){
                    printf("Line %d: (Semantic Error) variable %s can not be declared\n", current->data.line_number, current->data.str);
                    semantic_error += 1;
                }
				else if(for_flag == 0){
					printf("Line %d: (Semantic Error) %s is a for identifier, which cannot be declared\n",current->data.line_number,current->data.str);
                    semantic_error += 1;
				}
                else{
                    var_hash_entry *var_entry = (var_hash_entry *) malloc(sizeof(var_hash_entry));
                    strcpy(var_entry->lexeme,current->data.str);
                    var_entry->lno = current->data.line_number;
                    var_entry->flag = flag;
					var_entry->assigned = 0;
                    var_entry->while_level = -1;
                    var_entry->while_var_assign_level = -1;
                    // var_entry->offset = current_offset;
                    var_entry->offset = current_offset - parameter_offset;
                    var_entry->type = getType(current->parent->forward);       
                    var_entry->next = NULL;
                    var_entry->width = getBytes(current->parent->forward);
					int dynamic_error = 0;
                    if(var_entry->type.is_array == 1){
                        if(var_entry->type.t.a.is_static == 0){
							
                            var_entry->width = 1;
							if(var_entry->type.t.a.s_idx == -1){
								var_hash_entry* range_var = find_var(var_entry->type.t.a.s_str, current->data.line_number, current_var_st);
								if(range_var == NULL){
									printf("Line %d: (Semantic Error) variable %s not declared\n",current->data.line_number,var_entry->type.t.a.s_str);
									dynamic_error = 1;
								}
								else if(!(range_var->type.is_array==0 && range_var->type.t.a.dt == INTEGER)){
									dynamic_error = 1;
									printf("Line %d: (Semantic Error) Array range variable %s is not an integer\n",current->data.line_number,var_entry->type.t.a.s_str);
								}
							}
							if(var_entry->type.t.a.e_idx == -1){
								var_hash_entry* range_var = find_var(var_entry->type.t.a.e_str, current->data.line_number, current_var_st);
								if(range_var == NULL){
									printf("Line %d: (Semantic Error) variable %s not declared\n",current->data.line_number,var_entry->type.t.a.e_str);
									dynamic_error = 1;
								}
								else if(!(range_var->type.is_array==0 && range_var->type.t.a.dt == INTEGER)){
									dynamic_error = 1;
									printf("Line %d: (Semantic Error) Array range variable %s is not an integer\n",current->data.line_number,var_entry->type.t.a.e_str);
								}
							}
							
                        }
                        else{
                            var_entry->width = var_entry->width * (var_entry->type.t.a.e_idx - var_entry->type.t.a.s_idx + 1);
                            var_entry->width = var_entry->width + 1; // added 1 to total requirement for all elements of an array for holding address of the first element
                        }
                    }
                    if(dynamic_error == 0){
						current_offset = current_offset + var_entry->width;
						insert_var_hash(var_entry,current_var_st);
					}
					//current_offset = current_offset + var_entry->width;
                    //insert_var_hash(var_entry,current_var_st);
                }
            }
            
            if(is_func_invalid == 0){
                fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
			    fill_symbol_table(current->forward, next_var_st,for_string,size_for_string);

                //adding the total function memory requirement
                if((current->data.enum_value == T_PROGRAM) || (current->data.enum_value == T_ID && current->parent->data.enum_value == T_MODULE)){
                    func_hash_entry *found_entry = find_func_hash(current->data.str);
                    if(found_entry != NULL){
                        found_entry->total_func_memory = current_offset - parameter_offset;
                        // printf("\n---%s Function memory reg: %d \n", current->data.str, found_entry->total_func_memory);
                    }
                }
            }
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
            block_entry->end_lno = current->data.end_line;
            block_entry->start_lno = current->data.start_line;
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
            // int offset_val = current_offset;
            // current_offset = 0;
            
            /////////////semantics related to switch
            if(current->data.is_terminal == 1 && current->data.enum_value == T_SWITCH){
                var_hash_entry* id = find_var(current->left_child->data.str, current->left_child->data.line_number, current_var_st);
                int switch_error = 0;
                if(id == NULL){
                    printf("Line %d: (Semantic Error) variable %s is not declared\n",current->left_child->data.line_number,current->left_child->data.str);
                    semantic_error += 1;
                }
                else{
                    if(id->type.is_array){
                        switch_error = 1;
                        printf("Line %d: (Semantic Error) Switch variable cannot be array type\n",current->left_child->data.line_number);
                        semantic_error += 1;
					}
                    else{
                        if(id->type.t.v.dt == BOOLEAN){
                            ASTNode* val = current->left_child->forward;
                            while(!(val->data.is_terminal == 1 && val->data.enum_value == T_DEFAULT)){
                                if(!(val->data.is_terminal == 1 && (val->data.enum_value == T_FALSE  || val->data.enum_value == T_TRUE))){
                                    printf("Line %d: (Semantic Error) case value is not BOOLEAN\n",val->data.line_number);
                                    semantic_error += 1;
                                }
                                val = val->forward;
                            }
                            if(val->left_child != NULL){
                                printf("Line %d: (Semantic Error) Default should not be present\n",val->data.line_number);
                                semantic_error += 1;
                            }
                        }
                        else if(id->type.t.v.dt == INTEGER){
                            ASTNode* val = current->left_child->forward;
                            while(!(val->data.is_terminal == 1 && val->data.enum_value == T_DEFAULT)){
                                if(!(val->data.is_terminal == 1 && val->data.enum_value == T_NUM)){
                                    printf("Line %d: (Semantic Error) case value is not integer\n",val->data.line_number);
                                    semantic_error += 1;
                                }
                                val = val->forward;
                            }
                            if(val->left_child == NULL){
                                printf("Line %d: (Semantic Error) Default is missing \n",current->data.end_line);
                                semantic_error += 1;
                            }
                        }
                        else{
                            switch_error = 1;
                            printf("Line %d: (Semantic Error) Switch variable cannot be real\n",current->left_child->data.line_number);
                            semantic_error += 1;
                        }
                    }
                }

                if(switch_error == 0){
                    fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
                }
				else{ //error in switch statement therefore, we want to bypass the switch block
                    if(current_var_st->child->next == NULL && current_var_st->child->child == NULL){
                        current_var_st->child = NULL;
                    }
                    else{
                        var_st *temp = current_var_st->child;
                        while(temp->next->next != NULL){
                            temp = temp->next;
                        }
                        temp->next = NULL;   
                    }
                }
                
                // current_offset = offset_val;
				next_var_st = current_var_st;
				fill_symbol_table(current->forward, next_var_st, for_string, size_for_string);
            }
            
            // for variables cannot be declared again in the for
            else if(current->data.is_terminal == 1 && current->data.enum_value == T_FOR){
				var_hash_entry* id = find_var(current->left_child->data.str, current->left_child->data.line_number, current_var_st);
                if(id == NULL){
                    printf("Line %d: (Semantic Error) variable %s is not declared \n",current->left_child->data.line_number,current->left_child->data.str);
                    semantic_error += 1;
                }
                else{
                    strcpy(for_string[size_for_string],current->left_child->data.str);
				    size_for_string++;
				    if(size_for_string > 9){
                        printf("\nnesting limit exeeded\n");
                        semantic_error += 1;
                    }
				    fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
				    // current_offset = offset_val;
				    next_var_st = current_var_st;
				    fill_symbol_table(current->forward, next_var_st,for_string,size_for_string-1);
			    }
            }

			else{ //WHILE loop
                //Semantic check for WHILE (al least one variable must be assigned)
                char while_vars[10][21];
                int size_while_vars;
                //populare while_vars and set while_level
                size_while_vars = extract_while_exp_var(current, next_var_st, while_vars,0);
				
                fill_symbol_table(current->left_child, next_var_st, for_string, size_for_string);
                //check the WHILE semantics satisfy
                bool while_var_assigned = false;
                for(int i=0; i<=size_while_vars; i++){
                    var_hash_entry* var_entry = find_var(while_vars[i],current->data.line_number,current_var_st);
                    if(var_entry!=NULL){
                        //printf("%s %d %d",var_entry->lexeme,var_entry->while_level, var_entry->while_var_assign_level);
                        if(var_entry->while_level <= var_entry->while_var_assign_level){
                            while_var_assigned = true;
                            var_entry->while_var_assign_level =-1;
                        }
                        var_entry->while_level =-1;
                    }
                }
                if(while_var_assigned == false){
                    printf("Line %d: (Semantic Error) At least one while variable must be assigned\n",current->data.line_number);
                    semantic_error += 1;
                }

				// current_offset = offset_val;
				next_var_st = current_var_st;
				fill_symbol_table(current->forward, next_var_st, for_string, size_for_string);
			}
        }

        else{
            fill_symbol_table(current->left_child, next_var_st,for_string,size_for_string);
			if(current->data.is_terminal == 1 && current->data.enum_value == T_MODULE){
				func_hash_entry *found_entry = find_func_hash(current->left_child->data.str);
				if(found_entry!=NULL && found_entry->define_lno == current->left_child->data.line_number){
					int count = 0;
					struct parameter *list = found_entry->outputList;
					while(list!= NULL){
						count++;
						list = list->next;
					}
					//printf("actual: %d , assigned: %d\n",count,output_counter);
					if(count > output_counter){
                        printf("Line %d: (Semantic Error) All output parameters of function %s were not assigned\n",current->data.end_line, current->left_child->data.str);
                        semantic_error += 1;
                    }
					output_counter = 0;
				}
			}
            fill_symbol_table(current->forward, next_var_st,for_string,size_for_string);
        }
    }

    else{ //right child not NULL
		///////////////////////// Check assignment rules of while, output parameters and for
        if(current->data.is_terminal == 1 && (current->data.enum_value == T_ASSIGNOP || current->data.enum_value == T_GET_VALUE)){
            
			if(current->left_child!=NULL){
				if(current->left_child->data.is_terminal == 0 && current->left_child->data.enum_value == NT_idList){
					ASTNode* var = current->left_child->left_child;
					while(var!=NULL){
                        // printf("module resuse variable: %s\n",var->data.str);
						for(int i = 0 ; i<size_for_string ; i++){
							if(strcmp(for_string[i],var->data.str)==0){
								printf("Line %d: (Semantic Error) %s is a for variable which cannot be assigned\n",var->data.line_number,var->data.str);
                                semantic_error += 1;
							}
						}
						var_hash_entry* id = find_var(var->data.str, var->data.line_number, current_var_st);
						if(id!=NULL && id->flag == 1 && id->assigned == 0){ //used to count that number of output parameters assigned
							output_counter++;
                            id->assigned = 1;
						}
                        if(id!=NULL && id->while_level!=-1){ //used to check that at least one while var is assigned
                            id->while_var_assign_level = current_var_st->depth;
                            // printf("%s while_l: %d  while_l: %d\n",id->lexeme,id->while_level,id->while_var_assign_level);
                        }
						var = var->forward;
					}
				}
				else{
					ASTNode* var = current->left_child;
                    for(int i = 0 ; i<size_for_string ; i++){
						if(strcmp(for_string[i],var->data.str)==0){
							printf("Line %d: (Semantic Error) %s is a for variable which cannot be assigned \n",var->data.line_number,var->data.str);
                            semantic_error += 1;
						}
					}
                    var_hash_entry* id = find_var(var->data.str, var->data.line_number, current_var_st);
                    if(id!=NULL && id->flag == 1 &&id->assigned == 0){ //used to count that number of output parameters assigned
						output_counter++;
                        id->assigned = 1;
					}
                    if(id!=NULL && id->while_level!=-1){ //used to check that at least one while var is assigned
                        id->while_var_assign_level = current_var_st->depth;
                    }
				}
			}
		}

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

///// NOTE: is_array = -1 for error
struct struct_type expr_typechecker(ASTNode *current,var_st *current_var_st){
	struct struct_type error;
	error.is_array = -1;
	//printf("%s\n",current->data.str);
	if(current->data.enum_value == T_ID){
		var_hash_entry* id = find_var(current->data.str, current->data.line_number, current_var_st);
		/*if(id->lexeme!=NULL && strcmp(id->lexeme,"res_1") == 0){
			//printf("lno: %d , ")
			printf("\t%s %d %d %d\n",id->lexeme, id->lno, id->flag, (int)id->type.t.v.dt);
		}*/
		if(id == NULL){
            printf("Line %d: (Semantic Error) variable %s is not declared\n",current->data.line_number,current->data.str);
            semantic_error += 1;
			return error;
		}
		else{
			if(id->type.is_array == 1 && current->forward != NULL){
				ASTNode *var = current->forward;
				if(var->data.enum_value == T_RNUM){
					printf("Line %d: (Semantic Error) Array index not integer\n",var->data.line_number);
                    semantic_error += 1;
					return error;
				}
				else if(var->data.enum_value == T_ID){
					var_hash_entry* id2 = find_var(var->data.str, var->data.line_number, current_var_st);
					if(id2 == NULL){
						printf("Line %d: (Semantic Error) variable %s is not declared \n",var->data.line_number,var->data.str);
                        semantic_error += 1;
						return error;
					}
					else if(!(id2->type.is_array == 0 && id2->type.t.v.dt == INTEGER)){
						printf("Line %d: (Semantic Error) Array index not integer \n",var->data.line_number);
                        semantic_error += 1;
						return error;
					}
					else{
						//////bound check not implimented yet (dynamic for sure)
						struct struct_type t;
						t.is_array = 0;
						t.t.v.dt = id->type.t.a.dt;
						return t;
					}
				}
				else{
					////////////bound check not implimented yet    (dynamic if array is dynamic)
					if(id->type.t.a.is_static){
						int integer = atoi(var->data.str);
						if(!(integer<=id->type.t.a.e_idx && integer>=id->type.t.a.s_idx)) {
							printf("Line %d: (Semantic Error) Index out of bounds \n",var->data.line_number);
                            semantic_error += 1;
							return error;
						}
					}
					struct struct_type t;
					t.is_array = 0;
					t.t.v.dt = id->type.t.a.dt;
					return t;
				}
			}
			else if(id->type.is_array == 0 && current->forward != NULL){
				printf("Line %d: (Semantic Error) %s is not an array type \n",current->data.line_number,current->data.str);
                semantic_error += 1;
				return error;
			}
			else{
				return id->type;
			}
		}
	}
	else if(current->data.enum_value == T_NUM){
		struct struct_type t;
		t.is_array = 0;
		t.t.v.dt = INTEGER;
		return t;
	}
	else if(current->data.enum_value == T_RNUM){
		struct struct_type t;
		t.is_array = 0;
		t.t.v.dt = REAL;
		return t;
	}
	else if(current->data.enum_value == T_TRUE || current->data.enum_value == T_FALSE){
		struct struct_type t;
		t.is_array = 0;
		t.t.v.dt = BOOLEAN;
		return t;
	}
	else if(current->right_child!= NULL && current->left_child!= NULL && (current->data.enum_value == T_PLUS || current->data.enum_value == T_MINUS
			|| current->data.enum_value == T_DIV || current->data.enum_value == T_MUL)){
		struct struct_type type_l = expr_typechecker(current->left_child,current_var_st);
		struct struct_type type_r = expr_typechecker(current->right_child,current_var_st);
		if(type_l.is_array == -1 || type_r.is_array == -1){
			return error;
		}
		else if(type_l.is_array == 0 && type_l.t.v.dt == INTEGER
		        && type_r.is_array == 0 && type_r.t.v.dt == INTEGER){
			return type_l;	
		}
		else if(type_l.is_array == 0 && type_l.t.v.dt == REAL
		        && type_r.is_array == 0 && type_r.t.v.dt == REAL){
			return type_l;	
		}
		else{
			printf("Line %d: (Semantic Error) Type Error\n",current->data.line_number);
            semantic_error += 1;
			return error;
		}
	}
	else if(current->data.enum_value == T_LT || current->data.enum_value == T_LE
			|| current->data.enum_value == T_GE || current->data.enum_value == T_GT
			 || current->data.enum_value == T_EQ || current->data.enum_value == T_NE){
		struct struct_type type_l = expr_typechecker(current->left_child,current_var_st);
		struct struct_type type_r = expr_typechecker(current->right_child,current_var_st);
		if(type_l.is_array == -1 || type_r.is_array == -1){
			return error;
		}
		else if(type_l.is_array == 0 && type_l.t.v.dt == INTEGER
		        && type_r.is_array == 0 && type_r.t.v.dt == INTEGER){
			type_l.t.v.dt = BOOLEAN;
			return type_l;	
		}
		else if(type_l.is_array == 0 && type_l.t.v.dt == REAL
		        && type_r.is_array == 0 && type_r.t.v.dt == REAL){
			type_l.t.v.dt = BOOLEAN;
			return type_l;	
		}
		else{
			printf("Line %d: (Semantic Error) Type Error\n",current->data.line_number);
            semantic_error += 1;
			return error;
		}
	}
	else if(current->data.enum_value == T_AND || current->data.enum_value == T_OR){
		struct struct_type type_l = expr_typechecker(current->left_child,current_var_st);
		struct struct_type type_r = expr_typechecker(current->right_child,current_var_st);
		if(type_l.is_array == -1 || type_r.is_array == -1){
			return error;
		}
		else if(type_l.is_array == 0 && type_l.t.v.dt == BOOLEAN
		        && type_r.is_array == 0 && type_r.t.v.dt == BOOLEAN){
			return type_l;	
		}
		else{
			printf("Line %d: (Semantic Error) Type Error\n",current->data.line_number);
            semantic_error += 1;
			return error;
		}
	}
	else{
		struct struct_type type_l = expr_typechecker(current->left_child,current_var_st);
		return type_l;
	}
}


var_st * semantic_check(ASTNode *current, var_st *current_var_st,var_st *prev_child_var_st){
    if(current == NULL){
        return prev_child_var_st;
    }                                                                           
     //printf("%s , term: %d , enum: %d , lno: %d\n",current->data.str,current->data.is_terminal,current->data.enum_value,current->data.line_number);
    
    if(current->right_child == NULL){
		//printf("right null\n");
		//Semantic Check for Rules 3,5,6
        if(current->data.is_terminal==0 && current->data.enum_value == NT_moduleReuseStmt){
            func_hash_entry *func = find_func_hash(current->left_child->right_child->data.str);
            if(func != NULL && func->called == 1
                && (func->define_lno < current->left_child->right_child->data.line_number || func->declare_lno!=-1) //invoking in proper order
                && (strcmp(current_var_st->root_var_st->parent.f_parent->lexeme,current->left_child->right_child->data.str) != 0)) //avoid recursive invoking
            {
                //checking type and number of assigned and returned parameters
                ASTNode *assign_id_list = current->left_child->left_child;
                if(assign_id_list == NULL){
                    if(func->outputList != NULL){
                        // printf("Line %d: (Semantic Error) %s has return parameters that must be assigned (line number: %d)\n", current->left_child->right_child->data.str,  current->left_child->right_child->data.line_number);
                    }
                }
                else{
                    if(func->outputList == NULL){
                        printf("Line %d: (Semantic Error) %s does not return any parameters\n", current->left_child->right_child->data.line_number, current->left_child->right_child->data.str);
                        semantic_error += 1;
                    }
                    else{
                        ASTNode *assign_id = assign_id_list->left_child;
                        parameter *ret_id = func->outputList;
                        while(assign_id!=NULL && ret_id!=NULL){
                            var_hash_entry* assign_var = find_var(assign_id->data.str, assign_id->data.line_number, current_var_st);
                            if(assign_var == NULL){
                                printf("Line %d: (Semantic Error) variable %s is not declared\n", assign_id->data.line_number, assign_id->data.str);
                                semantic_error += 1;
                            }
                            else if(assign_var->type.is_array == ret_id->type.is_array){
                                if(assign_var->type.is_array){
                                    if(assign_var->type.t.a.dt != ret_id->type.t.a.dt){//datatype of array don't match
                                        printf("Line %d: (Semantic Error) Type mismatch in return type and assign type (%s)\n", assign_id->data.line_number, assign_id->data.str);
                                        semantic_error += 1;
                                    }
                                    else if(assign_var->type.t.a.s_idx != ret_id->type.t.a.s_idx || assign_var->type.t.a.e_idx != ret_id->type.t.a.e_idx){
                                        printf("Line %d: (Semantic Error) Bound mismatch in return type and assign type (%s)\n", assign_id->data.line_number, assign_id->data.str);
                                        semantic_error += 1;
                                    }
                                }
                                else{
                                    if(assign_var->type.t.v.dt != ret_id->type.t.v.dt){//datatype of variable don't match
                                        printf("Line %d: (Semantic Error) Type mismatch in return type and assign type (%s)\n", assign_id->data.line_number, assign_id->data.str);
                                        semantic_error += 1;
                                    }
                                }
                            }
                            else{//one is array and other is variable
                                printf("Line %d: (Semantic Error) Type mismatch in return type and assign type (%s)\n", assign_id->data.line_number, assign_id->data.str);
                                semantic_error += 1;
                            }
                            assign_id = assign_id->forward;
                            ret_id = ret_id->next;
                        }
                        if(assign_id!=NULL || ret_id!=NULL){
                            printf("Line %d: (Semantic Error) Mismatch in number of return parameters and assigned variables\n", current->left_child->right_child->data.line_number);
                            semantic_error += 1;
                        }
                    }
                }

                //checking type and number of passed and input parameters
                ASTNode *passed_id_list = current->left_child->right_child->forward;
                if(passed_id_list == NULL){
                    if(func->inputList != NULL){
                        printf("Line %d: (Semantic Error) input parameters required to invoke %s\n", current->left_child->right_child->data.line_number, current->left_child->right_child->data.str);
                        semantic_error += 1;
                    }
                }
                else{
                    if(func->inputList == NULL){
                        printf("Line %d: (Semantic Error) %s does not take any input parameters\n", current->left_child->right_child->data.line_number, current->left_child->right_child->data.str);
                        semantic_error += 1;
                    }
                    else{
                        ASTNode *passed_id = passed_id_list->left_child;
                        parameter *input_id = func->inputList;
                        while(passed_id!=NULL && input_id!=NULL){
                            var_hash_entry* passed_var = find_var(passed_id->data.str, passed_id->data.line_number, current_var_st);
                            if(passed_var == NULL){
                                printf("Line %d: (Semantic Error) variable %s is not declared\n",passed_id->data.line_number, passed_id->data.str);
                                semantic_error += 1;
                            }
                            else if(passed_var->type.is_array == input_id->type.is_array){
                                if(passed_var->type.is_array){
                                    if(passed_var->type.t.a.dt != input_id->type.t.a.dt){//datatype of array don't match
                                        printf("Line %d: (Semantic Error) Type mismatch in invoking variable (%s) type and module input parameter type\n",passed_id->data.line_number, passed_id->data.str);
                                        semantic_error += 1;
                                    }
                                    else if(passed_var->type.t.a.s_idx != input_id->type.t.a.s_idx || passed_var->type.t.a.e_idx != input_id->type.t.a.e_idx){
                                        printf("Line %d: (Semantic Error) Bound mismatch in invoking variable (%s) type and module input parameter type\n",passed_id->data.line_number, passed_id->data.str);
                                        semantic_error += 1;
                                    }
                                }
                                else{
                                    if(passed_var->type.t.v.dt != input_id->type.t.v.dt){//datatype of variable don't match
                                        printf("Line %d: (Semantic Error) Type mismatch in invoking variable (%s) type and module input parameter type\n",passed_id->data.line_number, passed_id->data.str);
                                        semantic_error += 1;
                                    }
                                }
                            }
                            else{//one is array and other is variable
                                printf("Line %d: (Semantic Error) Type mismatch in invoking variable (%s) type and module input parameter type\n",passed_id->data.line_number, passed_id->data.str);
                                semantic_error += 1;
                            }
                            passed_id = passed_id->forward;
                            input_id = input_id->next;
                        }
                        if(passed_id!=NULL || input_id!=NULL){
                            printf("Line %d: (Semantic Error) Mismatch in number of invoking variables and module input parameters\n",current->left_child->right_child->data.line_number);
                            semantic_error += 1;
                        }
                    }
                }
            }
            semantic_check(current->left_child,current_var_st,NULL);
			var_st * prev = semantic_check(current->forward,current_var_st,prev_child_var_st);
            if(current->parent!= NULL && current->parent->parent!= NULL && current->parent->parent->data.is_terminal == 1 && current->parent->parent->data.enum_value == T_SWITCH){
				return prev;
			}
        }

        //Type of the expression inside while
		else if(current->data.is_terminal == 1&& current->data.enum_value == T_WHILE){
			struct struct_type type_l = expr_typechecker(current->left_child,current_var_st);
            var_st * prev;
            if(type_l.is_array != -1 && !(type_l.is_array == 0 && type_l.t.v.dt == BOOLEAN)){
                printf("Line %d: (Semantic Error) While expression should be a boolean expression\n",current->left_child->data.line_number);
                semantic_error += 1;
            }
			if(prev_child_var_st!=NULL){
				semantic_check(current->left_child->forward,prev_child_var_st->next,NULL);///bypassing AOB
				prev = semantic_check(current->forward,current_var_st,prev_child_var_st->next);
			}
			else {
				semantic_check(current->left_child->forward,current_var_st->child,NULL); ///bypassing AOB
				prev = semantic_check(current->forward,current_var_st,current_var_st->child);
			}
            if(current->parent!= NULL && current->parent->parent!= NULL && current->parent->parent->data.is_terminal == 1 && current->parent->parent->data.enum_value == T_SWITCH){
				return prev;
			}
		}
        
        //Check that variables used in Print and Get_Value are declared
        else if(current->data.is_terminal == 1 && (current->data.enum_value == T_PRINT || current->data.enum_value == T_GET_VALUE)){
			ASTNode *var = current->left_child;
            var_st * prev;
			if(current->data.enum_value == T_PRINT){
				if(var->data.enum_value == T_ID){
					var_hash_entry* id = find_var(var->data.str, var->data.line_number, current_var_st);
					if(id == NULL){
						printf("Line %d: (Semantic Error) variable %s is not declared\n",var->data.line_number,var->data.str);
						semantic_error += 1;
					}
					else if(id->type.is_array == 1){
						if(var->forward != NULL && var->forward->data.is_terminal == 1 && var->forward->data.enum_value == T_ID){
							var_hash_entry* index = find_var(var->forward->data.str, var->forward->data.line_number, current_var_st);
							if(index == NULL){
								printf("Line %d: (Semantic Error) variable %s is not declared\n",var->forward->data.line_number,var->forward->data.str);
								semantic_error += 1;
							}
							else if(!(index->type.is_array == 0 && index->type.t.v.dt == INTEGER)){
								printf("Line %d: (Semantic Error) variable %s is not integer type\n",var->forward->data.line_number,var->forward->data.str);
								semantic_error += 1;
							}
						}
						else if(var->forward != NULL && id->type.t.a.is_static == 1 && var->forward->data.is_terminal == 1){
							int i = atoi(var->forward->data.str);
							if(!(i >= id->type.t.a.s_idx && i<=id->type.t.a.e_idx)) {
								printf("Line %d: (Semantic Error) Index out of bounds \n",var->data.line_number);
								semantic_error += 1;
							}
						}
					}
				}
			}
			else{
				var_hash_entry* id = find_var(var->data.str, var->data.line_number, current_var_st);
				if(id == NULL){
					printf("Line %d: (Semantic Error) variable %s is not declared\n",var->data.line_number,var->data.str);
					semantic_error += 1;
				}
			}
			prev = semantic_check(current->forward,current_var_st,prev_child_var_st);
			if(current->parent!= NULL && current->parent->parent!= NULL && current->parent->parent->data.is_terminal == 1 && current->parent->parent->data.enum_value == T_SWITCH){
				return prev;
			}
            ///////////////need to optimise by bypassing left child traversal
		}

        //WARNING: This part is for traversal of the semantic_check
        //Add your semantic Rule above this only!!

		else if(current->data.is_terminal == 1 && current->data.enum_value == T_SWITCH){
			var_hash_entry* id = find_var(current->left_child->data.str, current->left_child->data.line_number, current_var_st);
            var_st * prev;
            if(id!= NULL && (id->type.is_array == 1 || id->type.t.v.dt == REAL)){ ///////////////bypassing if switch is wrong
                prev = semantic_check(current->forward,current_var_st,prev_child_var_st);
            }
            else{
                if(prev_child_var_st!=NULL){
				    semantic_check(current->left_child,prev_child_var_st->next,NULL);
				    prev = semantic_check(current->forward,current_var_st,prev_child_var_st->next);
			    }
			    else {
				    semantic_check(current->left_child,current_var_st->child,NULL);
				    prev = semantic_check(current->forward,current_var_st,current_var_st->child);
			    }
            }
            if(current->parent!= NULL && current->parent->parent!= NULL && current->parent->parent->data.is_terminal == 1 && current->parent->parent->data.enum_value == T_SWITCH){
				return prev;
			}
		}
        
        else if(current->data.is_terminal == 1 && current->data.enum_value == T_FOR){
            var_st * prev;
            if(prev_child_var_st!=NULL){
				semantic_check(current->left_child,prev_child_var_st->next,NULL);
				prev = semantic_check(current->forward,current_var_st,prev_child_var_st->next);
			}
			else {
				semantic_check(current->left_child,current_var_st->child,NULL);
				prev = semantic_check(current->forward,current_var_st,current_var_st->child);
			}
            if(current->parent!= NULL && current->parent->parent!= NULL && current->parent->parent->data.is_terminal == 1 && current->parent->parent->data.enum_value == T_SWITCH){
				return prev;
			}
        }

		else if(current->data.is_terminal == 1 && current->data.enum_value == T_ID && current->parent->data.enum_value == T_MODULE){
			func_hash_entry *found_entry = find_func_hash(current->data.str);
			if(current->data.line_number == found_entry->define_lno){
                semantic_check(current->left_child,found_entry->child,NULL);
                semantic_check(current->forward,found_entry->child,NULL);
            }
            //semantic_check(current->left_child,found_entry->child,NULL);
			//semantic_check(current->forward,found_entry->child,NULL);
		}

		else if(current->data.is_terminal == 1 && current->data.enum_value == T_PROGRAM){
			func_hash_entry *found_entry = find_func_hash(current->data.str);
			/*if(current->data.line_number == found_entry->define_lno){
                semantic_check(current->left_child,found_entry->child,NULL);
            }*/
            semantic_check(current->left_child,found_entry->child,NULL);
			semantic_check(current->forward,NULL,NULL);
		}

        else if(current->parent != NULL && current->parent->data.is_terminal == 1 && current->parent->data.enum_value == T_SWITCH){
			var_st * prev = semantic_check(current->left_child,current_var_st,prev_child_var_st);
			semantic_check(current->forward,current_var_st,prev);
		}
		else if(current->parent!= NULL && current->parent->parent!= NULL && current->parent->parent->data.is_terminal == 1 && current->parent->parent->data.enum_value == T_SWITCH){
			semantic_check(current->left_child,current_var_st,NULL);
			var_st * prev = semantic_check(current->forward,current_var_st,prev_child_var_st);
			return prev;
		}

		else{
			semantic_check(current->left_child,current_var_st,NULL);
			semantic_check(current->forward,current_var_st,prev_child_var_st);
		}
    }

    else{//Right child is not NULL
		if(current->parent != NULL && current->parent->data.is_terminal == 0 && current->parent->data.enum_value == NT_assignmentStmt 
        && current->data.is_terminal == 1 && current->data.enum_value == T_ASSIGNOP){
			struct struct_type type_l = expr_typechecker(current->left_child,current_var_st);
			struct struct_type type_r = expr_typechecker(current->right_child,current_var_st);

			if(type_l.is_array != -1 && type_r.is_array != -1){
				if(type_l.is_array != type_r.is_array ){
					printf("Line %d: (Semantic Error) Type error\n",current->data.line_number);
                    semantic_error += 1;
				}
				else if(type_l.is_array == 1){
					if(type_l.t.a.is_static == 1 && type_r.t.a.is_static == 1){
						if(!(type_l.t.a.dt == type_r.t.a.dt && type_l.t.a.s_idx == type_r.t.a.s_idx && type_l.t.a.e_idx == type_r.t.a.e_idx)){
							printf("Line %d: (Semantic Error) Type error\n",current->data.line_number);
                            semantic_error += 1;
						}
					}
					else{
						///////////////////////////////dynamic arrays
						if(type_l.t.a.dt != type_r.t.a.dt){
                            printf("Line %d: (Semantic Error) Type error\n",current->data.line_number);
                            semantic_error += 1;
                        }
						else{
								//////////////////////////// s_idx and e_idx check for dynamic arrays is to be done
						}
					}
				}
				else{
					if(type_l.t.v.dt != type_r.t.v.dt){
                        printf("Line %d: (Semantic Error) Type error\n",current->data.line_number);
                        semantic_error += 1;
                    }
				}
			}
			semantic_check(current->forward,current_var_st,NULL);
		}

		else{
		    semantic_check(current->left_child,current_var_st,NULL);
            semantic_check(current->right_child,current_var_st,NULL);
			semantic_check(current->forward,current_var_st,NULL);
        }
    }
    return NULL;
}

// Print Symbol Table
void printSTEntry(var_st* table){
    if(table == NULL){
        return;
    }
    else{
        int level;
        char dt_type[21];
        for(int i = 0 ;i<SYMBOL_TABLE_SIZE; i++){
            var_hash_entry* temp = table->var_hash_table[i].head;
            while(temp != NULL){
                if(temp->flag == 0 || temp->flag == 1){
                    level = 0;
                }
                else{
                    level = table->depth;
                }

                char spacer[3];
                if(strlen(table->root_var_st->parent.f_parent->lexeme)<7){
                    strcpy(spacer,"\t\t");
                }
                else{
                    strcpy(spacer,"\t");
                }

                char lex_spacer[3];
                if(strlen(temp->lexeme)<=7){
                    strcpy(lex_spacer,"\t\t");
                }
                else{
                    strcpy(lex_spacer,"\t");
                }

                if(temp->type.is_array == 0){
                    if(temp->type.t.v.dt == INTEGER){
                        strcpy(dt_type,"integer");
                    }
                    else if(temp->type.t.v.dt == BOOLEAN){
                        strcpy(dt_type,"boolean");
                    }
                    else if(temp->type.t.v.dt == REAL){
                        strcpy(dt_type,"real");
                    }
                    printf("%s%s %s%s %d-%d   \t%d\tno\t---\t\t---\t\t%s\t   %d\t   %d\n",temp->lexeme, lex_spacer, table->root_var_st->parent.f_parent->lexeme, spacer, table->start_lno, table->end_lno, temp->width, dt_type, temp->offset, level);
                }
                else{
                    if(temp->type.t.a.dt == INTEGER){
                        strcpy(dt_type,"integer");
                    }
                    else if(temp->type.t.a.dt == BOOLEAN){
                        strcpy(dt_type,"boolean");
                    }
                    else if(temp->type.t.a.dt == REAL){
                        strcpy(dt_type,"real");
                    }
                    if(temp->type.t.a.is_static == 1){
                        printf("%s%s %s%s %d-%d   \t%d\tyes\tstatic  \t[%d, %d]  \t%s\t   %d\t   %d\n",temp->lexeme, lex_spacer, table->root_var_st->parent.f_parent->lexeme, spacer, table->start_lno, table->end_lno, temp->width, temp->type.t.a.s_idx, temp->type.t.a.e_idx, dt_type, temp->offset, level);
                    }
                    else{
                        if(temp->type.t.a.s_idx == -1 && temp->type.t.a.e_idx != -1){
                            printf("%s%s %s%s %d-%d   \t%d\tyes\tdynamic \t[%s, %d]  \t%s\t   %d\t   %d\n",temp->lexeme, lex_spacer, table->root_var_st->parent.f_parent->lexeme, spacer, table->start_lno, table->end_lno, temp->width, temp->type.t.a.s_str, temp->type.t.a.e_idx, dt_type, temp->offset, level);
                        }
                        else if(temp->type.t.a.s_idx != -1 && temp->type.t.a.e_idx == -1){
                            printf("%s%s %s%s %d-%d   \t%d\tyes\tdynamic \t[%d, %s]  \t%s\t   %d\t   %d\n",temp->lexeme, lex_spacer, table->root_var_st->parent.f_parent->lexeme, spacer, table->start_lno, table->end_lno, temp->width, temp->type.t.a.s_idx, temp->type.t.a.e_str, dt_type, temp->offset, level);
                        }
                        else{
                            printf("%s%s %s%s %d-%d   \t%d\tyes\tdynamic \t[%s, %s]  \t%s\t   %d\t   %d\n",temp->lexeme, lex_spacer, table->root_var_st->parent.f_parent->lexeme, spacer, table->start_lno, table->end_lno, temp->width, temp->type.t.a.s_str, temp->type.t.a.e_str, dt_type, temp->offset, level);
                        }
                    }
                }

                temp = temp->next;
            }
        }
        printSTEntry(table->child);
        printSTEntry(table->next);   
    }
}

void printSymbolTable(){
    printf("\n-----------------------------------------------------------------------------------------------------------------------\n");
    printf("var_name \t module_name     scope          width  isArray                  range           type       offset level\n");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    for(int i = 0 ; i<SYMBOL_TABLE_SIZE; i++){
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            printSTEntry(f_entry->child);
            f_entry = f_entry->next;
        }
    }
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
}


// Print Activation Records
void printActRecords(){
    for(int i = 0 ; i<SYMBOL_TABLE_SIZE; i++){
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            if(strlen(f_entry->lexeme)>7) printf("%s\t   %d\n",f_entry->lexeme, f_entry->total_func_memory);
            else printf("%s\t\t   %d\n",f_entry->lexeme, f_entry->total_func_memory);
            f_entry = f_entry->next;
        }
    }
}


// Print Array Entries
void printArrayEntry(var_st* table){
    if(table == NULL){
        return;
    }
    else{
        char dt_type[21];
        for(int i = 0 ;i<SYMBOL_TABLE_SIZE; i++){
            var_hash_entry* temp = table->var_hash_table[i].head;
            while(temp != NULL){
                if(temp->type.is_array == 1){
                    if(temp->type.t.a.dt == INTEGER){
                        strcpy(dt_type,"integer");
                    }
                    else if(temp->type.t.a.dt == BOOLEAN){
                        strcpy(dt_type,"boolean");
                    }
                    else if(temp->type.t.a.dt == REAL){
                        strcpy(dt_type,"real");
                    }

                    char spacer[3];
                    if(strlen(table->root_var_st->parent.f_parent->lexeme)<7){
                        strcpy(spacer,"\t\t");
                    }
                    else{
                        strcpy(spacer,"\t");
                    }

                    if(temp->type.t.a.is_static == 1){
                        printf("%s %s%d-%d\t   %s \tstatic array\t [%d, %d]  \t%s\n", table->root_var_st->parent.f_parent->lexeme ,spacer, table->start_lno, table->end_lno, temp->lexeme, temp->type.t.a.s_idx, temp->type.t.a.e_idx, dt_type);
                    }
                    else{
                        if(temp->type.t.a.s_idx == -1 && temp->type.t.a.e_idx != -1){
                            printf("%s %s%d-%d\t   %s \tdynamic array\t [%s, %d]  \t%s\n", table->root_var_st->parent.f_parent->lexeme ,spacer, table->start_lno, table->end_lno, temp->lexeme, temp->type.t.a.s_str, temp->type.t.a.e_idx, dt_type);
                        }
                        else if(temp->type.t.a.s_idx != -1 && temp->type.t.a.e_idx == -1){
                            printf("%s %s%d-%d\t   %s \tdynamic array\t [%d, %s]  \t%s\n", table->root_var_st->parent.f_parent->lexeme ,spacer, table->start_lno, table->end_lno, temp->lexeme, temp->type.t.a.s_idx, temp->type.t.a.e_str, dt_type);
                        }
                        else{
                            printf("%s %s%d-%d\t   %s \tdynamic array\t [%s, %s]  \t%s\n", table->root_var_st->parent.f_parent->lexeme ,spacer, table->start_lno, table->end_lno, temp->lexeme, temp->type.t.a.s_str, temp->type.t.a.e_str, dt_type);
                        }
                    }
                }

                temp = temp->next;
            }
        }
    }
    printArrayEntry(table->child);
    printArrayEntry(table->next);   
}

void printArrays(){
    printf("\n------------------------------------------------------------------------------------------------------\n");
    for(int i = 0 ; i<SYMBOL_TABLE_SIZE; i++){
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            printArrayEntry(f_entry->child);
            f_entry = f_entry->next;
        }
    }
    printf("-------------------------------------------------------------------------------------------------------\n");
}

////////printing the symbol table (for debugging)
// void print_var_st(var_st* table){
//     if(table == NULL) return;
//     else{
//         printf("\nVariable Symbol Table:\n");
//         printf("lno: %d , End_lno: %d , depth: %d, tag= %d\n",table->lno,table->end_lno,table->depth,table->tag);
//         printf("entries\n");
//         for(int i = 0 ;i<SYMBOL_TABLE_SIZE; i++){
//             var_hash_entry* temp = table->var_hash_table[i].head;
//             while(temp != NULL){
//                 if(temp->type.is_array == 0){
//                     printf("\t%s %d - %d %d %d %d %d\n",temp->lexeme, temp->lno,table->end_lno, temp->flag, temp->width, temp->offset, (int)temp->type.t.v.dt);
//                 }
//                 else{
//                     printf("\t%s %d - %d %d %d %d %d %d %d\n",temp->lexeme, temp->lno,table->end_lno, temp->flag, temp->width, temp->offset, (int)temp->type.t.a.dt, temp->type.t.a.s_idx, temp->type.t.a.e_idx);
//                 }
//                 temp = temp->next;
//             }
//         }
//         print_var_st(table->child);
//         print_var_st(table->next);
//     }
// }

// void print_st(){
//     for(int i = 0 ; i<SYMBOL_TABLE_SIZE; i++){
//         func_hash_entry *f_entry = func_st_root[i].head;
//         for(int j=0; j<func_st_root[i].size; j++){
//             printf("\nFUNCTION\n");
//             printf("%s\t%d\t%d",f_entry->lexeme, f_entry->declare_lno, f_entry->define_lno);
//             print_var_st(f_entry->child);
//             f_entry = f_entry->next;
//         }
//     }
// }