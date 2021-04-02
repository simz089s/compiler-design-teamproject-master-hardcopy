/* Code section */
%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.tab.h" // Token definitions from the parser

#define RETURN(token) lastToken = token; return token;

extern bool g_tokens;

int lastToken = -1;

int needsSemiColon()
{
	return lastToken == tIDENTIFIER
        || lastToken == tBLANKIDENTIFIER
        || lastToken == tFLOAT
        || lastToken == tINT
        || lastToken == tBOOL
        || lastToken == tRUNE
        || lastToken == tSTRING
        || lastToken == tINTVAL
        || lastToken == tFLOATVAL
		|| lastToken == tBOOLVAL
        || lastToken == tRAWSTRINGVAL
		|| lastToken == tSTRINGVAL
        || lastToken == tRUNEVAL
        || lastToken == tBREAK
        || lastToken == tCONTINUE
        || lastToken == tFALLTHROUGH
        || lastToken == tRETURN
        || lastToken == tINCREMENT
        || lastToken == tDECREMENT
        || lastToken == tRBRACE
        || lastToken == tRSQUAREBRACE
        || lastToken == tRPAREN;
}

// Stores the line number with each token sent to bison
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}

/* Helper definitions */

/* Alphanumerics */
DIGIT [0-9]
ESCAPES "\\n"|"\\a"|"\\b"|"\\f"|"\\r"|"\\t"|"\\v"|"\\\""|"\\\\"|""|"\\\'"
ASCIIchars [ -~]

/* SCANNER OPTIONS */
%option yylineno

/* Removes warnings about input/output functions */
%option noinput
%option nounput

/*Block comment state*/
%x COMMENT

/* TODO : fix ASCII characters in rules */

/* RULES SECTION */
%%

[ \t\r]+
\n              { if (needsSemiColon()) { printf(";\n"); RETURN(tSEMICOLON); } }
"struct"        { if (g_tokens) printf("struct"); RETURN(tSTRUCT); }
"break"         { if (g_tokens) printf("break"); RETURN(tBREAK); }
"case"          { if (g_tokens) printf("case"); RETURN(tCASE); }
"chan"          { if (g_tokens) printf("chan"); RETURN(tCHAN); }
"const"         { if (g_tokens) printf("const"); RETURN(tCONST); }
"continue"      { if (g_tokens) printf("continue"); RETURN(tCONTINUE); }
"default"       { if (g_tokens) printf("default"); RETURN(tDEFAULT); }
"defer"         { if (g_tokens) printf("defer"); RETURN(tDEFER); }
"else"	        { if (g_tokens) printf("else"); RETURN(tELSE); }
"fallthrough"   { if (g_tokens) printf("fallthrough"); RETURN(tFALLTHROUGH); }
"for"           { if (g_tokens) printf("for"); RETURN(tFOR); }
"func"          { if (g_tokens) printf("func"); RETURN(tFUNC); }
"go"            { if (g_tokens) printf("go"); RETURN(tGO); }
"goto"          { if (g_tokens) printf("goto"); RETURN(tGOTO); }
"if"	        { if (g_tokens) printf("if"); RETURN(tIF); }
"import"        { if (g_tokens) printf("import"); RETURN(tIMPORT); }
"interface"     { if (g_tokens) printf("interface"); RETURN(tINTERFACE); }
"map"           { if (g_tokens) printf("map"); RETURN(tMAP); }
"package"       { if (g_tokens) printf("package"); RETURN(tPACKAGE); }
"range"         { if (g_tokens) printf("range"); RETURN(tRANGE); }
"return"        { if (g_tokens) printf("return"); RETURN(tRETURN); }
"select"        { if (g_tokens) printf("select"); RETURN(tSELECT); }
"switch"        { if (g_tokens) printf("switch"); RETURN(tSWITCH); }
"type"          { if (g_tokens) printf("type"); RETURN(tTYPE); }
"var"           { if (g_tokens) printf("var"); RETURN(tVAR); }
"print"         { if (g_tokens) printf("print"); RETURN(tPRINT); }
"println"       { if (g_tokens) printf("println"); RETURN(tPRINTLN); }
"append"        { if (g_tokens) printf("append"); RETURN(tAPPEND); }
"len"           { if (g_tokens) printf("len"); RETURN(tLEN); }
"cap"           { if (g_tokens) printf("cap"); RETURN(tCAP); }
"nil"           { if (g_tokens) printf("nil"); RETURN(tNIL); }

