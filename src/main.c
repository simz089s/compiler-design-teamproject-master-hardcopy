#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "parser.tab.h"
#include "pretty_printer.h"
#include "tree.h"
#include "symbol_table.h"
#include "codegen.h"

// #include "lex.yy.c"
extern int yylex();
int yyparse();

bool g_tokens;
bool prettySymbols;

PROG* root;

int main(int argc, char* argv[]) {

    char* mode = argv[1];

    if (strcmp("scan", mode) == 0) {
        while (yylex() != 0)
            ;
        printf("OK\n");
        return 0;
    } else if (strcmp("tokens", mode) == 0) {
        g_tokens = true;
        while (yylex() != 0)
            ;
        printf("OK\n");
        return 0;
    } else if (strcmp("parse", mode) == 0) {
        if (yyparse() == 0) {
            printf("OK\n");
        }
        return 0;
    } else if (strcmp("pretty", mode) == 0) {
        if (yyparse() == 0) {
            prettyPROG(root);
        }
        return 0;
    } else if (strcmp("symbol", mode) == 0) {
        if (yyparse() == 0) {
            prettySymbols = true; // outputs the symbol table to stdout
            symPROG(root);
        }
        return 0;
    } else if (strcmp("typecheck", mode) == 0) {
         if(yyparse()==0){
            prettySymbols = false;
             symPROG(root);
            //  typePROG(root);
         }
        // outputs OK if input is type correct
        return 0;
    } else if (strcmp("codegen", mode) == 0){
        if(yyparse()==0){
            prettySymbols = false;
            // symPROG(root);
            // typePROG(root);
            codePROG(root);
        }
    }else {
        fprintf(stderr, "Invalid Argument: %s\nUsage:\n\t a.out [scan|tokens|parse]\n", mode);
        exit(1);
    }
}
