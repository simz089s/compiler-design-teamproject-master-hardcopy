#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pretty_printer.h"

// prints raw string
void printRaw(char* raw) {
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

void printTabs(int level) {
    for (int i = 0; i < level; i++) {
        printf("\t");
    }
}

void prettyPROG(PROG* prog) {
    if (prog != NULL) {
        prettyPKG(prog->package);
        prettyTOPDECL(prog->topDecls,0);
    }
}

void prettyPKG(PKG* pkg) {
    printf("package %s\n", pkg->packageName);
}

void prettyTOPDECL(TOPDECL* topDecl, int level) {
    if (topDecl != NULL) {
        switch (topDecl->kind) {
        case k_declarationKindEmptyDistributedVarDecl: // do nothing 
            break;
        case k_declarationKindEmptyDistributedTypeDecl:
            break;
        case k_declarationKindVariable:
            printf("\n");
            prettyVARDECL(topDecl, level);
            printf("\n");
            break;
        case k_declarationKindShortVariable:
            prettySHORTVARDECL(genSTMT_TOPDECL(topDecl));
            break;
        case k_declarationKindType:
            printf("\n");
            prettyTYPEDECL(topDecl, level);
            break;
        case k_declarationKindFunction:
            prettyFUNCDECL(topDecl->funcdecl.identifier, topDecl->funcdecl.sig,
                topDecl->funcdecl.block);
            break;
        }
        if (topDecl->next != NULL) {
            
            if(topDecl->kind != k_declarationKindType) printf("\n");
            prettyTOPDECL(topDecl->next, level);
        }
    }
}

void prettyVARDECL(TOPDECL* decl, int level) {

    if (decl != NULL && decl->kind == k_declarationKindVariable) {
        printTabs(level);
        printf("var ");
        prettyVAR(decl->vardecl.content, level);
        if(decl->vardecl.next != NULL){
            printf("\n");
            prettyVARDECL(decl->vardecl.next, level);
        }        
        if(decl->vardecl.distributedNext!=NULL){
            printf("\n");
            prettyVARDECL(decl->vardecl.distributedNext, level);
        }
    }




    // if (decl != NULL && decl->kind == k_declarationKindVariable) {
    //     printTabs(level);
    //     printf("var ");
    //     while (decl->vardecl.content != NULL) {
    //         printf("%s ", decl->vardecl.content->identifier);
    //         // support three forms of variable specifications
    //         // type only, leaving out the expression
    //         // expression only, leaving out the type
    //         // type and expression
    //         if (decl->vardecl.content->type != NULL)
    //             prettyTYPE(decl->vardecl.content->type, level);
    //         if (decl->vardecl.content->expression != NULL) {
    //             printf(" = ");
    //             prettyEXPR(decl->vardecl.content->expression);
    //         }
    //         if (decl->vardecl.next != NULL) { // multiple identifiers in one line
    //             printf("\n");
    //             printTabs(level + 1);
    //             printf("var ");
    //             decl = decl->vardecl.next;
    //         } 
    //         if (decl->vardecl.distributedNext != NULL) {
    //             prettyVARDECL(decl->vardecl.distributedNext, level);
    //         } else {
    //             break;
    //         }
    //     }
    //     printf("\n");
    // } else {
    //     throwError(decl->lineno, "not a variable declaration", 1);
    // }
}

// correct
void prettyTYPEDECL(TOPDECL* decl, int level) {
    if (decl != NULL && decl->kind == k_declarationKindType) {
        printTabs(level);
        printf("type ");
        prettyTYPE(decl->typedecl.content, level);

       

    } else {
        throwError(decl->lineno, "not a type declaration", 1);
    }
    if(decl->typedecl.distributedNext != NULL){
        printf("\n");
        prettyTYPEDECL(decl->typedecl.distributedNext, level);    
    }
    
}

// correct
void prettyTYPE(TYPE* type, int level) {

    switch(type->kind){
        case arrayType:
            printf("[%d]", type->array.size);
            break;
        case sliceType:
            printf("[]");
            break;
        case structType:
            if(type->struc.structfield!=NULL){
                printf("{\n");
                if(level==-1){
                    prettySTRUCSINGLELINE(type->struc.structfield);
                }
                else{
                    prettySTRUC(type->struc.structfield, level+1);
                }
                printTabs(level);
                printf("}\n");
            }
            else{
                printf("{\n");
                printTabs(level);
                printf("}\n");
            }
            break;

        case namedType: 
            printf("%s ", type->named.name);
    }

    // if (type != NULL) {
    //     if(strcmp(type->identifier,"")) printf("%s ", type->identifier);
    //     if (type->size == TYPE_SIZE_SLICE) // print slice
    //         printf("[]");
    //     if (type->size > 0) // print array
    //         printf("[%d]", type->size);
    //     if (type->baseType != NULL)
    //         prettyTYPE(type->baseType, level);
    //     else if (type->struc != NULL) { // print struct
    //     // printTabs(level) commented out 3/15 by ting
    //         printf("{\n");
    //         prettySTRUC(type->struc, level + 1);
    //         printTabs(level);
    //         printf("}\n");
    //     }
    //     else if (type->size == TYPE_SIZE_EMPTYSTRUCT){
    //         printf("{\n");
    //         printTabs(level);
    //         printf("}\n");
    //     }
    // }
}

// helper function for prettyTYPE()
// print one field declarations within a struct
void prettySTRUC(FIELDDECL* fieldDecl, int level) {
    if (fieldDecl != NULL) {
        printTabs(level);
        prettyARGSLIST(fieldDecl->regular.identifierlist, level);
        prettyTYPE(fieldDecl->regular.type, level);
        printf("\n");
        if (fieldDecl->next != NULL)
            prettySTRUC(fieldDecl->next, level); // next struct field declaration
    }
}

void prettySTRUCSINGLELINE(FIELDDECL* fieldDecl) {
    if (fieldDecl != NULL) {
        prettyARGSLIST(fieldDecl->regular.identifierlist, -1);
        prettyTYPE(fieldDecl->regular.type, -1);
        if (fieldDecl->next != NULL)
            prettySTRUCSINGLELINE(fieldDecl->next); // next struct field declaration
    }
}

void prettyFUNCDECL(char* identifier, SIGNATURE* signature, STMT* stmtBlock) {
    printf("func %s(", identifier);
    if (signature->decls != NULL) {
        switch (signature->decls->kind) {
        case k_signatureKindLong: // one id with one type
        case k_signatureKindShort: // many ids with one type
            prettyFUNCSIG(signature);
            break;
        }
    }
    printf(")");
    if (signature->optionalReturn)
        prettyTYPE(signature->optionalReturn, -1); //same as prettyRETURN but doesn't insert newlines
    printf(" {\n");
    if (stmtBlock != NULL)
        prettySTMT(stmtBlock, 1, true);
    printf("\n}\n");
}

// no longer used as of 3/8
// print function signature in long declaration form
// void prettyLONGFUNCSIG(SIGNATURE* sig, int level)
// {
//     while (sig->decls != NULL) {
//         printf("%s ", sig->decls->parameters.identifier);
//         prettyTYPE(sig->decls->valType, level);
//         if (sig->decls->next != NULL)
//             printf(", ");
//         sig->decls = sig->decls->next;
//     }
// }

// print function signature in short declaration form
void prettyFUNCSIG(SIGNATURE* sig) {
    if (sig->decls != NULL) {
        prettySIGDECL(sig->decls, 0);
    }
}

void prettySIGDECL(SIGDECL* sd, int l) {
    prettyARGSLIST(sd->parameters.identifierList, l);
    if (sd->valType != NULL)
        prettyTYPE(sd->valType, l);
    if (sd->next != NULL) {
        printf(", ");
        prettySIGDECL(sd->next, l);
    }
}

void prettyARGSLIST(ARGLIST* al, int l) {
    prettyVAR(al->thisVAR, l);
    if (al->next != NULL) {
        printf(", ");
        prettyARGSLIST(al->next, l);
    }
}

void prettyVAR(VAR* v, int l) {
    printf("%s ", v->identifier);
    if (v->type != NULL)
        prettyTYPE(v->type, l);
    if (v->expression != NULL) {
        printf("= ");
        prettyEXPR(v->expression);
    }
}

void prettySTMT(STMT* stmt, int level, bool onNewline) {
    if (stmt != NULL) {
        switch (stmt->kind) {
        case k_statementKindEmptyStmt: // empty statement
            break;
        case k_statementKindShortVarDecl: // simple statement
            printTabs(level);
            prettySHORTVARDECL(stmt);
            if (onNewline) printf("\n");
            break;
        case k_statementKindAssignment: // simple statement, e.g. "<=", "+="
            printTabs(level);
            prettyEXPR(stmt->val.assignment.identifier);
            printf(" ");
            switch (stmt->val.assignment.opassign) {
                case k_assignOpAddition :
                    printf("+");
                    break;
                case k_assignOpMult :
                    printf("*");
                    break;
                case k_assignOpDiv :
                    printf("/");
                    break;
                case k_assignOpModulo :
                    printf("%%"); 
                    break;
                case k_assignOpLeftShift :
                    printf("<<");
                    break;
                case k_assignOpRightShift :
                    printf(">>");
                    break;
                case k_assignOpBitAnd :
                    printf("&");
                    break;
                case k_assignOpAndNot : 
                    printf("&^");
                    break;
                case k_assignOpSubtraction : 
                    printf("-");
                    break;
                case k_assignOpBitOr :
                    printf("|");
                    break;
                case k_assignOpBitXor :
                    printf("^");
                    break;
            }
            printf("= ");
            prettyEXPR(stmt->val.assignment.expression);
            if (onNewline) printf("\n");
            break;
        case k_statementKindExprlistAssignment: // simple statement, "="
            printTabs(level);
            prettyEXPRLIST(stmt->val.exprlistAssignment.LHS);
            printf(" = ");
            prettyEXPRLIST(stmt->val.exprlistAssignment.RHS);
            if (onNewline) printf("\n");
            break;
        case k_statementKindDeclaration:
            printTabs(level);
            prettyTOPDECL(stmt->val.topDecl.topDecl, level); // prints newline
            break;
        case k_statementKindIncrement: // simple statement
            printTabs(level);
            prettyEXPR(stmt->val.incDec.expression);
            printf("++");
            if (onNewline) printf("\n");
            break;
        case k_statementKindDecrement: // simple statement
            printTabs(level);
            prettyEXPR(stmt->val.incDec.expression);
            printf("--");
            if (onNewline) printf("\n");
            break;
        case k_statementKindPrint:
            printTabs(level);
            printf("print(");
            prettyEXPRLIST(stmt->val.print.exprlist);
            printf(")\n");
            break;
        case k_statementKindPrintLn:
            printTabs(level);
            printf("println(");
            prettyEXPRLIST(stmt->val.print.exprlist);
            printf(")\n");
            break;
        case k_statementKindReturn:
            printTabs(level);
            printf("return ");
            if (stmt->val.returnStmt.expr != NULL) {
                prettyEXPR(stmt->val.returnStmt.expr);
            }
            // if (onNewline) printf("\n");
            break;
        case k_statementKindIf: 
            // "if" [ SimpleStmt ";" ] Expression Block [ "else" ( IfStmt | Block ) ]
            if (onNewline) printTabs(level);
            printf("if ");
            if (stmt->val.ifStmt.simpleStmt != NULL) {
                prettySTMT(stmt->val.ifStmt.simpleStmt, level, false); // not onNewline
                printf("; ");
            }
            prettyEXPR(stmt->val.ifStmt.condition);
            printf(" {\n");
            if (stmt->val.ifStmt.stmt != NULL)
                prettySTMT(stmt->val.ifStmt.stmt, level + 1, true); // stmt block goes on newline
            printTabs(level); // indent the closing bracket
            printf("}%s", (stmt->val.ifStmt.elseStmt == NULL) ? "\n" : " ");
            if (stmt->val.ifStmt.elseStmt != NULL)
                prettySTMT(stmt->val.ifStmt.elseStmt, level, true);
            break;
        case k_statementKindElse:
            printf("else {\n");
            if (stmt->val.elseStmt.stmt != NULL)
                prettySTMT(stmt->val.elseStmt.stmt, level, true);
            printTabs(level);
            printf("}\n");
            break;
        case k_statementKindElseIf:
            printf("else ");
            prettySTMT(stmt->val.elseIfStmt.ifStmt, level, true);
            break;
        case k_statementKindSwitch:
            printTabs(level);
            printf("switch ");
            if (stmt->val.switchStmt.simpleStmt != NULL) {
                prettySTMT(stmt->val.switchStmt.simpleStmt, level, false);
                printf("; ");
            }
            if (stmt->val.switchStmt.optionalExpr != NULL)
                prettyEXPR(stmt->val.switchStmt.optionalExpr);
            printf(" {\n");
            prettyCASE(stmt->val.switchStmt.c, level + 1);
            printTabs(level);
            printf("}\n");
            break;
        case k_statementKindForCondition: // condition only
            printTabs(level);
            printf("for ");
            if (stmt->val.forStmt.condition != NULL) 
                prettyEXPR(stmt->val.forStmt.condition);
            printf(" {\n");
            if (stmt->val.forStmt.stmtBlock)
                prettySTMT(stmt->val.forStmt.stmtBlock, level + 1, true);
            printTabs(level);
            printf("}\n");
            break;
        case k_statementKindFor: // initial stmt, condition, post stmt
            // "for" [ Condition | ForClause | RangeClause ] Block .
            // ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
            printTabs(level);
            printf("for (");
            if (stmt->val.forStmt.initStmt != NULL)
                prettySTMT(stmt->val.forStmt.initStmt, level - 1, false); // simpleStatement
            printf(" ; ");
            if (stmt->val.forStmt.condition != NULL)
                prettyEXPR(stmt->val.forStmt.condition);
            printf(" ; ");
            if (stmt->val.forStmt.postStmt != NULL)
                prettySTMT(stmt->val.forStmt.postStmt, level - 1, false); // simpleStatement
            printf(" ) {\n");
            if(stmt->val.forStmt.stmtBlock!=NULL)
                prettySTMT(stmt->val.forStmt.stmtBlock, level + 1, true);
            printTabs(level);
            printf("}\n");
            break;
        case k_statementKindContinue: // unlabelled
            printTabs(level);
            printf("continue\n");
            break;
        case k_statementKindBreak: // unlabelled
            printTabs(level);
            printf("break\n");
            break;
        case k_statementKindExpression: // simple statement
            printTabs(level);
            prettyEXPR(stmt->val.expression.expression);
            if (onNewline) printf("\n");
            break;
        }
        if (stmt->next != NULL)
            prettySTMT(stmt->next, level, onNewline);
    } else {
        throwError(stmt->lineno, "not a statement", 1);
    }
}

// ExprCaseClause = ExprSwitchCase ":" StatementList .
// ExprSwitchCase = "case" ExpressionList | "default" .

// redid this - ting. dont touch pls
void prettyCASE(CASE* c, int level) {
    if(c!=NULL){
        switch (c->caseType) {
        case k_Case:
            printTabs(level);
            printf("case ");
            prettyEXPRLIST(c->matches);
            printf(": ");
            break;
        case k_Default:
            printTabs(level);
            printf("default : ");
            break;
        case k_notACase: // aka its an actual caseClause & not switchCase
            prettyCASE(c->condition, level);
            printf("\n");
            break;
        }
        if(c->action!=NULL){
            prettySTMT(c->action, level+1, true);
        }
        if (c->next != NULL){ //for next "case :" statement
            printf("\n");
            prettyCASE(c->next, level);
        }
    }
}

// correct
// short variable declarations are statements, not top level declarations
// printed as regular variable declarations
void prettySHORTVARDECL(STMT* decl) {

    prettyARGSLIST(decl->val.shortVarDecl.identifierList, 0);
    printf(":= ");
    prettyEXPRLIST(decl->val.shortVarDecl.exprList);

    // if (decl != NULL) {
    //     while (decl->val.shortVarDecl.identifierList->thisVAR != NULL) {
    //         printf("%s := \n",
    //             decl->val.shortVarDecl.identifierList->thisVAR->identifier);
    //         prettyEXPR(decl->val.shortVarDecl.exprList->content.thisEXPR);
    //         // short variable declaration are initialized with expressions but no
    //         // types
    //         if (decl->val.shortVarDecl.identifierList->next != NULL && decl->val.shortVarDecl.exprList->next != NULL) {
    //             decl->val.shortVarDecl.identifierList->thisVAR = decl->val.shortVarDecl.identifierList->next->thisVAR;
    //             decl->val.shortVarDecl.exprList->content = decl->val.shortVarDecl.exprList->next->content;
    //         } else if (decl->val.shortVarDecl.identifierList->next == NULL && decl->val.shortVarDecl.exprList->next != NULL) {
    //             // weeding
    //
    //             exit(1);
    //         } else if (decl->val.shortVarDecl.identifierList->next != NULL && decl->val.shortVarDecl.exprList->next == NULL) {
    //             // weeding
    //
    //             exit(1);
    //         } else {
    //             break;
    //         }
    //     }
    // } else {
    //
    //     exit(1);
    // }
}

//
void prettyEXPRLIST(EXPRLIST* exprList) {

    if (exprList == NULL)
        return;
    prettyEXPR(exprList->content.thisEXPR);
    if (exprList->content.type != NULL) //?? when will this happen?
        prettyTYPE(exprList->content.type, 0); //add tabs to prettyEXPRLIST for this later
    if (exprList->next != NULL) {
        printf(", ");
        prettyEXPRLIST(exprList->next);
    } else
        return;
}

void prettyEXPR(EXPR* expr) {

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
            printRaw(expr->val.stringLiteral);
            break;
        case k_expressionKindRuneLiteral:
            printf("%s ", expr->val.runeLiteral);
            break;
        case k_expressionKindPlus:
            printf("+(");
            prettyEXPR(expr->val.unaryExpr);
            printf(")");
            break;
        case k_expressionKindMinus:
            printf("-(");
            prettyEXPR(expr->val.unaryExpr);
            printf(")");
            break;
        case k_expressionKindNot:
            printf("!(");
            prettyEXPR(expr->val.unaryExpr);
            printf(")");
            break;
        case k_expressionKindBitFlip:
            printf("^(");
            prettyEXPR(expr->val.unaryExpr);
            printf(")");
            break;
        case k_expressionKindMultiplication:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" * ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindDivision:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" / ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindAddition:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" + ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindSubtraction:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" - ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindGreaterEqual:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" >= ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindLessEqual:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" <= ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindGreater:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" > ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindLess:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" < ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindEquals:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" == ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindNotEquals:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" != ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindAnd:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" && ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindOr:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" || ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindBitXor:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" ^ ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindBitOr:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" | ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindAndNot:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" &^ ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindBitAnd:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" & ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindRightShift:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" >> ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindLeftShift:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" << ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindModulo:
            printf("(");
            prettyEXPR(expr->val.binary.lhs);
            printf(" %% ");
            prettyEXPR(expr->val.binary.rhs);
            printf(")");
            break;
        case k_expressionKindStructField:
            printf("(");
            prettyEXPR(expr->val.refobj.parent);
            printf(".");
            printf("%s", expr->val.refobj.fieldName);
            printf(")");
            break;
        case k_expressionKindArrayVal:
            printf("(");
            prettyEXPR(expr->val.refobj.parent);
            printf("[");
            prettyEXPR(expr->val.refobj.index);
            printf("])");
            break;
        case k_expressionKindFunctionCall:
            printf("%s(", expr->val.functionCall.methodName);
            prettyEXPRLIST(expr->val.functionCall.arguments);
            printf(")");
            break;
        case k_expressionKindLength:
            printf("len(");
            prettyEXPR(expr->val.unaryExpr);
            printf(")");
            break;
        case k_expressionKindCapacity:
            printf("cap(");
            prettyEXPR(expr->val.unaryExpr);
            printf(")");
            break;
        case k_expressionKindSliceAppend:
            printf("(");
            prettyEXPR(expr->val.refobj.parent);
            printf("),");
            prettyEXPR(expr->val.refobj.appendValue);
            printf(")");
            break;

    }
}
