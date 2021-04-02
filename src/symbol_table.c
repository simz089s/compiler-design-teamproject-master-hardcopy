#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"

extern bool prettySymbols; // true if we want to print symbols (golitec symbol) and 0 otherwise (golitec typecheck)
extern void printTabs(int level);

char* strcat(char* dest, const char* src) {
    char* origDest = dest;
    while (*dest)
        dest++;
    for (; *src; dest++, src++)
        *dest = *src;
    *dest = '\0';
    // strcpy(dest + strlen(src), src);
    return origDest;
}

char* strdup(const char* s) {
    size_t len = strlen (s) + 1;
    void* new = malloc(len * sizeof(char));
    if (new == NULL) {
        fprintf(stderr, "strdup malloc error\n");
        return NULL;
    }
    return (char *) memcpy(new, s, len * sizeof(char));
}

//should return [3][]int as such and struct{ a int; b float64; } as such..
//TODO resolve basetype to nametypes eg type a int type b a
char* getType(TYPE* type) {
    switch (type->kind) {
    case namedType:
        return type->named.name; // symbol table does not want base type
    case sliceType: // slice
    {
        char buffer[64] = {0,};
        buffer[0] = '[';
        buffer[1] = ']';
        return strcat(buffer, getType(type->slice)); //we need the fact that its a slice
    }
    case arrayType: // array
    {
        if (type->array.r == NULL)
            throwError(type->lineno, "array content type is null.", 1);
        char buffer[32] = {0,}; // arbitrary "practical" size
        *buffer = '[';
        sprintf(buffer + 1, "%d]", type->array.size);
        return strcat(buffer, getType(type->array.r));
    case structType: // struct or empty struct 
        //technically we need to get the body too, for struct equality. but this is for typechecker. which we're going to pretend doesn't exist. 
        //TODO: low priority
        return "struct";
    }
    }
    return ""; // default case should be empty string or NULL ? if even possible to reach here
}

SymbolTable* initSymbolTable() {
    SymbolTable* st = malloc(sizeof(SymbolTable));
    for (int i = 0; i < HASH_SIZE; i++) {
        st->table[i] = NULL;
    }
    st->parent = NULL;
    putSymbol(st, k_symbolKindNamedType, "true", "bool");
    putSymbol(st, k_symbolKindNamedType, "false", "bool");
    putSymbol(st, k_symbolKindNamedType, "int", "int");
    putSymbol(st, k_symbolKindNamedType, "float64", "float64");
    putSymbol(st, k_symbolKindNamedType, "rune", "rune");
    putSymbol(st, k_symbolKindNamedType, "string", "string");

    st->counter = 0;

    return st;
}

SymbolTable* scopeSymbolTable(SymbolTable* parent) {
    SymbolTable* child = initSymbolTable();
    child->parent = parent;
    return child;
}

int hash(char* s) {
    unsigned int hash = 0;
    while (*s)
        hash = (hash << 1) + (unsigned int)*(s++);
    return hash % HASH_SIZE;
}

SYMBOL* putSymbol(SymbolTable* st, SymbolKind symk, char* name, char* kind) {
    int hashIndex = hash(name);
    for (SYMBOL* sym = st->table[hashIndex]; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0)
            throwError(0, "variable redefinition", 1); //no need to check for symbolkind equality. 
    }
    SYMBOL* sym = malloc(sizeof(SYMBOL));
    char temp[64];
    sprintf(temp, "%d", st->counter++);

    const int prependMaxLen = (sizeof("__golite__type_structure__") + sizeof(temp)) * sizeof(char);
    char* prependType = malloc(prependMaxLen);
    switch (symk) {
        case k_symbolKindFunction:
            prependType = (char*) memcpy(prependType, "__golite__funcname__", prependMaxLen);
            break;
        case k_symbolKindVariable:
            prependType = (char*) memcpy(prependType, "__golite__id__", prependMaxLen);
            break;
        case k_symbolKindNamedType:
            prependType = (char*) memcpy(prependType, "__golite__type__", prependMaxLen);
            break;
        default:
            prependType = (char*) memcpy(prependType, "__golite__type_structure__", prependMaxLen);
    }

    char* suffix = strcat(prependType, temp);
    sym->name = name;
    sym->codeName = strcat(strdup(name), suffix);
    sym->kind = kind;
    sym->next = st->table[hashIndex];
    st->table[hashIndex] = sym;
    return sym;
}

