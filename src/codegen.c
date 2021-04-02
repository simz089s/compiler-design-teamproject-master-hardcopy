#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"

int postfix = 0; //for continue labels
int init_counter = 0; // counter for init()
int var_count = 0; // counter to generate variable name out of blank identifiers

void codePrintRaw(char* raw) {
    printf("`");
    for (int i = 1; raw[i] != '`'; i++) {
        switch (raw[i]) {
        case '\n':
            printf("\\n");
        case '\a':
            printf("\\a");
        case '\b':
            printf("\\b");
        case '\f':
            printf("\\f");
        case '\r':
            printf("\\r");
        case '\t':
            printf("\\t");
        case '\v':
            printf("\\v");
        case '\\':
            printf("\\\\");
        case '\'':
            printf("\\'");
        default:
            printf("%c", raw[i]);
        }
    }
    printf("`");
}

void codePrintTabs(int level) {
    // for (; level --> 0 ;) {
    for (int i = 0; i < level; i++) {
        printf("\t");
    }
}

void codePROG(PROG* prog) {
    if (prog != NULL) {
        codeSetup();
        codeTOPDECL(prog->topDecls, 0);
    }
}

void codeSetup() {
    codeINCLUDE();
    codeSetupSLICE();
    codeSetupSpecialFunctions();
}

void codeINCLUDE() {
    printf("\n\
#include <stdbool.h>\n\
#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\
    \n");
}

void codeSetupSLICE() {
    printf("\n\
#define DEFAULT_SLICE_SIZE 32\n\
typedef __golite_slice __golite_slice;\n\
struct __golite_slice {\n\
\tunion {\n\
\t\tbool content[DEFAULT_SLICE_SIZE];\n\
\t\tint content[DEFAULT_SLICE_SIZE];\n\
\t\tdouble content[DEFAULT_SLICE_SIZE];\n\
\t\tchar content[DEFAULT_SLICE_SIZE];\n\
\t} val;\n\
\tint length = 0;\n\
};\
    \n");
}

// TODO len(), cap(), append()
// Make one for each type?
void codeSetupSpecialFunctions() {
    printf("\n\
typedef __golite_len __golite_len;\n\
typedef __golite_cap __golite_cap;\n\
typedef __golite_append __golite_append;\n\
int __golite_len_int_array(int arr[]) {\n\
\treturn strlen(arr) / sizeof(int);\n\
}\n\
int __golite_len_int_slice(__golite_slice* slice) {\n\
\treturn slice.length;\n\
}\n\
int __golite_cap_int_slice(__golite_slice* slice) {\n\
\treturn strlen(slice.val.content) / sizeof(int);\n\
}\n\
    \n");
}

// common issues translating declarations
// naming conflicts with keywords handled in symbol table with codeName
// scoping differences
// blank identifiers TODO in progress
void codeTOPDECL(TOPDECL* topDecl, int level) {
    if (topDecl != NULL) {
        switch (topDecl->kind) {
        case k_declarationKindEmptyDistributedVarDecl:
        case k_declarationKindEmptyDistributedTypeDecl:
            // do nothing, not valid in C
            break;
        case k_declarationKindVariable:
            // var x int
            printf("\n");
            codeVARDECL(topDecl, level);
            printf("\n");
            break;
        case k_declarationKindShortVariable:
            // x := 0
            codeSHORTVARDECL(genSTMT_TOPDECL(topDecl), level); // genSTMT_TOPDECL used for topDecl->Stmt conversion
            break;
        case k_declarationKindType:
            // type x int
            printf("\n");
            codeTYPEDECL(topDecl, level);
            break;
        case k_declarationKindFunction:
            codeFUNCTION(topDecl->funcdecl.identifier, topDecl->funcdecl.sig, topDecl->funcdecl.block, level);
            if (strcmp(topDecl->funcdecl.identifier, "main") == 0) { // func main () { ... }
                char* funcName = "__golite__main";
                codeFUNCTION(funcName, topDecl->funcdecl.sig, topDecl->funcdecl.block, level);
            } else if (strcmp(topDecl->funcdecl.identifier, "init") == 0) { // func init() { ... }
                init_counter++;
                char funcName[32] = {
                    0,
                };
                snprintf(funcName, sizeof(funcName), "__golite__init%d", init_counter);
                codeFUNCTION(funcName, topDecl->funcdecl.sig, topDecl->funcdecl.block, level);
            } else { // user-declared functions
                codeFUNCTION(topDecl->funcdecl.identifier, topDecl->funcdecl.sig, topDecl->funcdecl.block, level);
            }
            break;
        }
        if (topDecl->next != NULL) {
            if (topDecl->kind != k_declarationKindType)
                printf("\n");
            codeTOPDECL(topDecl->next, level);
        }
        codeMAIN(); // assume that test cases always include a main method
    }
}

