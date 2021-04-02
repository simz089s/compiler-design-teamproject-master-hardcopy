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

//TODO change int/float64/string/rune/bool to not be reserved keywords


// Stores the line number with each token sent to bison
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}

/* Helper definitions */

/* Alphanumerics */
DIGIT [0-9]
ESCAPES "\\n"|"\\a"|"\\b"|"\\f"|"\\r"|"\\t"|"\\v"|"\\\""|"\\\\"|"\\\'"
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
\n              { if (needsSemiColon()) { if(g_tokens) printf("Inserted Semicolon\n"); RETURN(tSEMICOLON); } }
"struct"        { if (g_tokens) printf("tSTRUCT\n"); RETURN(tSTRUCT); }
"break"         { if (g_tokens) printf("tBREAK\n"); RETURN(tBREAK); }
"case"          { if (g_tokens) printf("tCASE\n"); RETURN(tCASE); }
"chan"          { if (g_tokens) printf("tCHAN\n"); RETURN(tCHAN); }
"const"         { if (g_tokens) printf("tCONST\n"); RETURN(tCONST); }
"continue"      { if (g_tokens) printf("tCONTINUE\n"); RETURN(tCONTINUE); }
"default"       { if (g_tokens) printf("tDEFAULT\n"); RETURN(tDEFAULT); }
"defer"         { if (g_tokens) printf("tDEFER\n"); RETURN(tDEFER); }
"else"	        { if (g_tokens) printf("tELSE\n"); RETURN(tELSE); }
"fallthrough"   { if (g_tokens) printf("tFALLTHROUGH\n"); RETURN(tFALLTHROUGH); }
"for"           { if (g_tokens) printf("tFOR\n"); RETURN(tFOR); }
"func"          { if (g_tokens) printf("tFUNC\n"); RETURN(tFUNC); }
"go"            { if (g_tokens) printf("tGO\n"); RETURN(tGO); }
"goto"          { if (g_tokens) printf("tGOTO\n"); RETURN(tGOTO); }
"if"	        { if (g_tokens) printf("tIF\n"); RETURN(tIF); }
"import"        { if (g_tokens) printf("tIMPORT"); RETURN(tIMPORT); }
"interface"     { if (g_tokens) printf("tINTERFACE\n"); RETURN(tINTERFACE); }
"map"           { if (g_tokens) printf("tMAP\n"); RETURN(tMAP); }
"package"       { if (g_tokens) printf("tPACKAGE\n"); RETURN(tPACKAGE); }
"range"         { if (g_tokens) printf("tRANGE\n"); RETURN(tRANGE); }
"return"        { if (g_tokens) printf("tRETURN\n"); RETURN(tRETURN); }
"select"        { if (g_tokens) printf("tSELECT\n"); RETURN(tSELECT); }
"switch"        { if (g_tokens) printf("tSWITCH\n"); RETURN(tSWITCH); }
"type"          { if (g_tokens) printf("tTYPE\n"); RETURN(tTYPE); }
"var"           { if (g_tokens) printf("tVAR\n"); RETURN(tVAR); }
"print"	        { if (g_tokens) printf("tPRINT\n"); RETURN(tPRINT); }
"println"	    { if (g_tokens) printf("tPRINTLN\n"); RETURN(tPRINTLN); }
"append"        { if (g_tokens) printf("tAPPEND\n"); RETURN(tAPPEND); }
"len"           { if (g_tokens) printf("tLEN\n"); RETURN(tLEN); }
"cap"           { if (g_tokens) printf("tCAP\n"); RETURN(tCAP); }

