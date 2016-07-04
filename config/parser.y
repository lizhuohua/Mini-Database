%{
#include <stdio.h>
#include <iostream>
#include <string>
#include "node.h"
//#define DEBUG
#ifdef DEBUG
#define debug(format, args...) fprintf(stdout, "\033[36m" format "\033[0m", ##args)
#else
#define debug(format, args...)
#endif
int yylex();
int yyerror(char*);
void handle_select(std::string table_name, std::vector<std::string> columns_name);
void handle_select(std::string table_name, std::vector<std::string> columns_name, std::string column, std::string op, ValueNode *node);
void handle_insert(std::string table_name,ValueListNode value_list);
void handle_create(std::string table_name,DefineListNode define_list);
void handle_update(std::string table_name,std::string column_name,ValueNode *newVal,std::string column, ValueNode *val);
void handle_delete(std::string table_name,std::string column_name,ValueNode *value);
%}

%union
{
	int integer;
	std::string *str;
	std::vector<std::string>* str_list;
	Node *node;
}

%token <integer>NUMBER <str>IDENTIFIER <str> STRING
%type <str_list>column_list
%type <node> value_list;
%type <node> value;
%type <node> define_list;
%type <node> define;
%type <str> op
%type <integer> type;
%token INSERT INTO VALUES WHERE SELECT FROM UPDATE SET DELETE CREATE TABLE INTEGER CHAR PRIMARY KEY NOT NIL FOREIGN REFERENCES
%token <str> EQ_OP NE_OP LE_OP GE_OP LT_OP GT_OP
%left EQ_OP NE_OP LE_OP GE_OP LT_OP GT_OP

%%
sql
	:{debug("welcome!\n");}
	|sql input_line
	;
input_line
	:INSERT INTO IDENTIFIER VALUES '(' value_list ')' ';'
	{
		debug("元组已插入\n");
		handle_insert(*$3,*(ValueListNode*)$6);
	}
	|CREATE TABLE IDENTIFIER '(' define_list ')' ';'
	{
		debug("已创建表%s\n",$3->c_str());
		handle_create(*$3,*(DefineListNode*)$5);
	}
	|SELECT column_list FROM IDENTIFIER ';'
	{
		debug("查询结果：\n");
		handle_select(*$4,*$2);
	}
	|SELECT column_list FROM IDENTIFIER WHERE IDENTIFIER op value ';'
	{
		debug("查询结果：\n");
		handle_select(*$4,*$2,*$6,*$7,(ValueNode*)$8);
	}
	|UPDATE IDENTIFIER SET IDENTIFIER EQ_OP value WHERE IDENTIFIER EQ_OP value ';'
	{
		debug("表%s已更新\n",$2->c_str());
		handle_update(*$2,*$4,(ValueNode*)$6,*$8,(ValueNode*)$10);
	}
	|DELETE FROM IDENTIFIER WHERE IDENTIFIER EQ_OP value ';'
	{
		debug("元组已删除\n");
		handle_delete(*$3,*$5,(ValueNode*)$7);
	}
	;
define_list
	:define_list ',' define
	{
		((DefineListNode*)$1)->append(*(DefineNode*)$3);
		$$=$1;
	}
	|define
	{
		$$=new DefineListNode();
		((DefineListNode*)$$)->append(*(DefineNode*)$1);
	}
	;
define
	:IDENTIFIER type
	{
		$$=new DefineNode(*$1,$2);
	}
	|IDENTIFIER type PRIMARY KEY
	{
		$$=new DefineNode(*$1,$2);
		((DefineNode*)$$)->primary_key=true;
	}
	|IDENTIFIER type FOREIGN KEY REFERENCES IDENTIFIER '(' IDENTIFIER ')'
	{
		$$=new DefineNode(*$1,$2);
		((DefineNode*)$$)->foreign_key=true;
		((DefineNode*)$$)->foreign_table=*$6;
		((DefineNode*)$$)->foreign_column=*$8;
	}
	|IDENTIFIER type NOT NIL
	{
		$$=new DefineNode(*$1,$2);
		((DefineNode*)$$)->not_null=true;
	}
	;
type
	:INTEGER
	{
		$$=1;
	}
	|CHAR
	{
		$$=2;
	}
	;
column_list
	:column_list ',' IDENTIFIER
	{
		$1->push_back(*$3);
		$$=$1;
	}
	|IDENTIFIER
	{
		$$=new std::vector<std::string>;
		$$->push_back(*$1);
	}
	;
value_list
	:value_list ',' value
	{
		((ValueListNode*)$1)->append((ValueNode*)$3);
		$$=$1;
	}
	|value
	{
		$$=new ValueListNode();
		((ValueListNode*)$$)->append((ValueNode*)$1);
	}
	;
value
	:NUMBER
	{
		$$=new NumberNode($1);
	}
	|STRING
	{
		$$=new StringNode(*$1);
	}
	|NIL
	{
		$$=NULL;
	}
	;
op
	:EQ_OP
	|NE_OP
	|LE_OP
	|GE_OP
	|LT_OP
	|GT_OP
	;
%%

int yyerror(char *s)
{
}
