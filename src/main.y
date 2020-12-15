%{
    #include "common.h"
    //#include "layerNode.h"
    #define YYSTYPE TreeNode *  
    TreeNode* root;
    layerNode* currentNode;
    layerNode* layer_root;
    list<TreeNode*> func_def_list;//一个存储了所有函数定义的列表，这样在遇到函数调用时能够不用遍历语法树而找到函数定义节点
    extern int lineno;
    int yylex();
    int yyerror( char const * );
%}
%token T_CHAR T_INT T_STRING T_BOOL 

%token LOP_ASSIGN LOP_ASSIGN_OR LOP_ASSIGN_AND LOP_ASSIGN_NOT LOP_ASSIGN_ADD LOP_ASSIGN_SUB LOP_ASSIGN_MULT LOP_ASSIGN_DIV LOP_ASSIGN_MOD

%token LOP_DOUBLE_OR LOP_DOUBLE_AND LOP_EQ LOP_NOT_EQ LOP_LESS LOP_GREATER LOP_LESS_EQ LOP_GREATER_EQ

%token SEMICOLON COMMA

%token IDENTIFIER INTEGER CHAR BOOL STRING

%token L_Braces R_Braces L_Small_Braces R_Small_Braces 

%token IF ELSE WHILE FOR

%token CONTINUE BREAK RETURN

%token DOUBLE_ADD DOUBLE_SUB

%token MAIN

%left LOP_ADD LOP_SUB LOP_MOD

%left LOP_MULT LOP_DIV LOP_REFERENCE LOP_NOT

%%

program
: statements {
    root = new TreeNode(0, NODE_PROG); 
    root->addChild($1);
    root->layer_node=currentNode;
}

;

statements
:  statement {
    $$=$1;

}
|  statements statement {
    $$=$1; 
    $$->addSibling($2);

}
;

statement
: SEMICOLON  {
    $$ = new TreeNode(lineno, NODE_STMT); 
    $$->stype = STMT_SKIP;
    $$->layer_node=currentNode;

}
| Exp SEMICOLON {
    $$ = $1;
}
| compound_Stmt{
    TreeNode* node = new TreeNode($1->lineno, NODE_BLOCK_FLAG); 
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //node->stype
    node->addChild($1);
    $$=node;
}
| selection_Stmt{
    $$=$1;
}
| iteration_Stmt{
    $$=$1;
}
| jump_Stmt {
    $$=$1;
}
| function_Definition{
    $$=$1;
}
| function_Call{
    $$=$1;
}
| T MAIN L_Small_Braces R_Small_Braces compound_Stmt{
    TreeNode* node = new TreeNode(0, NODE_MAIN); 
    node->addChild($1);
    node->addChild($5);
    node->layer_node=currentNode;
    $$=node;
}
;

function_Call
: Id L_Small_Braces Id_List R_Small_Braces{
    TreeNode* node = new TreeNode(lineno, NODE_FUNCTION_CALL); 
    node->layer_node=currentNode;
    node->func_info=new funcInfo;
    node->func_info->func_name=$1;
    node->func_info->arg_list=$3;
    node->addChild($1);
    node->addChild($3);
    setSymbolType(currentNode->section,$1,SYMBOL_FUNC);
    $1->type=TYPE_FUNC;
    node->func_info->func_def_loc=findFuncDef($1->var_name,func_def_list);//将函数调用节点的指向定义的指针指向该函数的定义节点
    if(node->func_info->func_def_loc==nullptr)//定义前引用
    {
        printf("func call before def at line %d\n",$1->lineno);
    }
    $$=node;
}
| Id L_Small_Braces R_Small_Braces {
    TreeNode* node = new TreeNode(lineno, NODE_FUNCTION_CALL); 
    node->layer_node=currentNode;
    node->func_info=new funcInfo;
    node->func_info->func_name=$1;
    node->addChild($3);
    setSymbolType(currentNode->section,$1,SYMBOL_FUNC);
    $1->type=TYPE_FUNC;
    node->func_info->func_def_loc=findFuncDef($1->var_name,func_def_list);//将函数调用节点的指向定义的指针指向该函数的定义节点
    if(node->func_info->func_def_loc==nullptr)//定义前引用
    {
        printf("func call before def at line %d\n",$1->lineno);
    }
    $$=node;
}
;

