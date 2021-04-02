#include "tree.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylineno;

_Noreturn void throwError(const int line, const char* msg, const int exitCode) {
    if (line > 0)
        fprintf(stderr, "Error: (line %d) '%s'\n", line, msg);
    else
        fprintf(stderr, "Error: '%s'\n", msg);
    exit(exitCode);
}

PROG* genPROG(PKG* package, TOPDECL* topLevelDecls) {
    PROG* prog = malloc(sizeof(PROG));
    prog->package = package;
    prog->topDecls = topLevelDecls;
    return prog;
}

// Package Declaration
PKG* genPKG(char* packageName) {
    PKG* package = malloc(sizeof(PKG));
    package->lineno = yylineno;
    package->packageName = packageName;
    return package;
}

// list of variable identifiers
// used in variable declaration to declare
// individual var decl : one or multiple individual identifiers
// distributed var decl over a list of variable declarations
ARGLIST* genIDENTIFIERS(char* identifier, ARGLIST* next) {
    ARGLIST* current = malloc(sizeof(ARGLIST));
    current->thisVAR = malloc(sizeof(VAR));
    current->thisVAR->identifier = identifier;
    current->thisVAR->type = NULL;
    current->thisVAR->expression = NULL;
    if (next != NULL) {
        current->next = genIDENTIFIERS(next->thisVAR->identifier, next->next);
    }
    return current;
}

EXPRLIST* genEXPRESSIONS(EXPR* thisExpr, EXPRLIST* next) {
    EXPRLIST* current = malloc(sizeof(EXPRLIST));
    current->content.thisEXPR = thisExpr;
    current->next = next;
    if (next != NULL) {
        current->next = genEXPRESSIONS(next->content.thisEXPR, next->next);
    }
    return current;
}

// var z1, z2 int = 1, 2 converted into var z1 int = 1; var z2 int = 2
TOPDECL* genVARDECL(DeclarationKind kind, ARGLIST* varlist, TYPE* type, EXPRLIST* values) {
    TOPDECL* vardecl = malloc(sizeof(TOPDECL));
    vardecl->lineno = yylineno;
    vardecl->kind = kind;
    if (kind == k_declarationKindEmptyDistributedVarDecl)
        return vardecl;
    if (varlist->thisVAR != NULL) { // generate variable node
        vardecl->vardecl.content = malloc(sizeof(VAR));
        vardecl->vardecl.content->identifier = varlist->thisVAR->identifier;
        if (type != NULL)
            vardecl->vardecl.content->type = type;
        if (values != NULL)
            vardecl->vardecl.content->expression = values->content.thisEXPR;
        if (varlist->next != NULL && values == NULL) { //in case there's no value assigned but its still a list
            vardecl->vardecl.next = genVARDECL(kind, varlist->next, type, NULL);
        } else if (varlist->next != NULL && values->next != NULL) {
            vardecl->vardecl.next = genVARDECL(kind, varlist->next, type, values->next);
        }
    }
    return vardecl;
}

TOPDECL* genTYPEDECL(DeclarationKind kind, char* identifier, TYPE* type) {
    TOPDECL* typedecl = malloc(sizeof(TOPDECL));
    typedecl->lineno = yylineno;
    typedecl->kind = kind;
    typedecl->typedecl.identifier = identifier;
    typedecl->typedecl.content = type;
    return typedecl;
}

TOPDECL* genTYPEDECL_emptyDistributed(){
    TOPDECL* typedecl = malloc(sizeof(TOPDECL));
    typedecl->lineno = yylineno;
    typedecl->kind = k_declarationKindEmptyDistributedTypeDecl;
    return typedecl;

}

TYPE* genTYPE(TypeKind tk, char* namedtype, TYPE* slicetype, int arraySize, TYPE* arraytype, FIELDDECL* structype){
    TYPE* typeNode = malloc(sizeof(TYPE));
    switch(tk){
        case arrayType:
            typeNode->array.size = arraySize;
            typeNode->array.r = arraytype;
            break;

        case sliceType:
            typeNode->slice = slicetype;
            break;

        case structType: 
            typeNode->struc.structfield = structype;
            
            break;

        case namedType: 
            typeNode->named.name = namedtype;
            break;
    }
    typeNode->kind = tk;
    typeNode->lineno = yylineno;
   
    return typeNode;
}

