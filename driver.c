/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#include "parser.h"
#include "ast.h"
#include "symbolTable.h"
#include "codegen.h"
#include "codegen_driver.h"

#include <time.h>

extern char buffer1[256];
extern char buffer2[256];
extern char lexeme_buffer[20]; //lexeme that is passed to tokenizer
extern int eof_buffer; //buffer that has EOF
extern int eof_marker; //position of EOF in the buffer
extern int curr_buffer; //active buffer
extern int buffer_itr; //buffer iterator
extern int lineNumber;
extern bool lex_error;
extern bool comment_flag; //true means that comment is ongoing
extern bool comment_flag_flag;
extern bool buffer1_empty;
extern bool buffer2_empty;
extern bool delimiter_flag;
extern bool error_long_id;
extern bool error_long_id_print;
extern bool file_finished;
extern int eps_enum;
extern stack_node *head;
extern int err_count;

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
            //declaration_maker();
            //printf("%d",ast->forward->forward->left_child->left_child->left_child->right_child->data.enum_value);
            //func_hash_entry *main_entry = find_func_hash("program");
            //var_st *main_st = main_entry->child;
            FILE *f = fopen("D:/Compiler_project/ollydbg/olo.asm", "w");
            //make_code(ast->forward->forward->left_child->left_child->left_child->right_child,main_st,f);
            declaration_maker(f);
            entire_code_gen(ast,f);
            fclose(f);
        }

        else if(ip==4){
            time_taken(argv[1]);
        }
        else{
            printf("Enter a correct case number!");
        }
    }
}