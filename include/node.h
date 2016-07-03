#ifndef __NODE_H__
#define __NODE_H__

#include <string>
#include <vector>

class Node
{
public:
	virtual ~Node(){}
};

class ValueNode:public Node
{
public:
	int type;
};

class NumberNode:public ValueNode
{
public:
	NumberNode(int n):number(n){type=1;}
	int number;
};

class StringNode:public ValueNode
{
public:
	StringNode(std::string s):str(s){type=2;}
	std::string str;
};

class ValueListNode:public Node
{
public:
	void append(ValueNode *n){values.push_back(n);}
	std::vector<ValueNode*> values;
};

class DefineNode:public Node
{
public:
	DefineNode(std::string name,int t):column_name(name),type(t),primary_key(false){}
	std::string column_name;
	int type;
	bool primary_key;
};

class DefineListNode:public Node
{
public:
	void append(DefineNode n){defines.push_back(n);}
	std::vector<DefineNode> defines;
};

#endif
