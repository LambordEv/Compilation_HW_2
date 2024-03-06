%{
/*Declaration Section*/
/*------- Include Section -------*/
#include <stdio.h>
#include "hw3_output.hpp"
#include "parser.tab.hpp"\

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
string                                     (\"([^\n\r\"\\]|\\[rnt"\\])+\")   

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
"void"               				       yylval=new Node(yytext);return VOID;
"int"                				       yylval=new Node(yytext);return INT;
"byte"               				       yylval=new Node(yytext);return BYTE;
"b"                  				       yylval=new Node(yytext);return B;
"bool"               				       yylval=new Node(yytext);return BOOL;
"and"                				       yylval=new Node(yytext);return AND;
"or"                 				       yylval=new Node(yytext);return OR;
"not"                				       yylval=new Node(yytext);return NOT;
"true"               				       yylval=new Node(yytext);return TRUE;
"false"              				       yylval=new Node(yytext);return FALSE;
"return"             				       yylval=new Node(yytext);return RETURN;
"if"                 				       yylval=new Node(yytext);return IF;
"else"               				       yylval=new Node(yytext);return ELSE;
"while"              				       yylval=new Node(yytext);return WHILE;
"break"              				       yylval=new Node(yytext);return BREAK;
"continue"           				       yylval=new Node(yytext);return CONTINUE;
";"                  				       yylval=new Node(yytext);return SC;
"("                 				       yylval=new Node(yytext);return LPAREN;
")"                 				       yylval=new Node(yytext);return RPAREN;
"{"                 				       yylval=new Node(yytext);return LBRACE;
"}"                 				       yylval=new Node(yytext);return RBRACE;
"="                  				       yylval=new Node(yytext);return ASSIGN;
{relop}              				       yylval=new Node(yytext);return RELOP;
{binop}              				       yylval=new Node(yytext);return BINOP;
{num}                                      yylval=new Node(yytext);return NUM;
{id}                                       yylval=new Node(yytext);return ID;
{string}                                   yylval=new Node(yytext);return STRING;
{whitespace}                   		       ;

.                                          {output::errorLex(yylineno); exit(0);}

