%{

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tree.h"

extern int yylineno;

extern PROG* root;

int yylex();

void blockCheck(STMT* stmt, bool mayBreaks, bool mayContinue);
void switchContinues(CASE* c);

//weeding variables to make sure breaks and continues are used in the appropriate places
int allowBreaks = 0;
int allowContinue = 0;

//weeding variable to make sure there is at most one default case per switch
int defaultCases = 0;

//weeding variable to make sure var decs LHS are not parenthesized
int allowParenthesizedExpr = 1; //default allowed

void yyerror(const char* s) {
    fprintf(stderr, "Error: (line %d) '%s'\n", yylineno, s);
    exit(1);
}

//makes sure the statement list may or may not have break
void blockCheck(STMT* stmt, bool mayBreaks, bool mayContinue){

    if(stmt==NULL) return;

    if(stmt->kind == k_statementKindElse){
        blockCheck(stmt->val.elseStmt.stmt, mayBreaks, mayContinue);
    }
    if(stmt->kind == k_statementKindElseIf){
        blockCheck(stmt->val.elseIfStmt.ifStmt, mayBreaks, mayContinue);
    }
    if(stmt->kind == k_statementKindIf){
        blockCheck(stmt->val.ifStmt.stmt, mayBreaks, mayContinue);
    }
    if(stmt->kind == k_statementKindSwitch){
        switchContinues(stmt->val.switchStmt.c);
    }

    if(mayBreaks==false){
        if(stmt->kind == k_statementKindBreak){
            yyerror("cannot have break statement here");
        }
    }
    if(mayContinue==false){
        if(stmt->kind == k_statementKindContinue){
            yyerror("cannot have continue statement here");
        }
    }
    if(stmt->next != NULL) blockCheck(stmt->next, mayBreaks, mayContinue);
}

//makes sure the switch bodies don't have continues
void switchContinues(CASE* c){
    if(c!=NULL){
        blockCheck(c->action, true, false);
        if(c->next != NULL){
            switchContinues(c->next);
        }
    }
}

//weeds for short decls. yes = 1 no = 0
int isThisExprlistAnIdentifierList(EXPRLIST* exprlist){
    if(exprlist == NULL) return 1;
    else{
        if(exprlist->content.thisEXPR->kind != k_expressionKindIdentifier){
            return 0;
        }
        else{
            return isThisExprlistAnIdentifierList(exprlist->next);
        }
    }
    return 1;
}
//function called after weeding above successfully (conversion)
ARGLIST* exprlist2arglist(EXPRLIST* exprlist){

    allowParenthesizedExpr=0;
    if(exprlist->content.thisEXPR->parenthesized==true) yyerror("No parenthesized expression allowed on LHS");

    ARGLIST* a = malloc(sizeof(ARGLIST));
    a->thisVAR = malloc(sizeof(VAR));
    a->thisVAR->identifier = exprlist->content.thisEXPR->val.identifier;
    if(exprlist->next!=NULL){
        a->next = exprlist2arglist(exprlist->next);
    }
    return a;

    allowParenthesizedExpr=1;
}

//check lists of rhs and lhs size, must be equal: returns 1 if ok, 0 if not ok
int idvsexpr(ARGLIST* lhs, EXPRLIST* rhs){
    if(lhs!=NULL && rhs!=NULL){
        if (lhs->next == NULL && rhs->next == NULL){
            return 1;
        }
        else if ((lhs->next == NULL && rhs->next != NULL) || (lhs->next != NULL && rhs->next == NULL)){
            return 0;
        }
	    else if(lhs->next != NULL && rhs->next !=NULL) return idvsexpr(lhs->next, rhs->next);
    }
    return 0;
}

int exprvsexpr(EXPRLIST* lhs, EXPRLIST* rhs){
    if(lhs!=NULL && rhs!=NULL){
        if (lhs->next == NULL && rhs->next == NULL){
            return 1;
        }
        else if ((lhs->next == NULL && rhs->next != NULL) || (lhs->next != NULL && rhs->next == NULL)){
            return 0;
        }
        else if(lhs->next != NULL && rhs->next !=NULL) return exprvsexpr(lhs->next, rhs->next);
    }
    return 0;
}

//weeding used to turn (((a)))() into a() (this is a function call)
char* exprtoident(EXPR* exp){
    return exp->val.identifier;
}

%}