/* stores main to be called after all the init
int main() { // called with any number of arguments
    main body
    return 0; 
}
*/
void codeMAIN() {
    printf("int main() {\n"); // function main takes no arguments and returns no value in Go
    for (int i = 0; i < init_counter; i++) {
        printf("__golite__init%d();", i); // invokes init functions in lexical order
    }
    printf("__golite__main();"); // invokes main()
    printf("return 0;\n");
    printf("}\n");
}

// TODO please review - Su 04/04/19
// variable definition
// implicit initialization : var x int -> int x;
// multiple declations : var a, b int -> int a, b;
// shadowing of true and false constants : var true bool = false TODO @Ting done in symbol ? yes
// scoping TODO @Ting renaming done in symbol ? yes
void codeVARDECL(TOPDECL* decl, int level) {
    if (decl != NULL && decl->kind == k_declarationKindVariable) {
        codePrintTabs(level);
        codeTYPE(decl->vardecl.content->type, level);
        printf(" ");
        codeVAR(decl->vardecl.content, level);
        // C variables must be initialized with default values
        // so we are explicitly initializing all variable declarations
        // TODO @Ting is this done in symbol? no, initialize float64 and int to 0, string to "" char to '' etc
        // int a, b , c = 5; only c is initialized to 5
        // int a = 0, b = f();
        codeEXPR(decl->vardecl.content->expression);
        printf(";\n");
        if (decl->vardecl.next != NULL) {
            printf("\n");
            codeVARDECL(decl->vardecl.next, level);
        }
        if (decl->vardecl.distributedNext != NULL) {
            printf("\n");
            codeVARDECL(decl->vardecl.distributedNext, level);
        }
    }
}

// type declaration

// in C: typdef gives symbolic names to types
/*
typedef unsigned char BYTE;
typedef BYTE BYTE2;
typedef int NUM;
*/
void codeTYPEDECL(TOPDECL* decl, int level) {
    printf("typedef ");
    // question was it typedecl.identifier again?
    switch (decl->symbol->symKind) {
    case k_symbolKindFunction:
    case k_symbolKindVariable: // var a int
        // not supposed to be this kind
        break;
    case k_symbolKindArray:
    case k_symbolKindSlice:
    case k_symbolKindStruct:
        // or
        /*
        typedef struct optionalStructName optionalStructName;
        struct optionalStructName { 
            fieldType1 fieldName1;
            fieldType2 fieldName2;
        };

        we will generate the optionalStructAlias for every 

        */
        printf("struct ");
    case k_symbolKindNamedType: // typedef type1 type2
        break;
    }
    // printf("%s", decl->typedecl.content); // baseType
    // namedType
    // printf("%s struct {\n", type->symbol->name);
    // TODO use codeARGLIST instead of making new?
    // codeFIELDDECL(type->structfield, level + 1);
    // TODO should we access SYMBOL or the TYPE inner struct?
    codePrintTabs(level);
    printf("}\n");
}

// TODO
// helper function for codeTYPEDECL() and others *IMPORTANT! (for indentation etc.)
// vvvvvvvvvvv BELOW MAY NOT APPLY WITH NEW AST STRUCTURE ! vvvvvvvvvvv
// arrays have size > 0 [n]array_name
// slices have size = 0 []slice_name
// structs have size -1 struct_name
// special case for function return types? (e.g. should struct return not have newlines etc.)
// ********************************************************************
// TODO @Ting i forgot how we're handling this let me do it -ting
// do i just print the codeName or the baseType assigned in the symbol?
// do we represent void in symbol ?
void codeTYPE(TYPE* type, int level) {
    switch (type->kind) {
    case namedType:
        printf("%s ", type->named.name);
        break;
    case arrayType:
        codeTYPE(type->array.r, level);
        printf("*");
        break;
    case sliceType:
        printf("__golite_slice"); // How to print slice type? __golite_slice... or internal array?
        break;
    case structType:
        codeSTRUC(type->struc.structfield, type->struc.structName, level);
        break;
    }
}

