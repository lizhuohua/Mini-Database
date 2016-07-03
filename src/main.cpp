#include "../include/node.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <string>
using namespace std;
extern int yyparse();
void read_table();
void write_table();
struct Table
{
	Table(string name):table_name(name),primary_key(-1){}
	Table(){}
	string table_name;
	vector<string> columns_name;
	vector< vector<ValueNode*> > column_values;
	int primary_key;
};
struct Database
{
public:
	void add_table(Table table)
	{
		tables_list.push_back(table);
	}
	vector<Table> tables_list;
};

Database db;
int main()
{
	read_table();
	yyparse();
	write_table();
	return 0;
}

void read_table()
{
	FILE *fp=fopen("Database","r");
	char table_name[100];
	int columns_num;
	int primary_key;
	char value[100];
	int t;
	while((t=fscanf(fp,"BeginTable %s %d columns",table_name,&columns_num))&&t!=EOF)
	{
		Table table(table_name);
		if(fscanf(fp," Primary Key %d",&primary_key))
			table.primary_key=primary_key;
		for(int i=0;i<columns_num;++i)
		{
			fscanf(fp,"%s",value);
			table.columns_name.push_back(value);
			table.column_values.push_back(vector<ValueNode*>());
		}
		bool finish=0;
		while(1)
		{
			if(finish)
				break;
			for(int i=0;i<columns_num;++i)
			{
				fscanf(fp,"%s",value);
				if(strcmp(value,"EndTable")==0)
				{
					finish=true;
					break;
				}
				if(value[0]=='\'')
				{
					table.column_values[i].push_back(new StringNode(value));
				}
				else
				{
					table.column_values[i].push_back(new NumberNode(atoi(value)));
				}
			}
		}
		db.add_table(table);
		//换行符需要读一下
		fgetc(fp);
	}
	fclose(fp);
}

void write_table()
{
	FILE *fp=fopen("Database","w");
	for(size_t j=0;j<db.tables_list.size();++j)
	{
		Table table=db.tables_list[j];
		int columns_num=table.column_values.size();
		fprintf(fp,"BeginTable %s %d columns",table.table_name.c_str(),columns_num);
		if(table.primary_key!=-1)
		{
			fprintf(fp," Primary Key %d",table.primary_key);
		}
		fprintf(fp,"\n");
		for(int i=0;i<columns_num;++i)
		{
			fprintf(fp,"%s ",table.columns_name[i].c_str());
		}
		fprintf(fp,"\n");
		for(size_t i=0;i<table.column_values[0].size();++i)
		{
			for(int k=0;k<columns_num;++k)
			{
				NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[k][i]);
				StringNode *sn=dynamic_cast<StringNode*>(table.column_values[k][i]);
				if(nn)
				{
					fprintf(fp,"%d ",nn->number);
				}
				else
					fprintf(fp,"%s ",sn->str.c_str());
			}
			fprintf(fp,"\n");
		}
		fprintf(fp,"EndTable\n");
	}
	fclose(fp);
}

void handle_select(string table_name, vector<string> columns_name)
{
	Table table;
	for(size_t i=0;i<db.tables_list.size();++i)
	{
		if(db.tables_list[i].table_name==table_name)
		{
			table=db.tables_list[i];
			break;
		}
	}
	int size=columns_name.size();
	for(int i=0;i<size;++i)
	{
		cout<<columns_name[i]<<"\t";
	}
	cout<<endl;
	size=table.column_values[0].size();
	for(int i=0;i<size;++i)
	{
		for(size_t j=0;j<table.columns_name.size();++j)
		{
			NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[j][i]);
			StringNode *sn=dynamic_cast<StringNode*>(table.column_values[j][i]);
			for(size_t k=0;k<columns_name.size();++k)
			{
				if(table.columns_name[j]==columns_name[k])
				{
					if(nn)
						cout<<nn->number<<"\t";
					else
						cout<<sn->str<<"\t";
					break;
				}
			}
		}
		cout<<endl;
	}
}
template<class T>
bool check(T a, string op, T b)
{
	if(op=="<")
		return a<b;
	if(op=="<=")
		return a<=b;
	if(op=="=")
		return a==b;
	if(op==">")
		return a>b;
	if(op==">=")
		return a>=b;

	//default
	return true;
}

