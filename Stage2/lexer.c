/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#include "lexer.h"

char buffer1[256];
char buffer2[256];
char lexeme_buffer[20]; //lexeme that is passed to tokenizer
int eof_buffer = 0; //buffer that has EOF
int eof_marker = -1; //position of EOF in the buffer
int curr_buffer = 1; //active buffer
int buffer_itr = 0; //buffer iterator
int lineNumber = 1;
bool lex_error=false;
bool comment_flag=false; //true means that comment is ongoing
bool comment_flag_flag=false;
bool buffer1_empty = true;
bool buffer2_empty = true;
bool delimiter_flag = false;
bool error_long_id = false;
bool error_long_id_print = false;
bool file_finished = false;
extern int err_count;

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
                if(itr!=256){ //EOF reached
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
                if(itr!=256){ //EOF reached
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
        if((curr_buffer == eof_buffer && buffer_itr>=eof_marker) || (eof_marker!= -1 && curr_buffer!= eof_buffer && buffer_itr-256 == eof_marker)){
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