// Type Declaration in C
/*
example : struct definition in C
typedef struct tag_name {
   type member1;
   type member2;
} struct_alias;
*/
void codeTYPEDEF(TOPDECL* decl, int level) {
    printf("typedef ");
    codeTYPE(decl->typedecl.content, level);

    // switch (type->kind) {
    // case namedType:
    //     printf("%s ", type->named.name);
    //     break;

    // case arrayType:
    //     printf("[%d]", type->array.size);
    //     codeTYPE(type->array.r, level);
    //     break;

    // case sliceType:
    //     printf("[]");
    //     codeTYPE(type->slice, level);
    //     break;

    // case structType:
    //     printf("%s struct {\n", type->symbol->name);
    //     // TODO use codeARGLIST instead of making new?
    //     codeFIELDDECL(type->structfield, level + 1); // TODO should we access SYMBOL or the TYPE inner struct?
    //     codePrintTabs(level);
    //     printf("}\n");
    //     break;
    // }
}

//TODO
// print one field declarations within a struct
/*
 * struct aStruct {
 *     int x, y, z;
 *     char* s1, s2;
 * };
 */
void codeSTRUC(FIELDDECL* fieldDecl, char* structName, int level) {
    printf("struct %s {\n", structName);
    codeFIELDDECL(fieldDecl, level + 1);
    codePrintTabs(level);
    printf("};\n");
}

// TODO
void codeFIELDDECL(FIELDDECL* fd, int lvl) {
    if (fd != NULL) {
        codePrintTabs(lvl);
        codeTYPE(fd->regular.type, lvl); // TODO make sure to get the base primitive type !!!
        printf(" ");
        codeARGLIST(fd->regular.identifierlist, lvl);
        printf(";\n");
        if (fd->next != NULL)
            codeFIELDDECL(fd->next, lvl); // next struct field declaration
    }
}

/* Function Definition
 * return_type function_name( parameter list ) {
 *    body of the function
 * }
 */
// TODO are we handling this case ? func foo(a struct { a int; }) { ... } (tydef struct)
void codeFUNCTION(char* functionName, SIGNATURE* signature, STMT* block, int level) {
    if (signature->optionalReturn == NULL) {
        printf("void "); // return void
    } else {
        codeTYPE(signature->optionalReturn, level); // return_type
    }
    printf("%s(", functionName);
    codeSIGDECL(signature->decls, level);
    printf(") {\n");
    codeSTMT(block, level, true, NULL); // true: onNewline, NULL: no continueLabel
    printf("}\n");
}

//TODO
// func main() {}
// void codeFUNCDECL(char* identifier, SIGNATURE* signature, STMT* stmtBlock) {
//     printf("%s(", identifier);
//     if (signature->decls != NULL) {
//         // Function parameters/arguments
//         switch (signature->decls->kind) {
//         // func f(x int, y int) vs f(x,y int)
//         case k_signatureKindLong: // one id with one type
//         case k_signatureKindShort: // many ids with one type
//             codeFUNCSIG(signature);
//             break;
//         }
//     }
//     printf(") ");
//     if (signature->optionalReturn)
//         codeTYPE(signature->optionalReturn, 0); // new helper function for function return types?
//     printf("{\n");
//     if (stmtBlock != NULL)
//         codeSTMT(stmtBlock, 1, true, NULL);
//     printf("\n}\n");
// }

// no longer used as of 3/8
// print function signature in long declaration form
// void codeLONGFUNCSIG(SIGNATURE* sig, int level)
// {
//     while (sig->decls != NULL) {
//         printf("%s ", sig->decls->parameters.identifier);
//         codeTYPE(sig->decls->valType, level);
//         if (sig->decls->next != NULL)
//             printf(", ");
//         sig->decls = sig->decls->next;
//     }
// }

