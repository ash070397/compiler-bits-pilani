/*---------------------------------------------------------------------------------------------
Group #28

2016B5A70607P  Manan Agarwal
2015B5A70614P  Anirudh Buvanesh
2015B4A70824P  Anubhav Bansal
2015B4A70716P  Aayush Agarwal
2015B4A70636P  Ashish Kumar

---------------------------------------------------------------------------------------------*/

#ifndef lexer
#define lexer
#include "lexerDef.h"
#endif

FILE *fp;
unsigned long hash(char *sym);
unsigned long keyword_hash(char *sym);
void populate_table();
void populate_keyword_table();
NODE string_to_enum(char* input);
NODE is_keyword(char *input);
tokenInfo* tokenize(char *str);
void getstream();
tokenInfo* getNextToken();
void removeComments(char *testcaseFile, char *cleanFile);
void printTokens(char *filename);