"."     { if (g_tokens) printf("tDOT\n"); RETURN(tDOT); }
"+"	    { if (g_tokens) printf("tPLUS\n"); RETURN(tPLUS); }
"-"	    { if (g_tokens) printf("tMINUS\n"); RETURN(tMINUS); }
"*"	    { if (g_tokens) printf("tTIMES\n"); RETURN(tTIMES); }
"/"	    { if (g_tokens) printf("tDIV\n"); RETURN(tDIV); }
"%"     { if (g_tokens) printf("tMODULO\n"); RETURN(tMODULO); }
"&"     { if (g_tokens) printf("tBITAND\n"); RETURN(tBITAND); }
"|"     { if (g_tokens) printf("tBITOR\n"); RETURN(tBITOR); }
"^"     { if (g_tokens) printf("tBITXOR\n"); RETURN(tBITXOR); }
"<<"    { if (g_tokens) printf("tLEFTSHIFT\n"); RETURN(tLEFTSHIFT); }
">>"    { if (g_tokens) printf("tRIGHTSHIFT\n"); RETURN(tRIGHTSHIFT); }
"&^"    { if (g_tokens) printf("tANDNOT\n"); RETURN(tANDNOT); }
"+="    { if (g_tokens) printf("tPLUSEQ\n"); RETURN(tPLUSEQ); }
"-="    { if (g_tokens) printf("tMINUSEQ\n"); RETURN(tMINUSEQ); }
"*="    { if (g_tokens) printf("tTIMESEQ\n"); RETURN(tTIMESEQ); }
"/="    { if (g_tokens) printf("tDIVEQ\n"); RETURN(tDIVEQ); }
"%="    { if (g_tokens) printf("tMODEQ\n"); RETURN(tMODEQ); }
"^="    { if (g_tokens) printf("tBITXOREQ\n"); RETURN(tBITXOREQ); }
"&="    { if (g_tokens) printf("tANDEQ\n"); RETURN(tANDEQ); }
"|="    { if (g_tokens) printf("tOREQ\n"); RETURN(tOREQ); }
"<<="   { if (g_tokens) printf("tLEFTSHIFTEQ\n"); RETURN(tLEFTSHIFTEQ); }
">>="   { if (g_tokens) printf("tRIGHTSHIFTEQ\n"); RETURN(tRIGHTSHIFTEQ); }
"&^="   { if (g_tokens) printf("tANDNOTEQ\n"); RETURN(tANDNOTEQ); }
"&&"    { if (g_tokens) printf("tAND\n"); RETURN(tAND); }
"||"    { if (g_tokens) printf("tOR\n"); RETURN(tOR); }
"<-"    { if (g_tokens) printf("tRECEIVE\n"); RETURN(tRECEIVE); }
"++"    { if (g_tokens) printf("tINCREMENT\n"); RETURN(tINCREMENT); }
"--"    { if (g_tokens) printf("tDECREMENT\n"); RETURN(tDECREMENT); }
"=="    { if (g_tokens) printf("tEQ\n"); RETURN(tEQ); }
"<"     { if (g_tokens) printf("tLT\n"); RETURN(tLT); }
">"     { if (g_tokens) printf("tGT\n"); RETURN(tGT); }
"="     { if (g_tokens) printf("tASSIGN\n"); RETURN(tASSIGN); }
"!"     { if (g_tokens) printf("tUNARY_NOT\n"); RETURN(tUNARY_NOT); }
"!="    { if (g_tokens) printf("tNEQ\n"); RETURN(tNEQ); }
"<="    { if (g_tokens) printf("tLEQ\n"); RETURN(tLEQ); }
">="    { if (g_tokens) printf("tGEQ\n"); RETURN(tGEQ); }
":="    { if (g_tokens) printf("tSHORTDEC\n"); RETURN(tSHORTDEC); }
"..."   { if (g_tokens) printf("tDOTDOTDOT\n"); RETURN(tDOTDOTDOT); }
"("	    { if (g_tokens) printf("tLPAREN\n"); RETURN(tLPAREN); }
")"	    { if (g_tokens) printf("tRPAREN\n"); RETURN(tRPAREN); }
"{"	    { if (g_tokens) printf("tLBRACE\n"); RETURN(tLBRACE); }
"}"	    { if (g_tokens) printf("tRBRACE\n"); RETURN(tRBRACE); }
"]"     { if (g_tokens) printf("tRSQUAREBRACE\n"); RETURN(tRSQUAREBRACE); }
"["     { if (g_tokens) printf("tLSQUAREBRACE\n"); RETURN(tLSQUAREBRACE); }
":"     { if (g_tokens) printf("tCOLON\n"); RETURN(tCOLON); }
";"     { if (g_tokens) printf("tSEMICOLON\n"); RETURN(tSEMICOLON); } 
","     { if (g_tokens) printf("tCOMMA\n"); RETURN(tCOMMA); }