//TODO
// void codeFUNCSIG(SIGNATURE* sig) {
//     if (sig->decls != NULL) {
//         codeSIGDECL(sig->decls, 0);
//     }
// }

//TODO
// x int, y int, z float64 or x,y int, z float64 etc.
// No distinction between long and short sig apparently (should refactor)
void codeSIGDECL(SIGDECL* sd, int lvl) {
    lvl = lvl + 0;
    if (sd->kind == k_signatureKindShort) { // TODO there is only one kind of sig
        // switch (sd->symbol->symKind) {
        // case k_signatureKindLong:
        //     codeARGLIST(sd->parameters.identifierList, lvl);
        //     break;

        // case k_signatureKindShort:
        //     // TODO hwo to deal with short sig
        //     codeARGLISTShort(); // TODO ? otherwise, for loop below vvvvvvvv
        //     for (SIGDECL* current = sd; current != NULL; current = current->next) {
        //         if (sd->valType != NULL) // does this make sense ?
        //             codeTYPE(sd->valType, lvl);
        //         if (sd->next != NULL) {
        //             printf(", ");
        //         }
        //     }
        //     break;
        // default:
    }
}

//TODO
// Should print the type or not? if yes does not work for "x,y,z int" ?
// TODO name blank identifiers with variable names
void codeARGLIST(ARGLIST* al, int lvl) {
    codeVAR(al->thisVAR, lvl);
    if (al->next != NULL) {
        printf(", ");
        codeARGLIST(al->next, lvl);
    }
}

//TODO
void codeVAR(VAR* v, int lvl) {
    if (strcmp(v->symbol->codeName, "_") == 0) { //TODO is this correct
        printf("%s%d", v->symbol->codeName, var_count);
    } else {
        printf("%s ", v->symbol->codeName); //TODO CHANGE ALL THE SHIT TO SYMBOL INSTEAD
    }
    if (v->type != NULL)
        codeTYPE(v->type, lvl);
    if (v->expression != NULL) {
        printf("= ");
        codeEXPR(v->expression);
    }
}

//Helper function that codegens the body of a 3-part-for statement, with input poststmt
void codeForBody(STMT* body, int level, STMT* post, char* thislabel) {
    codePrintTabs(level);
    printf("{\n");

    //create continue label
    char temp[100];
    sprintf(temp, "%d", postfix++);
    char* label = strcat("__golite_temp_continue", temp);

    codeSTMT(body, level + 1, true, label);

    codePrintTabs(level);
    printf("}\n");

    printf("%s: ;\n", label);
    codeSTMT(post, level, true, thislabel);
}

