/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/
#include "parser.h"

// extern FILE *fp;
int eps_enum = 56;
stack_node *head=NULL;
int err_count = 0;
extern char *non_terminal_map[NUM_NT];
extern char *terminal_map[NUM_T]; 
extern char *keyword_map[NUM_KEYWORD];

//Push function for Stack ADT
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
int follow_set(){
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