%locations
%error-verbose

%code requires
{
	#include "tree.h"
}

%union {
    bool boolval;
    int intval;
    float floatval;
    char* stringval;
    char* runeval;
    char* rawstringval;
    char* identifier;
    PROG* prog;
    PKG* pkg;
    EXPR* expr;
    EXPRLIST* exprlist;
    STMT* stmt;
    TYPE* type;
    TYPENAME* typename;
    FIELDDECL* fielddecl;
    SIGNATURE* signature;
    SIGDECL* sigdecl;
    TOPDECL* topdecl;
    ARGLIST* arglist;
    CASE* casee;
    ExpressionKind kind;
}

%type <prog> program start
%type <pkg> packageClause
%type <stmt> ifStmt elseStmt incDecStmt printlnStmt printStmt assignment simpleStatement statement 
%type <stmt> switchStmt shortVarDecl returnStmt blockStmt
%type <stmt> forStmt continueStmt breakStmt statementList
%type <expr> expression capExpr lengthExpr
%type <expr> functionCall arrayField structField condition
%type <topdecl> topLevelDecls topLevelDecl typeDecl distributedTypeDecl disBody disBodys
%type <topdecl> functionDecl varDecl varDecls distributedVarDecl declaration distributedInside
%type <fielddecl> FieldDecl FieldDecls 
%type <exprlist> switchExprList expressions exprList
%type <type> ArrayType StructType SliceType
%type <signature> signature
%type <sigdecl> sigDecl declList  
%type <arglist> identifierList identifiers
%type <type> Type TypeLit
%type <casee> caseClause switchCase

%token tBREAK
%token tCASE
%token tCHAN
%token tCONST
%token tCONTINUE
%token tDEFAULT
%token tDEFER
%token tELSE
%token tFALLTHROUGH
%token tFOR
%token tFUNC
%token tGO
%token tGOTO
%token tIF
%token tIMPORT
%token tINTERFACE
%token tMAP
%token tPACKAGE
%token tRANGE
%token tRETURN
%token tSELECT
%token tSWITCH
%token tTYPE
%token tVAR
%token tPRINT
%token tPRINTLN
%token tAPPEND
%token tLEN
%token tCAP
%token <boolval> tBOOLVAL
%token <intval> tINTVAL
%token <floatval> tFLOATVAL
%token <stringval> tSTRINGVAL
%token <runeval> tRUNEVAL
%token <rawstringval> tRAWSTRINGVAL

// unary operators : "+" | "-" | "!" | "^" | "*" | "&" | "<-" 
%token tUNARY_PLUS /* 0 + x */
%token tUNARY_MINUS /* 0 - x */
%token tUNARY_NOT /* !x */
%token tDEREF /* (*p) dereference operator */
%token tBIT_FLIP /* m ^ x is bitwise complement */
%token tADDRESS /* (&x) address operator TODO not supported ? */
%token tRECEIVE /* <- TODO not supported ? */

// arithmetic operators 
%token tPLUS
%token tMINUS
%token tTIMES
%token tDIV
%token tMODULO

%token tBITAND /* & bitwise AND */
%token tBITOR /* | bitwise OR */
%token tBITXOR /* ^ bitwise XOR */
%token tANDNOT /* &^ bit clear */

%token tLEFTSHIFT
%token tRIGHTSHIFT

%token tPLUSEQ
%token tMINUSEQ
%token tTIMESEQ
%token tDIVEQ
%token tMODEQ
%token tANDEQ
%token tOREQ
%token tLEFTSHIFTEQ
%token tRIGHTSHIFTEQ
%token tANDNOTEQ
%token tBITXOREQ

// logical operators
%token tAND /* conditional AND */
%token tOR /* conditional OR */
%token tINCREMENT
%token tDECREMENT

// comparision operators
%token tEQ
%token tLT
%token tGT
%token tNEQ
%token tLEQ
%token tGEQ