// field declaration in struct
FIELDDECL* genTYPEDECL_fieldDecl(ARGLIST* identifierList, TYPE* type, TYPENAME* typeName) {
    FIELDDECL* field = malloc(sizeof(FIELDDECL));
    if (identifierList != NULL) {
        field->regular.identifierlist = identifierList;
        field->regular.type = type;
    }
    return field;
}

TOPDECL* genFUNCDECL(char* functionName, SIGNATURE* signature, STMT* block) {
    TOPDECL* function = malloc(sizeof(TOPDECL));
    function->kind = k_declarationKindFunction;
    function->lineno = yylineno;
    if (block != NULL)
        function->funcdecl.block = block;
    function->funcdecl.identifier = functionName;
    if (signature != NULL)
        function->funcdecl.sig = signature;
    return function;
}

SIGDECL* genFUNCDECL_shortSigDecl(ARGLIST* identifierList, TYPE* type, SIGDECL* nextSigDecl) {
    SIGDECL* s = malloc(sizeof(SIGDECL));
    if (identifierList != NULL)
        s->parameters.identifierList = identifierList;
    if (type != NULL)
        s->valType = type;
    if (nextSigDecl != NULL)
        s->next = nextSigDecl;
    return s;
}

SIGNATURE* genSIGNATURE(SIGDECL* sig, TYPE* optional) {
    SIGNATURE* s = malloc(sizeof(SIGNATURE));
    if (sig != NULL)
        s->decls = sig;
    if (optional != NULL)
        s->optionalReturn = optional;
    return s;
}

/*************************************************************************/

// Statements

// converts declaration statement to top declaration
STMT* genSTMT_TOPDECL(TOPDECL* declaration) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = declaration->lineno;
    s->kind = k_statementKindDeclaration;
    s->val.topDecl.topDecl = declaration;
    return s;
}

STMT* genSTMT_expression(EXPR* expression) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindExpression;
    s->val.expression.expression = expression;
    return s;
}

STMT* genSTMT_assignment(EXPR* ident, EXPR* expr, assignOp aop) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindAssignment;
    s->val.assignment.identifier = ident;
    s->val.assignment.expression = expr;
    s->val.assignment.opassign = aop;
    return s;
}

// LHS_EXPRLIST = RHS_EXPRLIST
STMT* genSTMT_assignments(EXPRLIST* lhs, EXPRLIST* rhs) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindExprlistAssignment;
    s->val.exprlistAssignment.LHS = lhs;
    s->val.exprlistAssignment.RHS = rhs;
    return s;
}

STMT* genSTMT_shortVarDecl(ARGLIST* identifiers, EXPRLIST* exprList) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindShortVarDecl;
    s->val.shortVarDecl.identifierList = identifiers;
    s->val.shortVarDecl.exprList = exprList;
    return s;
}

STMT* genSTMT_incDec(StatementKind kind, EXPR* expr) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = kind;
    s->val.incDec.expression = expr;
    return s;
}

STMT* genSTMT_print(StatementKind kind, EXPRLIST* exprlist) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = kind; // for print and println
    s->val.print.exprlist = exprlist;
    return s;
}

STMT* genSTMT_if(EXPR* condition, STMT* optionalStmt, STMT* stmts, STMT* elseStmt) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindIf;
    s->val.ifStmt.simpleStmt = optionalStmt;
    s->val.ifStmt.condition = condition;
    s->val.ifStmt.stmt = stmts;
    s->val.ifStmt.elseStmt = elseStmt;
    return s;
}

STMT* genSTMT_else(STMT* statements) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindElse;
    s->val.elseStmt.stmt = statements;
    return s;
}

STMT* genSTMT_elseIf(STMT* ifStatement) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindElseIf;
    s->val.elseIfStmt.ifStmt = ifStatement;
    return s;
}

STMT* genSTMT_switch(STMT* stmt, EXPR* expr, CASE* c) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindSwitch;
    s->val.switchStmt.simpleStmt = stmt;
    s->val.switchStmt.optionalExpr = expr;
    s->val.switchStmt.c = c;
    return s;
}

STMT* genSTMT_empty() {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindEmptyStmt;
    return s;
}

