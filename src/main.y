%{
    #include "common.h"
    #define YYSTYPE TreeNode *  
    TreeNode* root;
    extern int lineno;
    int yylex();
    int yyerror( char const * );
%}
%token T_CHAR T_INT T_STRING T_BOOL 

%token LOP_ASSIGN LOP_ASSIGN_OR LOP_ASSIGN_AND LOP_ASSIGN_NOT LOP_ASSIGN_ADD LOP_ASSIGN_SUB LOP_ASSIGN_MULT LOP_ASSIGN_DIV LOP_ASSIGN_MOD

%token LOP_ADD LOP_SUB LOP_MULT LOP_DIV LOP_MOD 

%token LOP_DOUBLE_OR LOP_DOUBLE_AND LOP_EQ LOP_NOT_EQ LOP_LESS LOP_GREATER LOP_LESS_EQ LOP_GREATER_EQ

%token SEMICOLON COMMA

%token IDENTIFIER INTEGER CHAR BOOL STRING

%token L_Braces R_Braces L_Small_Braces R_Small_Braces 

%token IF ELSE WHILE FOR

%token CONTINUE BREAK RETURN

%token DOUBLE_ADD DOUBLE_SUB

%token MAIN

%%

program
: statements {
    root = new TreeNode(0, NODE_PROG); 
    root->addChild($1);
    printf("program\n");
}
| T MAIN L_Small_Braces R_Small_Braces compound_Stmt{
    root = new TreeNode(0, NODE_PROG); 
    root->addChild($1);
    root->addChild($5);
    printf("main\n");
}
;

statements
:  statement {
    $$=$1;
    printf("statement\n");
}
|  statements statement {
    $$=$1; 
    $$->addSibling($2);
    printf("statements statement\n");
}
;

statement
: SEMICOLON  {
    $$ = new TreeNode(lineno, NODE_STMT); 
    $$->stype = STMT_SKIP;
    printf("SEMICOLON\n");
}
| declaration SEMICOLON {
    $$ = $1;
    printf("declaration SEMICOLON\n");
}
| assignment_Stmt SEMICOLON{
    $$ = $1;
}
| compound_Stmt{
    TreeNode* node = new TreeNode($1->lineno, NODE_BLOCK_FLAG); 
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
;

jump_Stmt
: CONTINUE SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_CONTINUE;
    $$=node;
}
| BREAK SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_BREAK;
    $$=node;
}
| RETURN Exp SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_RETURN_EXP;
    $$=node;
}
| RETURN SEMICOLON{
    TreeNode* node = new TreeNode(lineno, NODE_JUMP_STMT);
    node->jumptype=JUMP_RETURN;
    $$=node;
}
;
compound_Stmt
: L_Braces statements R_Braces{
    $$=$2;
}
;


selection_Stmt
: IF L_Small_Braces Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NODE_SELECTION_STMT);
    node->stype = STMT_SELECTION;//以后可能改为STMT_SELECTION_IF,因为可能加入switch
    node->addChild($3);
    node->addChild($5);
    $$ = node;
    printf("if ( conditional_Exp ) statement\n");

}
| IF L_Small_Braces Exp R_Small_Braces statement ELSE statement{
    TreeNode* node = new TreeNode(lineno, NODE_SELECTION_STMT);
    node->stype = STMT_SELECTION;
    node->addChild($3);
    node->addChild($5);
    node->addChild($7);
    $$ = node;
    printf("if ( conditional_Exp ) statement else statement\n");
}
;

iteration_Stmt
: WHILE L_Small_Braces Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_WHILE;
    node->addChild($3);
    node->addChild($5);
    $$ = node;
    printf("WHILE L_Small_Braces conditional_Exp R_Small_Braces statement\n");
}
| FOR L_Small_Braces Exp SEMICOLON Exp SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_EEE;
    node->addChild($3);
    node->addChild($5);
    node->addChild($7);
    node->addChild($9);
    $$ = node;
    printf("FOR L_Small_Braces Exp SEMICOLON Exp SEMICOLON Exp R_Small_Braces statement\n");
}
| FOR L_Small_Braces Exp SEMICOLON Exp SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_EE_;
    node->addChild($3);
    node->addChild($5);
    node->addChild($8);
    $$ = node;
}
| FOR L_Small_Braces Exp SEMICOLON  SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_E_E;
    node->addChild($3);
    node->addChild($6);
    node->addChild($8);
    $$ = node;
}
| FOR L_Small_Braces Exp SEMICOLON  SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR_E__;
    node->addChild($3);
    node->addChild($7);
    $$ = node;
}
| FOR L_Small_Braces  SEMICOLON Exp SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR__EE;
    node->addChild($4);
    node->addChild($6);
    node->addChild($8);
    $$ = node;
}
| FOR L_Small_Braces  SEMICOLON Exp SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR__E_;
    node->addChild($4);
    node->addChild($7);
    $$ = node;
}
| FOR L_Small_Braces  SEMICOLON  SEMICOLON Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR___E;
    node->addChild($5);
    node->addChild($7);
    $$ = node;
} FOR L_Small_Braces  SEMICOLON  SEMICOLON  R_Small_Braces statement{
    TreeNode* node = new TreeNode(lineno, NDOE_ITERATION_STMT);
    node->iterationtype = ITERATION_FOR____;
    node->addChild($6);
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
    $$ = node;
    printf("logical_or_exp LOP_DOUBLE_OR logical_and_exp\n");
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
    $$ = node;
    printf("logical_and_exp LOP_DOUBLE_AND equality_exp\n");
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
    $$ = node;
    printf("equality_exp LOP_EQ relational_exp\n");
}
| equality_Exp LOP_NOT_EQ relational_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR );
    //node->stype = STMT_DECL;
    node->exprtype=NODE_NOT_EQUALITY_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("equality_exp LOP_NOT_EQ relational_exp\n");
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
    $$ = node;
    printf("relational_exp OP_LESS additive_Exp\n");
}
| relational_Exp LOP_GREATER additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    node->exprtype=NODE_RELATION_GREATER_EXP;
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_GREATER additive_Exp\n");
}
| relational_Exp LOP_LESS_EQ additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    //node->stype = STMT_DECL;
    node->exprtype=NODE_RELATION_LESS_EQ_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_GREATER additive_Exp\n");
}
| relational_Exp LOP_GREATER_EQ additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    //node->stype = STMT_DECL;
    node->exprtype=NODE_RELATION_GREATER_EQ_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_GREATER_EQ additive_Exp\n");
}
;