%token tASSIGN
%token tSHORTDEC
%token tDOTDOTDOT
%token tLPAREN
%token tRPAREN
%token tLBRACE
%token tRBRACE
%token tRSQUAREBRACE
%token tLSQUAREBRACE
%token tCOLON
%token tSEMICOLON

%token <identifier> tIDENTIFIER
%token tSTRUCT
%token tDOT
%token tCOMMA

%left tCOMMA /* lowest precedence */
%left tPLUSEQ tMINUSEQ tTIMESEQ tDIVEQ tMODEQ tANDEQ tOREQ tLEFTSHIFTEQ tRIGHTSHIFTEQ tANDNOTEQ
%left tOR
%left tAND
%left tEQ tLT tGT tNEQ tLEQ tGEQ
%left tPLUS tMINUS tBITOR tBITXOR
%left tTIMES tDIV tMODULO tLEFTSHIFT tRIGHTSHIFT tBITAND tANDNOT
%left tBIT_FLIP tUNARY_MINUS tUNARY_PLUS tUNARY_NOT
%left tRECEIVE tDOT tLSQUAREBRACE tLPAREN tINCREMENT tDECREMENT 
%left tDEREF /* highest precedence */

%start start

%%

start : program { root = $1; }
      ;

program : packageClause tSEMICOLON topLevelDecls { $$ = genPROG($1, $3); } 
        ;

packageClause   : tPACKAGE tIDENTIFIER { $$ = genPKG($2); }
                ;

topLevelDecls   : %empty { $$ = NULL; }
                | topLevelDecl topLevelDecls { $$=$1; if($$!=NULL) $$->next=$2; }
                ;

topLevelDecl    : functionDecl { $$ = $1; }
                | declaration { $$ = $1; }
                ;

declaration : varDecl { $$ = $1; } /* individual variable declaration */
            | distributedVarDecl { 
                if($$==NULL) $$=genVARDECL(k_declarationKindEmptyDistributedVarDecl, NULL, NULL, NULL);
                else $$ = $1;
            } /* distributed variable declaration */
            | typeDecl {$$ = $1; } /* individual type declaration */
            | distributedTypeDecl { $$ = $1; } /* distributed type declaration */
            ;

varDecls    : %empty { $$ = NULL; }
            | distributedInside varDecls { $$ = $1; if($$!=NULL) $$->vardecl.distributedNext = $2; } 
            ;

/* three forms of variable specification */
varDecl : tVAR identifierList Type tSEMICOLON { $$ = genVARDECL(k_declarationKindVariable, $2, $3, NULL); }
        | tVAR identifierList tASSIGN exprList tSEMICOLON { 
            if(idvsexpr($2, $4)==1){
                $$ = genVARDECL(k_declarationKindVariable, $2, NULL, $4); 
            }
            else{
                yyerror("LHS and RHS do not have the same number of arguments.");
            }
        }
        | tVAR identifierList Type tASSIGN exprList tSEMICOLON {  
            if(idvsexpr($2, $5)==1){
                $$ = genVARDECL(k_declarationKindVariable, $2, $3, $5); 
            }
            else{
               yyerror("LHS and RHS do not have the same number of arguments.");
            }
        }
        ;

distributedVarDecl  : tVAR tLPAREN varDecls tRPAREN tSEMICOLON { $$ = $3; }
                    ;

distributedInside   : identifierList Type tSEMICOLON { $$ = genVARDECL(k_declarationKindVariable, $1, $2, NULL); }
                    | identifierList tASSIGN exprList tSEMICOLON { 
                        if(idvsexpr($1, $3)==1){
                            $$ = genVARDECL(k_declarationKindVariable, $1, NULL, $3); 
                        }
                        else{
                            yyerror("LHS and RHS do not have the same number of arguments.");
                        }
                    }
                    | identifierList Type tASSIGN exprList tSEMICOLON {  
                        if(idvsexpr($1, $4)==1){
                             $$ = genVARDECL(k_declarationKindVariable, $1, $2, $4);
                        }
                        else{
                            yyerror("LHS and RHS do not have the same number of arguments.");
                        }
                    }
                    ;