SYMBOL* getSymbol(SymbolTable* st, SymbolKind symk, char* name) {
    int hashIndex = hash(name);

    for (SYMBOL* sym = st->table[hashIndex]; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0 && sym->symKind==symk)
            return sym;
    }

    if (st->parent == NULL)
        return NULL;

    return getSymbol(st->parent, symk, name);
}

//For declarations in only current scope
SYMBOL* getSymbolScope(SymbolTable* t, SymbolKind symk, char* name) {
    int i = hash(name);
    // Check the current scope
    for (SYMBOL* s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0 && s->symKind==symk)
            return s;
    }
    return NULL;
}

// check if the symbol is defined in the current scope
bool isDefined(SymbolTable* t, char* name) {
    int i = hash(name);
    for (SYMBOL* s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0)
            return true;
    }
    return false;
}

void symPROG(PROG* prog) {
    if (prog != NULL) {
        // pre-declared mapping of base types identifiers
        if (prettySymbols == true) {
            printf("Identifer [Category] = Type\n");
            printf("-----------------------------\n");
            printf("int [type] = int\nfloat64 [type] = float64\nbool [type] = bool\nrune [type] = rune\nstring [type] = string\ntrue [constant] = bool\nfalse [constant] = bool\n");
        }
        symTOPDECL(initSymbolTable(), prog->topDecls, 0);
    }
}

void symTOPDECL(SymbolTable* sym, TOPDECL* topDecl, int level) {
    if (topDecl != NULL) {
        switch (topDecl->kind) {
        case k_declarationKindEmptyDistributedVarDecl: // do nothing
            break;
        case k_declarationKindEmptyDistributedTypeDecl:
            break;
        case k_declarationKindVariable:
            if (prettySymbols)
                printf("\n");
            symVARDECL(sym, topDecl, level);
            if (prettySymbols)
                printf("\n");
            break;
        case k_declarationKindShortVariable:
            symSHORTVARDECL(sym, genSTMT_TOPDECL(topDecl));
            break;
        case k_declarationKindType:
            if (prettySymbols)
                printf("\n");
            symTYPEDECL(sym, topDecl, level);
            break;
        case k_declarationKindFunction:
            
            //pasted the function code here, because we need to directly store into ast
            
            if (prettySymbols)
                printf("%s -> func", topDecl->funcdecl.identifier);

            if(topDecl->funcdecl.sig->optionalReturn!=NULL){
                TYPE* t = topDecl->funcdecl.sig->optionalReturn;
                SYMBOL* s = putSymbol(sym, k_symbolKindFunction, topDecl->funcdecl.identifier, getType(t)); //store
                s->val.function.functionName = topDecl->funcdecl.identifier;
                s->val.function.returnType = t;
                s->val.function.sig = topDecl->funcdecl.sig; 
                topDecl->symbol = s; //tbh, not sure if this is needed other than in typecheck
            } 
            else{
                SYMBOL* s = putSymbol(sym, k_symbolKindFunction, topDecl->funcdecl.identifier, "void"); //store
                s->val.function.functionName = topDecl->funcdecl.identifier;
                s->val.function.returnType = NULL;
                s->val.function.sig = topDecl->funcdecl.sig; 
                topDecl->symbol = s; //tbh, not sure if this is needed other than in typecheck
            }

            SymbolTable* newTable = scopeSymbolTable(sym);

            if (topDecl->funcdecl.sig->decls != NULL) {
                switch (topDecl->funcdecl.sig->decls->kind) {
                case k_signatureKindLong: // one id with one type
                case k_signatureKindShort: // many ids with one type
                    symFUNCSIG(newTable, topDecl->funcdecl.sig);
                    break;
                }
            }
            if (topDecl->funcdecl.sig->optionalReturn) {
                if (prettySymbols)
                    printf(" = () -> %s", getType(topDecl->funcdecl.sig->optionalReturn));
            } else {
                if (prettySymbols)
                    printf(" = () -> void");
            }
            if (prettySymbols)
                printf(" {\n");
            if (topDecl->funcdecl.block != NULL)
                symSTMT(newTable, topDecl->funcdecl.block, 1, true);
            if (prettySymbols)
                printf("\n}\n");
            break;
        }
        if (topDecl->next != NULL) {
            if (topDecl->kind != k_declarationKindType) {
                if (prettySymbols)
                    printf("\n");
            }
            symTOPDECL(sym, topDecl->next, level);
        }
    }
}

