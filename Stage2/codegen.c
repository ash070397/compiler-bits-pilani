/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
#include "codegen.h"
int for_count=0;
int while_count=0;
int switch_count=0;
int print_count = 0;
int input_count = 0;
datatype type_flag;
int inverter_count = 0;
//type_flag update.
int cmp_count = 0;
//have to make it global as it gets
char temp_lexeme[50];

void declaration_maker(FILE *f1)
{
    fprintf(f1, "extern printf\nextern scanf\nextern printer\n");
    fprintf(f1,"SECTION .bss \n");
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        func_hash_entry *f_entry = func_st_root[i].head;
        for(int j=0; j<func_st_root[i].size; j++){
            declaration_varst(f_entry->child,f1);
            f_entry = f_entry->next;
        }
    }
    fprintf(f1,"sys_fleft : resb 4\n");
    fprintf(f1,"sys_fright : resb 4\n");
    fprintf(f1, "bool_input : resb 2\n");
    //JUST CHECK THIS OUT..
    fprintf(f1, "SECTION .data\n");
    fprintf(f1, "newline db \" \",10,0\n");
    fprintf(f1, "print_int db \"%%d\" ,10 ,0\n");
    fprintf(f1, "print_float db \"%%f\" ,10 ,0\n");
    fprintf(f1, "print_int_array db \"%%d \" ,0\n");
    fprintf(f1, "print_float_array db \"%%f \" ,0\n");
    fprintf(f1, "scan_int db \"%%d\", 0\n");
    fprintf(f1, "scan_float db \"%%f\", 0\n");
    fprintf(f1, "msg1_abort db \"Array out of bounds error at line %%d\",10,0\n");
    fprintf(f1, "print_true db \"true\",10,0\n");
    fprintf(f1, "print_false db \"false\",10, 0\n");
    fprintf(f1, "print_true_array db \"true \",0\n");
    fprintf(f1, "print_false_array db \"false \",0\n");
    fprintf(f1, "len_abort1 equ $ - msg1_abort\n");
    fprintf(f1, "nega db \"-\",0\n");
    fprintf(f1 , "integer_prompt db \"Input: Enter an integer value \",10,0\n");
    fprintf(f1 , "boolean_prompt db \"Input: Enter a boolean value \",10,0\n");
    fprintf(f1 , "real_prompt db \"Input: Enter a real value \",10,0\n");
    fprintf(f1 , "output_prompt db \"Output: \",0\n");
    fprintf(f1, "integer_array_prompt db \"Enter %%d array elements of integer type for range %%d to %%d\",10,0\n");
    fprintf(f1, "boolean_array_prompt db \"Enter %%d array elements of boolean type for range %%d to %%d\",10,0\n");
    fprintf(f1, "real_array_prompt db \"Enter %%d array elements of real type for range %%d to %%d\",10,0\n");

    fprintf(f1, "msg2_abort db \"Arrays not of same type so can't copy at line %%d\",10,0\n");
    fprintf(f1, "len_abort2 equ $ - msg1_abort\n");


    fprintf(f1,"\n\n");
    fprintf(f1, "section .text\n");
    fprintf(f1, "global main\n");

}

