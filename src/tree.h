#ifndef TREE_H
#define TREE_H

#include "pch.h"
#include "type.h"

enum NodeType
{
    NODE_CONST, 
    NODE_VAR,
    NODE_EXPR,
    NODE_TYPE, //声明节点如int,char

    NODE_ASSIGN_STMT,//赋值语句
    NODE_STMT,
    NODE_DECL_STMT,//声明语句
    NODE_SELECTION_STMT,//条件语句if
    NODE_PROG,
    NODE_IDENTIFIER,//标识符
    NODE_OPERATOR,//运算符
    NODE_additive_Exp,//算数表达式
    NODE_BLOCK_FLAG, //语句块即{.....}的标志
    NODE_LOGICAL_OR_EXP,//或运算表达式
    NODE_LOGICAL_AND_EXP,//与运算表达式
    NODE_EQUALITY_EXP,//等于号表达式
    NODE_NOT_EQUALITY_EXP,//不等号表达式
    NODE_RELATION_LESS_EXP,//小于号表达式
    NODE_RELATION_GREATER_EXP,//大于号表达式
    NODE_RELATION_LESS_EQ_EXP,//小于等于表达式
    NODE_RELATION_GREATER_EQ_EXP,//大于等于表达式

};

enum OperatorType
{
    OP_EQ,  // ==
    OP_OR_EQ, //|=
    OP_ASSIGN_EQ, //=
    OP_AND_EQ, //&=
    OP_ASSIGN_NOT_EQ, //^=
    OP_ADD, //+
    OP_SUB, //-
    OP_MULT, //*
    OP_DIV, // /
    OP_MOD, //%
    OP_DOUBLE_OR,// ||
    OP_DOUBLE_AND,// &&
    OP_NOT_EQ,// !=
    OP_LESS,// <
    OP_GREATER,// >
    OP_LESS_EQ,// <=
    OP_GREATER_EQ, // >=
};

enum StmtType {
    STMT_NULL,
    STMT_SKIP,
    STMT_DECL,
    STMT_ASSIGN,//赋值
    STMT_SELECTION,//条件语句

}
;

struct TreeNode {
public:
    int nodeID;  // 用于作业的序号输出
    int lineno;
    NodeType nodeType;
    TreeNode* child = nullptr;
    TreeNode* sibling = nullptr;

    void addChild(TreeNode*);
    void addSibling(TreeNode*);
    
    void printNodeInfo();
    void printChildrenId();

    void printAST(); // 先输出自己 + 孩子们的id；再依次让每个孩子输出AST。
    void printSpecialInfo();

    void genNodeId();



public:
    OperatorType optype;  // 如果是表达式
    Type* type;  // 变量、类型、表达式结点，有类型。
    StmtType stype;
    int int_val;
    char ch_val;
    bool b_val;
    string str_val;
    string var_name;
public:
    static string nodeType2String (NodeType type);
    static string opType2String (OperatorType type);
    static string sType2String (StmtType type);

public:
    TreeNode(int lineno, NodeType type);
};

#endif