function_Definition
: T Id L_Small_Braces declaration R_Small_Braces compound_Stmt{
    TreeNode* node = new TreeNode(lineno, NODE_FUNCTION_DEF); 
    node->layer_node=currentNode;
    node->func_info=new funcInfo;
    node->func_info->return_value=$1;
    node->func_info->func_name=$2;
    node->func_info->decl_list=$4;
    node->func_info->func_body=$6;
    node->func_info->func_def_loc=node;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    node->addChild($6);
    setProperty(currentNode->section,$2,PROPERTY_DEF);
    setSymbolType(currentNode->section,$2,SYMBOL_FUNC);
    $2->type=TYPE_FUNC;
    $6->change_field.is_func_field=1;//用来将对应的layernode标识为函数体
    func_def_list.push_back(node);//将这个函数定义节点放入func_def_list
    $$=node;
}
| T Id L_Small_Braces R_Small_Braces compound_Stmt{
    TreeNode* node = new TreeNode(lineno, NODE_FUNCTION_DEF);
    node->layer_node=currentNode;
    node->func_info=new funcInfo;
    node->func_info->return_value=$1;
    node->func_info->func_name=$2;
    node->func_info->func_body=$5;
    node->addChild($1);
    node->addChild($2);
    node->addChild($5);
    setProperty(currentNode->section,$2,PROPERTY_DEF);
    setSymbolType(currentNode->section,$2,SYMBOL_FUNC);
    $2->type=TYPE_FUNC;
    $5->change_field.is_func_field=1;//用来将对应的layernode标识为函数体
    func_def_list.push_back(node);//将这个函数定义节点放入func_def_list
    $$=node;
    
}
;
jump_Stmt
: CONTINUE SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_CONTINUE;
    node->layer_node=currentNode;
    $$=node;
}
| BREAK SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_BREAK;
    node->layer_node=currentNode;
    $$=node;
}
| RETURN Exp SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_RETURN_EXP;
    node->layer_node=currentNode;
    node->addChild($2);
    $$=node;
}
| RETURN SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_RETURN;
    node->layer_node=currentNode;
    $$=node;
}
;
compound_Stmt
: L_Braces statements R_Braces{
    $$=$2;
}
| L_Braces R_Braces{
    $$ = new TreeNode(lineno, NODE_EMPTY);
    $$->layer_node=currentNode; 
    $$->stype = STMT_SKIP;
}
;


selection_Stmt
: IF L_Small_Braces Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NODE_SELECTION_STMT);
    node->stype = STMT_SELECTION;//以后可能改为STMT_SELECTION_IF,因为可能加入switch
    node->addChild($3);
    node->addChild($5);
    node->layer_node=currentNode;
    //类型检查
    node->check_type();
    $$ = node;
}
| IF L_Small_Braces Exp R_Small_Braces statement ELSE statement{
    TreeNode* node = new TreeNode(lineno, NODE_SELECTION_STMT);
    node->stype = STMT_SELECTION;
    node->addChild($3);
    node->addChild($5);
    node->addChild($7);
    node->layer_node=currentNode;
    //类型检查
    node->check_type();
    $$ = node;
}
;

iteration_Stmt
: WHILE L_Small_Braces Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_WHILE;
    node->addChild($3);
    node->addChild($5);
    node->layer_node=currentNode;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces Exp SEMICOLON Exp SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_EEE;
    node->addChild($3);
    node->addChild($5);
    node->addChild($7);
    node->addChild($9);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces Exp SEMICOLON Exp SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_EE_;
    node->addChild($3);
    node->addChild($5);
    node->addChild($8);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces Exp SEMICOLON  SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_E_E;
    node->addChild($3);
    node->addChild($6);
    node->addChild($8);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces Exp SEMICOLON  SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_E__;
    node->addChild($3);
    node->addChild($7);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces  SEMICOLON Exp SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR__EE;
    node->addChild($4);
    node->addChild($6);
    node->addChild($8);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces  SEMICOLON Exp SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR__E_;
    node->addChild($4);
    node->addChild($7);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
}
| FOR L_Small_Braces  SEMICOLON  SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR___E;
    node->addChild($5);
    node->addChild($7);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->check_type();
    $$ = node;
} FOR L_Small_Braces  SEMICOLON  SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR____;
    node->addChild($6);
    node->layer_node=currentNode;
    node->change_field.accessTime=currentNode->accessTime-1;
    node->change_field.needChange=1;
    //类型检查
    node->type=TYPE_VOID;
    $$ = node;
}
;

Exp 
: conditional_Exp{
    $$=$1;
}
| assignment_Stmt{
    $$=$1;
}
| declaration{
    $$=$1;
}
;

conditional_Exp
: logical_or_Exp{
    $$=$1;
}
;

logical_or_Exp
: logical_and_Exp{
    $$=$1;
}
| logical_or_Exp LOP_DOUBLE_OR logical_and_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR );
    //node->stype = STMT_DECL;
    node->exprtype=NODE_LOGICAL_OR_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
;