CASE* genCASE(CaseType ct, CASE* cc, STMT* actions, EXPRLIST* matches) {
    CASE* c = malloc(sizeof(CASE));
    c->caseType = ct;
    c->condition = cc;
    if (actions != NULL)
        c->action = actions;
    if (matches != NULL)
        c->matches = matches;
    return c;
}

STMT* genSTMT_forCondition(EXPR* condition, STMT* stmtBlock) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindForCondition;
    s->val.forStmt.condition = condition;
    s->val.forStmt.stmtBlock = stmtBlock;
    return s;
}

STMT* genSTMT_forStatement(STMT* initStmt, EXPR* condition, STMT* postStmt, STMT* stmtBlock) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindFor;
    s->val.forStmt.condition = condition;
    s->val.forStmt.initStmt = initStmt;
    s->val.forStmt.postStmt = postStmt;
    s->val.forStmt.stmtBlock = stmtBlock;
    return s;
}

STMT* genSTMT_return(EXPR* expr) {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindReturn;
    s->val.returnStmt.expr = expr;
    return s;
}

STMT* genSTMT_continue() {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindContinue;
    return s;
}

STMT* genSTMT_break() {
    STMT* s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_statementKindBreak;
    return s;
}

STMT* genSTMTS(STMT* thisStmt, STMT* list) {
    if (list != NULL)
        thisStmt->next = list;
    return thisStmt;
}

/*************************************************************************/

// Expressions

EXPR* genEXPR_identifier(char* identifier) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindIdentifier;
    e->val.identifier = identifier;
    return e;
}

EXPR* genEXPR_boolLiteral(bool boolLiteral) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindBoolLiteral;
    e->val.boolLiteral = boolLiteral;
    return e;
}

EXPR* genEXPR_intLiteral(int intLiteral) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindIntLiteral;
    e->val.intLiteral = intLiteral;
    return e;
}

EXPR* genEXPR_floatLiteral(float floatLiteral) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindFloatLiteral;
    e->val.floatLiteral = floatLiteral;
    return e;
}

EXPR* genEXPR_stringLiteral(char* stringLiteral) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindStringLiteral;
    e->val.stringLiteral = stringLiteral;
    return e;
}

EXPR* genEXPR_rawstringLiteral(char* rawLit) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindRawStringLiteral;
    e->val.stringLiteral = rawLit;
    return e;
}

EXPR* genEXPR_runeLiteral(char* character) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = k_expressionKindRuneLiteral;
    e->val.runeLiteral = character;
    return e;
}

EXPR* genEXPR_unary(ExpressionKind op, EXPR* expr) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.unaryExpr = expr;
    return e;
}

EXPR* genEXPR_binaryOperation(ExpressionKind op, EXPR* lhs, EXPR* rhs) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.binary.lhs = lhs;
    e->val.binary.rhs = rhs;
    return e;
}

EXPR* genEXPR_structField(ExpressionKind op, EXPR* ident, char* fieldname) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.refobj.parent = ident;
    e->val.refobj.fieldName = fieldname;
    return e;
}

EXPR* genEXPR_arrayVal(ExpressionKind op, EXPR* ident, EXPR* index) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.refobj.parent = ident;
    e->val.refobj.index = index;
    return e;
}

EXPR* genEXPR_sliceAppend(ExpressionKind op, EXPR* slice, EXPR* toBeAppendedVal) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.refobj.parent = slice;
    e->val.refobj.appendValue = toBeAppendedVal;
    return e;
}

EXPR* genEXPR_functionCall(ExpressionKind op, char* funcname, EXPRLIST* arguments) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.functionCall.methodName = funcname;
    e->val.functionCall.arguments = arguments;
    return e;
}

EXPR* genEXPR_structFunctionCall(ExpressionKind op, EXPR* caller, char* methodName, EXPRLIST* arguments) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = op;
    e->val.functionCall.caller = caller;
    e->val.functionCall.methodName = methodName;
    e->val.functionCall.arguments = arguments;
    return e;
}

EXPR* genEXPR_LenOrCap(ExpressionKind kind, EXPR* expr) {
    EXPR* e = malloc(sizeof(EXPR));
    e->lineno = yylineno;
    e->kind = kind;
    e->val.unaryExpr = expr;
    return e;
}