/*
exprlist on LHS should be identifierlist, but this is weeded out to prevent SR conflicts
*/
shortVarDecl    : exprList tSHORTDEC exprList { 
                        if(isThisExprlistAnIdentifierList($1)==1){
                            if(exprvsexpr($1, $3)==1){
                                $$ = genSTMT_shortVarDecl(exprlist2arglist($1), $3); 
                            }
                            else{
                                yyerror("LHS and RHS do not have the same number of arguments.");   
                            }
                        }
                        else{
                            yyerror("identifiers expected on LHS");
                        }
                        
                    }
                ; /* 2.8.6 */

/************************************************************************************/

/* individual type declaration */
typeDecl    : tTYPE tIDENTIFIER Type tSEMICOLON { $$ = genTYPEDECL(k_declarationKindType, $2, $3); }
            ;

/* distributed type declaration */
distributedTypeDecl : tTYPE tLPAREN disBodys tRPAREN tSEMICOLON { 
                        if($3==NULL){
                            $$ = genTYPEDECL_emptyDistributed();
                        }    
                        else{
                            $$ = $3;
                        }     
                    }
                    ;

disBody     : tIDENTIFIER Type tSEMICOLON { $$ = genTYPEDECL(k_declarationKindType, $1, $2); }
            ;

disBodys    : %empty { $$ = NULL; }
            | disBody disBodys { $$ = $1; $$->typedecl.distributedNext = $2; }
            ;

Type    : tIDENTIFIER { $$ = genTYPE(namedType, $1, NULL, -1, NULL, NULL); }
        | TypeLit { $$ = $1; } /* built-ins */ 
        | tLPAREN Type tRPAREN { $$ = $2; }
        ;

TypeLit : ArrayType { $$ = $1; }
        | StructType { $$ = $1; }
        | SliceType { $$ = $1; }
        ;

ArrayType   : tLSQUAREBRACE tINTVAL tRSQUAREBRACE Type { $$ = genTYPE(arrayType, "", NULL, $2, $4, NULL); }
            ;

SliceType   : tLSQUAREBRACE tRSQUAREBRACE Type { $$ = genTYPE(sliceType, "", $3, -1, NULL, NULL); }
            ;

/************************************************************************************/    

StructType  : tSTRUCT tLBRACE FieldDecls tRBRACE {
                if($3==NULL){
                    $$ = genTYPE(structType, "", NULL, TYPE_SIZE_EMPTYSTRUCT, NULL, NULL);
                }
                else{
                    $$ = genTYPE(structType, "", NULL, -1, NULL, $3);
                }
            } /*pls dont change this*/
            ;

/* NOTE : field names must be unique */
FieldDecls  : %empty { $$ = NULL; } /* empty struct */
            | FieldDecl FieldDecls { $$ = $1; if($$!=NULL) $$->next = $2; }
            ;

/* a field in a struct has a field name and a field type */
FieldDecl   : identifierList Type tSEMICOLON { $$ = genTYPEDECL_fieldDecl($1, $2, NULL); } /* explicitly specified field names */
            
            ;


/************************************************************************************/    

functionDecl    : tFUNC tIDENTIFIER signature blockStmt tSEMICOLON { $$ = genFUNCDECL($2, $3, $4); if($4!=NULL) { blockCheck($4, false, false); } }
                ;

signature   : tLPAREN declList tRPAREN  { $$ = genSIGNATURE($2, NULL); }
            | tLPAREN declList tRPAREN Type { $$ = genSIGNATURE($2, $4); }
            ;

declList    : %empty { $$ = NULL; }
            | identifierList Type sigDecl { $$ = genFUNCDECL_shortSigDecl($1, $2, $3); } /* short form : many ids with one type */
            ;

sigDecl : %empty { $$ = NULL; }
        | tCOMMA identifierList Type sigDecl { $$ = genFUNCDECL_shortSigDecl($2, $3, $4); } /* short form */
        ;

/************************************************************************************/    
                