//ok
void symVARDECL(SymbolTable* sym, TOPDECL* decl, int level) {

    if (decl != NULL && decl->kind == k_declarationKindVariable) {
        if (prettySymbols)
            printTabs(level);
        symVAR(sym, decl->vardecl.content, level);
        if (decl->vardecl.next != NULL) {
            if (prettySymbols)
                printf("\n");
            symVARDECL(sym, decl->vardecl.next, level);
        }
        if (decl->vardecl.distributedNext != NULL) {
            if (prettySymbols)
                printf("\n");
            symVARDECL(sym, decl->vardecl.distributedNext, level);
        }
    }
}

// probably ok - ting 4/3
//TODO: this function - change type resolving for codegen
void symTYPEDECL(SymbolTable* sym, TOPDECL* decl, int level) {
    if (decl != NULL && decl->kind == k_declarationKindType) {
        if (prettySymbols)
            printTabs(level);
        char* name = decl->typedecl.identifier;
        
        char* rhstype = getType(decl->typedecl.content);

        if ((getSymbol(sym, k_symbolKindNamedType, rhstype)) == NULL)
            decl->typedecl.content->symbol = putSymbol(sym, k_symbolKindNamedType, name, rhstype);
        else
            throwError(decl->lineno, "type already declared", 1);

        if (prettySymbols){
            printf("%s[type] -> %s", name, rhstype);
        }
    } else {
        throwError(decl->lineno, "not a type declaration", 1);
    }
    if (decl->typedecl.distributedNext != NULL) {
        if (prettySymbols)
            printf("\n");
        symTYPEDECL(sym, decl->typedecl.distributedNext, level);
    }
}

// helper function for symTYPEDECL()
// not used rn
//should be OK. the only time this is called is when it's on the RHS. Essentially prettyprint
// void symTYPE(SymbolTable* sym, TYPE* type, int level) {
//     if (type != NULL) {
//         if (strcmp(type->identifier, "")) {
//             if (prettySymbols)
//                 printf("%s ", type->identifier);
//         }
//         if (type->size == TYPE_SIZE_SLICE) // print slice
//             if (prettySymbols)
//                 printf("[]");
//         if (type->size > 0) // print array
//             if (prettySymbols)
//                 printf("[%d]", type->size);
//         if (type->baseType != NULL)
//             symTYPE(sym, type->baseType, level);
//         else if (type->struc != NULL) { // print struct
//             if (prettySymbols)
//                 printf("{");
//             SymbolTable* new = scopeSymbolTable(sym);
//             symSTRUC(new, type->struc, level + 1);
//             if (prettySymbols)
//                 printTabs(level);
//             if (prettySymbols)
//                 printf("}\n");
//         } else if (type->size == TYPE_SIZE_EMPTYSTRUCT) {
//             if (prettySymbols)
//                 printf("{ }");
//         }
//     }
// }

// helper function for symTYPE()
// not used rn
// void symSTRUC(SymbolTable* sym, FIELDDECL* fieldDecl, int level) {
//     if (fieldDecl != NULL) {
//         symARGSLISTTYPE(sym, fieldDecl->regular.identifierlist, fieldDecl->regular.type);
//         if (prettySymbols)
//             printf("; ");
//         if (fieldDecl->next != NULL)
//             symSTRUC(sym, fieldDecl->next, level); // next struct field declaration
//     }
// }

//ok, pasted into symTOPDECLS so this is now commented out
// void symFUNCDECL(SymbolTable* sym, char* identifier, SIGNATURE* signature, STMT* stmtBlock) {
//     if (prettySymbols)
//         printf("%s -> func", identifier);

//     if(signature->optionalReturn!=NULL){
//         TYPE* t = signature->optionalReturn;
//         putSymbol(sym, k_symbolKindFunction, identifier, getType(t)); //store
//     } 
//     else{
//         putSymbol(sym, k_symbolKindFunction, identifier, "void"); //store
//     }

//     SymbolTable* newTable = scopeSymbolTable(sym);