char *name_in_asm_gen(char *lex, var_hash_entry *id_entry){
    if(id_entry == NULL){
        return "\0";
    }
    int lno = id_entry->lno;

    char string_id_line[30];
    strcpy(temp_lexeme, lex);
    strcat(temp_lexeme, "_");
    sprintf(string_id_line, "%d", lno);
    strcat(temp_lexeme, string_id_line);
    return temp_lexeme;

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
                   
                    //fprintf(f,"%s: resb %d \n",temp_lexeme,temp->width);
                    fprintf(f, "%s: resb 4\n",temp_lexeme);
                   
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
//is_const_indx-> flag denoting if indx is a constant (1)/ variable(0).
//indx_name either stores a integer const/index var name in asm file
//array_name in asm file.
//current ASTNode to get the line number (for err reporting)
void bound_checker(int is_const_indx, char *indx_name, FILE *f1, char *arr_name, ASTNode *curr){
    fprintf(f1, "push edx\nxor edx,edx\n");
    if(is_const_indx == 1){
        fprintf(f1, "mov ax, %s\n", indx_name);
    }
    else{
        fprintf(f1, "mov ax, [%s]\n", indx_name);
    }
    fprintf(f1, "mov ebx, [%s]\n", arr_name);
    fprintf(f1, "push ecx\nxor ecx, ecx\n");
    fprintf(f1, "mov cx, word[ebx]\n");
    fprintf(f1, "cmp ax, cx\n");
    fprintf(f1, "pop ecx\n mov edx,%d \npush edx\n",curr->data.line_number);
    fprintf(f1, "jl abort1\n");
    //restoring stuff
    fprintf(f1, "pop edx\npop edx\n");

    fprintf(f1, "xor ebx, ebx\n");

    fprintf(f1, "push edx\nxor edx,edx\n");
    fprintf(f1, "mov ebx, [%s]\n", arr_name);
    fprintf(f1, "push ecx\nxor ecx, ecx\n");
    fprintf(f1, "mov cx, word[ebx + 2]\n");
    fprintf(f1, "cmp ax, cx\n");
    fprintf(f1, "pop ecx\n mov edx,%d \npush edx\n",curr->data.line_number);
    fprintf(f1, "jg abort1\n");
    //restoring stuff
    fprintf(f1, "pop edx\npop edx\n");
}

//generate code for getting proper index
//ax = index (done in calling function)
//bx = lo
//ax = ax - bx
//Multiply by width
//assign finally in correct position
void calc_offset_assign(char *arr_name, type type_of_arr, FILE *f1){
    fprintf(f1, "push ebx\npush ecx\n");
    fprintf(f1, "xor ebx, ebx\n");
    fprintf(f1, "mov ecx, [%s]\nmov bx, word[ecx]\n", arr_name);
    fprintf(f1, "sub ax, bx\n");
    fprintf(f1, "pop ecx\npop ebx\n");

   

    fprintf(f1, "push ecx\nxor ecx, ecx\n");
    if(type_of_arr.t.a.dt == INTEGER)
        fprintf(f1, "mov cx, 2\n");
    else if(type_of_arr.t.a.dt == REAL)
        fprintf(f1, "mov cx, 4\n");
    else if(type_of_arr.t.a.dt == BOOLEAN)
        fprintf(f1, "mov cx, 1\n");
   
    fprintf(f1, "push edx\nmul cx\npop edx\n");
    //base in edx
    fprintf(f1, "mov edx, [%s]\n", arr_name);
    fprintf(f1, "add edx, eax\n");
    fprintf(f1, "add edx, 4\n");
    fprintf(f1, "pop ecx\n");//line 128 push

    //the pushes I did in the calling function
    fprintf(f1, "pop ecx\n");

    if(type_of_arr.t.a.dt == INTEGER){
        //fprintf(f1, "pop edx\n");
        fprintf(f1, "pop ebx\n");
        fprintf(f1, "pop eax\n");

        fprintf(f1, "mov word[edx], bx\n");
    }
    else if(type_of_arr.t.a.dt == REAL){
        //fprintf(f1, "pop edx\n");
        fprintf(f1, "mov ebx, dword[sys_fright]\n");
        fprintf(f1, "mov dword[edx], ebx\n");
        fprintf(f1, "pop ebx\n");
        fprintf(f1, "pop eax\n");
    }
    else if(type_of_arr.t.a.dt == BOOLEAN){
        //fprintf(f1, "pop edx\n");
        fprintf(f1, "pop ebx\n");
        fprintf(f1, "pop eax\n");
        fprintf(f1, "mov [edx], bl\n");
    }
   


}

//ANUBHAV START
void for_gen(ASTNode *rfor, var_st *curr_scope, var_st *prev_scope, FILE *f1){
    for_count++;
    int current_for;
    //printf("%d--->\n\n\n", curr_scope->lno);
    ASTNode *start_index=rfor->left_child->forward->left_child;
    ASTNode *end_index = start_index->forward;
    int dif=-atoi(start_index->data.str)+atoi(end_index->data.str) + 1;
    current_for=for_count;
    char temp[40];
    ASTNode * m_node = rfor->left_child;
    var_hash_entry *var_id= find_var(m_node->data.str,m_node->data.line_number,curr_scope->parent.v_parent);
    int line_number=var_id->lno;
    sprintf(temp,"%s_%d",m_node->data.str,line_number);
    fprintf(f1,"mov word[%s] , %d\n",temp,atoi(start_index->data.str));
    fprintf(f1, "PUSH EDX\nXOR EDX,EDX\nPUSH ECX\n");  
    fprintf(f1,"xor ECX,ECX\n");
    fprintf(f1,"MOV ECX , %d\n",dif);
    fprintf(f1,"FOR_%d:\n",current_for);
    fprintf(f1,"push ECX\n");
    //statments call out here..
    ASTNode *stmts = rfor->left_child->forward->forward;
    //fprintf(f1, "\n\n\n");
    fprintf(f1,"push eax\npush ebx\npush ecx\npush edx\n");
    statements_gen(stmts, curr_scope, prev_scope, f1);
    fprintf(f1,"pop edx\npop ecx\npop ebx\npop eax\n");
    //fprintf(f1, "\n\n\n");
    fprintf(f1,"lab_%d: POP ECX\n",current_for);
    fprintf(f1, "mov dx, [%s]\ninc dx\nmov [%s] ,dx\n",temp,temp);
    fprintf(f1,"dec ecx\ncmp ecx,0\njne FOR_%d\n",current_for);
    fprintf(f1, "POP ECX\nPOP EDX\n");

}

void while_gen(ASTNode *rwhile, var_st *curr_scope, var_st *prev_scope, FILE *f1){
    while_count++;
    ASTNode *aob=rwhile->left_child;//boolean as of now
    int current_while=while_count;
    fprintf(f1,"WHILE_%d:\n",current_while);//start of iterations and comp
    make_code(aob, curr_scope->parent.v_parent, f1);
   

    fprintf(f1,"xor ecx,ecx\nMOV ECX,EAX\n");
    fprintf(f1,"CMP ECX, 0\n");
    fprintf(f1,"JE EXIT_WHILE_%d\n",while_count);
    // stmts call out here to construct body..
    ASTNode *stmts_in_while_loop = aob->forward;

    statements_gen(stmts_in_while_loop, curr_scope, prev_scope, f1);
    fprintf(f1,"JMP WHILE_%d\n",current_while);
    fprintf(f1,"EXIT_WHILE_%d:",current_while);
}

void switch_gen(ASTNode *rswitch, var_st *curr_scope, var_st *prev_scope, FILE *f1){
    //this is the value of the switch
    switch_count++;
    int current_switch=switch_count;
    ASTNode *switch_id=rswitch->left_child;
    ASTNode *temp=switch_id->forward;
    fprintf(f1,"SWITCH_%d:\n",current_switch);

    int count;
    int len=atoi(switch_id->data.str);
    //stores the variable name
    char temp2[len+10];
    var_hash_entry *var_id= find_var(switch_id->data.str,switch_id->data.line_number,curr_scope->parent.v_parent);
    int line_number=var_id->lno;
    sprintf(temp2,"%s_%d",switch_id->data.str,line_number);
    while(temp->data.is_terminal==1 && temp->data.enum_value!=T_DEFAULT){
        if(strcmp(temp->data.str,"true")==0){
            fprintf(f1, "xor ecx,ecx\n mov cl,byte[%s]\n",temp2);
            fprintf(f1,"CMP ECX,1\n");
            fprintf(f1,"JE CASE_1\n");
        }
        else if(strcmp(temp->data.str,"false")==0){
            fprintf(f1, "xor ecx,ecx\n mov cl,byte[%s]\n",temp2);
            fprintf(f1,"CMP ECX,0\n");
            fprintf(f1,"JE CASE_0\n");
        }
        else{      
            fprintf(f1, "xor ecx,ecx\n mov cx,word[%s]\n",temp2);
            count=atoi(temp->data.str);
            fprintf(f1,"CMP ECX,%d\n",count);
            fprintf(f1,"JE CASE_%d\n",count);
        }    
        temp=temp->forward;
    }
    if(temp->data.is_terminal==1 && temp->data.enum_value==T_DEFAULT && var_id->type.t.v.dt != BOOLEAN){
        fprintf(f1,"JMP DEFAULT_%d\n",current_switch);
    }

    temp=switch_id->forward;
    while(temp->data.is_terminal==1 && temp->data.enum_value!=T_DEFAULT ){ //generated all case statements now call switch case
        if(strcmp(temp->data.str,"true")==0){
           

            fprintf(f1,"CASE_1:\n");
            // call statements function
            ASTNode *stmts=temp->left_child;
            prev_scope = statements_gen(stmts,curr_scope,prev_scope,f1);
            fprintf(f1,"JMP END_SWITCH_%d\n",current_switch);
        }
        else if(strcmp(temp->data.str,"false")==0){
           
            fprintf(f1,"CASE_0:\n");
            ASTNode *stmts=temp->left_child;
            prev_scope = statements_gen(stmts,curr_scope,prev_scope,f1);
            fprintf(f1,"JMP END_SWITCH_%d\n",current_switch);
        }
        else{    

            count=atoi(temp->data.str);
            fprintf(f1,"CASE_%d:\n",count);
            ASTNode *stmts=temp->left_child;

            prev_scope = statements_gen(stmts,curr_scope,prev_scope,f1);
            fprintf(f1,"JMP END_SWITCH_%d\n",current_switch);    
        }
        temp=temp->forward;
    }
    if(var_id->type.t.v.dt != BOOLEAN){
        fprintf(f1,"DEFAULT_%d:\n",current_switch);

        ASTNode *stmts=temp->left_child;
        prev_scope = statements_gen(stmts,curr_scope,prev_scope,f1);
    }
    fprintf(f1,"END_SWITCH_%d:\n",current_switch);

}



//copy input param..
//takes a module reuse node..
//Needs the symbol table of the called function..
void module_input_copy(func_hash_entry *module_st, ASTNode *module_reuse_node, var_st *curr_scope, var_st *prev_scope, FILE *temp_file){
    //I get a moduleResuse smtmt here
    //Need to push i/p param onto stack
    ASTNode *assign_op = module_reuse_node->left_child;
    ASTNode *module_id = assign_op->right_child;
    ASTNode *id_list = module_id->forward;
    //local parameter
    ASTNode *head_input_param = id_list->left_child;
   
    //formal parameter
    parameter *input_list_called_fn = module_st->inputList;
   
    fprintf(temp_file,"push eax\n");
    while(head_input_param != NULL){
        //local is the one which is rcvd formal is passed
        char name_in_asm_local[30];
        //getting name of vat
        strcpy(name_in_asm_local, input_list_called_fn->lexeme);
        //appending _
        strcat(name_in_asm_local, "_");
        char string_line_no_local[20];
        //Getting line number in string format
        //itoa(this_node->data.line_number, string_line_no, 10);
        sprintf(string_line_no_local, "%d", input_list_called_fn->lno);
        //Appending line number
        strcat(name_in_asm_local, string_line_no_local);

       
        //move the stuff into reg (I need the width of the variable)

        var_hash_entry *this_var = find_var(head_input_param->data.str, head_input_param->data.line_number, curr_scope);

        char name_in_asm_formal[30];
        strcpy(name_in_asm_formal, name_in_asm_gen(head_input_param->data.str, this_var));


        fprintf(temp_file,"xor eax, eax\n");
        if(this_var == NULL){
            //means u are passing a constant could be real int or bool
            //cant be of arr type
            if(head_input_param->data.enum_value == T_RNUM){
                fprintf(temp_file, "mov eax, %s\n", head_input_param->data.str);
                fprintf(temp_file, "mov [%s], eax\n", name_in_asm_local);

            }
            else if(head_input_param->data.enum_value == T_NUM){
                fprintf(temp_file, "mov ax, %s\n", head_input_param->data.str);
                fprintf(temp_file, "mov [%s], ax\n", name_in_asm_local);

            }
            //what to do ashish ...
            else if(strcmp(head_input_param->data.str, "true") == 0){
                //
                fprintf(temp_file, "mov al, 1\n");
                fprintf(temp_file, "mov [%s], al\n", name_in_asm_local);

            }
            else if(strcmp(head_input_param->data.str, "false") == 0){
                fprintf(temp_file, "mov al, 0\n");
                fprintf(temp_file, "mov [%s], al\n", name_in_asm_local);

            }


        }
        else{//its a variable
            type type_info = this_var->type;

            if(type_info.is_array == 0){//its an id
                datatype this_var = type_info.t.v.dt;
                if(this_var == INTEGER){
                    fprintf(temp_file, "mov ax, [%s]\n", name_in_asm_formal);
                    fprintf(temp_file, "mov [%s], ax\n",name_in_asm_local);

                }
                else if(this_var == BOOLEAN){
                    fprintf(temp_file, "mov al, [%s]\n", name_in_asm_formal);
                    fprintf(temp_file, "mov [%s], al\n", name_in_asm_local);
                }
                else if(this_var == REAL){
                    fprintf(temp_file, "mov eax, [%s]\n", name_in_asm_formal);
                    fprintf(temp_file, "mov [%s], eax\n", name_in_asm_local);
                }
            }
            else{//its an arr
                if(input_list_called_fn->type.t.a.s_idx != -1)
                {
                    fprintf(temp_file, "push eax\npush ebx\npush ecx\nxor eax,eax\nxor ebx,ebx\n");
                    fprintf(temp_file,"mov ecx, [%s]\n", name_in_asm_formal);
                    fprintf(temp_file,"mov ax , word[ecx]\n");

                    fprintf(temp_file,"mov bx , %d\n",input_list_called_fn->type.t.a.s_idx);
                    fprintf(temp_file, "push edx\n xor edx,edx\n");
                    fprintf(temp_file, "mov edx,%d \npush edx\n",head_input_param->data.line_number);
                    fprintf(temp_file, "cmp ax, bx\n");
                    fprintf(temp_file, "jne abort2\n");
                    //New add
                    fprintf(temp_file, "pop edx\npop edx\n");
                    fprintf(temp_file, "pop ecx\npop ebx\npop eax\n");

                }
                if(input_list_called_fn->type.t.a.e_idx != -1)
                {
                    fprintf(temp_file, "push eax\npush ebx\npush ecx\nxor eax,eax\nxor ebx,ebx\n");
                    fprintf(temp_file,"mov ecx, [%s]\n", name_in_asm_formal);
                    fprintf(temp_file,"mov ax , word[ecx+2]\n");

                    fprintf(temp_file,"mov bx , %d\n",input_list_called_fn->type.t.a.e_idx);
                    fprintf(temp_file, "push edx\n xor edx,edx\n");
                    fprintf(temp_file, "mov edx,%d \npush edx\n",head_input_param->data.line_number);
                    fprintf(temp_file, "cmp ax, bx\n");
                    fprintf(temp_file, "jne abort2\n");
                    //New add
                    fprintf(temp_file, "pop edx\npop edx\n");
                    fprintf(temp_file, "pop ecx\npop ebx\npop eax\n");
                }
                //Bound check
                fprintf(temp_file, "mov eax, [%s]\n", name_in_asm_formal);
                fprintf(temp_file, "mov [%s], eax\n", name_in_asm_local);
            }
           
        }
 
        head_input_param = head_input_param->forward;
        input_list_called_fn = input_list_called_fn->next;
       
    }

    fprintf(temp_file, "pop eax\n");

}

void declaration_arr(ASTNode *declare_node, var_st *curr_st, var_st *prev_st , FILE *temp_file){

    //Just saving these reg as I am going to use them
    fprintf(temp_file, "push eax\n");
    fprintf(temp_file, "push ebx\n");

   
    ASTNode *idlist = declare_node->left_child;
    ASTNode *head_of_list = idlist->left_child;
    ASTNode *array = idlist->forward;
    ASTNode *rangeop = array->left_child;
    ASTNode *type = rangeop->forward;
    ASTNode *index1 = rangeop->left_child;
    ASTNode *index2 = index1->forward;

    while(head_of_list!=NULL){
        //for array name
        char name_in_asm_array[30];
        //getting name of vat
        strcpy(name_in_asm_array, head_of_list->data.str);
        strcat(name_in_asm_array, "_");
        char string_line_no[20];

        sprintf(string_line_no, "%d", head_of_list->data.line_number);
        //Appending line number
        strcat(name_in_asm_array, string_line_no);

        //for index1
        fprintf(temp_file,"xor ebx, ebx\n");
        char name_in_asm_index1[30];
        if(index1->data.is_terminal == 1 && index1->data.enum_value == T_NUM){
            fprintf(temp_file,"mov bx, %s\n", index1->data.str);
        }
        else if(index1->data.is_terminal == 1 && index1->data.enum_value == T_ID){
           
            var_hash_entry *v1 = find_var(index1->data.str, head_of_list->data.line_number, curr_st);
            strcpy(name_in_asm_index1, name_in_asm_gen(index1->data.str, v1));
            fprintf(temp_file, "mov bx, [%s]\n", name_in_asm_index1);

        }
       

        //for 2nd index no
        fprintf(temp_file,"xor eax, eax\n");
        char name_in_asm_index2[30];
        if(index2->data.is_terminal == 1 && index2->data.enum_value == T_NUM){
            fprintf(temp_file,"mov ax, %s\n", index2->data.str);

        }
        else if(index2->data.is_terminal == 1 && index2->data.enum_value == T_ID){

            var_hash_entry *v2 = find_var(index2->data.str, head_of_list->data.line_number, curr_st);
            strcpy(name_in_asm_index2, name_in_asm_gen(index2->data.str, v2));      
            fprintf(temp_file,"mov ax, [%s]\n", name_in_asm_index2);

        }
       
        fprintf(temp_file, "push ecx\n");
        fprintf(temp_file, "xor ecx, ecx\n");
        //gives size of the array
        //ax has lower index and bx has higher index
        fprintf(temp_file, "push edx\n");
        fprintf(temp_file, "xor edx, edx\n");
        //getting 2nd idx
        fprintf(temp_file, "mov dx, ax\n");
        fprintf(temp_file, "sub ax, bx\n");
        //arr[2..4] then size is 3
        fprintf(temp_file, "inc ax\n");

        if(type->data.enum_value == T_INTEGER)
            fprintf(temp_file, "mov cx, 2\n");
        else if(type->data.enum_value == T_REAL)
            fprintf(temp_file, "mov cx, 4\n");
        else if(type->data.enum_value == T_BOOLEAN)
            fprintf(temp_file, "mov cx, 1\n");
       
        fprintf(temp_file, "push edx\nmul cx\npop edx\n");
        //dx has the 2nd idx and bx has the 1st idx I need to store this also
        //I need to store start and end idx at 0-2 start idx and 2-4 end idx
        fprintf(temp_file, "add ax, 4\n");
        fprintf(temp_file, "mov [esi], bx\n");
        fprintf(temp_file, "mov [esi + 2], dx\n");
        //first set the base then move up
        fprintf(temp_file, "mov [%s], esi\n", name_in_asm_array);
        fprintf(temp_file,"add esi, eax\n");
        fprintf(temp_file,"add esi ,4\n");
        fprintf(temp_file, "pop edx\n");
        fprintf(temp_file, "pop ecx\n");

        head_of_list = head_of_list->forward;
    }

    fprintf(temp_file, "pop ebx\n");
    fprintf(temp_file, "pop eax\n");
}

void assignment_stmt(ASTNode *assign_node, var_st *curr_st, var_st *prev_st, FILE *temp_file){
    //could be a moduleResuse stmt result
    //or an array variable assignment
    //or a normal variable assignment

    //FILE *temp_file = fopen("assignment_stmt_arr_varidx.asm","w");
    //op param copying you cant have array variable out here
    //printf("I AM HEREEE\n");
    if(assign_node->parent->data.is_terminal == 0 && assign_node->parent->data.enum_value == NT_moduleReuseStmt){
        if(assign_node->left_child != NULL){//there is some return param to catch

            ASTNode *idlist = assign_node->left_child;
            ASTNode *head_of_ret_list = idlist->left_child;
            ASTNode *called_module = assign_node->right_child;
            func_hash_entry *called_module_scope = find_func_hash(called_module->data.str);
            parameter *oplist_head = called_module_scope->outputList;

            fprintf(temp_file, "push eax\n");
           
            while(head_of_ret_list != NULL){//iterating over the return list
                //need line number -- problems out here...
                fprintf(temp_file, "xor eax, eax\n");
                char name_in_asm_op_param[30];
                char string_line_no_op_param[30];
                strcpy(name_in_asm_op_param, oplist_head->lexeme);
                strcat(name_in_asm_op_param, "_");
                sprintf(string_line_no_op_param, "%d", oplist_head->lno);
                strcat(name_in_asm_op_param, string_line_no_op_param);
               
                var_hash_entry *ret_param = find_var(head_of_ret_list->data.str, head_of_ret_list->data.line_number, curr_st);
                char name_in_asm_ret_param[30];
                strcpy(name_in_asm_ret_param, name_in_asm_gen(head_of_ret_list->data.str, ret_param));
       
                //I have names of the locations
                //need the type of the param
                if(oplist_head->type.is_array == 1){
                    //Need to copy heads
                    fprintf(temp_file, "mov eax, [%s]\n", name_in_asm_op_param);
                    fprintf(temp_file, "mov [%s], eax\n", name_in_asm_ret_param);

                }
                else{
                    if(oplist_head->type.t.v.dt == INTEGER){
                        fprintf(temp_file, "mov ax, [%s]\n", name_in_asm_op_param);
                        fprintf(temp_file, "mov [%s], ax\n", name_in_asm_ret_param);

                    }
                    else if(oplist_head->type.t.v.dt == BOOLEAN){
                        fprintf(temp_file, "mov al, [%s]\n", name_in_asm_op_param);
                        fprintf(temp_file, "mov [%s], al\n", name_in_asm_ret_param);

                    }
                    else if(oplist_head->type.t.v.dt == REAL){
                        fprintf(temp_file, "mov eax, [%s]\n", name_in_asm_op_param);
                        fprintf(temp_file, "mov [%s], eax\n", name_in_asm_ret_param);


                    }

                }

                oplist_head = oplist_head->next;
                head_of_ret_list = head_of_ret_list->forward;
            }
            fprintf(temp_file, "pop eax\n");
           
        }
    }
    //verified till here..
    else{

        ASTNode *lvalueid = assign_node->left_child;
        //getting the name of the lvalueid
        //giving = lno
        var_hash_entry *lvalue_entry = find_var(lvalueid->data.str, assign_node->data.line_number, curr_st);
        int lno_of_dec_lvalueid = lvalue_entry->lno;

        char name_in_asm[30];
        strcpy(name_in_asm, name_in_asm_gen(lvalueid->data.str, lvalue_entry));
       
        //Assuming ashish result is in EBX/SYSWRITE.
       
        if(lvalueid->forward == NULL && lvalue_entry->type.is_array == 0){//a normal id
            fprintf(temp_file, "push ebx\n");
            type this_type = lvalue_entry->type;  
            if(this_type.t.v.dt == INTEGER){
                fprintf(temp_file, "mov [%s], bx\n", name_in_asm);
            }
            else if(this_type.t.v.dt == BOOLEAN){
                fprintf(temp_file, "mov [%s], bl\n", name_in_asm);

            }
            else if(this_type.t.v.dt == REAL){
                fprintf(temp_file, "mov ebx, [sys_fright]\n");
                fprintf(temp_file, "mov [%s], ebx\n", name_in_asm);
            }
            fprintf(temp_file, "pop ebx\n");
        }
       
        //verified.. till here
        else{//an arr elt
            //need to check if static or dynamic
            //after vound checking I will assign..
            ASTNode *lvalueindx = lvalueid->forward;
            type this_type = lvalue_entry->type;
            var_hash_entry *lvalue_indx_entry;
            if(lvalueindx != NULL) lvalue_indx_entry = find_var(lvalueindx->data.str, lvalueindx->data.line_number,curr_st);
            //getting value in ax
            //here ashish value is started to get preserved..
            fprintf(temp_file,"push eax\n");
            fprintf(temp_file,"xor eax, eax\n");
            fprintf(temp_file,"push ebx\n");
            fprintf(temp_file,"xor ebx, ebx\n");
            fprintf(temp_file,"xor edx, edx\n");
            fprintf(temp_file,"push ecx\n");
            fprintf(temp_file,"xor ecx, ecx\n");
            char name_in_asm_index[30];
            char string_line_no_index[30];
            int lno_indx;
            if(lvalue_indx_entry != NULL)
                lno_indx = lvalue_indx_entry->lno;
           
           
           
            //the case of assigning an array elt to another
            ASTNode *rhs_of_assignop = assign_node->right_child;
            //If the right child is of arr type
            var_hash_entry *rhs_entry = find_var(rhs_of_assignop->data.str, rhs_of_assignop->data.line_number, curr_st);
            if(lvalueindx == NULL){//its an identifier
                if(lvalue_entry->type.is_array == 1){//its an array
                    char name_in_asm_rhs_arr[30];
                    strcpy(name_in_asm_rhs_arr, name_in_asm_gen(rhs_of_assignop->data.str, rhs_entry));
               
                    //Do a same no of elts check..
                    //will bring in ax and bx
                    fprintf(temp_file, "push eax\npush ebx\npush ecx\nxor eax,eax\nxor ebx,ebx\n");
                    fprintf(temp_file,"mov ecx, [%s]\n", name_in_asm);
                    fprintf(temp_file,"mov ax , word[ecx]\n");

                    fprintf(temp_file,"mov ecx, [%s]\n", name_in_asm_rhs_arr);
                    fprintf(temp_file,"mov bx , word[ecx]\n");
                    fprintf(temp_file, "push edx\n xor edx,edx\n");
                    fprintf(temp_file, "mov edx,%d \npush edx\n",lvalueid->data.line_number);
                    fprintf(temp_file, "cmp ax, bx\n");
                    fprintf(temp_file, "jne abort2\n");
                    //New add
                    fprintf(temp_file, "pop edx\npop edx\n");
                    fprintf(temp_file, "pop ecx\npop ebx\npop eax\n");

                    fprintf(temp_file, "push eax\npush ebx\npush ecx\nxor eax,eax\nxor ebx,ebx\n");
                    fprintf(temp_file,"mov ecx, [%s]\n", name_in_asm);
                    fprintf(temp_file,"mov ax , word[ecx+2]\n");

                    fprintf(temp_file,"mov ecx, [%s]\n", name_in_asm_rhs_arr);
                    fprintf(temp_file,"mov bx , word[ecx+2]\n");
                    fprintf(temp_file, "push edx\n xor edx,edx\n");
                    fprintf(temp_file, "mov edx,%d \npush edx\n",lvalueid->data.line_number);
                    fprintf(temp_file, "cmp ax, bx\n");
                    fprintf(temp_file, "jne abort2\n");
                    //New add
                    fprintf(temp_file, "pop edx\npop edx\n");
                    fprintf(temp_file, "pop ecx\npop ebx\npop eax\n");

                    //Good to copy
                    fprintf(temp_file, "mov ebx, [%s]\n", name_in_asm_rhs_arr);
                    fprintf(temp_file, "mov [%s], ebx\n", name_in_asm);


                    fprintf(temp_file, "pop ecx\n");
                    fprintf(temp_file, "pop ebx\n");
                    fprintf(temp_file, "pop eax\n");
                    return;
                   
                }  
            }

            if(lvalue_indx_entry == NULL){//its a number
                //getting bound in bx (lower).
                bound_checker(1, lvalueindx->data.str, temp_file, name_in_asm, lvalueindx);
                //bound check is over
            }
            else{//its an id
                //line number if inside is an id..
                //doesnt matter if wrong value filled
                //used in ryt place
                //Value restoration is done over here..
                strcpy(name_in_asm_index, name_in_asm_gen(lvalueindx->data.str, lvalue_indx_entry));
   
                bound_checker(0, name_in_asm_index, temp_file, name_in_asm, lvalueindx);
            }
            //copying elt..only in case it is not A = B (arr to arr)
            //See I have returned on top.
            fprintf(temp_file, "xor eax, eax\n");
            //(elt - lo *size + base + 4)
            if(lvalue_indx_entry == NULL){//constant idx
                fprintf(temp_file, "mov ax, %s\n", lvalueindx->data.str);
            }
            else{
                fprintf(temp_file, "mov ax, [%s]\n", name_in_asm_index);
            }
            //to get right pos
            //ax must hold the value of the index either a integer const or the value of the variable index
            type type_of_arr = lvalue_entry->type;
            calc_offset_assign(name_in_asm, type_of_arr, temp_file);  
        }
    }
}

void input_gen(ASTNode *get_input_node, var_st *curr_scope, var_st *prev_scope, FILE *temp_file){
    //will be taking a get_value node
    //you cant input something like get_value(id[a]).. so no bound check..
    input_count++;
    ASTNode *id_node = get_input_node->left_child;
    var_hash_entry *id_entry = find_var(id_node->data.str, id_node->data.line_number, curr_scope);
    char name_id_in_asm[30];

    strcpy(name_id_in_asm, name_in_asm_gen(id_node->data.str, id_entry));

    if(id_entry->type.is_array == 1){//is an array variable
        fprintf(temp_file,"push eax\n");
        fprintf(temp_file,"push ebx\n");
        fprintf(temp_file, "push ecx\n");
        fprintf(temp_file, "push edx\n");
        fprintf(temp_file, "xor eax, eax\n");
        fprintf(temp_file, "xor ebx, ebx\n");
        fprintf(temp_file, "xor ecx, ecx\n");
        fprintf(temp_file, "xor edx, edx\n");
        
        //moving address
        fprintf(temp_file, "mov eax, [%s]\n", name_id_in_asm);
        //New add to get to the base of elts.
        //bx has higher indx
        fprintf(temp_file, "mov bx, [eax]\n");
        //xx has higher indx
        fprintf(temp_file, "mov cx, [eax + 2]\n");
        //new line
        fprintf(temp_file, "push edx\n");
        fprintf(temp_file,"mov dx , cx\n"); //dx has upper bound now;

        fprintf(temp_file, "add eax, 4\n");
        //bx - ax
        fprintf(temp_file, "sub cx, bx\n");
        fprintf(temp_file, "inc cx\n");
        fprintf(temp_file, "push eax\npush ebx\npush ecx\n");
        //parameters
        fprintf(temp_file, "push edx\n");
        fprintf(temp_file, "push ebx\n");
        fprintf(temp_file, "push ecx\n");
        if(id_entry->type.t.a.dt == INTEGER){
            fprintf(temp_file, "push integer_array_prompt\ncall printf\nadd esp,16\n");
        }
        else if(id_entry->type.t.a.dt == BOOLEAN){
            fprintf(temp_file, "push boolean_array_prompt\ncall printf\nadd esp,16\n");
        }
        else{
            fprintf(temp_file, "push real_array_prompt\ncall printf\nadd esp,16\n");
        }
        fprintf(temp_file, "pop ecx\n");
        fprintf(temp_file, "pop ebx\n");
        fprintf(temp_file, "pop eax\n");
        fprintf(temp_file, "pop edx\n");

        //using current line number as a label to aboid duplicacy
        fprintf(temp_file, "input_loop_%d:\n", input_count);
        fprintf(temp_file, "cmp dx, cx\n");
        fprintf(temp_file, "je input_loop_out_%d\n", input_count);
        //eax holds the address to scan in
        fprintf(temp_file,"push eax\npush ebx\npush ecx\n push edx\n");
       
        if(id_entry->type.t.a.dt == INTEGER){
  
            fprintf(temp_file, "push eax\n");
            fprintf(temp_file, "push scan_int\n");
           
        }


        else if(id_entry->type.t.a.dt == BOOLEAN){
    
            fprintf(temp_file, "push bool_input\n");
            fprintf(temp_file, "push scan_int\n");
        }


        else if(id_entry->type.t.a.dt == REAL){
    
            fprintf(temp_file, "push eax\n");
            fprintf(temp_file, "push scan_float\n");
        }
        fprintf(temp_file, "call scanf\n");
        fprintf(temp_file, "add esp, 8\n");

        //getting value back
       
        fprintf(temp_file,"pop edx\npop ecx\n pop ebx\npop eax\n");
        if(id_entry->type.t.a.dt == BOOLEAN){
            fprintf(temp_file, "push edx\nmov dx, word[bool_input]\nand dx, 0x0001\n");
            fprintf(temp_file, "mov [eax], dl\npop edx\n");

        }
        if(id_entry->type.t.a.dt == INTEGER)
        {
            fprintf(temp_file,"add eax ,2\n");
        }
        else if(id_entry->type.t.a.dt == BOOLEAN)
        {
            fprintf(temp_file,"add eax ,1\n");
        }
        else
        {
            fprintf(temp_file,"add eax ,4\n");
        }
        fprintf(temp_file,"inc dx\n");
        fprintf(temp_file, "jmp input_loop_%d\n", input_count);
        fprintf(temp_file, "input_loop_out_%d:\n", input_count);
        fprintf(temp_file, "pop edx\n");
        fprintf(temp_file, "pop ecx\n");
        fprintf(temp_file, "pop ebx\n");
        fprintf(temp_file, "pop eax\n");



    }
    else{//is a normal variable
        //scan_int db "%d",0
       
       
        if(id_entry->type.t.v.dt == INTEGER){
            fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
            fprintf(temp_file, "push integer_prompt\ncall printf\nadd esp,4\n");
            fprintf(temp_file, "pop edx\n");
            fprintf(temp_file, "pop ecx\n");
            fprintf(temp_file, "pop ebx\n");
            fprintf(temp_file, "pop eax\n");
            fprintf(temp_file, "push %s\n",name_id_in_asm);
            fprintf(temp_file, "push scan_int\n");    
        }
        //scan_bool
        else if(id_entry->type.t.v.dt == BOOLEAN){
            fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
            fprintf(temp_file, "push boolean_prompt\ncall printf\nadd esp,4\n");
            fprintf(temp_file, "pop edx\n");
            fprintf(temp_file, "pop ecx\n");
            fprintf(temp_file, "pop ebx\n");
            fprintf(temp_file, "pop eax\n");
            fprintf(temp_file, "push bool_input\n");
            fprintf(temp_file, "push scan_int\n");

        }
        //scan_real db "%f",0
        else if(id_entry->type.t.v.dt == REAL){
            fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
            fprintf(temp_file, "push real_prompt\ncall printf\nadd esp,4\n");
            fprintf(temp_file, "pop edx\n");
            fprintf(temp_file, "pop ecx\n");
            fprintf(temp_file, "pop ebx\n");
            fprintf(temp_file, "pop eax\n");
            fprintf(temp_file, "push %s\n",name_id_in_asm);
            fprintf(temp_file, "push scan_float\n");
        }
        fprintf(temp_file, "call scanf\n");
        fprintf(temp_file, "add esp, 8\n");

        //copying from bool var into
        if(id_entry->type.t.v.dt == BOOLEAN){
            fprintf(temp_file, "mov ax, word[bool_input]\nand ax, 0x0001\n");
            fprintf(temp_file, "mov [%s], al\n", name_id_in_asm);
        }
    }
    //fclose(temp_file);

}

void print_gen(ASTNode *print_node, var_st *curr_scope, var_st *prev_scope ,FILE *temp_file){
    ASTNode *id_node = print_node->left_child;
    var_hash_entry *id_entry = find_var(id_node->data.str, id_node->data.line_number, curr_scope);
    char name_id_in_asm[30];
    strcpy(name_id_in_asm, name_in_asm_gen(id_node->data.str, id_entry));
    print_count++;
    fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
    fprintf(temp_file, "push output_prompt\ncall printf\nadd esp,4\n");
    fprintf(temp_file, "pop edx\n");
    fprintf(temp_file, "pop ecx\n");
    fprintf(temp_file, "pop ebx\n");
    fprintf(temp_file, "pop eax\n");
    if(id_entry == NULL){//printing constant
        fprintf(temp_file, "push eax\n");
        if(id_node->data.is_terminal == 1 && id_node->data.enum_value == T_RNUM){//floating constant
            fprintf(temp_file,"mov eax, __float32__(%s)\n", id_node->data.str);
            //line 34-65 (in final_float.asm)
            fprintf(temp_file,"call float_printer\n");
        }
        //print_int db "%d",0
        else if(id_node->data.is_terminal == 1 && id_node->data.enum_value == T_NUM){//integer constant
            fprintf(temp_file, "xor eax, eax\n");
            fprintf(temp_file, "mov eax, %s\n", id_node->data.str);
            fprintf(temp_file, "push eax\n");
            fprintf(temp_file, "push print_int\n");
            fprintf(temp_file, "call printf\n");
            fprintf(temp_file, "add esp, 8\n");
        }
        else{//true or false constt..
            ////print_true db "TRUE",0
            if(strcmp(id_node->data.str, "true") == 0){
                fprintf(temp_file,"push print_true\n");
                fprintf(temp_file,"call printf\n");
                fprintf(temp_file,"add esp, 4\n");
            }
            ////print_false db "FALSE",0
            else if(strcmp(id_node->data.str, "false") == 0){
                fprintf(temp_file,"push print_false\n");
                fprintf(temp_file,"call printf\n");
                fprintf(temp_file,"add esp, 4\n");
            }

        }
        fprintf(temp_file, "pop eax\n");
    }
    else{//its a variable
        //1. real_var
        //2. Bool_var
        //3. int_var
        //4. entire arr
        //5. A[i] type  -->bound check
       
        if(id_entry->type.is_array == 1){//4 and 5
            fprintf(temp_file,"push eax\n");
            fprintf(temp_file, "push ebx\n");
            fprintf(temp_file, "push ecx\n");
            fprintf(temp_file, "push edx\n");
            fprintf(temp_file, "xor eax, eax\n");
            fprintf(temp_file, "xor ebx, ebx\n");
            fprintf(temp_file, "xor ecx, ecx\n");
            fprintf(temp_file, "xor edx, edx\n");
            if(id_entry->type.t.a.dt == REAL){

                //case 4 (a real array)
                if(id_node->forward == NULL){
                    //getting base address of arr
                    fprintf(temp_file,"mov eax, dword[%s]\n", name_id_in_asm);
                    fprintf(temp_file, "mov bx, word[eax]\n");
                    fprintf(temp_file, "mov cx, word[eax + 2]\n");
                    //base of elts
                    fprintf(temp_file, "add eax, 4\n");
                    //number of elts
                    fprintf(temp_file, "sub cx, bx\n");
                    fprintf(temp_file, "inc cx\n");

                    //start looping
                    fprintf(temp_file, "print_loop_%d:\n", print_count);
                    fprintf(temp_file, "cmp dx, cx\n");
                    fprintf(temp_file, "je print_loop_out_%d\n", print_count);
                    //saving the address of elt
                    fprintf(temp_file, "push eax\n");
                    //getting the val of elt in eax (as float_printer requires it in eax.)
                    fprintf(temp_file, "mov eax, dword[eax]\n");
                    fprintf(temp_file, "call float_printer_arr\n");
                    fprintf(temp_file, "pop eax\n");
                    fprintf(temp_file, "add eax, 4\n");
                    fprintf(temp_file, "inc dx\n");
                    fprintf(temp_file, "jmp print_loop_%d\n", print_count);
                    //final loop out var
                    fprintf(temp_file, "print_loop_out_%d:\n", print_count);
                    fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
                    fprintf(temp_file, "push newline\n");
                    fprintf(temp_file, "call printf\n");
                    fprintf(temp_file, "add esp, 4\n");
                    fprintf(temp_file, "pop edx\npop ecx\npop ebx\npop eax\n");

                }
                else{
                    //case 5
                    ASTNode *indx_of_arr = id_node->forward;
                    var_hash_entry *indx_entry = find_var(indx_of_arr->data.str, indx_of_arr->data.line_number, curr_scope);
                    char name_indx_in_asm[30];
                    strcpy(name_indx_in_asm, name_in_asm_gen(indx_of_arr->data.str, indx_entry));
                   
                    //eax will store the content..
                    if(indx_entry == NULL){
                        //bound check for the constant
                        bound_checker(1, indx_of_arr->data.str, temp_file, name_id_in_asm, id_node);    
                    }
                    else{
     
                        bound_checker(0, name_indx_in_asm, temp_file, name_indx_in_asm, id_node);

                    }

                    fprintf(temp_file , "push ecx\nxor ecx,ecx\n mov ecx,[%s]\n",name_id_in_asm);
                    fprintf(temp_file, "push ebx\npush eax\nxor ebx,ebx\nxor eax,eax\n");
                    if(indx_entry == NULL)
                        fprintf(temp_file,"mov ax,%s\n",indx_of_arr->data.str);
                    else
                        fprintf(temp_file, "mov ax, [%s]\n", name_indx_in_asm);
                    fprintf(temp_file, "mov bx ,[ecx]\nsub ax, bx\n");
                    fprintf(temp_file, "mov eax, [ecx + 4 + 4 * eax]\n");
                    fprintf(temp_file, "call float_printer\n");
                    fprintf(temp_file, "pop eax\npop ebx\npop ecx\n");
                   
                }


            }

            else if(id_entry->type.t.a.dt == BOOLEAN){
                //case 4
                if(id_node->forward == NULL){
                    fprintf(temp_file,"mov eax, dword[%s]\n", name_id_in_asm);
                    fprintf(temp_file, "mov bx, word[eax]\n");
                    fprintf(temp_file, "mov cx, word[eax + 2]\n");
                    //base of elts
                    fprintf(temp_file, "add eax, 4\n");
                    //number of elts
                    fprintf(temp_file, "sub cx, bx\n");
                    fprintf(temp_file, "inc cx\n");

                    //start looping
                    fprintf(temp_file, "print_loop_%d:\n", print_count);
                    fprintf(temp_file, "cmp dx, cx\n");
                    fprintf(temp_file, "je print_loop_out_%d\n", print_count);
                    //saving the address of elt
                    fprintf(temp_file, "push eax\n");
                    //getting the val of elt in eax (as float_printer requires it in eax.)
                    fprintf(temp_file, "mov al, byte[eax]\nand eax,0x00000001\n");
                    //pushing the val of the integer
                    fprintf(temp_file, "push ecx\npush edx\npush eax\npush ebx\n");

                    fprintf(temp_file, "cmp al, 1\n");
                    fprintf(temp_file, "je true_%d\n", print_count);
                    fprintf(temp_file, "cmp al, 0\n");
                    fprintf(temp_file, "je false_%d\n", print_count);
               
                    fprintf(temp_file, "true_%d:\n", print_count);
                    fprintf(temp_file,"push print_true_array\n");
                    fprintf(temp_file,"call printf\n");
                    fprintf(temp_file,"add esp, 4\n");
                    fprintf(temp_file, "pop ebx\npop eax\npop edx\npop ecx\npop eax\n");
                    fprintf(temp_file, "inc dx\nadd eax, 1\n");
                    fprintf(temp_file, "jmp print_loop_%d\n", print_count);

                    fprintf(temp_file, "false_%d:\n", print_count);
                    fprintf(temp_file,"push print_false_array\n");
                    fprintf(temp_file,"call printf\n");
                    fprintf(temp_file,"add esp, 4\n");
                    fprintf(temp_file, "pop ebx\npop eax\npop edx\npop ecx\npop eax\n");
                    fprintf(temp_file, "inc dx\nadd eax, 1\n");
                    fprintf(temp_file, "jmp print_loop_%d\n", print_count);

                    //final loop out var
                    fprintf(temp_file, "print_loop_out_%d:\n", print_count);
                    fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
                    fprintf(temp_file, "push newline\n");
                    fprintf(temp_file, "call printf\n");
                    fprintf(temp_file, "add esp, 4\n");
                    fprintf(temp_file, "pop edx\npop ecx\npop ebx\npop eax\n");

                   


                }

                //case 5
                else{
                    ASTNode *indx_of_arr = id_node->forward;
                    var_hash_entry *indx_entry = find_var(indx_of_arr->data.str, indx_of_arr->data.line_number, curr_scope);

                    char name_indx_in_asm[30];
                    strcpy(name_indx_in_asm, name_in_asm_gen(indx_of_arr->data.str, indx_entry));
                   
                    if(indx_entry == NULL){
                        bound_checker(1, indx_of_arr->data.str, temp_file, name_id_in_asm, id_node);
           
                    }
                    else{
                       
                        bound_checker(0, name_indx_in_asm, temp_file, name_id_in_asm, id_node);
                    }
                    fprintf(temp_file , "push ecx\nxor ecx,ecx\n mov ecx,[%s]\n",name_id_in_asm);
                    fprintf(temp_file, "push ebx\npush eax\nxor ebx,ebx\nxor eax,eax\n");

                    if(indx_entry == NULL)
                        fprintf(temp_file,"mov al,%s\n",indx_of_arr->data.str);
                    else
                        fprintf(temp_file, "mov al, [%s]\n", name_indx_in_asm);


                    fprintf(temp_file, "mov bx ,[ecx]\nsub ax, bx\n");
                    fprintf(temp_file, "mov al, byte[ecx + 4 + eax]\n");
                    fprintf(temp_file, "push ecx\npush edx\npush ebx\npush eax\n");
                    fprintf(temp_file, "and eax,0x00000001\n");
                   
                    fprintf(temp_file, "cmp al, 1\n");
                    fprintf(temp_file, "je true_%d\n", print_count);
                    fprintf(temp_file, "cmp al, 0\n");
                    fprintf(temp_file, "je false_%d\n", print_count);
                   
                    fprintf(temp_file, "true_%d:\n", print_count);
                    fprintf(temp_file,"push print_true\n");
                    fprintf(temp_file,"call printf\n");
                    fprintf(temp_file,"add esp, 4\n");
                    fprintf(temp_file, "pop eax\npop ebx\npop edx\npop ecx\n");
                    fprintf(temp_file, "jmp outer_%d\n", print_count);

                    fprintf(temp_file, "false_%d:\n", print_count);
                    fprintf(temp_file,"push print_false\n");
                    fprintf(temp_file,"call printf\n");
                    fprintf(temp_file,"add esp, 4\n");
                    fprintf(temp_file, "pop eax\npop ebx\npop edx\npop ecx\n");
                    fprintf(temp_file, "jmp outer_%d\n", print_count);

                    fprintf(temp_file, "outer_%d:\n", print_count);
                }


            }
            else if(id_entry->type.t.a.dt == INTEGER){
                //case 4
                if(id_node->forward == NULL){
                    fprintf(temp_file,"mov eax, dword[%s]\n", name_id_in_asm);
                    fprintf(temp_file, "mov bx, word[eax]\n");
                    fprintf(temp_file, "mov cx, word[eax + 2]\n");
                    //base of elts
                    fprintf(temp_file, "add eax, 4\n");
                    //number of elts
                    fprintf(temp_file, "sub cx, bx\n");
                    fprintf(temp_file, "inc cx\n");

                    //start looping
                    fprintf(temp_file, "print_loop_%d:\n", print_count);
                    fprintf(temp_file, "cmp dx, cx\n");
                    fprintf(temp_file, "je print_loop_out_%d\n", print_count);
                    //saving the address of elt
                    fprintf(temp_file, "push eax\n");
                    //getting the val of elt in eax (as float_printer requires it in eax.)
                    fprintf(temp_file, "mov ax, word[eax]\nand eax,0x0000FFFF\n");
                    //pushing the val of the integer
                    fprintf(temp_file, "push ecx\npush edx\n");

                    neg_printer(temp_file,1);

                    fprintf(temp_file, "pop edx\npop ecx\n");
                   
                    fprintf(temp_file, "pop eax\n");
                    fprintf(temp_file, "add eax, 2\n");
                    fprintf(temp_file, "inc dx\n");
                    fprintf(temp_file, "jmp print_loop_%d\n", print_count);
                    //final loop out var
                    fprintf(temp_file, "print_loop_out_%d:\n", print_count);
                    fprintf(temp_file, "push eax\npush ebx\npush ecx\npush edx\n");
                    fprintf(temp_file, "push newline\n");
                    fprintf(temp_file, "call printf\n");
                    fprintf(temp_file, "add esp, 4\n");
                    fprintf(temp_file, "pop edx\npop ecx\npop ebx\npop eax\n");


                }

                //case 5
                else{
                    ASTNode *indx_of_arr = id_node->forward;
                    var_hash_entry *indx_entry = find_var(indx_of_arr->data.str, indx_of_arr->data.line_number, curr_scope);
                    char name_indx_in_asm[30];
                    strcpy(name_indx_in_asm, name_in_asm_gen(indx_of_arr->data.str, indx_entry));
                   
                    if(indx_entry == NULL){
                        bound_checker(1, indx_of_arr->data.str, temp_file, name_id_in_asm, id_node);
           
                    }
                    else{
                        bound_checker(0, name_indx_in_asm, temp_file, name_id_in_asm, id_node);
                    }
                   
                    //eax will store the content..
                    fprintf(temp_file , "push ecx\nxor ecx,ecx\n mov ecx,[%s]\n",name_id_in_asm);
                    fprintf(temp_file, "push ebx\npush eax\nxor ebx,ebx\nxor eax,eax\n");
                    if(indx_entry == NULL)
                        fprintf(temp_file,"mov ax,%s\n",indx_of_arr->data.str);
                    else
                        fprintf(temp_file, "mov ax, [%s]\n", name_indx_in_asm);
                    fprintf(temp_file, "mov bx ,[ecx]\nsub ax, bx\n");
                    fprintf(temp_file, "mov ax, word[ecx + 4 + 2 * eax]\n");
                                       
                    fprintf(temp_file, "push ecx\npush edx\n");// deleted push eax;
                    neg_printer(temp_file,0);
                    fprintf(temp_file, "pop edx\npop ecx\n");

                    fprintf(temp_file, "pop eax\npop ebx\npop ecx\n");
                   
                }
            }
            fprintf(temp_file, "pop edx\n");
            fprintf(temp_file, "pop ecx\n");
            fprintf(temp_file, "pop ebx\n");
            fprintf(temp_file,"pop eax\n");

        }
        else{//1 2 and 3
            fprintf(temp_file, "push eax\n");
            fprintf(temp_file, "xor eax, eax\n");
            //1. real_var
            //
            if(id_entry->type.t.v.dt == REAL){
                fprintf(temp_file, "mov eax, [%s]\n", name_id_in_asm);
                fprintf(temp_file, "call float_printer\n");


            }
            //2. Boolean var
            else if(id_entry->type.t.v.dt == BOOLEAN){
                    fprintf(temp_file,"mov ax,[%s]\n",name_id_in_asm);

                    fprintf(temp_file, "push ecx\npush edx\npush ebx\npush eax\n");
                    fprintf(temp_file, "and eax,0x0001\n");
                   
                    fprintf(temp_file, "cmp al, 1\n");
                    fprintf(temp_file, "je true_%d\n", print_count);
                    fprintf(temp_file, "cmp al, 0\n");
                    fprintf(temp_file, "je false_%d\n", print_count);
                   
                    fprintf(temp_file, "true_%d:\n", print_count);
                    fprintf(temp_file,"push print_true\n");
                    fprintf(temp_file,"call printf\n");
                    fprintf(temp_file,"add esp, 4\n");
                    fprintf(temp_file, "pop eax\npop ebx\npop edx\npop ecx\n");
                    fprintf(temp_file, "jmp outer_%d\n", print_count);

                    fprintf(temp_file, "false_%d:\n", print_count);
                    fprintf(temp_file,"push print_false\n");
                    fprintf(temp_file,"call printf\n");
                    fprintf(temp_file,"add esp, 4\n");
                    fprintf(temp_file, "pop eax\npop ebx\npop edx\npop ecx\n");
                    fprintf(temp_file, "jmp outer_%d\n", print_count);

                    fprintf(temp_file, "outer_%d:\n", print_count);
            }
            //3. Integer var
            //print_int db "%d",0
            else if(id_entry->type.t.v.dt == INTEGER){

                fprintf(temp_file, "xor eax, eax\nmov ax, [%s]\n", name_id_in_asm);
                neg_printer(temp_file,0);
            }
            fprintf(temp_file, "pop eax\n");

        }

    }
}

void neg_printer(FILE *temp_file, int array_flag)
{
    fprintf(temp_file, "cmp ax, 0\n");
    fprintf(temp_file, "jl inverter_%d\n", inverter_count);
    fprintf(temp_file, "jmp inverter_out_%d\n", inverter_count);
   
    fprintf(temp_file, "inverter_%d:\n", inverter_count);
    fprintf(temp_file, "neg ax\npush eax\n");
    fprintf(temp_file, "push nega\n");
    fprintf(temp_file, "call printf\n");
    fprintf(temp_file, "add esp, 4\npop eax\n");


    fprintf(temp_file, "inverter_out_%d:\n", inverter_count);
    fprintf(temp_file, "push eax\n");
    if(array_flag) fprintf(temp_file, "push print_int_array\n");
    else fprintf(temp_file, "push print_int\n");
    fprintf(temp_file, "call printf\n");
    fprintf(temp_file, "add esp ,8\n");
    inverter_count ++;
}


//ANIRUDH END

//ASHISH START
char *lexeme_generator(char * lexeme , int lno, var_st *table)
{
    var_hash_entry *entry = find_var(lexeme,lno,table);
   
    char temp_buffer[20]; // i am taking this as a bound for variable name + line_number +1 .. See if it does not fit.
    sprintf(temp_buffer,"%d",entry->lno);
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
        fprintf(f,"mov dword[sys_fleft], __float32__(%s)\n",ast_node->data.str);
        else fprintf(f,"mov dword[sys_fright], __float32__(%s)\n",ast_node->data.str);
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
            //Dynamic bound check to be done here.
            fprintf(f,"push eax\npush ebx\npush ecx\npush edx\n");
            char arr_name[100];
            char arr_id[100];
            strcpy(arr_name,lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
            if(ast_node->forward->data.enum_value == T_ID )
            {
                strcpy(arr_id,lexeme_generator(ast_node->forward->data.str,ast_node->forward->data.line_number,table));
                bound_checker(0,arr_id,f,arr_name,ast_node);
            }
            else
            {
                bound_checker(1,ast_node->forward->data.str,f,arr_name,ast_node);
            }
            fprintf(f,"pop edx\npop ecx\npop ebx\npop eax\n");
            if(ast_node->forward != NULL && ast_node->forward->data.enum_value == T_ID)
            {
                fprintf(f,"push ecx\nxor eax,eax\nxor ebx,ebx\n");
                fprintf(f, "mov ax , [%s]\n",lexeme_generator(ast_node->forward->data.str,ast_node->forward->data.line_number,table));
                fprintf(f,"xor ecx,ecx\n mov ecx ,[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                fprintf(f, "mov bx, word[ecx]\n sub ax,bx\n");
                fprintf(f,"pop ecx\n");
                fprintf(f, "push ecx\nxor ecx, ecx\n");
                if(temp->type.t.a.dt == INTEGER)
                    fprintf(f, "mov cx, 2\n");
                else if(temp->type.t.a.dt == REAL)
                    fprintf(f, "mov cx, 4\n");
                else if(temp->type.t.a.dt == BOOLEAN)
                    fprintf(f, "mov cx, 1\n");

                fprintf(f, "push edx\nmul cx\npop edx\n");
                fprintf(f, "mov edx, [%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                fprintf(f, "add edx, eax\n");
                fprintf(f, "add edx, 4\n");
                fprintf(f, "pop ecx\n");

                if(temp->type.t.a.dt == INTEGER)
                {
                    //remember things happen in 16 bit regs
                    type_flag = INTEGER;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov ax, word[edx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bx, word[edx]\n");
                    }
                   
                }
                else if(temp->type.t.a.dt == BOOLEAN)
                {
                    type_flag = BOOLEAN;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov al, byte[edx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bl, byte[edx]\n");
                    }
                }
                else
                {
                    type_flag = REAL;
                    //things happen in 32 bit regs
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"mov ecx , dword[edx]\n");
                        fprintf(f,"xor eax,eax\n mov dword[sys_fleft], ecx\n");
                    }                
                    else
                    {
                        fprintf(f,"mov ecx , dword[edx]\n");
                        fprintf(f,"xor ebx,ebx\n mov dword[sys_fright], ecx\n");
                    }
                }              
            }
            else if(ast_node->forward != NULL && ast_node->forward->data.enum_value == T_NUM)
            {
                fprintf(f,"push ecx\nxor eax,eax\nxor ebx,ebx\n");
                fprintf(f, "mov ax , %s\n",ast_node->forward->data.str);
                fprintf(f,"xor ecx,ecx\n mov ecx ,[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                fprintf(f, "mov bx, word[ecx]\n sub ax,bx\n");
                fprintf(f,"pop ecx\n");
                fprintf(f, "push ecx\nxor ecx, ecx\n");
                if(temp->type.t.a.dt == INTEGER)
                    fprintf(f, "mov cx, 2\n");
                else if(temp->type.t.a.dt == REAL)
                    fprintf(f, "mov cx, 4\n");
                else if(temp->type.t.a.dt == BOOLEAN)
                    fprintf(f, "mov cx, 1\n");

                fprintf(f, "push edx\nmul cx\npop edx\n");
                fprintf(f, "mov edx, [%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                fprintf(f, "add edx, eax\n");
                fprintf(f, "add edx, 4\n");
                fprintf(f, "pop ecx\n");

                if(temp->type.t.a.dt == INTEGER)
                {
                    //remember things happen in 16 bit regs
                    type_flag = INTEGER;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov ax, word[edx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bx, word[edx]\n");
                    }
                   
                }
                else if(temp->type.t.a.dt == BOOLEAN)
                {
                    type_flag = BOOLEAN;
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"xor eax,eax\n mov al, byte[edx]\n");
                    }                    
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov bl, byte[edx]\n");
                    }
                }
                else
                {
                    type_flag = REAL;
                    //things happen in 32 bit regs
                    if(is_left_child(ast_node))
                    {
                        fprintf(f,"mov ecx , dword[edx]\n");
                        fprintf(f,"xor eax,eax\n mov dword[sys_fleft], ecx\n");
                    }                
                    else
                    {
                        fprintf(f,"mov ecx , dword[edx]\n");
                        fprintf(f,"xor ebx,ebx\n mov dword[sys_fright], ecx\n");
                    }
                }
           
            }
            else
            {
                //do nothing.
            }
           
        }
        else
        {
            if(temp->type.t.v.dt == INTEGER)
            {
                type_flag = INTEGER;
                if(is_left_child(ast_node))
                {
                    fprintf(f,"xor eax,eax\nmov ax, word[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov bx, word[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));    
                }
               
            }
            else if(temp->type.t.v.dt == BOOLEAN)
            {
                type_flag = BOOLEAN;
                if(is_left_child(ast_node))
                {
                    fprintf(f,"xor eax,eax\nmov al, byte[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov bl, byte[%s]\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));    
                }
            }
            else
            {
                type_flag = REAL;
                if(is_left_child(ast_node))
                {
                    fprintf(f,"xor eax,eax\nmov eax, dword[%s]\nmov dword[sys_fleft] ,eax\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov ebx, dword[%s]\nmov dword[sys_fright], ebx\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number,table));    
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
                if(!is_left_child(ast_node)) fprintf(f,"mov bx , ax\n");
            }
            else
            {
                fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfaddp\n");
                if(is_left_child(ast_node)) fprintf(f,"fstp dword[sys_fleft]\n");
                else fprintf(f,"fstp dword[sys_fright]\n");
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
                if(!is_left_child(ast_node)) fprintf(f,"mov bx , ax\n");
            }
            else
            {
                fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfsubp st1,st0\n");
                if(is_left_child(ast_node)) fprintf(f,"fstp dword[sys_fleft]\n");
                else fprintf(f,"fstp dword[sys_fright]\n");
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
                fprintf(f,"fstp dword[sys_fleft]\n");
            }
        }
    }
    if(operator == 3)
    {
        if(type_flag == BOOLEAN)
        {
            fprintf(f,"and al , bl\n");
            if(!is_left_child(ast_node)) fprintf(f,"mov bl , al\n");
        }
    }
    if(operator == 4)
    {
        if(type_flag == BOOLEAN)
        {
            fprintf(f,"or al , bl\n");
            if(!is_left_child(ast_node)) fprintf(f,"mov bl , al\n");
        }
    }
    if(operator == 5)
    {
        //LT
        cmp_count++;
        if(type_flag == INTEGER)
        {
            fprintf(f,"cmp ax , bx\n");
            if(is_left_child(ast_node)) fprintf(f,"jl sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jl sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\n");
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nmov cx,ax\nand ax, 0x4000\ncmp ax,0x0\njne sys_false_ax%d\nmov ax,cx\nand ax,0x0100\ncmp ax,0x0\nje sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nmov cx,ax\nand ax, 0x4000\ncmp ax,0x0\njne sys_false_bx%d\nmov ax,cx\nand ax,0x0100\ncmp ax,0x0\nje sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count,cmp_count);
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
            if(is_left_child(ast_node)) fprintf(f,"jle sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jle sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\n");
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njnz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njnz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
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
            if(is_left_child(ast_node)) fprintf(f,"jg sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jg sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\n");
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nmov cx,ax\nand ax, 0x4000\ncmp ax,0x0\njne sys_false_ax%d\nmov ax,cx\nand ax,0x0100\ncmp ax,0x0\njne sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nmov cx,ax\nand ax, 0x4000\ncmp ax,0x0\njne sys_false_bx%d\nmov ax,cx\nand ax,0x0100\ncmp ax,0x0\njne sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count,cmp_count);
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
            if(is_left_child(ast_node)) fprintf(f,"jge sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jge sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\n");
             if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nmov cx,ax\nand ax, 0x4000\ncmp ax,0x0\njne sys_true_ax%d\nmov ax,cx\nand ax,0x0100\ncmp ax,0x0\njne sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nmov cx,ax\nand ax, 0x4000\ncmp ax,0x0\njne sys_true_bx%d\nmov ax,cx\nand ax,0x0100\ncmp ax,0x0\njne sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count,cmp_count);
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
            if(is_left_child(ast_node)) fprintf(f,"je sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"je sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\n");
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x4000\ncmp ax,0x0\njz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x4000\ncmp ax,0x0\njz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
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
            if(is_left_child(ast_node)) fprintf(f,"jne sys_true_ax%d\njmp sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"jne sys_true_bx%d\njmp sys_false_bx%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_ax%d: xor eax,eax\nmov al, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_ax%d: xor eax,eax\nmov al, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_true_bx%d: xor ebx,ebx\nmov bl, 0x1\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_false_bx%d: xor ebx,ebx\nmov bl, 0x0\njmp sys_cmp_exit%d\n",cmp_count,cmp_count);
            fprintf(f, "sys_cmp_exit%d:",cmp_count);
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\n");
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x4000\ncmp ax,0x0\njnz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x4000\ncmp ax,0x0\njnz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
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
            if(!is_left_child(ast_node)) fprintf(f,"mov bx , ax\n");
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfmulp st1,st0\n");
            if(is_left_child(ast_node)) fprintf(f,"fstp dword[sys_fleft]\n");
            else fprintf(f,"fstp dword[sys_fright]\n");
        }
    }
    if(operator == 12)
    {
        //DIV
        if(type_flag == INTEGER)
        {
            fprintf(f,"push edx\ncwd\nidiv bx\npop edx\n");
            if(!is_left_child(ast_node)) fprintf(f,"mov bx , ax\n");
        }
        else
        {
            fprintf(f, "fld dword[sys_fleft]\nfld dword[sys_fright]\nfdivp st1,st0\n");
            if(is_left_child(ast_node)) fprintf(f,"fstp dword[sys_fleft]\n");
            else fprintf(f,"fstp dword[sys_fright]\n");
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
        if (ast_node -> right_child-> data.enum_value == T_ID || ast_node -> right_child-> data.enum_value == T_NUM ||ast_node -> right_child-> data.enum_value == T_RNUM ||ast_node -> right_child-> data.enum_value == T_TRUE ||ast_node -> right_child-> data.enum_value == T_FALSE)
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
        make_code(ast_node->right_child,table,f);//
        fprintf(f,"push ebx\n push dword[sys_fright]\n");
        make_code(ast_node->left_child,table,f);
        fprintf(f,"pop dword[sys_fright]\n pop ebx\n");
        expr_code_writer(ast_node,table,f,0,operator);
    }
    else if (flag == 1)
    {
        make_code(ast_node->right_child,table,f);
        fprintf(f,"push ebx\n push dword[sys_fright]\n");
        terminals_handler(ast_node->left_child,table,f);
        fprintf(f,"pop dword[sys_fright]\n pop ebx\n");
        expr_code_writer(ast_node,table,f,1,operator);
    }
    else if (flag == 2)
    {
        terminals_handler(ast_node->right_child,table,f);
        fprintf(f,"push ebx\n push dword[sys_fright]\n");
        make_code(ast_node->left_child,table,f);
        fprintf(f,"pop dword[sys_fright]\n pop ebx\n");
        expr_code_writer(ast_node,table,f,2,operator);
    }
    else if (flag == 3)
    {
        terminals_handler(ast_node->right_child,table,f);
        fprintf(f,"push ebx\n push dword[sys_fright]\n");
        terminals_handler(ast_node->left_child,table,f);
        fprintf(f,"pop dword[sys_fright]\n pop ebx\n");
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

//ASHISH END


//combining start..
var_st *statements_gen(ASTNode *stmts, var_st *curr, var_st* prev, FILE *f1){
    ASTNode *statement;
    if(stmts->parent->data.is_terminal == 1 && (stmts->parent->data.enum_value == T_MODULE || stmts->parent->data.enum_value == T_PROGRAM)){
        statement = stmts->left_child;
    }
    else{
        statement = stmts;
    }
    while(statement != NULL){
        if(statement->data.is_terminal == 1 && statement->data.enum_value == T_FOR){
            if(prev == NULL){
                for_gen(statement, curr->child, NULL, f1);
                prev = curr->child;
            }
            else{
                for_gen(statement, prev->next, NULL, f1);
                prev = prev->next;
            }
            fprintf(f1, "\n");
        }
        else if(statement->data.is_terminal == 1 && statement->data.enum_value == T_WHILE){
            if(prev == NULL){
                while_gen(statement, curr->child, NULL, f1);
                prev = curr->child;
            }
            else{
                while_gen(statement, prev->next, NULL, f1);
                prev = prev->next;
            }
            fprintf(f1, "\n");
        }
        else if(statement->data.is_terminal == 1 && statement->data.enum_value == T_SWITCH){
            if(prev == NULL){
                switch_gen(statement, curr->child, NULL, f1);
                prev = curr->child;
            }
            else{
                switch_gen(statement, prev->next, NULL, f1);
                prev = prev->next;
            }
            fprintf(f1, "\n");
        }
        else if(statement->data.is_terminal == 1 && statement->data.enum_value == T_GET_VALUE){
            input_gen(statement, curr, prev, f1);
            fprintf(f1, "\n");
        }
        else if(statement->data.is_terminal == 1 && statement->data.enum_value == T_PRINT){
            print_gen(statement, curr, prev, f1);
            fprintf(f1, "\n");
        }
        else if(statement->data.is_terminal == 0 && statement->data.enum_value == NT_assignmentStmt){

            ASTNode *assign_op = statement->left_child;
            var_hash_entry *temp = find_var(assign_op->left_child->data.str,assign_op->left_child->data.line_number,curr);
            if(!(temp->type.is_array && assign_op->left_child->forward == NULL))
            {make_code(assign_op->right_child, curr, f1);}

            assignment_stmt(assign_op, curr, prev, f1);
            fprintf(f1,"\n");
        }
        else if(statement->data.is_terminal == 0 && statement->data.enum_value == NT_moduleReuseStmt){
            // I am on moduleResuseStmt node
            ASTNode *assign_op = statement->left_child;
            ASTNode *module_name_node = assign_op->right_child;

            func_hash_entry *module_entry = find_func_hash(module_name_node->data.str);
    
            module_input_copy(module_entry, statement, curr, prev, f1);
           
            fprintf(f1, "call %s\n", module_name_node->data.str);
            //op param copy
            assignment_stmt(assign_op, curr, prev, f1);

            fprintf(f1, "\n");
        }
        else if(statement->data.is_terminal == 1 && statement->data.enum_value == T_DECLARE){
            //only in case of array call
            ASTNode *id_list_node = statement->left_child;
            ASTNode *datatype_node = id_list_node->forward;
            if(datatype_node->data.is_terminal == 1 && datatype_node->data.enum_value == T_ARRAY){
                declaration_arr(statement, curr, prev, f1);
            }
            fprintf(f1, "\n");
        }
        if(statement->forward == NULL && (statement->parent->data.enum_value == T_TRUE || statement->parent->data.enum_value == T_FALSE || statement->parent->data.enum_value == T_NUM || statement->parent->data.enum_value == T_DEFAULT)  && statement->parent->parent != NULL && statement->parent->parent->data.enum_value == T_SWITCH && statement->parent->data.is_terminal == 1)
            return prev;
        statement = statement->forward;
    }
    return NULL;
}

//top level node its not the id node it expects modules node
void module_body_gen(ASTNode *modules_node, FILE *f1){
    ASTNode *moduleid_node = modules_node->left_child;
    func_hash_entry *module_entry = find_func_hash(moduleid_node->data.str);
    ASTNode *input_list = moduleid_node->forward;
    ASTNode *ret = input_list->forward;
    ASTNode *stmts_in_module =  ret->forward;
    fprintf(f1, "%s:\n", modules_node->left_child->data.str);
    statements_gen(stmts_in_module, module_entry->child, NULL, f1);
    fprintf(f1, "ret\n");

}

void other_modules_gen(ASTNode *othermodules, FILE *f1){
    ASTNode *module = othermodules->left_child;
    while(module != NULL){
        module_body_gen(module, f1);
        module = module->forward;
    }
}

//driver_program_gen
void driverprogram_gen(ASTNode *driver_program, FILE *f1){
    fprintf(f1, "main:\nenter 0,0\n");
    fprintf(f1,"sub esp, 1000\n");
    fprintf(f1,"mov esi,esp\n");
    ASTNode *stmts = driver_program->left_child;
    func_hash_entry *main_entry = find_func_hash("driver");
    var_st *main_st = main_entry->child;
   
    statements_gen(stmts, main_st, NULL, f1);
    fprintf(f1, "jmp exiter\n");
}

void float_printer(FILE *f1){
    //check it out..
    fprintf(f1, "float_printer:\n");
    fprintf(f1, "push edx\npush ecx\n");
    fprintf(f1, "push eax\n");
    fprintf(f1, "and eax, 7\n");
    fprintf(f1, "shl eax, 29\n");
    fprintf(f1, "mov edx, eax\n");
    fprintf(f1, "pop eax\n");
   
    fprintf(f1, "push eax\n");
    fprintf(f1, "and eax, 2147483648\n");
    fprintf(f1, "mov ecx, eax\n");
    fprintf(f1, "pop eax\n");
   
    fprintf(f1, "push eax\n");
    fprintf(f1, "shr eax, 23\n");
    fprintf(f1, "and eax, 255\n");
    fprintf(f1, "add eax, 896\n");
    fprintf(f1, "shl eax, 20\n");
    fprintf(f1, "add ecx, eax\n");
    fprintf(f1, "pop eax\n");

    fprintf(f1, "shr eax, 3\n");
    fprintf(f1, "and eax, 1048575\n");
    fprintf(f1, "add ecx, eax\n");  

    //ecx:edx//
    fprintf(f1, "push ecx\npush edx\n");
    fprintf(f1, "and ecx, 0x80000000\n");
    fprintf(f1, "cmp ecx, 0x80000000\n");
    fprintf(f1, "pop edx\npop ecx\n");
    fprintf(f1, "je float_inverter\n");
    fprintf(f1, "jmp float_inverter_out\n");


    fprintf(f1, "float_inverter:\n");
    fprintf(f1, "xor ecx, 0x80000000\n");
    fprintf(f1 , "push ecx\npush edx\n");
    fprintf(f1, "push nega\n");
    fprintf(f1, "call printf\n");
    fprintf(f1, "add esp, 4\n");
    fprintf(f1 , "pop edx\npop ecx\n");
   


    fprintf(f1, "float_inverter_out:\n");
    fprintf(f1, "push ecx\n");
    fprintf(f1, "push edx\n");

    fprintf(f1, "push print_float\n");
    fprintf(f1, "call printf\n");
    fprintf(f1, "add esp, 12\n");
    fprintf(f1, "pop ecx\npop edx\n");

    fprintf(f1, "ret\n");
}

void float_printer_arr(FILE *f1){
    //check it out..
    fprintf(f1, "float_printer_arr:\n");
    fprintf(f1, "push edx\npush ecx\n");
    fprintf(f1, "push eax\n");
    fprintf(f1, "and eax, 7\n");
    fprintf(f1, "shl eax, 29\n");
    fprintf(f1, "mov edx, eax\n");
    fprintf(f1, "pop eax\n");
   
    fprintf(f1, "push eax\n");
    fprintf(f1, "and eax, 2147483648\n");
    fprintf(f1, "mov ecx, eax\n");
    fprintf(f1, "pop eax\n");
   
    fprintf(f1, "push eax\n");
    fprintf(f1, "shr eax, 23\n");
    fprintf(f1, "and eax, 255\n");
    fprintf(f1, "add eax, 896\n");
    fprintf(f1, "shl eax, 20\n");
    fprintf(f1, "add ecx, eax\n");
    fprintf(f1, "pop eax\n");

    fprintf(f1, "shr eax, 3\n");
    fprintf(f1, "and eax, 1048575\n");
    fprintf(f1, "add ecx, eax\n");  

    //ecx:edx//
    fprintf(f1, "push ecx\npush edx\n");
    fprintf(f1, "and ecx, 0x80000000\n");
    fprintf(f1, "cmp ecx, 0x80000000\n");
    fprintf(f1, "pop edx\npop ecx\n");
    fprintf(f1, "je float_inverter_arr\n");
    fprintf(f1, "jmp float_inverter_out_arr\n");


    fprintf(f1, "float_inverter_arr:\n");
    fprintf(f1, "xor ecx, 0x80000000\n");
    fprintf(f1 , "push ecx\npush edx\n");
    fprintf(f1, "push nega\n");
    fprintf(f1, "call printf\n");
    fprintf(f1, "add esp, 4\n");
    fprintf(f1 , "pop edx\npop ecx\n");
   


    fprintf(f1, "float_inverter_out_arr:\n");
    fprintf(f1, "push ecx\n");
    fprintf(f1, "push edx\n");

    fprintf(f1, "push print_float_array\n");
    fprintf(f1, "call printf\n");
    fprintf(f1, "add esp, 12\n");
    fprintf(f1, "pop ecx\npop edx\n");
    fprintf(f1, "ret\n");
}

void entire_code_gen(ASTNode *moduledec, FILE *f1){
    ASTNode *othermodule1 = moduledec->forward;
    ASTNode *driver = othermodule1->forward;
    ASTNode *othermodule2 = driver->forward;
    other_modules_gen(othermodule1, f1);
   
    driverprogram_gen(driver, f1);
    other_modules_gen(othermodule2, f1);
    //float printer function..
    float_printer(f1);
    float_printer_arr(f1);

   
    fprintf(f1, "exiter:add esp,1000\n");
    fprintf(f1, "leave\n");
    fprintf(f1, "mov eax, 1\n");
    fprintf(f1, "xor ebx, ebx\n");
    fprintf(f1, "int 0x80\n\n");


    fprintf(f1, "abort1:\n");

    fprintf(f1,"push msg1_abort\n");
    fprintf(f1,"call printf\n add esp,12\n");
    fprintf(f1, "jmp exiter\n");

    fprintf(f1, "abort2:\n");

    fprintf(f1,"push msg2_abort\n");
    fprintf(f1,"call printf\n add esp,12\n");
    fprintf(f1, "jmp exiter\n");


}
