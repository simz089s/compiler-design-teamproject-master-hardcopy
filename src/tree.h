#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

static const int TYPE_SIZE_SIMPLE = -1; // integer, float, rune, string, raw string literals
static const int TYPE_SIZE_SLICE = -2;
static const int TYPE_SIZE_EMPTYSTRUCT = -3;

typedef struct PROG PROG;
typedef struct PKG PKG;

typedef struct TOPDECL TOPDECL;

typedef struct VAR VAR;
typedef struct TYPE TYPE;
typedef struct STRUCTFIELD STRUCTFIELD;

typedef struct FIELDDECL FIELDDECL;
typedef struct TYPENAME TYPENAME;

typedef struct FUNCDECL FUNCDECL; // TODO
typedef struct FUNC FUNC;
typedef struct SIGNATURE SIGNATURE;
typedef struct SIGDECL SIGDECL;

typedef struct STMT STMT;

typedef struct CASE CASE;
typedef struct CASECONDITION CASECONDITION;

typedef struct EXPR EXPR;
typedef struct ARGLIST ARGLIST;
typedef struct EXPRLIST EXPRLIST;

typedef struct SYMBOL SYMBOL;

typedef enum {
    k_expressionKindIdentifier,
    k_expressionKindBoolLiteral,
    k_expressionKindIntLiteral,
    k_expressionKindFloatLiteral,
    k_expressionKindStringLiteral,
    k_expressionKindRawStringLiteral,
    k_expressionKindRuneLiteral,
    k_expressionKindPlus, // unary
    k_expressionKindMinus, // unary
    k_expressionKindNot, // unary
    k_expressionKindBitFlip, // unary
    k_expressionKindMultiplication,
    k_expressionKindDivision,
    k_expressionKindAddition,
    k_expressionKindSubtraction,
    k_expressionKindGreaterEqual,
    k_expressionKindLessEqual,
    k_expressionKindGreater,
    k_expressionKindLess,
    k_expressionKindEquals,
    k_expressionKindNotEquals,
    k_expressionKindAnd,
    k_expressionKindOr,
    k_expressionKindBitXor,
    k_expressionKindBitOr,
    k_expressionKindAndNot,
    k_expressionKindBitAnd,
    k_expressionKindRightShift,
    k_expressionKindLeftShift,
    k_expressionKindModulo,
    k_expressionKindStructField,
    k_expressionKindArrayVal,
    k_expressionKindFunctionCall,
    k_expressionKindLength,
    k_expressionKindCapacity,
    k_expressionKindSliceAppend,
} ExpressionKind;

typedef enum {
    k_assignOpMult,
    k_assignOpDiv,
    k_assignOpModulo,
    k_assignOpLeftShift,
    k_assignOpRightShift,
    k_assignOpBitAnd,
    k_assignOpAndNot,
    k_assignOpAddition,
    k_assignOpSubtraction,
    k_assignOpBitOr,
    k_assignOpBitXor
} assignOp; // symbol in front of equal

typedef enum {
    k_statementKindShortVarDecl,
    k_statementKindAssignment, // += -=
    k_statementKindExprlistAssignment, // =
    k_statementKindDeclaration, // variable and type decl
    k_statementKindIncrement,
    k_statementKindDecrement,
    k_statementKindPrint,
    k_statementKindPrintLn,
    k_statementKindReturn,
    k_statementKindIf,
    k_statementKindElse,
    k_statementKindElseIf,
    k_statementKindSwitch,
    k_statementKindFor,
    k_statementKindForCondition,
    k_statementKindBreak,
    k_statementKindContinue,
    k_statementKindExpression,
    k_statementKindEmptyStmt
} StatementKind;

typedef enum {
    k_declarationKindVariable,
    k_declarationKindShortVariable,
    k_declarationKindType,
    k_declarationKindFunction,
    k_declarationKindEmptyDistributedVarDecl,
    k_declarationKindEmptyDistributedTypeDecl
} DeclarationKind;

typedef enum {
    k_signatureKindLong,
    k_signatureKindShort
} SignatureKind; // signature declaration

typedef enum {
    arrayType,
    sliceType,
    structType,
    namedType
} TypeKind;

typedef enum {
    k_Case,
    k_Default,
    k_notACase
} CaseType;

typedef enum {
    k_symbolKindFunction,
    k_symbolKindArray,
    k_symbolKindSlice,
    k_symbolKindStruct,
    k_symbolKindNamedType, //type a int
    k_symbolKindVariable //var a int
} SymbolKind;

// For symbol table
struct SYMBOL {
    char* name;
    char* kind; // user-defined and basic types

    char* codeName; // for codegen, renaming variables

    SymbolKind symKind;
    union {
        struct {
            char* functionName;
            SIGNATURE* sig; //a bit different
            TYPE* returnType;
        } function;
        struct {
            int size;
            TYPE* type;
        } array;
        struct {
            TYPE* type;
        } slice;
        struct {
            FIELDDECL* structfield; //for now
        } struc;

    } val;
    struct SYMBOL* next;
};

// AST representations