incDecStmt  : expression tINCREMENT { $$ = genSTMT_incDec(k_statementKindIncrement, $1); }
            | expression tDECREMENT { $$ = genSTMT_incDec(k_statementKindDecrement, $1); }
            ; /* 2.8.7 */

printStmt   : tPRINT tLPAREN exprList tRPAREN { $$ = genSTMT_print(k_statementKindPrint, $3); }
            | tPRINT tLPAREN tRPAREN { $$ = genSTMT_print(k_statementKindPrint, NULL); }
            ; /* 2.8.8 */

printlnStmt : tPRINTLN tLPAREN exprList tRPAREN { $$ = genSTMT_print(k_statementKindPrintLn, $3); }
            | tPRINTLN tLPAREN tRPAREN { $$ = genSTMT_print(k_statementKindPrintLn, NULL); }
            ; /* 2.8.8 */

returnStmt  : tRETURN expression { $$ = genSTMT_return($2); }
            | tRETURN { $$ = genSTMT_return(NULL);}
            ; /* 2.8.9 */

/************************************************************************************/    

ifStmt  : tIF expression blockStmt elseStmt { $$ = genSTMT_if($2, NULL, $3, $4);  }
        | tIF simpleStatement tSEMICOLON expression blockStmt elseStmt {  $$ = genSTMT_if($4, $2, $5, $6);  }
        ;

elseStmt    : tELSE blockStmt tSEMICOLON { $$ = genSTMT_else($2);  }
            | tELSE ifStmt { $$ = genSTMT_elseIf($2);  }
            | tSEMICOLON { $$ = NULL; }
            ;

/* example of if statements

if x > max {
	x = max
}

if x := f(); x < y {
	return x
} else if x > z {
	return z
} else {
	return y
}

*/

/************************************************************************************/    

switchStmt  : tSWITCH simpleStatement tSEMICOLON expression tLBRACE caseClause tRBRACE  { $$ = genSTMT_switch($2, $4, $6); defaultCases=0; }
            | tSWITCH expression tLBRACE caseClause tRBRACE {  $$ = genSTMT_switch(NULL, $2, $4); allowBreaks=0; defaultCases=0; }
            | tSWITCH simpleStatement tSEMICOLON tLBRACE caseClause tRBRACE {  $$ = genSTMT_switch($2, NULL, $5); defaultCases=0; }
            | tSWITCH tLBRACE caseClause tRBRACE { $$ = genSTMT_switch(NULL, NULL, $3);  defaultCases=0; }
            ;

caseClause : switchCase tCOLON statementList caseClause { 
                $$ = genCASE(k_notACase, $1, $3, NULL); 
                $$->next = $4; 
                if(defaultCases>1){
                    yyerror("switch statement contains multiple default cases");
                }
            }
            | %empty { $$ = NULL; }
            ;

switchCase  : tCASE expression switchExprList { $$ = genCASE(k_Case, NULL, NULL, genEXPRESSIONS($2, $3)); }
            | tDEFAULT { defaultCases++; $$ = genCASE(k_Default, NULL, NULL, NULL); }
            | %empty { $$ = NULL; } /* default can be anywhere */
            ;

switchExprList  : tCOMMA expression switchExprList { $$ = genEXPRESSIONS($2, $3); }
                | %empty { $$ = NULL; }
                ;

/************************************************************************************/    

forStmt : tFOR condition blockStmt  { $$ = genSTMT_forCondition($2, $3); }
        | tFOR simpleStatement tSEMICOLON condition tSEMICOLON assignment blockStmt { $$ = genSTMT_forStatement($2, $4, $6, $7); }
        | tFOR simpleStatement tSEMICOLON condition tSEMICOLON incDecStmt blockStmt { $$ = genSTMT_forStatement($2, $4, $6, $7); }
        | tFOR simpleStatement tSEMICOLON condition tSEMICOLON functionCall blockStmt { $$ = genSTMT_forStatement($2, $4, genSTMT_expression($6), $7); }
        | tFOR simpleStatement tSEMICOLON condition tSEMICOLON blockStmt { $$ = genSTMT_forStatement($2, $4, NULL, $6); }
        ;

