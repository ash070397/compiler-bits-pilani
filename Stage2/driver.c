/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#include "codegen.h"
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
extern int ans_parse;
extern int ans_ast;
extern FILE *fp;
extern int semantic_error;

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
    printf("%d Number of Syntatic Errors\n",err_count);
    if(err_count==0){
        //Code is syntatically correct
        ASTNode *ast_root=postorder(main_tree->root,NULL);
        fill_ast_parent(ast_root);
        initialise_func_st_root();
        char for_string[10][21];
        int size_for_string = 0;
        printf("\n\n\n");
        fill_symbol_table(ast_root,NULL,for_string,size_for_string);
        semantic_check(ast_root,NULL,NULL);
           
        printf("%d Number of Semantic errors\n",semantic_error);
    }
    end_time = clock();
    total_CPU_time  =  (double) (end_time - start_time);
    total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;
    printf("\nTotal CPU Time %f\n",total_CPU_time);
    printf("Total CPU Time In Seconds %f\n",total_CPU_time_in_seconds);
    fclose(fp);
}

int main(int argc,char *argv[]){
    populate_table();
    populate_keyword_table();
    populate_grammar("grammar.txt");
    ComputeFirstAndFollowSets();
    createParseTable();

    if(argc!=3){
        printf("\nEnter a valid number of parameters\n");
        return 0;
    }

    FILE *fp_temp;
    fp_temp = fopen(argv[1],"r");
    if(fp_temp==NULL){
        printf("\nError in opening the test file\n");
        printf("\n Please enter a valid test file\n");
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
        ans_parse=0;
        ans_ast=0;
        semantic_error=0;
        printf("\n\n\n");
        printf("0: Exit from the loop and terminate the program\n");
        printf("1: Printing the Token List\n");
        printf("2: Printing Syntatic Errors and Parse Tree\n");
        printf("3: Printing the AST formed\n");
        printf("4: Memory Allocated for Parse Tree and AST and Compression Ratio\n");
        printf("5: Printing the Symbol Table\n");
        printf("6: Activation Record Size\n");
        printf("7: Static and Dynamic Array Printing\n");
        printf("8: Error Reporting and Time for Execution of Code\n");
        printf("9: Intermediate Code Generation and .asm file gereration\n");
        printf("\n\nEnter the case number:");
        scanf("%d",&ip);

        if(ip==0){
            printf("Exiting the Loop\n");
            printf("\n************************\n");
            exit(0);
        }
        else if(ip==1){
            printf("Printing the tokens in the file\n");
            printTokens(argv[1]);
            printf("\n************************\n");
        }
        else if(ip==2){
            fp = fopen(argv[1], "r");
            if (fp == NULL)
            {
                printf("File is not available \n");
            }
            parseTree *main_tree = parseInputSourceCode();
            FILE *fp2 = fopen("temp.txt", "w");
            fprintf(fp2, "LEXEME        LINE NO         TOKEN NAME             VALUE OF CONST                PARENT NODE                     IS LEAF                     CURRENT NODE\n");
            printParseTree(main_tree->root, fp2);
            fclose(fp2);
            printf("%d Number of Syntatic errors\n",err_count);
            fp2=fopen("temp.txt","r");
            if(fp2==NULL){
                printf("Parse Tree Not Constucted\n");
            }
            else{
                char c;
                c=fgetc(fp2);
                printf("Printing the Parse Tree on the console\n");
                while(c!=EOF){
                    printf("%c",c);
                    c=fgetc(fp2);    
                }
                fclose(fp2);
            }
            printf("\n************************\n");
            fclose(fp);
            
        }
        else if(ip==3){
                fp=fopen(argv[1],"r");
                if (fp == NULL)
                {
                    printf("\nError in opening the file\n");
                    return 0;
                }
                parseTree *main_tree = parseInputSourceCode();
                FILE *fp2 = fopen("temp.txt", "w");
                printParseTree(main_tree->root, fp2);
                fclose(fp2);
                if(err_count>0){
                    printf("Code is syntatically incorrect, no AST is being made\n");
                    printf("%d Number of Syntatic errors\n",err_count);
                    continue;
                }

                printf("\nAST Traversal\n");
                printf("PreOrder Traversal for all the Nodes except Expression where Inorder Traversal is done\n\n");
                
                ASTNode *ast_root=postorder(main_tree->root,NULL);
                printTree(ast_root);
                printf("\n************************\n");
                fclose(fp);
        }
        else if(ip==4){
            fp=fopen(argv[1],"r");
            if (fp == NULL)
            {
                printf("\nError in opening the file\n");
                return 0;
            }
            parseTree *main_tree = parseInputSourceCode();
            if(err_count>0){
                    printf("Code is syntatically incorrect, no AST is being made\n");
                    printf("%d Number of Syntatic errors\n",err_count);
                    continue;
            }
            ASTNode *ast_root=postorder(main_tree->root,NULL);
            printf("\n\n\n\n");
            printf("Total Number of Nodes for ParseTree: %d\n",ans_parse);
            int temp_parse_size=sizeof(tree_node)*ans_parse;
            printf("Memory Allocated for ParseTree: %u\n",temp_parse_size);
            
            printf("Total Number of Nodes for AST: %d\n",ans_ast);
            int temp_ast=sizeof(ASTNode)*ans_ast;
            printf("Memory Allocated for Abstract Syntax Tree: %u\n",temp_ast);
            float ratio=((float)(temp_parse_size-temp_ast)/((float)temp_parse_size))*100;
            printf("Compression Percentage: %f\n",ratio);
            printf("\n************************\n");
            fclose(fp);
        }
        else if(ip==5){
            fp=fopen(argv[1],"r");
            if (fp == NULL)
            {
                printf("\nError in opening the file\n");
                return 0;
            }
            parseTree *main_tree = parseInputSourceCode();
            if(err_count>0){
                    printf("Code is syntatically incorrect, no AST is being made\n");
                    printf("%d Number of Syntatic errors\n",err_count);
                    continue;
            }
            ASTNode *ast_root=postorder(main_tree->root,NULL);
            fill_ast_parent(ast_root);
            initialise_func_st_root();
            char for_string[10][21];
            int size_for_string = 0;
            fill_symbol_table(ast_root,NULL,for_string,size_for_string);
            printf("\n\nPrinting the Symbol Table\n\n");
            printSymbolTable();
            printf("\n************************\n");
        }
        else if(ip==6){
            fp=fopen(argv[1],"r");
            if (fp == NULL)
            {
                printf("\nError in opening the file\n");
                return 0;
            }
            parseTree *main_tree = parseInputSourceCode();
            if(err_count>0){
                    printf("Code is syntatically incorrect, no AST is being made\n");
                    printf("%d Number of Syntatic errors\n",err_count);
                    continue;
            }
            ASTNode *ast_root=postorder(main_tree->root,NULL);
            fill_ast_parent(ast_root);
            initialise_func_st_root();
            char for_string[10][21];
            int size_for_string = 0;
            fill_symbol_table(ast_root,NULL,for_string,size_for_string);
            printf("\n\nPrinting the Activation Record\n\n");
            printActRecords();
            printf("\n************************\n");            
        }
        else if(ip==7){
            fp=fopen(argv[1],"r");
            if (fp == NULL)
            {
                printf("\nError in opening the file\n");
                return 0;
            }
            parseTree *main_tree = parseInputSourceCode();
            if(err_count>0){
                    printf("Code is syntatically incorrect, no AST is being made\n");
                    printf("%d Number of Syntatic errors\n",err_count);
                    continue;
            }
            ASTNode *ast_root=postorder(main_tree->root,NULL);
            fill_ast_parent(ast_root);
            initialise_func_st_root();
            char for_string[10][21];
            int size_for_string = 0;
            fill_symbol_table(ast_root,NULL,for_string,size_for_string);
            printf("\n\nPrinting the info. for Static and Dynamic Arrays\n\n");
            printArrays();
            printf("\n************************\n");
        }
        else if(ip==8){
            time_taken(argv[1]);
            printf("\n************************\n");
        }
        else if(ip==9){
                //third arguemnt will be used for asm file
                fp=fopen(argv[1],"r");
                if (fp == NULL)
                {
                    printf("\nError in opening the file\n");
                    return 0;
                }
                parseTree *main_tree = parseInputSourceCode();
                printf("%d Number of Syntactic Errors\n",err_count);
                if(err_count>0){
                    printf("Code is syntatically incorrect, no AST is being made\n");
                    printf("%d Number of Syntatic errors\n",err_count);
                    continue;
                }
                
                
                ASTNode *ast_root=postorder(main_tree->root,NULL);
                fill_ast_parent(ast_root);
                initialise_func_st_root();
                char for_string[10][21];
                int size_for_string = 0;
                fill_symbol_table(ast_root,NULL,for_string,size_for_string);
                semantic_check(ast_root,NULL,NULL);
                
                printf("%d Number of Semantic errors\n",semantic_error);
                if(semantic_error>0){
                    printf("There are Semantic Errors so no Intermediate Code Generation\n\n");
                    continue;
                }
                printf("\nLEVEL 4:Symbol table/Type Checking/Semantic rules Work/\nStatic and Dynamic arrays handled in type checking/Code Generation working\n");
                FILE *f_asm=fopen(argv[2],"w");
                declaration_maker(f_asm);
                entire_code_gen(ast_root, f_asm);
                printf("\nCode Generated in the specified file\n");
                printf("\n************************\n");
                fclose(f_asm);
        }
        else{
            //call all the functions again;
            printf("\nEnter a correct case number!\n");
            printf("\n************************\n");
        }
    }
    
}
