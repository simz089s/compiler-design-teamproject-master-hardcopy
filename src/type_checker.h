#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "symbol_table.h"
#include "tree.h"

void typePROG(PROG* prog);    
char* typeVAR(SymbolTable* t, char* identifier);
bool isNumeric(TYPE* type);
bool isInteger(TYPE* type);
bool isString(TYPE* type);
bool isBool(TYPE* type);
bool isComparable(TYPE* type);
bool isStruct(TYPE* type);
bool isArrayOrSlice(TYPE* type);
bool isOrdered(TYPE* type);
bool identicalTypes(char* t1, char* t2);
char* resolveType(TYPE* userDefinedType);
void typeTOPDECL(TOPDECL* topDecl); 
void typeEXPRLIST(SymbolTable* t, EXPRLIST* exprList);
void typeSTMT(SymbolTable* t, STMT* stmt, TYPE* returnTYPE);
void typeEXPR(SymbolTable* t, EXPR* expr);
void typeFUNC(SymbolTable* t, FUNC* func);
void typeFUNCCALL(SymbolTable* t, EXPR* funcCall, FUNC* func, SIGNATURE* sig); // TODO sig necessary?

#endif /* TYPE_CHECKER_H */