logical_and_Exp
: equality_Exp{
    $$=$1;
}
| logical_and_Exp LOP_DOUBLE_AND equality_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR );
    //node->stype = STMT_DECL;
    node->exprtype=NODE_LOGICAL_AND_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
;

equality_Exp
: relational_Exp{
    $$=$1;
}
| equality_Exp LOP_EQ relational_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR );
    //node->stype = STMT_DECL;
    node->exprtype=NODE_EQUALITY_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
| equality_Exp LOP_NOT_EQ relational_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR );
    //node->stype = STMT_DECL;
    node->exprtype=NODE_NOT_EQUALITY_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
;

relational_Exp
: additive_Exp{
    $$=$1;
}
| relational_Exp LOP_LESS additive_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR );
    //node->stype = STMT_DECL;
    node->exprtype=NODE_RELATION_LESS_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
| relational_Exp LOP_GREATER additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    node->exprtype=NODE_RELATION_GREATER_EXP;
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
| relational_Exp LOP_LESS_EQ additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    //node->stype = STMT_DECL;
    node->exprtype=NODE_RELATION_LESS_EQ_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
| relational_Exp LOP_GREATER_EQ additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    //node->stype = STMT_DECL;
    node->exprtype=NODE_RELATION_GREATER_EQ_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_BOOL;
    node->check_type();
    $$ = node;
}
;

assignment_Stmt
: assignment_Exp{
    $$=$1;
}
| assignment_Stmt COMMA assignment_Exp
{
    TreeNode* node = new TreeNode(lineno, NODE_ASSIGN_STMT);
    node->exprtype=NODE_ASSIGN_EXP_WITH_COMMA;
    node->addChild($1);
    node->addChild($3);
    node->layer_node=currentNode;
    //此处貌似不需要进行类型检查
    node->type=TYPE_VOID;
    $$ = node;
}
;

assignment_Exp
: unary_Exp assignment_Operator additive_Exp{//有可能需要函数如 a=func()
    TreeNode* node = new TreeNode(lineno, NODE_ASSIGN_EXPR);
    //node->stype = STMT_ASSIGN;
    node->exprtype=NODE_ASSIGN_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查
    node->type=TYPE_VOID;
    node->check_type();
    $$ = node;
}
/*| additive_Exp{//会产生两项归约/归约冲突
    $$=$1;
}*/
;


additive_Exp
: mult_Exp{
    $$=$1;
}
| additive_Exp LOP_ADD mult_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_additive_Exp;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查 node节点默认以$1的type为准
    node->type=$1->type;
    //检查$1 $2是否具有相同类型，目前假设只有整形可以算数运算
    node->check_type();
    $$ = node;
}
| additive_Exp LOP_SUB mult_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_additive_Exp;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查 node节点默认以$1的type为准
    node->type=$1->type;
    //检查$1 $2是否具有相同类型，目前假设只有整形可以算数运算
    node->check_type();
    $$ = node;
}
;


mult_Exp
: cast_Exp{
    $$=$1;
}
| mult_Exp LOP_MULT cast_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_MULT_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查 node节点默认以$1的type为准
    node->type=$1->type;
    //检查$1 $2是否具有相同类型，目前假设只有整形可以算数运算
    node->check_type();
    $$ = node;
}
| mult_Exp LOP_DIV cast_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_MULT_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查 node节点默认以$1的type为准
    node->type=$1->type;
    //检查$1 $2是否具有相同类型，目前假设只有整形可以算数运算
    node->check_type();
    $$ = node;
}
| mult_Exp LOP_MOD cast_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_MULT_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->layer_node=currentNode;
    //类型检查 node节点默认以$1的type为准
    node->type=$1->type;
    //检查$1 $2是否具有相同类型，目前假设只有整形可以算数运算
    node->check_type();
    $$ = node;
}
;

cast_Exp
: unary_Exp{
    $$=$1;
}
| L_Small_Braces T R_Small_Braces cast_Exp{
    //涉及类型转换
}
;


unary_Exp
: postfix_Exp{
    $$=$1;//增加这一层是为了方便以后扩充
}
| unary_Operator cast_Exp{
    TreeNode* node=new TreeNode(lineno,NODE_EXPR);
    node->exprtype=NODE_UNARY_EXP;
    node->addChild($1);
    node->addChild($2);
    node->layer_node=currentNode;
    node->type=$2->type;//类型检查
    //进一步检查unary_Operator是否适用于cast_Exp
    node->check_type();
    $$=node;
}
;

unary_Operator
: LOP_SUB{
    $$=$1;
}
| LOP_REFERENCE{
    $$=$1;
}
| LOP_NOT{
    $$=$1;
}
;


