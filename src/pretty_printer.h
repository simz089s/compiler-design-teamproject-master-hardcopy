#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "tree.h"

void printRaw(char*);
void printTabs(int);

void prettyPROG(PROG*);
void prettyPKG(PKG*);
void prettyTOPDECL(TOPDECL*, int);
void prettyVARDECL(TOPDECL*, int);
void prettyTYPEDECL(TOPDECL*, int);
void prettyTYPE(TYPE*, int);
void prettySTRUC(FIELDDECL*, int);
void prettyFUNCDECL(char*, SIGNATURE*, STMT*);
// void prettyLONGFUNCSIG(SIGNATURE*);
void prettyFUNCSIG(SIGNATURE*);
void prettySTMT(STMT*, int, bool);
void prettyCASE(CASE*, int);
void prettySHORTVARDECL(STMT*);
void prettyEXPRLIST(EXPRLIST*);
void prettyEXPR(EXPR*);
void prettyARGSLIST(ARGLIST* al, int l);
void prettySIGDECL(SIGDECL* sd, int l);
void prettyVAR(VAR* v, int l);
void prettySTRUCSINGLELINE(FIELDDECL*);
#endif /* PRETTY_PRINTER_H */
