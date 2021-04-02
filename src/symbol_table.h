#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

#include "tree.h"

#define HASH_SIZE 317

typedef struct SymbolTable {
    SYMBOL* table[HASH_SIZE];
    struct SymbolTable* parent;
    int counter;
} SymbolTable;

SymbolTable* initSymbolTable(void);
SymbolTable* scopeSymbolTable(SymbolTable*);
int hash(char* s);
SYMBOL* putSymbol(SymbolTable*, SymbolKind, char*, char*);
SYMBOL* getSymbol(SymbolTable*, SymbolKind, char*);
SYMBOL* getSymbolScope(SymbolTable *t, SymbolKind, char *name);

bool isDefined(SymbolTable* t, char* name);

void unscopeSymbolTable(SymbolTable*);

char* getType(TYPE* type);
void symPROG(PROG* prog);
void symTOPDECL(SymbolTable*, TOPDECL*, int);
void symVARDECL(SymbolTable*, TOPDECL*, int);
void symTYPEDECL(SymbolTable*,TOPDECL*, int);
void symTYPE(SymbolTable*, TYPE*, int);
void symSTRUC(SymbolTable*, FIELDDECL*, int);
void symFUNCDECL(SymbolTable*, char*, SIGNATURE*, STMT*);
// void symLONGFUNCSIG(SIGNATURE*);
void symFUNCSIG(SymbolTable*, SIGNATURE*);
void symSTMT(SymbolTable*, STMT*, int, bool);
void symCASE(SymbolTable*, CASE*, int);
void symSHORTVARDECL(SymbolTable*, STMT*);
void symEXPRLIST(SymbolTable*, EXPRLIST*);
void symEXPR(SymbolTable*, EXPR*);
void symARGSLIST(SymbolTable*, ARGLIST* al, int l,bool);
void symSIGDECL(SymbolTable*, SIGDECL* sd, int l);
void symVAR(SymbolTable*, VAR* v, int l);
void symARGSLISTTYPE(SymbolTable*, ARGLIST*, TYPE*);
#endif /* SYMBOL_TABLE_H */
