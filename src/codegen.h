#ifndef CODEGEN_H
#define CODEGEN_H

#include "tree.h"

void codePrintRaw(char*);
void codePrintTabs(int);

void codePROG(PROG*);
void codeSetup(void);
void codeINCLUDE(void);
void codeSetupSLICE(void);
void codeSetupSpecialFunctions(void);
void codeTOPDECL(TOPDECL*, int);
void codeMAIN(void);
void codeFUNCTION(char*, SIGNATURE*, STMT*, int);
void codeVARDECL(TOPDECL*, int);
void codeTYPEDECL(TOPDECL*, int);
void codeTYPE(TYPE*, int);
void codeSTRUC(FIELDDECL*, char*, int);
void codeFIELDDECL(FIELDDECL*, int);
void codeFUNCDECL(char*, SIGNATURE*, STMT*);
void codeTYPEDEF(TOPDECL*, int); // TODO keep or use other function?

void codeFUNCSIG(SIGNATURE*);
void codeForBody(STMT*, int, STMT*, char*);
void codeSTMT(STMT*, int, bool, char*);
void codeCASE(CASE*, int, char*);
void codeSHORTVARDECL(STMT*, int);
void codeEXPRLIST(EXPRLIST*);
void codeEXPR(EXPR*);
void codeARGLIST(ARGLIST* al, int l);
void codeSIGDECL(SIGDECL* sd, int l);
void codeVAR(VAR* v, int l);

bool isDeclared(VAR*);


#endif /* CODEGEN_H */