// whole program (root)
struct PROG {
    PKG* package;
    TOPDECL* topDecls;
};

// package node
struct PKG {
    int lineno;
    char* packageName;
};

// top level declaration node
struct TOPDECL {
    int lineno;
    DeclarationKind kind;
    struct {
        VAR* content;
        TOPDECL* next;
        TOPDECL* distributedNext;
    } vardecl;
    struct {
        char* identifier;
        TYPE* content;
        TOPDECL* distributedNext;
    } typedecl;
    struct {
        char* identifier;
        SIGNATURE* sig;
        STMT* block;
    } funcdecl;
    TOPDECL* next;
    SYMBOL* symbol;
};

/*************************************************************************/

struct VAR { // variable node
    char* identifier;
    int lineno;
    TYPE* type;
    EXPR* expression;
    SYMBOL* symbol;
};

// ARGLIST only holds the variable identifiers
struct ARGLIST { // e.g. a, b int, c string, d int or x, y float64 etc.
    VAR* thisVAR;
    ARGLIST* next;
};

/*************************************************************************/

struct TYPE {
    // char* identifier; // new name for type
    // TYPE* baseType; // if primitive type, baseType is NULL
    // int size;
    // FIELDDECL* struc;
    SYMBOL* symbol;

    int lineno;
    TypeKind kind;
    union {
        struct {
            char* name; // for symtab and typecheck
            char* baseType; // for codegen, assigned during symtab
        } named;
        TYPE* slice;
        struct {
            int size;
            TYPE* r;
        } array;
        struct {
            char* structName;
            FIELDDECL* structfield;
        } struc;
    };
};

/*************************************************************************/

struct FIELDDECL { // fieldDecl for declarations within a struct object
    int lineno;
    struct {
        ARGLIST* identifierlist;
        TYPE* type;
    } regular;
    FIELDDECL* next;
    SYMBOL* symbol;
};

/*************************************************************************/

struct FUNC { // function node
    char* identifier;
    ARGLIST* parameters;
    STMT* body; // block statement
    TYPE* returnType;
    SYMBOL* symbol;
};

struct SIGNATURE {
    SIGDECL* decls;
    TYPE* optionalReturn;
};

struct SIGDECL {
    SignatureKind kind;
    struct {
        char* identifier; // long form : one id with one type
        ARGLIST* identifierList; // short form : many ids with one type
    } parameters;
    TYPE* valType;
    SIGDECL* next; // the next SIGDEL follows after a TYPE tCOMMA
    // e.g. func main("a, b int", "c bool") where " " delimits a SIGDECL
    SYMBOL* symbol;
};

/*************************************************************************/

struct EXPRLIST { // for assignment, like a[1], a[2], a[3] = 3, a[4], b.field (lhs and rhs are both exprlists)
    struct {
        EXPR* thisEXPR;
        TYPE* type;
    } content;
    EXPRLIST* next;
};

/*************************************************************************/

// block statement node
struct STMT {
    int lineno;
    StatementKind kind;
    union {
        // empty statements
        struct { // expression statements
            EXPR* expression;
        } expression;
        struct { // assignment statements
            EXPR* identifier;
            EXPR* expression;
            assignOp opassign; //a += 3
        } assignment;
        struct { // implicit list assignment
            EXPRLIST* LHS;
            EXPRLIST* RHS;
            // only allow to use "=" operator
        } exprlistAssignment;
        struct { // short variable declaration statement
            ARGLIST* identifierList;
            EXPRLIST* exprList;
        } shortVarDecl;
        struct { // only to convert declaration STMT to TOPDECL
            TOPDECL* topDecl;
        } topDecl;
        struct { // increment and decrement statements
            EXPR* expression;
        } incDec;
        struct { // print and println statements
            EXPRLIST* exprlist;
        } print;
        struct { // return statement
            EXPR* expr;
        } returnStmt;
        struct { // if statement
            STMT* simpleStmt; // optional simpleStatement
            EXPR* condition;
            STMT* stmt;
            STMT* elseStmt;
        } ifStmt;
        struct {
            STMT* stmt;
        } elseStmt;
        struct {
            STMT* ifStmt;
        } elseIfStmt;
        struct { // for statement
            EXPR* condition;
            STMT* initStmt;
            STMT* postStmt;
            STMT* stmtBlock;
        } forStmt;
        struct { // switch statement
            STMT* simpleStmt; // optional simpleStatement
            EXPR* optionalExpr; // optional expression
            CASE* c;
        } switchStmt;
    } val;
    STMT* next;
    SYMBOL* symbol;
};

/*************************************************************************/

struct CASE {
    CASE* condition;

    CaseType caseType; //case/default
    EXPRLIST* matches; // case 2,3,4  <- "2,3,4"

    STMT* action;
    CASE* next;

    SYMBOL* symbol;
};

/*************************************************************************/