//     if (signature->decls != NULL) {
//         switch (signature->decls->kind) {
//         case k_signatureKindLong: // one id with one type
//         case k_signatureKindShort: // many ids with one type
//             symFUNCSIG(newTable, signature);
//             break;
//         }
//     }
//     if (signature->optionalReturn) {
//         if (prettySymbols)
//             printf(" = () -> %s", getType(signature->optionalReturn));
//     } else {
//         if (prettySymbols)
//             printf(" = () -> void");
//     }
//     if (prettySymbols)
//         printf(" {\n");
//     if (stmtBlock != NULL)
//         symSTMT(newTable, stmtBlock, 1, true);
//     if (prettySymbols)
//         printf("\n}\n");
// }

//ok..
void symFUNCSIG(SymbolTable* sym, SIGNATURE* sig) {
    if (sig->decls != NULL) {
        symSIGDECL(sym, sig->decls, 0);
    }
}
//ok..
void symARGSLISTTYPE(SymbolTable* sym, ARGLIST* list, TYPE* t) {
    if (list != NULL) {
        list->thisVAR->symbol = putSymbol(sym, k_symbolKindVariable, list->thisVAR->identifier, getType(t));
        if (list->next != NULL) {
            symARGSLISTTYPE(sym, list->next, t);
        }
    }
}
// ok..
void symSIGDECL(SymbolTable* sym, SIGDECL* sd, int l) {
    if (sd->valType != NULL) {
        symARGSLISTTYPE(sym, sd->parameters.identifierList, sd->valType);
    } else
        symARGSLISTTYPE(sym, sd->parameters.identifierList, NULL);

    if (sd->next != NULL) {
        symSIGDECL(sym, sd->next, l);
    }
}

// this isn't going to be called much.
// decls like var a,b int = 3,4 will be handled without this call
// function signatures (func a(a,b int, c float64)) will be handled without this call
// typedecls will be handled with symARGSLISTTYPE
// it's only called in the two cases below:
// a,b int = 3,4 only requires pulling from symtab
// a,b := 3,4 has no type assignment (is inferred)

//var a int = 0
//a,b,c := 3,2,1 TODO @ting note to self need to do finish this correctly

//TODO &TO_INFER lmao

//only called in short var decl
void symARGSLIST(SymbolTable* sym, ARGLIST* al, int l, bool isValid) { //change this bool to "isvalidshortdecl" initialized at false, and if there are no more nexts and still false, throw error (aka everything is initialized already)

    // if (isShortDecl == true) { //shortdecl
        if (getSymbol(sym, k_symbolKindVariable, al->thisVAR->identifier) == NULL){ //not yet declared ok
            al->thisVAR->symbol = putSymbol(sym, k_symbolKindVariable, al->thisVAR->identifier, "&TO_INFER");
            isValid = true;
            //mark this variable as not yet initialized for codegen
        }
        else{ //need to mark this variable as already initialized for codegen and put symbol in it
            al->thisVAR->symbol = getSymbol(sym, k_symbolKindVariable, al->thisVAR->identifier);
        }
    // } 
    // else { // assignment ?? not used in assignment
    //     if (getSymbol(sym, k_symbolKindVariable, al->thisVAR->identifier) == NULL) {
    //         throwError(al->thisVAR->lineno, "undeclared variable in assignment", 1);
    //     }
    // }
    if (al->next != NULL) {
        symARGSLIST(sym, al->next, l, isValid);
    }
    else{
        if(isValid==false){
            printf("shortdecl variables are all already initialized.\n");
            exit(1);
        }
    }
}

//ok
void symVAR(SymbolTable* sym, VAR* v, int l) {

    //check for declaration in current scope only. if not, places it.
    if (getSymbolScope(sym, k_symbolKindVariable, v->identifier) == NULL) {

        // if (v->type->baseType == NULL) { //then this is a basetype, aka int,bool, etc.
        v->symbol = putSymbol(sym, k_symbolKindVariable, v->identifier, getType(v->type));
        if (prettySymbols)
            printf("%s [variable]-> %s\n", v->identifier, getType(v->type));
        // } else {
        //     //need to somehow store info like [2][3]int
        //     v->symbol = putSymbol(sym, v->identifier, getType(v->type));
        //     if (prettySymbols)
        //         printf("%s [variable]-> %s\n", v->identifier, getType(v->type));
        // }

    } else {
        throwError(v->lineno, "variable already declared in current scope", 1);
    }

    if (v->expression != NULL) {
        symEXPR(sym, v->expression); 
    }
}