//TODO
void codeSTMT(STMT* stmt, int level, bool onNewline, char* continuelabel) {
    if (stmt != NULL) {
        switch (stmt->kind) {
        case k_statementKindEmptyStmt: // empty statement
            break;
        case k_statementKindShortVarDecl: // simple statement
            codePrintTabs(level);
            codeSHORTVARDECL(stmt, level);
            if (onNewline)
                printf("\n");
            break;
        case k_statementKindAssignment: // simple statement, e.g. "<=", "+="
            codePrintTabs(level);
            codeEXPR(stmt->val.assignment.identifier);
            printf(" ");
            switch (stmt->val.assignment.opassign) {
            case k_assignOpAddition:
                printf("+");
                break;
            case k_assignOpMult:
                printf("*");
                break;
            case k_assignOpDiv:
                printf("/");
                break;
            case k_assignOpModulo:
                printf("%%");
                break;
            case k_assignOpLeftShift:
                printf("<<");
                break;
            case k_assignOpRightShift:
                printf(">>");
                break;
            case k_assignOpBitAnd:
                printf("&");
                break;
            case k_assignOpAndNot:
                printf("&^");
                break;
            case k_assignOpSubtraction:
                printf("-");
                break;
            case k_assignOpBitOr:
                printf("|");
                break;
            case k_assignOpBitXor:
                printf("^");
                break;
            }
            printf("= ");
            codeEXPR(stmt->val.assignment.expression);
            if (onNewline)
                printf("\n");
            break;
        case k_statementKindExprlistAssignment: // simple statement, "="
            codePrintTabs(level);
            codeEXPRLIST(stmt->val.exprlistAssignment.LHS);
            printf(" = ");
            codeEXPRLIST(stmt->val.exprlistAssignment.RHS);
            if (onNewline)
                printf("\n");
            break;
        case k_statementKindDeclaration:
            codePrintTabs(level);
            codeTOPDECL(stmt->val.topDecl.topDecl, level); // prints newline
            break;
        case k_statementKindIncrement: // simple statement
            codePrintTabs(level);
            codeEXPR(stmt->val.incDec.expression);
            printf("++");
            if (onNewline)
                printf("\n");
            break;
        case k_statementKindDecrement: // simple statement
            codePrintTabs(level);
            codeEXPR(stmt->val.incDec.expression);
            printf("--");
            if (onNewline)
                printf("\n");
            break;
        case k_statementKindPrint:
            codePrintTabs(level);
            printf("printf(\"");
            for (EXPRLIST* current = stmt->val.print.exprlist; current != NULL; current = current->next) {
                printf("%%s");
            }
            printf("\", ");
            codeEXPRLIST(stmt->val.print.exprlist);
            printf(");\n");
            break;
        case k_statementKindPrintLn:
            codePrintTabs(level);
            printf("printf(\"");
            for (EXPRLIST* current = stmt->val.print.exprlist; current != NULL; current = current->next) {
                printf("%%s");
            }
            printf("\", ");
            codeEXPRLIST(stmt->val.print.exprlist);
            printf(");\n");
            break;
        case k_statementKindReturn:
            codePrintTabs(level);
            printf("return ");
            if (stmt->val.returnStmt.expr != NULL) {
                codeEXPR(stmt->val.returnStmt.expr);
            }
            // if (onNewline) printf("\n");
            break;
        case k_statementKindIf:
            // "if" [ SimpleStmt ";" ] Expression Block [ "else" ( IfStmt | Block ) ]
            if (onNewline)
                codePrintTabs(level);
            printf("if ");
            if (stmt->val.ifStmt.simpleStmt != NULL) {
                codeSTMT(stmt->val.ifStmt.simpleStmt, level, false, continuelabel); // not onNewline
                printf("; ");
            }
            codeEXPR(stmt->val.ifStmt.condition);
            printf(" {\n");
            if (stmt->val.ifStmt.stmt != NULL)
                codeSTMT(stmt->val.ifStmt.stmt, level + 1, true, continuelabel); // stmt block goes on newline
            codePrintTabs(level); // indent the closing bracket
            printf("}%s", (stmt->val.ifStmt.elseStmt == NULL ? "\n" : " "));
            if (stmt->val.ifStmt.elseStmt != NULL)
                codeSTMT(stmt->val.ifStmt.elseStmt, level, true, continuelabel);
            break;
        case k_statementKindElse:
            printf("else {\n");
            if (stmt->val.elseStmt.stmt != NULL)
                codeSTMT(stmt->val.elseStmt.stmt, level, true, continuelabel);
            codePrintTabs(level);
            printf("}\n");
            break;
        case k_statementKindElseIf:
            printf("else ");
            codeSTMT(stmt->val.elseIfStmt.ifStmt, level, true, continuelabel);
            break;
        case k_statementKindSwitch:
            codePrintTabs(level);
            printf("switch ");
            if (stmt->val.switchStmt.simpleStmt != NULL) {
                codeSTMT(stmt->val.switchStmt.simpleStmt, level, false, continuelabel);
                printf("; ");
            }
            if (stmt->val.switchStmt.optionalExpr != NULL)
                codeEXPR(stmt->val.switchStmt.optionalExpr);
            printf(" {\n");
            codeCASE(stmt->val.switchStmt.c, level + 1, continuelabel);
            codePrintTabs(level);
            printf("}\n");
            break;

        //Done
        case k_statementKindForCondition: // condition only: a while loop
            codePrintTabs(level);
            printf("while( ");
            if (stmt->val.forStmt.condition != NULL)
                codeEXPR(stmt->val.forStmt.condition);
            printf(") {\n");
            if (stmt->val.forStmt.stmtBlock)
                codeSTMT(stmt->val.forStmt.stmtBlock, level + 1, true, continuelabel);
            codePrintTabs(level);
            printf("}\n");
            break;

        //INIT before, POST inside with GOTO
        case k_statementKindFor: // initial stmt, condition, post stmt
            // "for" [ Condition | ForClause | RangeClause ] Block .
            // ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
            codePrintTabs(level);
            printf("{\n");
            codePrintTabs(level + 1);
            if (stmt->val.forStmt.initStmt != NULL)
                codeSTMT(stmt->val.forStmt.initStmt, level + 1, false, continuelabel); // simpleStatement
            printf("while (");
            if (stmt->val.forStmt.condition != NULL)
                codeEXPR(stmt->val.forStmt.condition);
            printf(" ){\n");

            if (stmt->val.forStmt.stmtBlock != NULL) {
                if (stmt->val.forStmt.postStmt != NULL) {
                    codeForBody(stmt->val.forStmt.stmtBlock, level + 2, stmt->val.forStmt.postStmt, continuelabel);
                } else {
                    codeForBody(stmt->val.forStmt.stmtBlock, level + 2, NULL, continuelabel);
                }
            } else if (stmt->val.forStmt.postStmt != NULL) { //empty body, so just perform poststmt as many times as the loop iterates
                codeSTMT(stmt->val.forStmt.postStmt, level + 2, true, continuelabel);
            }

            codePrintTabs(level + 1);
            printf("}\n");
            codePrintTabs(level);
            printf("}\n");
            break;
        //done
        case k_statementKindContinue: // unlabelled
            codePrintTabs(level);
            printf("goto %s;\n", continuelabel);
            break;
        //does not need modification
        case k_statementKindBreak: // unlabelled
            codePrintTabs(level);
            printf("break;\n");
            break;
        case k_statementKindExpression: // simple statement
            codePrintTabs(level);
            codeEXPR(stmt->val.expression.expression);
            if (onNewline)
                printf("\n");
            break;
        }
        if (stmt->next != NULL)
            codeSTMT(stmt->next, level, onNewline, continuelabel);
    } else {
        throwError(stmt->lineno, "not a statement", 1);
    }
}

