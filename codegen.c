#include"codegen.h"

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
                itoa(temp->lno,temp_buffer,10);
                strcpy(temp_lexeme,temp->lexeme);
                strcat(temp_lexeme,"_");
                strcat(temp_lexeme,temp_buffer);
                fprintf(f,"%s: resb %d \n",temp_lexeme,temp->width);
                temp = temp->next;
            }
        }
        declaration_varst(table->child,f);
        declaration_varst(table->next,f);
    }
    
}