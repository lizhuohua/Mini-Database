%{
#include <string>
#include "node.h"
#include "parser.h"
%}

letter	[A-Za-z]
digit	[0-9]
id		{letter}({letter}|{digit})*
number	{digit}+

%%
[ \t]		{}
create		{return CREATE;}
table		{return TABLE;}
integer		{return INTEGER;}
insert		{return INSERT;}
into		{return INTO;}
values		{return VALUES;}
select		{return SELECT;}
from		{return FROM;}
where		{return WHERE;}
update		{return UPDATE;}
set			{return SET;}
delete		{return DELETE;}
char		{return CHAR;}
primary		{return PRIMARY;}
key			{return KEY;}
not			{return NOT;}
null		{return NIL;}
foreign		{return FOREIGN;}
references	{return REFERENCES;}
{id}		{yylval.str=new std::string(yytext);
			return IDENTIFIER;}
"*"			{yylval.str=new std::string(yytext);
			return IDENTIFIER;}
{number}	{yylval.integer=atoi(yytext);
			return NUMBER;}
"<"			{yylval.str=new std::string(yytext);return LT_OP;}
"<="		{yylval.str=new std::string(yytext);return LE_OP;}
"="			{yylval.str=new std::string(yytext);return EQ_OP;}
"!="		{yylval.str=new std::string(yytext);return NE_OP;}
">="		{yylval.str=new std::string(yytext);return GE_OP;}
">"			{yylval.str=new std::string(yytext);return GT_OP;}
'[^'.]*'	{yylval.str=new std::string(yytext);return STRING;}
	/*以上不能用'.*'，否则'abc' 'def'会被连续匹配为一个token*/
.			{return *yytext;}
%%
int yywrap()
{
	return 1;
}

/*
int main()
{
	yylex();
	return 0;
}
*/