"//".*\n        { if (needsSemiColon()) { RETURN(tSEMICOLON); } }
"//".*          

\/\* 		        { BEGIN(COMMENT); }
<COMMENT>\*\/       { BEGIN(INITIAL); }
<COMMENT>.
<COMMENT>\n         { if (needsSemiColon()) { RETURN(tSEMICOLON); } }
<COMMENT><<EOF>>    { fprintf(stderr, "Error: (line %d) unclosed block comment\n", yylineno); exit(EXIT_FAILURE); }


0|([1-9][0-9]*)	 				{
									if (g_tokens) printf("tINTVAL(%s)\n", yytext);
									yylval.intval = strtol(yytext, NULL, 10);
									RETURN(tINTVAL);
							 	}

[0][0-7]*						{
									if (g_tokens) printf("tINTVAL(%s)\n", yytext);
									yylval.intval = strtol(yytext, NULL, 10);
									RETURN(tINTVAL);
								}

"0x"[0-9a-f]+					{
									if (g_tokens) printf("tINTVAL(%s)\n", yytext);
									yylval.intval = strtol(yytext, NULL, 10);
									RETURN(tINTVAL);
								}

[0-9]*\.[0-9]*|[0]\.[0-9]*		{
									if (g_tokens) printf("tFLOATVAL(%s)\n", yytext);
									yylval.floatval = strtof(yytext, NULL);
									RETURN(tFLOATVAL);
								}

"true"|"false"  {
    if (g_tokens) printf("tBOOLVAL(%s)\n", yytext);
    if (strcmp(yytext, "true") == 0) { yylval.boolval = true; }
    else if (strcmp(yytext, "false") == 0) { yylval.boolval = false; }
    RETURN(tBOOLVAL);
}

"\""([a-zA-Z0-9 ~!@#$%^&*\-+\/`<>=_|';:.,?{}\[\]\(\)]|{ESCAPES})*"\"" {
    if (g_tokens) printf("tSTRINGVAL(%s)\n", yytext);
    yylval.stringval = strdup(yytext);
    RETURN(tSTRINGVAL);
}

"\'"({ESCAPES}|[a-zA-Z0-9])"\'" {
    if (g_tokens) printf("tRUNEVAL(%s)\n", yytext);
    yylval.runeval = strdup(yytext);
    RETURN(tRUNEVAL);
}

"`"[^`]*"`" {
    if (g_tokens) printf("tRAWSTRINGVAL(%s)\n", yytext);
    yylval.rawstringval = strdup(yytext);
    RETURN(tRAWSTRINGVAL);
}

[a-zA-Z_][a-zA-Z0-9_]* {
        if (g_tokens) printf("tIDENTIFIER(%s)\n", yytext);
        yylval.identifier = strdup(yytext);
        RETURN(tIDENTIFIER);
    }


<<EOF>> { 
            if(lastToken!=tSEMICOLON){
                RETURN(tSEMICOLON); 
                if(g_tokens) printf("inserted semicolon\n"); 
            }
            yyterminate(); 

        }

.	{
	fprintf(stderr, "Error: (line %d) unexpected character '%s'\n", yylineno, yytext);
	exit(1);
}
%%