// ExprCaseClause = ExprSwitchCase ":" StatementList .
// ExprSwitchCase = "case" ExpressionList | "default" .

//TODO
// case 0:
// case a.b, 0:
// case a.b > 0: exprlist
// case int: not supported
// default:
void codeCASE(CASE* c, int level, char* continuelabel) {
    if (c != NULL) {
        switch (c->caseType) {
        case k_Case:
            codePrintTabs(level);
            printf("case ");
            codeEXPRLIST(c->matches);
            printf(":\n");
            break;
        case k_Default:
            codePrintTabs(level);
            printf("default:\n");
            break;
        case k_notACase: // aka it's an actual caseClause & not switchCase
            codeCASE(c->condition, level, continuelabel);
            printf("\n");
            break;
        }

        if (c->action != NULL) {
            codeSTMT(c->action, level + 1, true, continuelabel);
        }
        if (c->next != NULL) { // for next "case :" statement
            printf("\n");
            codeCASE(c->next, level, continuelabel);
        }
    }
}

// TODO please review - Su 04/04/19
// short variable declarations converted to successive variable declarations
/* GoLite -> C
var x , y = 2, 4 -> int x = 2, y = 4;
x, y, z := 1, 0, 0 -> x = 1; y = 0; int z = 0;
*/
// TODO add isDeclared field for variables
//@TING note to self I missed this in symbol table
void codeSHORTVARDECL(STMT* decl, int level) {

    ARGLIST* al = decl->val.shortVarDecl.identifierList;
    EXPRLIST* el = decl->val.shortVarDecl.exprList;
    for (; al != NULL; al = al->next, el = el->next) {
        VAR* var = al->thisVAR;
        EXPR* expr = el->content.thisEXPR;
        if (!isDeclared(var)) { // TODO isDeclared
            codeTYPE(var->type, level);
        }
        printf(" ");
        codeVAR(var, level);
        printf(" = ");
        codeEXPR(expr);
        printf(";\n");
    }
}

// TODO if we use this we will have to connect the symbol table to codegen?
// Same as symbol table? see isDefined()
bool isDeclared(VAR* var) { // TODO should arglist
    // var->symbol->
    var = var + 0;

    return false;
}

