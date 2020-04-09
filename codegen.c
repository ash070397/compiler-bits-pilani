#include"codegen.h"

datatype type_flag; 
//type_flag update.
int cmp_count = 0;
//have to make it global as it gets locally chutiyapa
char temp_lexeme[50];

void declaration_maker(FILE *f1)
{
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
    fprintf(f1,"section .txt\n");
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
        fprintf(f,"mov dword[sys_fleft], __float32__(%s)\n",ast_node->data.str); // not writing DWORD as it is implicit.
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
                        fprintf(f,"xor eax,eax\n mov dword[sys_fleft], ecx\n");
                    }                 
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov dword[sys_fright], ecx\n");
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
                        fprintf(f,"xor eax,eax\n mov dword[sys_fleft], ecx\n");
                    }                 
                    else
                    {
                        fprintf(f,"xor ebx,ebx\n mov dword[sys_fright], ecx\n");
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
                    fprintf(f,"xor eax,eax\nmov eax, dword[%s]\nmov dword[sys_fleft] eax\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));
                }
                else
                {
                    fprintf(f,"xor ebx,ebx\nmov ebx, dword[%s]\nmov dword[sys_fright] ebx\n",lexeme_generator(ast_node->data.str,ast_node->data.line_number));    
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
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njz sys_true_ax%d\njnz sys_false_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njz sys_true_bx%d\njnz sys_false_bx%d\n",cmp_count,cmp_count);
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
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njz sys_false_ax%d\njnz sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njz sys_false_bx%d\njnz sys_true_bx%d\n",cmp_count,cmp_count);
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
            if(is_left_child(ast_node))fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njz sys_false_ax%d\njmp sys_true_ax%d\n",cmp_count,cmp_count);
            else fprintf(f,"fcompp\nxor eax,eax\nfstsw ax\nand ax,0x0100\ncmp ax,0x0\njz sys_false_bx%d\njmp sys_true_bx%d\n",cmp_count,cmp_count);
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
            fprintf(f,"xchg ax,bx\n idiv ax , bx\n");
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
        fprintf(f,"push ebx\n");
        make_code(ast_node->left_child,table,f);
        fprintf(f,"pop ebx\n");
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