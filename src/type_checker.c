#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type_checker.h"

// resolve the type of an identifier by querying the symbol table
// NOTE SymbolTable is passed in all the functions because of getSymbol
char* typeVAR(SymbolTable* t, char* identifier) {
    VAR* var = NULL; // TODO VAR* should be passed ?
    SYMBOL* type = getSymbol(t, var->symbol->symKind, identifier);
    return type->name; // TODO codeName is the "written" type while name is the primitive, should return which?
}

// TODO IMPORTANT NOTE : should the isX functions take char* or TYPE* as argument?
// SIMON : makes more sense to take TYPE* but I might be misunderstanding, I changed it for now so some lines became obsolete far below
// also according to Alex, do NOT use strcmp for type checking? See helper function(s)

// numeric type : int, float64, rune
bool isNumeric(TYPE* type) {
    char* baseType = resolveType(type);
    if ((strcmp(baseType, "int") == 0)
        || (strcmp(baseType, "float64") == 0)
        || (strcmp(baseType, "rune") == 0))
        return true;
    return false;
}

// integer type : int, rune
bool isInteger(TYPE* type) {
    char* baseType = resolveType(type);
    if ((strcmp(baseType, "int") == 0)
        || (strcmp(baseType, "rune") == 0))
        return true;
    return false;
}

// string type : string, rawstring
bool isString(TYPE* type) {
    char* baseType = resolveType(type);
    if ((strcmp(baseType, "string") == 0)
        || (strcmp(baseType, "rawstring") == 0))
        return true;
    return false;
}

bool isBool(TYPE* type) {
    char* baseType = resolveType(type);
    if (strcmp(baseType, "bool") == 0)
        return true;
    return false;
}

// TODO struct and array comparison
bool isComparable(TYPE* type) {
    char* baseType = resolveType(type);
    if ((strcmp(baseType, "bool") == 0)
        || (strcmp(baseType, "int") == 0)
        || (strcmp(baseType, "float64") == 0)
        || (strcmp(baseType, "string") == 0)
        || (strcmp(baseType, "rawstring") == 0))
        return true;
    return false;
}
// slice and function values are not comparable
// Struct values are comparable if all their fields are comparable. Two struct values are equal if their corresponding non-blank fields are equal.
// Array values are comparable if values of the array element type are comparable. Two array values are equal if their corresponding elements are equal.

// TODO isStruct isArrayOrSlice
// since changes, we only need to check TypeKind?

bool isStruct(TYPE* type) {
    return type->kind == structType;
}

bool isArrayOrSlice(TYPE* type) {
    return type->kind == arrayType || type->kind == sliceType;
}

bool isOrdered(TYPE* type) {
    char* baseType = resolveType(type); // resolveType takes a TYPE* not char* which type is (yeah what)
    if ((strcmp(baseType, "int") == 0)
        || (strcmp(baseType, "float64") == 0)
        || (strcmp(baseType, "string") == 0)
        || (strcmp(baseType, "rawstring") == 0))
        return true;
    return false;
}

// two types are either identical or different
// a defined type is always different from any other type but itself
// TODO make sure this works for array/slice/structs/functions ?
bool identicalTypes(char* t1, char* t2) {
    if (strcmp(t1, t2) == 0)
        return true;
    return false;
}

// TODO cannot access TYPE (explained in report)
// TODO only for user defined (named) types? or see report
char* resolveType(TYPE* userDefinedType) {
    if (userDefinedType->kind == namedType)
        return getType(userDefinedType);
    else
        throwError(userDefinedType->lineno, "not a named type", 1);
}

void typePROG(PROG* prog) {
    typeTOPDECL(prog->topDecls);
}

void typeTOPDECL(TOPDECL* topDecl) {
    if (topDecl != NULL) {
        switch (topDecl->kind) {
        case k_declarationKindEmptyDistributedVarDecl: // do nothing
            break;
        case k_declarationKindEmptyDistributedTypeDecl:
            break;
        case k_declarationKindVariable:
            //todo
            break;
        case k_declarationKindShortVariable:
            //todo
            break;
        case k_declarationKindType:
            //todo
            break;
        case k_declarationKindFunction:
            //todo
            break;
        }
        if (topDecl->next != NULL) {
            typeTOPDECL(topDecl->next);
        }
    }
}