//ok
//statements don't have associated symbols bc they aint types but its in the tree just in case
void symSTMT(SymbolTable* sym, STMT* stmt, int level, bool onNewline) {
    if (stmt != NULL) {
        switch (stmt->kind) {
        case k_statementKindEmptyStmt: // empty statement
            break;
        case k_statementKindShortVarDecl: // simple statement
            if (prettySymbols)
                printTabs(level);
            symSHORTVARDECL(sym, stmt);
            if (onNewline && prettySymbols)
                printf("\n");
            break;
        case k_statementKindAssignment: // simple statement, e.g. "<=", "+="
            if (prettySymbols)
                printTabs(level);
            symEXPR(sym, stmt->val.assignment.identifier);
            symEXPR(sym, stmt->val.assignment.expression);
            if (onNewline && prettySymbols)
                printf("\n");
            break;
        case k_statementKindExprlistAssignment: // simple statement, "="
            if (prettySymbols)
                printTabs(level);
            symEXPRLIST(sym, stmt->val.exprlistAssignment.LHS);
            symEXPRLIST(sym, stmt->val.exprlistAssignment.RHS);
            if (onNewline && prettySymbols)
                printf("\n");
            break;
        case k_statementKindDeclaration:
            if (prettySymbols)
                printTabs(level);
            symTOPDECL(sym, stmt->val.topDecl.topDecl, level); // prints newline
            break;
        case k_statementKindIncrement: // simple statement
            if (prettySymbols)
                printTabs(level);
            symEXPR(sym, stmt->val.incDec.expression);
            if (onNewline && prettySymbols)
                printf("\n");
            break;
        case k_statementKindDecrement: // simple statement
            if (prettySymbols)
                printTabs(level);
            symEXPR(sym, stmt->val.incDec.expression);
            if (onNewline && prettySymbols)
                printf("\n");
            break;
        case k_statementKindPrint:
            if (prettySymbols)
                printTabs(level);
            symEXPRLIST(sym, stmt->val.print.exprlist);
            break;
        case k_statementKindPrintLn:
            if (prettySymbols)
                printTabs(level);
            symEXPRLIST(sym, stmt->val.print.exprlist);
            break;
        case k_statementKindReturn:
            if (prettySymbols)
                printTabs(level);
            if (stmt->val.returnStmt.expr != NULL) {
                symEXPR(sym, stmt->val.returnStmt.expr);
            }
            // if (onNewline) printf("\n");
            break;
        case k_statementKindIf:
            // "if" [ SimpleStmt ";" ] Expression Block [ "else" ( IfStmt | Block ) ]
            if (prettySymbols)
                printTabs(level);
            SymbolTable* newscope = scopeSymbolTable(sym);
            if (onNewline && prettySymbols)
                printTabs(level);
            if (stmt->val.ifStmt.simpleStmt != NULL) {
                symSTMT(sym, stmt->val.ifStmt.simpleStmt, level, false); // not onNewline
            }
            symEXPR(sym, stmt->val.ifStmt.condition);
            if (stmt->val.ifStmt.stmt != NULL)
                symSTMT(sym, stmt->val.ifStmt.stmt, level + 1, true); // stmt block goes on newline
            if (prettySymbols)
                printTabs(level); // indent the closing bracket
            if (stmt->val.ifStmt.elseStmt != NULL)
                symSTMT(newscope, stmt->val.ifStmt.elseStmt, level, true);
            break;
        case k_statementKindElse:
            if (prettySymbols)
                printTabs(level);
            SymbolTable* elseScope = scopeSymbolTable(sym);
            if (stmt->val.elseStmt.stmt != NULL)
                symSTMT(elseScope, stmt->val.elseStmt.stmt, level, true);
            if (prettySymbols)
                printTabs(level);
            if (prettySymbols)
                printf("}\n");
            break;
        case k_statementKindElseIf:
            if (prettySymbols)
                printTabs(level);
            symSTMT(sym, stmt->val.elseIfStmt.ifStmt, level, true);
            break;
        case k_statementKindSwitch:
            if (prettySymbols)
                printTabs(level);
            SymbolTable* switchScope = scopeSymbolTable(sym);
            if (stmt->val.switchStmt.simpleStmt != NULL) {
                symSTMT(sym, stmt->val.switchStmt.simpleStmt, level, false);
                printf("; ");
            }
            if (stmt->val.switchStmt.optionalExpr != NULL)
                symEXPR(sym, stmt->val.switchStmt.optionalExpr);
            if (prettySymbols)
                printf(" {\n");
            symCASE(switchScope, stmt->val.switchStmt.c, level + 1);
            if (prettySymbols)
                printTabs(level);
            if (prettySymbols)
                printf("}\n");
            break;
        case k_statementKindForCondition: // condition only
            if (prettySymbols)
                printTabs(level);
            SymbolTable* forCondScope = scopeSymbolTable(sym);
            if (stmt->val.forStmt.condition != NULL)
                symEXPR(sym, stmt->val.forStmt.condition);
            if (stmt->val.forStmt.stmtBlock)
                symSTMT(forCondScope, stmt->val.forStmt.stmtBlock, level + 1, true);

            break;
        case k_statementKindFor: // initial stmt, condition, post stmt
            // "for" [ Condition | ForClause | RangeClause ] Block .
            // ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
            if (prettySymbols)
                printTabs(level);
            SymbolTable* forScope = scopeSymbolTable(sym);
            if (stmt->val.forStmt.initStmt != NULL)
                symSTMT(sym, stmt->val.forStmt.initStmt, level - 1, false); // simpleStatement
            if (stmt->val.forStmt.condition != NULL)
                symEXPR(sym, stmt->val.forStmt.condition);
            if (stmt->val.forStmt.postStmt != NULL)
                symSTMT(sym, stmt->val.forStmt.postStmt, level - 1, false); // simpleStatement
            if (stmt->val.forStmt.stmtBlock != NULL)
                symSTMT(forScope, stmt->val.forStmt.stmtBlock, level + 1, true);
            if (prettySymbols)
                printf("\n");
            break;
        case k_statementKindContinue:
            break;
        case k_statementKindBreak:
            break;
        case k_statementKindExpression: // simple statement
            if (prettySymbols)
                printTabs(level);
            symEXPR(sym, stmt->val.expression.expression);
            if (onNewline && prettySymbols)
                printf("\n");
            break;
        }
        if (stmt->next != NULL)
            symSTMT(sym, stmt->next, level, onNewline);
    } else {
        throwError(stmt->lineno, "not a statement", 1);
    }
}