// expression node
struct EXPR {
    int lineno;
    ExpressionKind kind;
    TYPE* type; // never assigned nor used
    bool parenthesized;
    union {
        char* identifier;
        bool boolLiteral;
        int intLiteral;
        float floatLiteral;
        char* stringLiteral;
        char* runeLiteral;
        EXPR* unaryExpr; // unary operations
        struct {
            EXPR* lhs;
            EXPR* rhs;
        } binary; // binary operations
        struct {
            EXPR* parent; // expr in expr[index] and expr.id for array/slice/struct
            char* fieldName; // id in expr.id in struct
            EXPR* index; // index in expr[index] for array and slice indexing
            EXPR* appendValue; // e2 in append(e1, e2) in array/slice
        } refobj; // for structs, arrays, and slices
        struct { // function call is an expression
            EXPRLIST* arguments;
            EXPR* caller;
            char* methodName; // struct function call
        } functionCall;
    } val;
    SYMBOL* symbol;
};

/*************************************************************************/

PROG* genPROG(PKG* package, TOPDECL* topLevelDecls);
PKG* genPKG(char* packageName);

ARGLIST* genIDENTIFIERS(char* identifier, ARGLIST* next);
EXPRLIST* genEXPRESSIONS(EXPR* thisExpr, EXPRLIST* next);

TOPDECL* genVARDECL(DeclarationKind kind, ARGLIST* varlist, TYPE* type, EXPRLIST* values);
TOPDECL* genTYPEDECL(DeclarationKind kind, char* identifier, TYPE* type);
TOPDECL* genTYPEDECL_emptyDistributed(void);

TYPE* genTYPE(TypeKind tk, char* namedtype, TYPE* slicetype, int arraySize, TYPE* arraytype, FIELDDECL* structype);

FIELDDECL* genTYPEDECL_fieldDecl(ARGLIST* identifierList, TYPE* type, TYPENAME* typeName);

TOPDECL* genFUNCDECL(char* functionName, SIGNATURE* signature, STMT* block);

SIGDECL* genFUNCDECL_shortSigDecl(ARGLIST* identifierList, TYPE* type, SIGDECL* nextSigDecl);
SIGNATURE* genSIGNATURE(SIGDECL* sig, TYPE* optional);

// generate block statement node

STMT* genSTMT_TOPDECL(TOPDECL* declaration);

STMT* genSTMT_shortVarDecl(ARGLIST* identifiers, EXPRLIST* exprList);

STMT* genSTMT_expression(EXPR* expression);

STMT* genSTMT_assignment(EXPR* ident, EXPR* expr, assignOp aop);
STMT* genSTMT_assignments(EXPRLIST* lhs, EXPRLIST* rhs);

STMT* genSTMT_incDec(StatementKind kind, EXPR* expr);
STMT* genSTMT_print(StatementKind kind, EXPRLIST* exprlist);

STMT* genSTMT_if(EXPR* condition, STMT* optionalStmt, STMT* stmts, STMT* elseStmt);
STMT* genSTMT_else(STMT* statements);
STMT* genSTMT_elseIf(STMT* ifStatement);

STMT* genSTMT_switch(STMT* stmt, EXPR* expr, CASE* c);

CASE* genCASE(CaseType ct, CASE* cc, STMT* actions, EXPRLIST* matches);

STMT* genSTMT_forCondition(EXPR* condition, STMT* stmtBlock);
STMT* genSTMT_forStatement(STMT* initStmt, EXPR* condition, STMT* postStmt, STMT* stmtBlock);

STMT* genSTMT_return(EXPR* expr);
STMT* genSTMT_break(void);
STMT* genSTMT_continue(void);

STMT* genSTMTS(STMT* thisStmt, STMT* list);
STMT* genSTMT_empty(void);

// generate expression node

EXPR* genEXPR_identifier(char* identifier);
EXPR* genEXPR_boolLiteral(bool boolLiteral);
EXPR* genEXPR_intLiteral(int intLiteral);
EXPR* genEXPR_floatLiteral(float floatLiteral);
EXPR* genEXPR_stringLiteral(char* stringLiteral);
EXPR* genEXPR_runeLiteral(char* character);
EXPR* genEXPR_rawstringLiteral(char* rawLit);

EXPR* genEXPR_unary(ExpressionKind op, EXPR* expr);
EXPR* genEXPR_binaryOperation(ExpressionKind op, EXPR* lhs, EXPR* rhs);

EXPR* genEXPR_structField(ExpressionKind op, EXPR* ident, char* fieldname);
EXPR* genEXPR_arrayVal(ExpressionKind op, EXPR* ident, EXPR* index); //ident : name of the struct obj, needs to be EXPR type because it could nest
EXPR* genEXPR_sliceAppend(ExpressionKind op, EXPR* slice, EXPR* toBeAppendedVal);
EXPR* genEXPR_functionCall(ExpressionKind op, char* funcname, EXPRLIST* arguments);
EXPR* genEXPR_structFunctionCall(ExpressionKind op, EXPR* caller, char* methodName, EXPRLIST* arguments); //this should be straightforward XD
EXPR* genEXPR_LenOrCap(ExpressionKind kind, EXPR* expr);

_Noreturn void throwError(const int, const char*, const int);

#endif /* TREE_H */
