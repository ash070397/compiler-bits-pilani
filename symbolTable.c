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