assignment_Stmt
: assignment_Exp{
    $$=$1;
}
| assignment_Stmt COMMA assignment_Exp
{
    TreeNode* node = new TreeNode(lineno, NODE_ASSIGN_STMT);
    node->stype = STMT_ASSIGN;
    node->addChild($1);
    node->addChild($3);
    $$ = node;
}
;

assignment_Exp
: unary_Exp assignment_Operator additive_Exp{
    TreeNode* node = new TreeNode(lineno, NODE_EXPR);
    //node->stype = STMT_ASSIGN;
    node->exprtype=NODE_ASSIGN_EXP;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("unaryExp assignment_Operator additive_Exp\n");
}
| additive_Exp{//会产生两项归约/归约冲突
    $$=$1;
}
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
    $$ = node;
}
| additive_Exp LOP_SUB mult_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_additive_Exp;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
;


mult_Exp
: unary_Exp{
    $$=$1;
}
| mult_Exp LOP_MULT unary_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_additive_Exp;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
| mult_Exp LOP_DIV unary_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_additive_Exp;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
| mult_Exp LOP_MOD unary_Exp{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    //有可能加stype?
    node->exprtype=NODE_additive_Exp;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
;

unary_Exp
: postfix_Exp{
    $$=$1;//增加这一层是为了方便以后扩充
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
    $$=node;
}
| postfix_Exp DOUBLE_SUB{
    TreeNode* node = new TreeNode(lineno,NODE_EXPR);
    node->exprtype=NODE_POSTFIX_EXP;
    node->addChild($1);
    node->addChild($2);
    $$=node;
}
;



primary_Exp
: Id{
    $$ = $1;
    printf("unaryExp\n");
}
| paperConst{
    $$=$1;
}
;

assignment_Operator
:
LOP_ASSIGN{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_ASSIGN_EQ;
    printf("assignment_Operator");
}
| LOP_ASSIGN_OR{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_OR_EQ;
    printf("assignment_Operator");
}
| LOP_ASSIGN_AND{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_AND_EQ;
    printf("assignment_Operator");
}
| LOP_ASSIGN_NOT{
    $$ = new TreeNode(lineno, NODE_OPERATOR); 
    $$->optype=OP_ASSIGN_NOT_EQ;
    printf("assignment_Operator");
}
| LOP_ASSIGN_ADD{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$ -> optype=OP_ADD_EQ;
}
| LOP_ASSIGN_SUB{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_SUB_EQ;
}
| LOP_ASSIGN_MULT{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_MULT_EQ;
}
| LOP_ASSIGN_DIV{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_DIV_EQ;
}
| LOP_ASSIGN_MOD{
    $$ = new TreeNode(lineno, NODE_OPERATOR);
    $$->optype=OP_MOD_EQ;
}
;

declaration
: T IDENTIFIER assignment_Operator additive_Exp{  // declare and init
    TreeNode* node = new TreeNode(lineno, NODE_DECL_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    $$ = node;
    printf("T IDENTIFIER LOP_ASSIGN expr\n");   
}
| T IDENTIFIER {
    TreeNode* node = new TreeNode(lineno, NODE_DECL_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    $$ = node; 
    printf("T IDENTIFIER\n");  
}
;

Id
: IDENTIFIER {
    $$ = $1;
    printf("IDENTIFIER\n");
}
;

paperConst
: INTEGER {
    $$ = $1;
    printf("INTEGER\n");
}
| CHAR {
    $$ = $1;
    printf("CHAR\n");
}
| STRING {
    $$ = $1;
    printf("STRING\n");
}
| BOOL {
    $$=$1;
    printf("BOOL\n");
}
;

T: T_INT {
    $$ = new TreeNode(lineno, NODE_TYPE); 
    $$->type = TYPE_INT;
    printf("T_INT\n");
} 
| T_CHAR {
    $$ = new TreeNode(lineno, NODE_TYPE); 
    $$->type = TYPE_CHAR;
    printf("T_CHAR\n");
}
| T_BOOL {
    $$ = new TreeNode(lineno, NODE_TYPE); 
    $$->type = TYPE_BOOL;
    printf("T_BOOL\n");
}
| T_STRING{
    $$ = new TreeNode(lineno,NODE_TYPE);
    $$->type = TYPE_STRING;
    printf("T_STRING\n");
}
;

%%

int yyerror(char const* message)
{
  cout << message << " at line " << lineno << endl;
  return -1;
}