// ExprCaseClause = ExprSwitchCase ":" StatementList .
// ExprSwitchCase = "case" ExpressionList | "default" .
void symCASE(SymbolTable* sym, CASE* c, int level) {
    if (c != NULL) {
        switch (c->caseType) {
        case k_Case:
            symEXPRLIST(sym, c->matches);
            break;
        case k_Default:
            break;
        case k_notACase: // aka its an actual caseClause & not switchCase
            symCASE(sym, c->condition, level);
            break;
        }
        if (c->action != NULL) {
            symSTMT(sym, c->action, level + 1, true);
        }
        if (c->next != NULL) { //for next "case :" statement
            symCASE(sym, c->next, level);
        }
    }
}

//type should be inferred, so handle the RHS to symbol when it comes to typechecking
//TODO @TING HERE IT IS - ME FROM THE PAST
void symSHORTVARDECL(SymbolTable* sym, STMT* decl) {
    symARGSLIST(sym, decl->val.shortVarDecl.identifierList, 0, true);
}

//never add to scope here, only check it
//
void symEXPRLIST(SymbolTable* sym, EXPRLIST* exprList) {

    if (exprList == NULL)
        return;
    symEXPR(sym, exprList->content.thisEXPR);
    if (exprList->next != NULL) {
        symEXPRLIST(sym, exprList->next);
    } else
        return;
}