// TODO all cases
void typeSTMT(SymbolTable* t, STMT* stmt, TYPE* returnTYPE) {
    if (stmt == NULL)
        return;
    switch (stmt->kind) {
    case k_statementKindEmptyStmt:
    case k_statementKindBreak:
    case k_statementKindContinue:
        break; // trivially well-typed
    case k_statementKindExpression: // expr
    {
        // only function call expressions are allowed to be used as statements
        // TODO to check
        if (stmt->kind != k_statementKindExpression || stmt->val.expression.expression->kind != k_expressionKindFunctionCall)
            throwError(stmt->lineno, "not a function call expression.", 1);
        typeEXPR(t, stmt->val.expression.expression);
        break;
    }
    case k_statementKindReturn: // return or return expr
    {
        if (stmt->val.returnStmt.expr != NULL)
            typeEXPR(t, stmt->val.returnStmt.expr);
        break;
    }
    case k_statementKindShortVarDecl: {
        // x1, x2, ..., xk := e1, e2, ..., ek
        ARGLIST* lhs = stmt->val.shortVarDecl.identifierList;
        EXPRLIST* rhs = stmt->val.shortVarDecl.exprList;
        // 1. all the expressions on the RHS are well-typed
        typeEXPRLIST(t, rhs);
        // 2. if 1 then at least one variable on the LHS is not declared in the current scope
        bool isValidShortVarDecl = false;
        for (ARGLIST* current = stmt->val.shortVarDecl.identifierList; current != NULL && !isValidShortVarDecl; current = current->next) {
            VAR* thisVAR = current->thisVAR;
            if (!isDefined(t, thisVAR->identifier)) // should this function be named isDeclared?
                isValidShortVarDecl = true;
        }
        // 3. if 2 then already declared variables are assigned expressions with matching types
        //    otherwise compile error e.g. "cannot use X (type T2) as type T1 in assignment"
        ARGLIST* currentLHS = stmt->val.shortVarDecl.identifierList;
        EXPRLIST* currentRHS = stmt->val.shortVarDecl.exprList;
        for (; currentLHS != NULL && currentRHS != NULL; currentLHS = currentLHS->next, currentRHS = currentRHS->next)
            if (isValidShortVarDecl) {
                // TODO which one should be used?
                identicalTypes(resolveType(currentLHS->thisVAR->type), resolveType(currentRHS->content.type));
                identicalTypes(resolveType(currentLHS->thisVAR->type), resolveType(currentRHS->content.thisEXPR->type));
            }
        break;
    }
    case k_statementKindDeclaration: {
        // LHS is not declared in the current scope
        typeEXPR(t, stmt->val.topDecl.topDecl->vardecl.content->expression);
        break;
    }
    case k_statementKindExprlistAssignment: // v1, v2, ..., vk = e1, e2, ..., en
    {
        // LHS expressions must be addressable (variables, slice/array indexing, field of struct)
        typeEXPRLIST(t, stmt->val.exprlistAssignment.LHS);
        typeEXPRLIST(t, stmt->val.exprlistAssignment.RHS);
        // all the expressions on the LHS and RHS are well-typed and match types
        break;
    }
    case k_statementKindAssignment: // v op= expr : op-assignment
    {
        typeEXPR(t, stmt->val.assignment.expression); // RHS expression is well-typed
        typeEXPR(t, stmt->val.assignment.identifier); // LHS is well-typed
        // LHS expressions must also lvalues TODO
        char* rhsType = getType(stmt->val.assignment.expression->type);
        char* lhsType = getType(stmt->val.assignment.identifier->type);
        // return a value of type v
        if (!identicalTypes(lhsType, rhsType)) {
            throwError(stmt->lineno, "illegal assignment", 1);
        }
        break;
    }
    case k_statementKindPrint: {
    }
    case k_statementKindPrintLn: {
        typeEXPRLIST(t, stmt->val.print.exprlist);
        break;
    }
    case k_statementKindIf: {
        if (stmt->val.ifStmt.simpleStmt != NULL) // init typecheck
            typeSTMT(t, stmt->val.ifStmt.simpleStmt, returnTYPE);
        typeEXPR(t, stmt->val.ifStmt.condition);
        char* type = getType(stmt->val.ifStmt.condition->type);
        if (isBool(stmt->val.ifStmt.condition->type)) {
            typeSTMT(t, stmt->val.ifStmt.stmt, returnTYPE);
        } else {
            throwError(stmt->lineno, "invalid condition expression", 1);
        }
        if (stmt->val.ifStmt.elseStmt != NULL)
            typeSTMT(t, stmt->val.ifStmt.elseStmt, returnTYPE);
        break;
    }
    case k_statementKindElse: {
        typeSTMT(t, stmt->val.elseStmt.stmt, returnTYPE);
        break;
    }
    case k_statementKindElseIf: {
        typeSTMT(t, stmt->val.elseIfStmt.ifStmt, returnTYPE);
        break;
    }
    case k_statementKindSwitch: {
        typeSTMT(t, stmt->val.switchStmt.simpleStmt, returnTYPE);
        if (stmt->val.switchStmt.optionalExpr != NULL) {
            typeEXPR(t, stmt->val.switchStmt.optionalExpr);
            char* type = getType(stmt->val.switchStmt.optionalExpr->type);
            if (!isComparable(stmt->val.switchStmt.optionalExpr->type)) {
                throwError(stmt->lineno, "Optional Expression must be comparable", 1);
            }
        }
        break;
    }
    case k_statementKindFor: {
        if (stmt->val.forStmt.condition != NULL)
            typeEXPR(t, stmt->val.forStmt.condition);
        if (stmt->val.forStmt.initStmt != NULL)
            typeSTMT(t, stmt->val.forStmt.initStmt, returnTYPE);
        // init statements can shadow variables declared in the same scope
        if (stmt->val.forStmt.postStmt != NULL)
            typeSTMT(t, stmt->val.forStmt.postStmt, returnTYPE);
        char* conditionType = getType(stmt->val.forStmt.condition->type);
        if (isBool(stmt->val.forStmt.condition->type)) {
            typeSTMT(t, stmt->val.forStmt.stmtBlock, returnTYPE);
        }
        break;
    }
    case k_statementKindForCondition: {
        typeEXPR(t, stmt->val.forStmt.condition);
        char* conditionType = getType(stmt->val.forStmt.condition->type);
        if (isBool(stmt->val.forStmt.condition->type)) {
            typeSTMT(t, stmt->val.forStmt.stmtBlock, returnTYPE);
        }
        break;
    }
    case k_statementKindIncrement: {
    }
    case k_statementKindDecrement: {
        typeEXPR(t, stmt->val.incDec.expression);
        char* exprType = getType(stmt->val.incDec.expression->type);
        if (isNumeric(stmt->val.incDec.expression->type)) {
            throwError(stmt->lineno, "illegal increment/decrement", 1);
        }
        break;
    }
    }
    if (stmt->next != NULL)
        typeSTMT(t, stmt->next, returnTYPE);
}