postfix_Exp
: primary_Exp{
    $$=$1;
}
| postfix_Exp DOUBLE_ADD{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    node->exprtype=NODE_POSTFIX_EXP;
    node->addChild($1);
    node->addChild($2);
    node->layer_node=currentNode;
    node->type=$1->type;//类型检查
    //进一步检查此类型是否适合于此操作符
    node->check_type();
    $$=node;
}
| postfix_Exp DOUBLE_SUB{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    node->exprtype=NODE_POSTFIX_EXP;
    node->addChild($1);
    node->addChild($2);
    node->layer_node=currentNode;
    node->type=$1->type;//类型检查
    //进一步检查此类型是否适合于此操作符
    /*code*/
    node->check_type();
    $$=node;
}
;



primary_Exp
: Id{
    $$ = $1;
}
| paperConst{
    $$=$1;
}
| L_Small_Braces Exp R_Small_Braces{
    $$=$2;
}
;

assignment_Operator
:
LOP_ASSIGN{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_ASSIGN_EQ;
    $$->layer_node=currentNode;

}
| LOP_ASSIGN_OR{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_OR_EQ;
    $$->layer_node=currentNode;

}
| LOP_ASSIGN_AND{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_AND_EQ;
    $$->layer_node=currentNode;

}
| LOP_ASSIGN_NOT{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_ASSIGN_NOT_EQ;
    $$->layer_node=currentNode;

}
| LOP_ASSIGN_ADD{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$ -> optype=OP_ADD_EQ;
    $$->layer_node=currentNode;
}
| LOP_ASSIGN_SUB{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_SUB_EQ;
    $$->layer_node=currentNode;
}
| LOP_ASSIGN_MULT{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_MULT_EQ;
    $$->layer_node=currentNode;
}
| LOP_ASSIGN_DIV{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_DIV_EQ;
    $$->layer_node=currentNode;
}
| LOP_ASSIGN_MOD{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_MOD_EQ;
    $$->layer_node=currentNode;
}
;

declaration
: T IDENTIFIER assignment_Operator additive_Exp{  // declare and init
    TreeNode* node = new TreeNode(lineno, NODE_DECL_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    node->layer_node=currentNode;
    setProperty(currentNode->section,$2,PROPERTY_DEF);
    //setSymbolType(currentNode->section,$2,SYMBOL_VAR);
    $2->type=$1->type;//类型检查
    node->type=$1->type;//类型检查
    //进一步检查T与additive_Exp的类型是否相同
    node->check_type();
    $$ = node;
}
| declaration COMMA IDENTIFIER {
    TreeNode* node = new TreeNode(lineno, NODE_DECL_STMT_LIST);
    node->stype = STMT_DECL;
    node->addChild($1);
    //TreeNode* node1 = new TreeNode(lineno, NODE_DECL_STMT);
    node->addChild($3);
    //node->addChild(node1);
    node->layer_node=currentNode;
    setProperty(currentNode->section,$3,PROPERTY_DEF);
    //setSymbolType(currentNode->section,$3,SYMBOL_VAR);
    $3->type=$1->type;//类型检查
    node->type=$1->type;
    $$ = node;
}
| T IDENTIFIER{
    TreeNode* node = new TreeNode(lineno, NODE_DECL_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->layer_node=currentNode;
    setProperty(currentNode->section,$2,PROPERTY_DEF);
    //setSymbolType(currentNode->section,$2,SYMBOL_VAR);
    $2->type=$1->type;//类型检查
    node->type=$1->type;
    $$ = node; 

}
;

Id_List
: Id_List COMMA additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_ID_LIST);
    node->addChild($1);
    node->addChild($3);
    node->layer_node=currentNode;
    $$ = node; 
}
| additive_Exp{
    $$=$1;
}
;

Id
: IDENTIFIER {
    $$ = $1;
    //cout<<$1->var_name<<endl;
}
;

paperConst
: INTEGER {
    $$ = $1;

}
| CHAR {
    $$ = $1;

}
| STRING {
    $$ = $1;

}
| BOOL {
    $$=$1;

}
;

T: T_INT {
    $$ = new TreeNode(lineno, NODE_TYPE); 
    $$->type = TYPE_INT;
    $$->layer_node=currentNode;

} 
| T_CHAR {
    $$ = new TreeNode(lineno, NODE_TYPE); 
    $$->type = TYPE_CHAR;
    $$->layer_node=currentNode;

}
| T_BOOL {
    $$ = new TreeNode(lineno, NODE_TYPE); 
    $$->type = TYPE_BOOL;
    $$->layer_node=currentNode;

}
| T_STRING{
    $$ = new TreeNode(lineno,NODE_TYPE);
    $$->type = TYPE_STRING;
    $$->layer_node=currentNode;

}
;

%%

int yyerror(char const* message)
{
  cout << message << " at line " << lineno << endl;
  return -1;
}