//
void codeEXPRLIST(EXPRLIST* exprList) {

    if (exprList == NULL)
        return;
    codeEXPR(exprList->content.thisEXPR);
    if (exprList->content.type != NULL) //?? when will this happen?
        codeTYPE(exprList->content.type, 0); //add tabs to codeEXPRLIST for this later
    if (exprList->next != NULL) {
        printf(", ");
        codeEXPRLIST(exprList->next);
    } else
        return;
}

void codeEXPR(EXPR* expr) {

    switch (expr->kind) {
    case k_expressionKindIdentifier:
        // if (expr->val.identifier != NULL)
        printf("%s", expr->val.identifier);
        break;
    case k_expressionKindBoolLiteral:
        printf("%s", expr->val.boolLiteral ? "true" : "false");
        break;
    case k_expressionKindIntLiteral:
        printf("%i", expr->val.intLiteral);
        break;
    case k_expressionKindFloatLiteral:
        printf("%f", (double)expr->val.floatLiteral);
        break;
    case k_expressionKindStringLiteral:
        printf("%s", expr->val.stringLiteral);
        break;
    case k_expressionKindRawStringLiteral:
        codePrintRaw(expr->val.stringLiteral);
        break;
    case k_expressionKindRuneLiteral:
        printf("%s ", expr->val.runeLiteral);
        break;
    case k_expressionKindPlus:
        printf("+(");
        codeEXPR(expr->val.unaryExpr);
        printf(")");
        break;
    case k_expressionKindMinus:
        printf("-(");
        codeEXPR(expr->val.unaryExpr);
        printf(")");
        break;
    case k_expressionKindNot:
        printf("!(");
        codeEXPR(expr->val.unaryExpr);
        printf(")");
        break;
    case k_expressionKindBitFlip:
        printf("^(");
        codeEXPR(expr->val.unaryExpr);
        printf(")");
        break;
    case k_expressionKindMultiplication:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" * ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindDivision:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" / ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindAddition:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" + ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindSubtraction:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" - ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindGreaterEqual:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" >= ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindLessEqual:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" <= ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindGreater:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" > ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindLess:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" < ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindEquals:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" == ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindNotEquals:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" != ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindAnd:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" && ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindOr:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" || ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindBitXor:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" ^ ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindBitOr:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" | ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindAndNot:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" &^ ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindBitAnd:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" & ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindRightShift:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" >> ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindLeftShift:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" << ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindModulo:
        printf("(");
        codeEXPR(expr->val.binary.lhs);
        printf(" %% ");
        codeEXPR(expr->val.binary.rhs);
        printf(")");
        break;
    case k_expressionKindStructField:
        printf("(");
        codeEXPR(expr->val.refobj.parent);
        printf(".");
        printf("%s", expr->val.refobj.fieldName);
        printf(")");
        break;
    case k_expressionKindArrayVal:
        printf("(");
        codeEXPR(expr->val.refobj.parent);
        printf("[");
        codeEXPR(expr->val.refobj.index);
        printf("])");
        break;
    case k_expressionKindFunctionCall:
        printf("%s(", expr->val.functionCall.methodName);
        codeEXPRLIST(expr->val.functionCall.arguments);
        printf(")");
        break;
    case k_expressionKindLength:
        printf("__golite_len(");
        codeEXPR(expr->val.unaryExpr);
        printf(")");
        break;
    case k_expressionKindCapacity:
        printf("__golite_cap(");
        codeEXPR(expr->val.unaryExpr);
        printf(")");
        break;
    case k_expressionKindSliceAppend:
        printf("__golite_append((");
        codeEXPR(expr->val.refobj.parent);
        printf("),");
        codeEXPR(expr->val.refobj.appendValue);
        printf("))");
        break;
        // case k_expressionKindStructFunctionCall: // TODO REMOVE
        //     // a.f()
        //     // counts as a functionCall
        //     printf("(");
        //     // codeEXPR(expr->val.refobj.parent);
        //     codeEXPR(expr->val.functionCall.caller);
        //     printf(".");
        //     // printf("%s", expr->val.refobj.fieldName);
        //     printf("%s(", expr->val.functionCall.methodName);
        //     codeEXPRLIST(expr->val.functionCall.arguments);

        //     printf(")");
        //     break;
    }
}
