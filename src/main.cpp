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
#define note(format, args...) fprintf(stdout, "\033[36m" format "\033[0m", ##args)

struct ForeignKey
{
	ForeignKey(string a,string b,string c):column(a),foreign_table(b),foreign_column(c){}
	string column;
	string foreign_table;
	string foreign_column;
};
struct Table
{
	Table(string name):table_name(name),primary_key(-1){}
	Table(){}
	string table_name;
	vector<string> columns_name;
	vector< vector<ValueNode*> > column_values;
	vector<int> columns_type;
	int primary_key;
	vector<int> not_null;
	vector< ForeignKey > foreign_key;
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
	note("welcome!\n");
	yyparse();
	write_table();
	note("bye!\n");
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
		int c;
		if(fscanf(fp," Not Null %d:",&c))
		{
			for(int i=0;i<c;++i)
			{
				int t;
				fscanf(fp,"%d",&t);
				table.not_null.push_back(t);
			}
		}
		if(fscanf(fp," Foreign Key %d",&c))
		{
			char t[100];
			char col[100];
			for(int i=0;i<c;++i)
			{
				fscanf(fp,"\nForeign Key for %s :table %s column %s",value,t,col);
				table.foreign_key.push_back(ForeignKey(value,t,col));
			}
		}
		for(int i=0;i<columns_num;++i)
		{
			fscanf(fp,"%s",value);
			if(strcmp(value,"integer")==0)
				table.columns_type.push_back(1);
			else
				table.columns_type.push_back(2);
		}
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
				else if(strcmp(value,"NULL")==0)
				{
					table.column_values[i].push_back(0);
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
		if(!table.not_null.empty())
		{
			fprintf(fp," NotNull %d: ",(int)table.not_null.size());
			for(size_t i=0;i<table.not_null.size();++i)
			{
				fprintf(fp,"%d ",table.not_null[i]);
			}
		}
		if(!table.foreign_key.empty())
		{
			fprintf(fp," Foreign Key %d\n",(int)table.foreign_key.size());
			for(size_t i=0;i<table.foreign_key.size();++i)
			{
				fprintf(fp,"Foreign Key for %s :table %s column %s\n",table.foreign_key[i].column.c_str(),table.foreign_key[i].foreign_table.c_str(),table.foreign_key[i].foreign_column.c_str());
			}
		}
		else
			fprintf(fp,"\n");
		for(int i=0;i<columns_num;++i)
		{
			if(table.columns_type[i]==1)
				fprintf(fp,"integer ");
			else
				fprintf(fp,"char ");
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
				if(table.column_values[k][i]==0)
					fprintf(fp,"NULL ");
				else
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
	note("查询结果：\n");

	//处理星号
	if(columns_name[0]=="*")
		columns_name=table.columns_name;

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
			if(table.column_values[j][i]==0)
				cout<<"NULL"<<"\t";
			else
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

	note("查询结果：\n");

	//处理星号
	if(columns_name[0]=="*")
		columns_name=table.columns_name;

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
						if(table.column_values[j][i]==0)
							cout<<"NULL\t";
						else
						{
							NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[j][i]);
							StringNode *sn=dynamic_cast<StringNode*>(table.column_values[j][i]);
							if(nn)
								cout<<nn->number<<"\t";
							else
								cout<<sn->str<<"\t";
						}
						break;
					}
				}
			}
			cout<<endl;
		}
	}
}

bool integrity_check(Table table)
{
	//检查类型
	for(size_t i=0;i<table.columns_name.size();++i)
	{
		for(size_t j=0;j<table.column_values[i].size();++j)
		{
			NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[i][j]);
			StringNode *sn=dynamic_cast<StringNode*>(table.column_values[i][j]);
			if((nn && table.columns_type[i]!=1)||(sn && table.columns_type[i]!=2))
			{
				note("不符合类型约束\n");
				return false;
			}
		}
	}
	//检查primary key
	if(table.primary_key==-1)
		return true;
	int primary_key=table.primary_key;
	//int end=table.column_values[primary_key].size()-1;
	for(size_t i=0;i<table.column_values[0].size();++i)
	{
		for(size_t j=0;j<table.column_values[0].size();++j)
		{
			if(i!=j)
			{
				NumberNode *nn=dynamic_cast<NumberNode*>(table.column_values[primary_key][i]);
				StringNode *sn=dynamic_cast<StringNode*>(table.column_values[primary_key][i]);
				NumberNode *nn2=dynamic_cast<NumberNode*>(table.column_values[primary_key][j]);
				StringNode *sn2=dynamic_cast<StringNode*>(table.column_values[primary_key][j]);
				if(nn)
				{
					if(nn->number==nn2->number)
					{
						note("不符合Primary Key完整性要求\n");
						return false;
					}
				}
				else
				{
					if(sn->str==sn2->str)
					{
						note("不符合Primary Key完整性要求\n");
						return false;
					}
				}
			}
			//if(table.column_values[primary_key][i]==table.column_values[primary_key][j])
				//return false;
		}
	}
	//检查not null
	for(size_t i=0;i<table.not_null.size();++i)
	{
		int index=table.not_null[i];
		for(size_t j=0;j<table.column_values[index].size();++j)
		{
			if(table.column_values[index][j]==0)
			{
				note("不符合Not Null完整性要求\n");
				return false;
			}
		}
	}
	return true;
}

void handle_insert(string table_name,ValueListNode value_list)
{
	size_t index;
	for(index=0;index<db.tables_list.size();++index)
	{
		if(db.tables_list[index].table_name==table_name)
			break;
	}
	Table table=db.tables_list[index];
	for(size_t i=0;i<value_list.values.size();++i)
	{
		ValueNode *n=value_list.values[i];
		table.column_values[i].push_back(n);
	}
	if(integrity_check(table))
	{
		db.tables_list[index]=table;
		note("元组已插入\n");
	}
}

void handle_create(string table_name,DefineListNode define_list)
{
	Table table(table_name);
	for(size_t i=0;i<define_list.defines.size();++i)
	{
		table.columns_name.push_back(define_list.defines[i].column_name);
		table.column_values.push_back(vector<ValueNode*>());
		//primary key
		if(define_list.defines[i].primary_key)
			table.primary_key=i;
		//not null
		if(define_list.defines[i].not_null)
			table.not_null.push_back(i);
		//type
		table.columns_type.push_back(define_list.defines[i].type);
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
	if(integrity_check(table))
	{
		db.tables_list[i]=table;
		note("元组已更新\n");
	}
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
	if(integrity_check(table))
		db.tables_list[i]=table;
}
