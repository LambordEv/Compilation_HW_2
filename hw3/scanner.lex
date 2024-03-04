%{
/*Declaration Section*/
/*------- Include Section -------*/
#include <stdio.h>
#include "tokens.hpp"

/*------- Function Declarion Section -------*/
void printRecognisedToken(const char* whichToken);
void printCommentToken(void);
void accumulateStringToken(const char* toAdd, int toAddLen);
void checkAsciiHexValueRange(char toCheck);

%}

%option yylineno
%option noyywrap

whitespace          				       ([\t\n\r ])
printable           				       ([\x20-\x7E\x09\x0A\x0D])
    
decimalDigit        				       [0-9]
hexDigit            				       [0-9a-fA-F]
letter              				       [a-zA-Z]
    
relop               				       (==|!=|<=|>=|<|>)
binop               				       (\+|\-|\*|\/)
id                  				       ({letter}+)(({decimalDigit}|{letter})*)
num                 				       ((0|([1-9]+{decimalDigit}*))) 
    
%x COMMENT		       
enterComment        				       "\/\/"
    
%x STRING				       
enterString         				       \"
stringLegalChars            		       ([\x20-\x7e\x09\x0a\x0d]{-}["\n\r\\])
stringIlegalChars                          ([^ ]{-}{stringLegalChars})
    
%x STRING_ESCAPE       
escapeChar                                 "\\"
twoDigitsHexNum                            (x([0-9a-fA-F]{2}))
    
%%		       
"void"               				       printRecognisedToken("VOID");
"int"                				       printRecognisedToken("INT");
"byte"               				       printRecognisedToken("BYTE");
"b"                  				       printRecognisedToken("B");
"bool"               				       printRecognisedToken("BOOL");
"and"                				       printRecognisedToken("AND");
"or"                 				       printRecognisedToken("OR");
"not"                				       printRecognisedToken("NOT");
"true"               				       printRecognisedToken("TRUE");
"false"              				       printRecognisedToken("FALSE");
"return"             				       printRecognisedToken("RETURN");
"if"                 				       printRecognisedToken("IF");
"else"               				       printRecognisedToken("ELSE");
"while"              				       printRecognisedToken("WHILE");
"break"              				       printRecognisedToken("BREAK");
"continue"           				       printRecognisedToken("CONTINUE");
";"                  				       printRecognisedToken("SC");
"("                 				       printRecognisedToken("LPAREN");
")"                 				       printRecognisedToken("RPAREN");
"{"                 				       printRecognisedToken("LBRACE");
"}"                 				       printRecognisedToken("RBRACE");
"="                  				       printRecognisedToken("ASSIGN");
{relop}              				       printRecognisedToken("RELOP");
{binop}              				       printRecognisedToken("BINOP");
{num}                                      printRecognisedToken("NUM");
{id}                                       printRecognisedToken("ID");
{whitespace}                   		       ;
    
    
{enterComment}       				       { BEGIN(COMMENT); printCommentToken(); }
<COMMENT>[\r\n]             	           { BEGIN(INITIAL); }
<COMMENT>.                                 ;

{enterString}        				       { BEGIN(STRING); }
<STRING>{enterString}  				       { accumulateStringToken(NULL, 0); BEGIN(INITIAL); }
<STRING>(\n) 		        		       { printf("Error unclosed string\n"); exit(0); }
<STRING>(\r) 		        		       { printf("Error unclosed string\n"); exit(0); }

<STRING>{escapeChar}                       { BEGIN(STRING_ESCAPE); }
<STRING_ESCAPE>[t]                         { accumulateStringToken("\t", 1); BEGIN(STRING); }
<STRING_ESCAPE>[n]                         { accumulateStringToken("\n", 1); BEGIN(STRING); }
<STRING_ESCAPE>[r]                         { accumulateStringToken("\r", 1); BEGIN(STRING); }
<STRING_ESCAPE>["]                         { accumulateStringToken("\"", 1); BEGIN(STRING); }
<STRING_ESCAPE>[0]                         { accumulateStringToken("\0", 1); BEGIN(STRING); }
<STRING_ESCAPE>[\\]                        { accumulateStringToken("\\", 1); BEGIN(STRING); }
<STRING_ESCAPE>{twoDigitsHexNum}           { char toPrint = strtol(&yytext[1], NULL, 16); checkAsciiHexValueRange(toPrint); BEGIN(STRING); }
<STRING_ESCAPE>x..                         { printf("Error undefined escape sequence %s\n", yytext); exit(0); }
<STRING_ESCAPE>.                           { printf("Error undefined escape sequence %s\n", yytext); exit(0); }

<STRING>{stringLegalChars}         		   { accumulateStringToken(yytext, yyleng); }
<STRING>[^"]                               { printf("Error %s\n", yytext); exit(0); }

.                                          { printf("Error %s\n", yytext); exit(0); }

%%
void printRecognisedToken(const char* whichToken)
{
    printf("%d ", yylineno);
    printf("%s ", whichToken);
    printf("%s\n", yytext);
}

void printCommentToken(void)
{
    printf("%d ", yylineno);
    printf("%s ", "COMMENT");
    printf("//\n");
}

void accumulateStringToken(const char* toAdd, int toAddLen)
{
    static char accumulated[2096] = "";
    static int currStringLen = 0;

    if(NULL == toAdd)
    {
        char acctualToPrint[2096] = "";
        memcpy(acctualToPrint, accumulated, currStringLen);

        printf("%d ", yylineno);
        printf("%s ", "STRING");
        printf("%s\n", acctualToPrint);
        currStringLen = 0;
    }
    else
    {
        memcpy(accumulated + currStringLen, toAdd, toAddLen);
        currStringLen += toAddLen;
    }
}

void checkAsciiHexValueRange(char toCheck)
{
    if(0x0A == toCheck || 0x0D == toCheck || 0x09 == toCheck || (0x20 <= toCheck && 0x7E >= toCheck))
    {
        accumulateStringToken(&toCheck, 1);
    }
    else
    {
        printf("Error undefined escape sequence %s\n", yytext); 
        exit(0);
    }
}