condition   : expression /* while loop */ { $$ = $1; }
            | %empty /* infinite loop */ { $$ = NULL; }
            ;

breakStmt   : tBREAK { $$ = genSTMT_break(); }
            ;

continueStmt    : tCONTINUE { $$ = genSTMT_continue(); }
                ;

statementList   : %empty { $$ = NULL; }
                | statement statementList { $$ = $1; if($$!=NULL) $$->next = $2; }
                ;

blockStmt   : tLBRACE statementList tRBRACE { $$ = $2; }
            ;

statement   : tSEMICOLON { $$ = genSTMT_empty(); }
            | declaration { if($$!=NULL) $$ = genSTMT_TOPDECL($1); }
            | returnStmt tSEMICOLON { $$ = $1; }
            | breakStmt tSEMICOLON { $$ = $1; }
            | continueStmt tSEMICOLON { $$ = $1; }
            | blockStmt tSEMICOLON { $$ = $1; blockCheck($1, false, false); }
            | switchStmt tSEMICOLON { $$ = $1; }
            | ifStmt { $$ = $1; }
            | forStmt tSEMICOLON { $$ = $1; }
            | printStmt tSEMICOLON { $$ = $1; }
            | printlnStmt tSEMICOLON { $$ = $1; }
            | assignment tSEMICOLON { $$ = $1; }
            | shortVarDecl tSEMICOLON { $$ = $1; }
            | incDecStmt tSEMICOLON { $$ = $1; }
            | functionCall tSEMICOLON { $$ = genSTMT_expression($1); }
            ;

/* simple statement used in if stmt and for stmt and switch stmt */
simpleStatement   : %empty { $$ = genSTMT_empty(); }
                | expression {$$ = genSTMT_expression($1); }
                | assignment { $$ = $1; }
                | shortVarDecl { $$ = $1; }
                | incDecStmt { $$ = $1; }
                ;

assignment  : exprList tASSIGN exprList { 
                if(exprvsexpr($1, $3)==1){ 
                    $$ = genSTMT_assignments($1, $3); 
                }
                else{
                    yyerror("LHS and RHS do not have the same number of arguments.");
                }
            }
            | expression tPLUSEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpAddition); }
            | expression tMINUSEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpSubtraction); }
            | expression tOREQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpBitOr); }
            | expression tBITXOREQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpBitXor); }
            | expression tTIMESEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpMult); }
            | expression tDIVEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpDiv); }
            | expression tMODEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpModulo); }
            | expression tLEFTSHIFTEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpLeftShift); }
            | expression tRIGHTSHIFTEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpRightShift); }
            | expression tANDEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpBitAnd); }
            | expression tANDNOTEQ expression { $$ = genSTMT_assignment($1, $3, k_assignOpAndNot); }
;

exprList    : expression expressions { $$ = genEXPRESSIONS($1, $2); }
            ;

expressions : tCOMMA expression expressions { $$ = genEXPRESSIONS($2, $3); }
            | %empty { $$ = NULL; }
            ;

