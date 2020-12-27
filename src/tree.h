#ifndef TREE_H
#define TREE_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "pch.h"
#include "type.h"
#include "SymbolTable.h"
//#include "layerNode.h"
#define layerNum 50
#define layerDescNum 100
struct layerNode;
struct changeField;
struct TreeNode;
struct funcInfo;
struct ThreeAdCodeItem;
struct layerNode
{
    layerNode* prev;
    layerNode* list[layerNum];
    layerNode* root;//指向根节点
    int layerDesc[layerDescNum];
    int nodeCount;
    int accessTime;
    int is_func;
    SymbolTableSection * section;
    int total_count;//对于只考虑全局作用域而言，这个值只在根节点有用，记录了栈顶
    int last_size;//在内存中分配的上一个变量的大小
};
void check_section(layerNode* node);//检查所有符号表的重定义以及定义前引用
struct changeField
{
    int needChange;
    int accessTime;
    int is_func_field;
};

struct funcInfo{
    TreeNode* return_value;
    TreeNode* func_name;
    TreeNode* decl_list;
    TreeNode* func_body;
    TreeNode* arg_list;
    TreeNode* func_def_loc;//如果是函数定义，则指向自身，如果是函数调用，则指向函数定义的节点
};
TreeNode* findFuncDef(string func_name,list<TreeNode*> func_list);//通过函数名找到函数定义的相关节点并返回
struct Label{
    string* begin_label;
    string* next_label;
    string* false_label;
    string* true_label;
   
};
enum NodeType
{
    NODE_CONST, 
    NODE_VAR,
    NODE_EXPR,
    NODE_TYPE, //声明节点如int,char

    NODE_ASSIGN_STMT,//赋值语句 done
    NODE_ASSIGN_EXPR,//赋值表达式 done
    NODE_MAIN,//main函数  done
    NODE_STMT,
    NODE_DECL_STMT,//声明语句 done
    NODE_SELECTION_STMT,//条件语句if done
    NDOE_ITERATION_STMT, //循环语句
    NODE_PROG,
    NODE_IDENTIFIER,//标识符
    NODE_OPERATOR,//运算符
    NODE_FUNCTION_DEF,//函数定义
    NODE_BLOCK_FLAG, //语句块即{.....}的标志
    NODE_JUMP_STMT,//控制语句
    NODE_EMPTY,//空
    NODE_DECL_STMT_LIST,//定义列表 done
    NODE_ID_LIST,//函数参数列表
    NODE_FUNCTION_CALL,//函数调用

};


enum ExprType
{
    NODE_additive_Exp,//算数表达式
    NODE_ASSIGN_EXP,//赋值表达式
    NODE_ASSIGN_EXP_WITH_COMMA,
    NODE_LOGICAL_OR_EXP,//或运算表达式
    NODE_LOGICAL_AND_EXP,//与运算表达式
    NODE_EQUALITY_EXP,//等于号表达式
    NODE_NOT_EQUALITY_EXP,//不等号表达式
    NODE_RELATION_LESS_EXP,//小于号表达式
    NODE_RELATION_GREATER_EXP,//大于号表达式
    NODE_RELATION_LESS_EQ_EXP,//小于等于表达式
    NODE_RELATION_GREATER_EQ_EXP,//大于等于表达式
    NODE_POSTFIX_EXP, //后缀表达式 如 a++
    NODE_UNARY_EXP,//前缀如-1
    NODE_MULT_EXP,
};


enum OperatorType
{
    OP_EQ,  // ==
    OP_OR_EQ, //|=
    OP_ASSIGN_EQ, //=
    OP_AND_EQ, //&=
    OP_ASSIGN_NOT_EQ, //^=
    OP_ADD_EQ, //+=
    OP_SUB_EQ, //-=
    OP_MULT_EQ, //*=
    OP_DIV_EQ,  // /=
    OP_MOD_EQ,  // %=
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
    OP_DOUBLE_ADD, //++
    OP_DOUBLE_SUB, //--
    OP_UNARY_REFERENCE, //&
    OP_UNARY_NOT,//!
};

enum StmtType {
    STMT_NULL,
    STMT_SKIP,
    STMT_DECL,
    STMT_ASSIGN,//赋值
    STMT_SELECTION,//条件语句
    
}
;

enum IterationType{
    ITERATION_WHILE,//while循环 done
    ITERATION_FOR_EEE,//for循环里面三个表达式全都有 done
    ITERATION_FOR_EE_,//最后一个为空 done
    ITERATION_FOR_E_E,//中间为空 done
    ITERATION_FOR_E__,//后两个为空 done
    ITERATION_FOR__EE,//第一个为空 done
    ITERATION_FOR__E_,//左右两个为空 done
    ITERATION_FOR___E,//前两个为空 done
    ITERATION_FOR____,//全为空 done
};

enum JumpStmtType{
    JUMP_CONTINUE,//
    JUMP_BREAK,
    JUMP_RETURN_EXP,
    JUMP_RETURN,
};

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
    void gen_label(TreeNode*);
    void print_label(TreeNode*root,list<string*>*str_list);
    void allocate_stack_space(TreeNode*);//分配栈空间
    string gen_ASM_code(TreeNode*);//产生汇编代码
    int has_ID();

public:
    OperatorType optype;  // 如果是表达式
    Type* type;  // 变量、类型、表达式结点，有类型。
    StmtType stype;
    ExprType exprtype;// 如果是表达式，其具体类型
    IterationType iterationtype; //如果是循环，其具体类型
    JumpStmtType jumptype;
    int int_val;
    char ch_val;
    bool b_val;
    Label label;
    string str_val;
    string var_name;
    string* code;//代码
    layerNode* layer_node;
    changeField change_field;
    funcInfo* func_info;
    int is_def;//是否是定义的变量
    Item* item;//如果这个节点在内存中有位置，则这个指针指向它所分配的符号
    ThreeAdCodeItem* ThreeAD;
    void print_code();
    
public:
    static string nodeType2String (NodeType type);
    static string opType2String (OperatorType type);
    static string sType2String (StmtType type);
    static string exprType2String(ExprType type);
    static string IterationType2String(IterationType type); 
    static string JumpStmtType2String(JumpStmtType type);
    void change_Field(TreeNode* node);
    void change_Child_Field(TreeNode* node);
    int check_type();//类型检查
    TreeNode* get_child(int child_num);//返回对应编号孩子的type
    int child_num();//返回孩子个数
public:
    TreeNode(int lineno, NodeType type);
};
int find_str(list<string*>*str_list,string*str);
string opType2_String (OperatorType type);
//三地址码
struct ThreeAdCodeItem{
    OperatorType op;
    TreeNode* arg1;
    TreeNode* arg2;
    TreeNode* result;
};

string gen_expr_asm(TreeNode* node);
string get_location_or_value(TreeNode* node);
string get_code(TreeNode*);
void get_all_arg(TreeNode* id_list,list<TreeNode*>*arg_list);
#endif