"."     { if (g_tokens) printf("."); RETURN(tDOT); }
"+"     { if (g_tokens) printf("+"); RETURN(tPLUS); }
"-"     { if (g_tokens) printf("-"); RETURN(tMINUS); }
"*"     { if (g_tokens) printf("*"); RETURN(tTIMES); }
"/"     { if (g_tokens) printf("/"); RETURN(tDIV); }
"%"     { if (g_tokens) printf("%%"); RETURN(tMODULO); }
"&"     { if (g_tokens) printf("&"); RETURN(tBITAND); }
"|"     { if (g_tokens) printf("|"); RETURN(tBITOR); }
"^"     { if (g_tokens) printf("^"); RETURN(tBITXOR); }
"<<"    { if (g_tokens) printf("<<"); RETURN(tLEFTSHIFT); }
">>"    { if (g_tokens) printf(">>"); RETURN(tRIGHTSHIFT); }
"&^"    { if (g_tokens) printf("&^"); RETURN(tANDNOT); }
"+="    { if (g_tokens) printf("+="); RETURN(tPLUSEQ); }
"-="    { if (g_tokens) printf("-="); RETURN(tMINUSEQ); }
"*="    { if (g_tokens) printf("*="); RETURN(tTIMESEQ); }
"/="    { if (g_tokens) printf("/="); RETURN(tDIVEQ); }
"%="    { if (g_tokens) printf("%%="); RETURN(tMODEQ); }
"&="    { if (g_tokens) printf("&="); RETURN(tANDEQ); }
"|="    { if (g_tokens) printf("|="); RETURN(tOREQ); }
"<<="   { if (g_tokens) printf("<<="); RETURN(tLEFTSHIFTEQ); }
">>="   { if (g_tokens) printf(">>="); RETURN(tRIGHTSHIFTEQ); }
"&^="   { if (g_tokens) printf("&^="); RETURN(tANDNOTEQ); }
"&&"    { if (g_tokens) printf("&&"); RETURN(tAND); }
"||"    { if (g_tokens) printf("||"); RETURN(tOR); }
"<-"    { if (g_tokens) printf("<-"); RETURN(tRECEIVE); }
"++"    { if (g_tokens) printf("++"); RETURN(tINCREMENT); }
"--"    { if (g_tokens) printf("--"); RETURN(tDECREMENT); }
"=="    { if (g_tokens) printf("=="); RETURN(tEQ); }
"<"     { if (g_tokens) printf("<"); RETURN(tLT); }
">"     { if (g_tokens) printf(">"); RETURN(tGT); }
"="     { if (g_tokens) printf("="); RETURN(tASSIGN); }
"!"     { if (g_tokens) printf("!"); RETURN(tUNARY_NOT); }
"!="    { if (g_tokens) printf("!="); RETURN(tNEQ); }
"<="    { if (g_tokens) printf("<="); RETURN(tLEQ); }
">="    { if (g_tokens) printf(">="); RETURN(tGEQ); }
":="    { if (g_tokens) printf(":="); RETURN(tSHORTDEC); }
"..."   { if (g_tokens) printf("..."); RETURN(tDOTDOTDOT); }
"("	    { if (g_tokens) printf("("); RETURN(tLPAREN); }
")"	    { if (g_tokens) printf(")"); RETURN(tRPAREN); }
"{"	    { if (g_tokens) printf("{"); RETURN(tLBRACE); }
"}"	    { if (g_tokens) printf("}"); RETURN(tRBRACE); }
"]"     { if (g_tokens) printf("]"); RETURN(tRSQUAREBRACE); }
"["     { if (g_tokens) printf("["); RETURN(tLSQUAREBRACE); }
":"     { if (g_tokens) printf(":"); RETURN(tCOLON); }
";"     { if (g_tokens) printf(";"); RETURN(tSEMICOLON); } 
","     { if (g_tokens) printf(","); RETURN(tCOMMA); }

"float64"   { if (g_tokens) printf("float64"); RETURN(tFLOAT); }
"int"	    { if (g_tokens) printf("int"); RETURN(tINT); }
"string"    { if (g_tokens) printf("string"); RETURN(tSTRING); }
"bool"      { if (g_tokens) printf("bool"); RETURN(tBOOL); }
"rune"      { if (g_tokens) printf("rune"); RETURN(tRUNE); }

"//".*['\n']

\/\* 		        { BEGIN(COMMENT); }
<COMMENT>\*\/       { BEGIN(INITIAL); }
<COMMENT>.
<COMMENT>\n         { if (needsSemiColon()) { RETURN(tSEMICOLON); } }
<COMMENT><<EOF>>    { fprintf(stderr, "Error: (line %d) unclosed block comment\n", yylineno); exit(EXIT_FAILURE); }


0|([1-9][0-9]*)	 				{
									if (g_tokens) printf("%s", yytext);
									yylval.intval = strtol(yytext, NULL, 10);
									RETURN(tINTVAL);
							 	}

[0][0-7]*						{
									if (g_tokens) printf("%s", yytext);
									yylval.intval = strtol(yytext, NULL, 10);
									RETURN(tINTVAL);
								}

"0x"[0-9a-f]+					{
									if (g_tokens) printf("%s", yytext);
									yylval.intval = strtol(yytext, NULL, 10);
									RETURN(tINTVAL);
								}

([0-9]*\.[0-9]*)|(.[0-9]+)		{
									if (g_tokens) printf("%s", yytext);
									yylval.floatval = strtof(yytext, NULL);
									RETURN(tFLOATVAL);
								}

"true"|"false"  {
    if (g_tokens) printf("%s", yytext);
    if (strcmp(yytext, "true") == 0) { yylval.boolval = true; }
    else if (strcmp(yytext, "false") == 0) { yylval.boolval = false; }
    RETURN(tBOOLVAL);
}

"\""([a-zA-Z0-9 ~!@#$%^&*\-+\/`<>=_|';:.,?{}\[\]\(\)]|{ESCAPES}|{ASCIIchars})*"\"" {
    if (g_tokens) printf("%s", yytext);
    yylval.stringval = strdup(yytext);
    RETURN(tSTRINGVAL);
}

"\'"({ESCAPES}|{ASCIIchars})"\'" {
    if (g_tokens) printf("%s", yytext);
    yylval.runeval = strdup(yytext);
    RETURN(tRUNEVAL);
}

"`"([^\']({ESCAPES}|{ASCIIchars}))*"`" {
    if (g_tokens) printf("%s", yytext);
    yylval.rawstringval = strdup(yytext);
    RETURN(tRAWSTRINGVAL);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    if (strcmp(yytext, "_") == 0) {
        if (g_tokens) printf("%s", yytext);
        yylval.identifier = strdup(yytext);
        RETURN(tBLANKIDENTIFIER);
    } else {
        if (g_tokens) printf("%s", yytext);
        yylval.identifier = strdup(yytext);
        RETURN(tIDENTIFIER);
    }
}

.	{
	fprintf(stderr, "Error: (line %d) unexpected character '%s'\n", yylineno, yytext);
	exit(1);
}
%%