//expr->s assignments to do as part of typechecking, but in this file
//example:
//case k_expressionKindMinus:
//  expr->type = expr->lhs.type
//turns out we actually need expr struct to have symbols. to make sure the following fails to typecheck:
// var true int = 3
// var a int = true
// var b bool = true
void symEXPR(SymbolTable* sym, EXPR* expr) {

    switch (expr->kind) {
    case k_expressionKindIdentifier:
        // if (expr->val.identifier != NULL)
        // if (getSymbol(sym, k_symbolKindVariable, expr->val.identifier) == NULL)
        //     throwError(expr->lineno, "undeclared variable", 1);
        // else
            expr->symbol = getSymbol(sym, k_symbolKindVariable, expr->val.identifier); // TODO insertionSort problem
        break;
    case k_expressionKindBoolLiteral:
        expr->symbol = getSymbol(sym, k_symbolKindVariable, (expr->val.boolLiteral == true ? "true" : "false"));
        break;
    case k_expressionKindIntLiteral:
        //no ambiguity here
        break;
    case k_expressionKindFloatLiteral:
        //no ambiguity here
        break;
    case k_expressionKindStringLiteral:
        //no ambiguity here i think
        break;
    case k_expressionKindRawStringLiteral:
        //no ambiguity here i think
        break;
    case k_expressionKindRuneLiteral:
        //no ambiguity here
        break;
    case k_expressionKindPlus:
        //fallthrough
    case k_expressionKindMinus:
        //fallthrough
    case k_expressionKindNot:
        //fallthrough
    case k_expressionKindBitFlip:
        symEXPR(sym, expr->val.unaryExpr);
        break;
    case k_expressionKindMultiplication:
        //fallthrough
    case k_expressionKindDivision:
        //fallthrough
    case k_expressionKindAddition:
        //fallthrough
    case k_expressionKindSubtraction:
        symEXPR(sym, expr->val.binary.lhs);
        symEXPR(sym, expr->val.binary.rhs);
        break;
    case k_expressionKindGreaterEqual: //boolfor sure
        //fallthrough
    case k_expressionKindLessEqual:
        //fallthrough
    case k_expressionKindGreater:
        //fallthrough
    case k_expressionKindLess:
        //fallthrough
    case k_expressionKindEquals:
        //fallthrough
    case k_expressionKindNotEquals:
        //fallthrough
    case k_expressionKindAnd:
        //fallthrough
    case k_expressionKindOr:
        //fallthrough
    case k_expressionKindBitXor:
        //fallthrough
    case k_expressionKindBitOr:
        //fallthrough
    case k_expressionKindAndNot:
        //fallthrough
    case k_expressionKindBitAnd:
        symEXPR(sym, expr->val.binary.lhs);
        symEXPR(sym, expr->val.binary.rhs);
        break;
    case k_expressionKindRightShift:
        symEXPR(sym, expr->val.binary.lhs);
        symEXPR(sym, expr->val.binary.rhs);
        break;
    case k_expressionKindLeftShift:
        symEXPR(sym, expr->val.binary.lhs);
        symEXPR(sym, expr->val.binary.rhs);
        break;
    case k_expressionKindModulo:
        symEXPR(sym, expr->val.binary.lhs);
        symEXPR(sym, expr->val.binary.rhs);
        break;
    case k_expressionKindStructField:
        symEXPR(sym, expr->val.refobj.parent);
        expr->symbol = getSymbol(sym, k_symbolKindStruct, expr->val.refobj.fieldName);
        break;
    case k_expressionKindArrayVal:
        symEXPR(sym, expr->val.refobj.parent);
        symEXPR(sym, expr->val.refobj.index);
        expr->symbol = getSymbol(sym, k_symbolKindArray, expr->val.refobj.fieldName);
        break;
    case k_expressionKindFunctionCall:
        expr->symbol = getSymbol(sym, k_symbolKindFunction, expr->val.functionCall.methodName);
        symEXPRLIST(sym, expr->val.functionCall.arguments);
        break;
    case k_expressionKindLength:
        symEXPR(sym, expr->val.unaryExpr);
        break;
    case k_expressionKindCapacity:
        symEXPR(sym, expr->val.unaryExpr);
        break;
    case k_expressionKindSliceAppend:
        symEXPR(sym, expr->val.refobj.parent);
        symEXPR(sym, expr->val.refobj.appendValue);
        break;
    // case k_expressionKindStructFunctionCall:
        //     // expression tDOT tIDENTIFIER tLPAREN exprList tRPAREN
        //     symEXPR(sym, expr->val.functionCall.caller);
        //     expr->symbol = getSymbol(sym, expr->val.functionCall.methodName);
        //     symEXPRLIST(sym, expr->val.functionCall.arguments);
        //     break;
    }
}