expression  : tINTVAL { $$ = genEXPR_intLiteral($1); }
            | tFLOATVAL { $$ = genEXPR_floatLiteral($1); }
            | tSTRINGVAL { $$ = genEXPR_stringLiteral($1); }
            | tRAWSTRINGVAL { $$ = genEXPR_rawstringLiteral($1); }
            | tBOOLVAL { $$ = genEXPR_boolLiteral($1); }
            | tRUNEVAL { $$ = genEXPR_runeLiteral($1); }
            | tIDENTIFIER { $$ = genEXPR_identifier($1); }
            | expression tOR expression { $$ = genEXPR_binaryOperation(k_expressionKindOr, $1, $3); }
            | expression tAND expression { $$ = genEXPR_binaryOperation(k_expressionKindAnd, $1, $3); }
            | expression tEQ expression { $$ = genEXPR_binaryOperation(k_expressionKindEquals, $1, $3); }
            | expression tNEQ expression { $$ = genEXPR_binaryOperation(k_expressionKindNotEquals, $1, $3); }
            | expression tLT expression { $$ = genEXPR_binaryOperation(k_expressionKindLess, $1, $3); }
            | expression tGT expression { $$ = genEXPR_binaryOperation(k_expressionKindGreater, $1, $3); }
            | expression tLEQ expression { $$ = genEXPR_binaryOperation(k_expressionKindLessEqual, $1, $3); }
            | expression tGEQ expression { $$ = genEXPR_binaryOperation(k_expressionKindGreaterEqual, $1, $3); }
            | expression tPLUS expression { $$ = genEXPR_binaryOperation(k_expressionKindAddition, $1, $3); }
            | expression tMINUS expression { $$ = genEXPR_binaryOperation(k_expressionKindSubtraction, $1, $3); }
            | expression tBITOR expression { $$ = genEXPR_binaryOperation(k_expressionKindBitOr, $1, $3); }
            | expression tBITXOR expression { $$ = genEXPR_binaryOperation(k_expressionKindBitXor, $1, $3); }
            | expression tTIMES expression { $$ = genEXPR_binaryOperation(k_expressionKindMultiplication, $1, $3); }
            | expression tDIV expression { $$ = genEXPR_binaryOperation(k_expressionKindDivision, $1, $3); }
            | expression tMODULO expression { $$ = genEXPR_binaryOperation(k_expressionKindModulo, $1, $3); }
            | expression tLEFTSHIFT expression { $$ = genEXPR_binaryOperation(k_expressionKindLeftShift, $1, $3); }
            | expression tRIGHTSHIFT expression { $$ = genEXPR_binaryOperation(k_expressionKindRightShift, $1, $3); }
            | expression tBITAND expression { $$ = genEXPR_binaryOperation(k_expressionKindBitAnd, $1, $3); }
            | expression tANDNOT expression { $$ = genEXPR_binaryOperation(k_expressionKindAndNot, $1, $3); }
            | tLPAREN expression tRPAREN { 
                    $$=$2;
                    $$->parenthesized = true;
            }
            | tAPPEND tLPAREN expression tCOMMA expression tRPAREN { $$ = genEXPR_sliceAppend(k_expressionKindSliceAppend, $3, $5); }
            | arrayField { $$ = $1; }
            | structField { $$ = $1; }
            | lengthExpr { $$ = $1; }
            | capExpr { $$ = $1; }
            | functionCall { $$ = $1; }
            | tPLUS expression %prec tUNARY_PLUS { $$ = genEXPR_unary(k_expressionKindPlus,$2); }
            | tMINUS expression %prec tUNARY_MINUS { $$ = genEXPR_unary(k_expressionKindMinus, $2); }
            | tUNARY_NOT expression { $$ = genEXPR_unary(k_expressionKindNot, $2); }
            | tBITXOR expression %prec tBIT_FLIP { $$ = genEXPR_unary(k_expressionKindBitFlip, $2); }
            ;

/* expr is stored in UnaryExpr in the EXPR struct */
lengthExpr  : tLEN tLPAREN expression tRPAREN { $$ = genEXPR_LenOrCap(k_expressionKindLength, $3); } 
            ;

capExpr     : tCAP tLPAREN expression tRPAREN { $$ = genEXPR_LenOrCap(k_expressionKindCapacity, $3); }
            ;

structField : expression tDOT tIDENTIFIER { $$ = genEXPR_structField(k_expressionKindStructField, $1, $3); }
            ;

arrayField  : expression tLSQUAREBRACE expression tRSQUAREBRACE { $$ = genEXPR_arrayVal(k_expressionKindArrayVal, $1, $3); }
            ;

functionCall    : expression tLPAREN exprList tRPAREN { $$ = genEXPR_functionCall(k_expressionKindFunctionCall, exprtoident($1), $3); }
                | expression tLPAREN tRPAREN { $$ = genEXPR_functionCall(k_expressionKindFunctionCall, exprtoident($1), NULL); }
                ;

identifierList  : tIDENTIFIER identifiers { $$ = genIDENTIFIERS($1, $2); }
                ;

identifiers : %empty { $$ = NULL; }
            | tCOMMA identifierList { $$ = $2; }
            ;
%%