void typeEXPRLIST(SymbolTable* t, EXPRLIST* exprList) {
    typeEXPR(t, exprList->content.thisEXPR);
    if (exprList->next != NULL) {
        typeEXPRLIST(t, exprList->next);
    }
}

// type stored in AST itself
void typeEXPR(SymbolTable* t, EXPR* expr) {
    char errMsg[256];
    switch (expr->kind) {
    case k_expressionKindIdentifier:
        if (!isDefined(t, expr->val.identifier)) { // if identifier cannot be found in the symbol table
            snprintf(errMsg, sizeof(errMsg), "‘%s’ is not declared.", expr->val.identifier);
            throwError(expr->lineno, errMsg, 1);
        }
        expr->symbol->kind = typeVAR(t, expr->val.identifier);
        break;
    case k_expressionKindBoolLiteral:
        expr->symbol->kind = "bool";
        break;
    case k_expressionKindIntLiteral:
        expr->symbol->kind = "int";
        break;
    case k_expressionKindFloatLiteral:
        expr->symbol->kind = "float64";
        break;
    case k_expressionKindStringLiteral:
        expr->symbol->kind = "string";
        break;
    case k_expressionKindRawStringLiteral:
        expr->symbol->kind = "rawstring";
        break;
    case k_expressionKindRuneLiteral:
        expr->symbol->kind = "rune";
        break;
    case k_expressionKindPlus: // unop plus must resolve to a numeric type (int, float64, rune)
    case k_expressionKindMinus: // unop minus must resolve to a numeric type (int, float64, rune)
    {
        EXPR* e = expr->val.unaryExpr;
        typeEXPR(t, e);
        char* type = getType(e->type);
        if (isNumeric(e->type)) {
            expr->symbol->kind = type;
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong type (%s) used on unary plus or minus.", type);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindNot: // logical negation must resolve to a bool
    {
        EXPR* e = expr->val.unaryExpr;
        typeEXPR(t, e);
        char* type = getType(e->type);
        if (isBool(e->type)) {
            expr->symbol->kind = type;
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong type (%s) used on logical negation.", type);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindBitFlip: // bitwise negation must resolve to an integer type (int, rune)
    {
        EXPR* e = expr->val.unaryExpr;
        typeEXPR(t, e);
        char* type = getType(e->type);
        if (isInteger(e->type)) {
            expr->symbol->kind = type;
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong type (%s) used on bitwise negation.", type);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindOr: // bool || bool
    case k_expressionKindAnd: // bool && bool
    {
        EXPR* lhs = expr->val.binary.lhs;
        EXPR* rhs = expr->val.binary.rhs;
        typeEXPR(t, lhs);
        typeEXPR(t, rhs);
        char* t1 = getType(lhs->type);
        char* t2 = getType(rhs->type);
        if (identicalTypes(t1, t2) && isBool(lhs->type) && isBool(rhs->type)) {
            expr->symbol->kind = "bool";
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong types (%s, %s) used on logical and or or.", t1, t2);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindEquals: // comparable == comparable
    case k_expressionKindNotEquals: // comparable != comparable
    {
        EXPR* lhs = expr->val.binary.lhs;
        EXPR* rhs = expr->val.binary.rhs;
        typeEXPR(t, lhs);
        typeEXPR(t, rhs);
        char* t1 = getType(lhs->type);
        char* t2 = getType(rhs->type);
        if (identicalTypes(t1, t2) && isComparable(lhs->type) && isComparable(rhs->type)) {
            expr->symbol->kind = "bool";
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong types (%s, %s) used for comparable types.", t1, t2);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindLess: // ordered < ordered
    case k_expressionKindLessEqual: // ordered <= ordered
    case k_expressionKindGreater: // ordered > ordered
    case k_expressionKindGreaterEqual: // ordered >= ordered
    {
        EXPR* lhs = expr->val.binary.lhs;
        EXPR* rhs = expr->val.binary.rhs;
        typeEXPR(t, lhs);
        typeEXPR(t, rhs);
        char* t1 = getType(lhs->type);
        char* t2 = getType(rhs->type);
        if (identicalTypes(t1, t2) && isOrdered(lhs->type) && isOrdered(rhs->type)) {
            expr->symbol->kind = "bool";
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong types (%s, %s) used for ordered types.", t1, t2);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindAddition: // numeric + numeric or string + string
    {
        EXPR* lhs = expr->val.binary.lhs;
        EXPR* rhs = expr->val.binary.rhs;
        typeEXPR(t, lhs);
        typeEXPR(t, rhs);
        char* t1 = getType(lhs->type);
        char* t2 = getType(rhs->type);
        if (identicalTypes(t1, t2) && ((isNumeric(lhs->type) && isNumeric(rhs->type)) || (isString(lhs->type) && isString(rhs->type)))) {
            expr->symbol->kind = t1;
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong types (%s, %s) used for numeric addition/string concatenation.", t1, t2);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindSubtraction: // numeric - numeric
    case k_expressionKindMultiplication: // numeric * numeric
    case k_expressionKindDivision: // numeric / numeric
    {
        EXPR* lhs = expr->val.binary.lhs;
        EXPR* rhs = expr->val.binary.rhs;
        typeEXPR(t, lhs);
        typeEXPR(t, rhs);
        char* t1 = getType(lhs->type);
        char* t2 = getType(rhs->type);
        if (identicalTypes(t1, t2) && isNumeric(lhs->type) && isNumeric(rhs->type)) {
            expr->symbol->kind = t1;
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong types (%s, %s) used for numeric operation.", t1, t2);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindModulo: // integer % integer
    case k_expressionKindBitOr: // integer | integer
    case k_expressionKindBitAnd: // integer & integer
    case k_expressionKindLeftShift: // integer << integer
    case k_expressionKindRightShift: // integer >> integer
    case k_expressionKindAndNot: // integer &^ integer
    case k_expressionKindBitXor: // integer ^ integer
    {
        EXPR* lhs = expr->val.binary.lhs;
        EXPR* rhs = expr->val.binary.rhs;
        typeEXPR(t, lhs);
        typeEXPR(t, rhs);
        char* t1 = getType(lhs->type);
        char* t2 = getType(rhs->type);
        if (identicalTypes(t1, t2) && isInteger(lhs->type) && isInteger(rhs->type)) {
            expr->symbol->kind = t1;
        } else {
            snprintf(errMsg, sizeof(errMsg), "Wrong types (%s, %s) used for integer operation.", t1, t2);
            throwError(expr->lineno, errMsg, 1);
        }
        break;
    }
    case k_expressionKindFunctionCall: // expr(arg1, arg2, ..., argk)
    {
        typeEXPRLIST(t, expr->val.functionCall.arguments); // arg1, arg2, . . . , argk are well-typed
        // arg1, arg2, . . . ,argk have types T1, T2, . . . ,Tk respectively
        typeVAR(t, expr->val.functionCall.methodName); // expr is well-typed
        // NOTE: according to specifications 4.8 expr has function type (T1 * T2 * ...  * Tk) -> Tr
        // but methodName is char* in our AST implementation
        expr->symbol->kind = "func"; // TODO function type (T1 * T2 * ...  * Tk) -> Tr
        // SIMON : function type is just abstraction? concatenate signature element names to create function type maybe?
        break;
    }
    case k_expressionKindArrayVal: // expr[index] : indexing into a slice or an array
    {
        typeEXPR(t, expr->val.refobj.parent); // expr is well-typed
        char* type = NULL; // expr resolves to []T or [N]T TODO
        typeEXPR(t, expr->val.refobj.index); // index is well-typed
        char* indexType = getType(expr->val.refobj.index->type);
        if (isInteger(expr->val.refobj.index->type)) { // index resolves to int
            expr->symbol->kind = indexType; // TODO or type ?? // the result of the indexing expression is T
        }
        break;
    }
    case k_expressionKindStructField: // expr.id : field selection
    {
        typeEXPR(t, expr->val.refobj.parent); // expr is well-typed and has type S
        char* type; // S resolves to a struct type TODO
        typeVAR(t, expr->val.refobj.fieldName); // S has a field named id
        char* fieldType = NULL/* = getTYPE(...)*/; // TODO get type of field id
        // type of a field section expression is the type associated with id in struct definition
        expr->symbol->kind = fieldType; // TODO see above
        break;
    }
    case k_expressionKindSliceAppend: // append(e1, e2)
    {
        typeEXPR(t, expr->val.refobj.parent); // e1 is the slice expr to append
        // TODO double check this stuff
        char* sliceType = resolveType(expr->val.refobj.parent->type); // e1 has type S and resolves to []T
        typeEXPR(t, expr->val.refobj.appendValue); // element to add is well-typed and has type
        expr->symbol->kind = sliceType; // type of append is S
        break;
    }
    case k_expressionKindCapacity: // cap(expr)
    {
        typeEXPR(t, expr->val.unaryExpr); // expr is well-typed, has type S
        char* type = getType(expr->val.unaryExpr->type); // S resolves to []T or [N]T
        expr->symbol->kind = "int"; // the result has type int
        break;
    }
    case k_expressionKindLength: // len(expr)
    {
        typeEXPR(t, expr->val.unaryExpr); // expr is well-typed, has type S
        char* type = getType(expr->val.unaryExpr->type); // S resolves to string
        if (isString(expr->val.unaryExpr->type))
            expr->symbol->kind = "int"; // the result has type int
        break;
    }
    // case k_expressionKindStructFunctionCall: // NOT SUPPORTED IN GOLITE
    //     throwError(expr->lineno, "struct function call expression not supported in GoLite.", 1);
    //     break;
    // }
    // TODO TYPECAST ?? => talk about putting two entries in symtable per type created (type+typecast func mapping)
    }
}

void typeFUNC(SymbolTable* t, FUNC* func) {
    // typecasts?
    // Check return statements
    TYPE* returnType = func->returnType;
    for (STMT* current = func->body; current != NULL; current = current->next) {
        if (current->kind == k_statementKindReturn) {
            // TODO should look up in the table instead of the FUNC* itself?
            // should wait for symbol table final design decision for this?
            if (!identicalTypes(getType(current->val.returnStmt.expr->type), getType(returnType))) {
                throwError(current->lineno, "expression does not match function return type.", 1);
            }
        }
    }
}

// TODO check arguments given
// Also do struct call in same function?
void typeFUNCCALL(SymbolTable* t, EXPR* funcCall, FUNC* func, SIGNATURE* sig) {
    //
    if (funcCall->kind == k_expressionKindFunctionCall) {
        for (EXPRLIST* currentArg = funcCall->val.functionCall.arguments;
             currentArg != NULL;
             currentArg = currentArg->next) {
            // TODO compare types
        }
    }
}