void handle_select(string table_name, vector<string> columns_name, string column, string op, ValueNode * value)
{
	Table table;
	for(size_t i=0;i<db.tables_list.size();++i)
	{
		if(db.tables_list[i].table_name==table_name)
		{
			table=db.tables_list[i];
			break;
		}
	}

	size_t index;
	for(index=0;index<table.columns_name.size();++index)
		if(table.columns_name[index]==column)
			break;

	int size=columns_name.size();
	for(int i=0;i<size;++i)
	{
		cout<<columns_name[i]<<"\t";
	}
	cout<<endl;
	size=table.column_values[0].size();
	for(int i=0;i<size;++i)
	{
		NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[index][i]);
		StringNode *sn=dynamic_cast<StringNode*>(table.column_values[index][i]);
		bool ok;
		if(nn)
		{
			ok=check(nn->number,op,((NumberNode*)value)->number);
		}
		else
		{
			ok=check(sn->str,op,((StringNode*)value)->str);
		}
		if(ok)
		{
			for(size_t j=0;j<table.columns_name.size();++j)
			{
				for(size_t k=0;k<columns_name.size();++k)
				{
					if(table.columns_name[j]==columns_name[k])
					{
						NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[j][i]);
						StringNode *sn=dynamic_cast<StringNode*>(table.column_values[j][i]);
						if(nn)
							cout<<nn->number<<"\t";
						else
							cout<<sn->str<<"\t";
						break;
					}
				}
			}
			cout<<endl;
		}
	}
}

void handle_insert(string table_name,ValueListNode value_list)
{
	size_t index;
	for(index=0;index<db.tables_list.size();++index)
	{
		if(db.tables_list[index].table_name==table_name)
			break;
	}
	for(size_t i=0;i<value_list.values.size();++i)
	{
		ValueNode *n=value_list.values[i];
		db.tables_list[index].column_values[i].push_back(n);
	}
}

void handle_create(string table_name,DefineListNode define_list)
{
	Table table(table_name);
	for(size_t i=0;i<define_list.defines.size();++i)
	{
		table.columns_name.push_back(define_list.defines[i].column_name);
		table.column_values.push_back(vector<ValueNode*>());
	}
	db.add_table(table);
}

void handle_update(string table_name,string column_name,ValueNode *newVal,string column, ValueNode *val)
{
	size_t i,j,k;
	for(i=0;i<db.tables_list.size();++i)
	{
		if(db.tables_list[i].table_name==table_name)
			break;
	}
	Table table=db.tables_list[i];
	for(j=0;j<table.columns_name.size();++j)
	{
		if(table.columns_name[j]==column_name)
			break;
	}
	for(k=0;k<table.columns_name.size();++k)
	{
		if(table.columns_name[k]==column)
			break;
	}
	for(size_t p=0;p<table.column_values[j].size();++p)
	{
		NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[k][p]);
		StringNode *sn=dynamic_cast<StringNode*>(table.column_values[k][p]);
		int ok=0;
		if(nn)
		{
			if(nn->number==((NumberNode*)val)->number)
				ok=1;
		}
		else
		{
			if(sn->str==((StringNode*)val)->str)
			{
				ok=1;
			}
		}
		if(ok)
		{
			table.column_values[j][p]=newVal;
		}
	}
	db.tables_list[i]=table;
}

void handle_delete(string table_name,string column_name,ValueNode *value)
{
	size_t i,j;
	for(i=0;i<db.tables_list.size();++i)
	{
		if(db.tables_list[i].table_name==table_name)
			break;
	}
	Table table=db.tables_list[i];
	for(j=0;j<table.columns_name.size();++j)
	{
		if(table.columns_name[j]==column_name)
			break;
	}
	for(size_t p=0;p<table.column_values[j].size();++p)
	{
		NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[j][p]);
		StringNode *sn=dynamic_cast<StringNode*>(table.column_values[j][p]);
		int ok=0;
		if(nn)
		{
			if(nn->number==((NumberNode*)value)->number)
				ok=1;
		}
		else
		{
			if(sn->str==((StringNode*)value)->str)
			{
				ok=1;
			}
		}
		if(ok)
		{
			for(size_t k=0;k<table.columns_name.size();++k)
			{
				table.column_values[k].erase(table.column_values[k].begin()+p);
			}
		}
	}
	db.tables_list[i]=table;
}
