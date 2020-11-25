%{
    #include "common.h"
    #define YYSTYPE TreeNode *  
    TreeNode* root;
    extern int lineno;
    int yylex();
    int yyerror( char const * );
%}
%token T_CHAR T_INT T_STRING T_BOOL 

%token LOP_ASSIGN LOP_ASSIGN_OR LOP_ASSIGN_AND LOP_ASSIGN_NOT LOP_ADD LOP_SUB LOP_MULT LOP_DIV LOP_MOD 

%token LOP_DOUBLE_OR LOP_DOUBLE_AND LOP_EQ LOP_NOT_EQ LOP_LESS LOP_GREATER LOP_LESS_EQ LOP_GREATER_EQ

%token SEMICOLON

%token IDENTIFIER INTEGER CHAR BOOL STRING

%token L_Braces R_Braces L_Small_Braces R_Small_Braces IF ELSE


%%

program
: statements {
    root = new TreeNode(0, NODE_PROG); 
    root->addChild($1);
    printf("program\n");
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
;

compound_Stmt
: L_Braces statements R_Braces{
    $$=$2;
}
;


selection_Stmt
: IF L_Small_Braces conditional_Exp R_Small_Braces statement{
    TreeNode* node = new TreeNode($1->lineno, NODE_SELECTION_STMT);
    //node->stype = STMT_DECL;
    node->addChild($3);
    node->addChild($5);
    $$ = node;
    printf("if ( conditional_Exp ) statement\n");

}
| IF L_Small_Braces conditional_Exp R_Small_Braces statement ELSE statement{
    TreeNode* node = new TreeNode($1->lineno, NODE_SELECTION_STMT);
    //node->stype = STMT_DECL;
    node->addChild($3);
    node->addChild($5);
    node->addChild($7);
    $$ = node;
    printf("if ( conditional_Exp ) statement else statement\n");
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
    TreeNode* node = new TreeNode($1->lineno, NODE_LOGICAL_OR_EXP);
    //node->stype = STMT_DECL;
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
    TreeNode* node = new TreeNode($1->lineno, NODE_LOGICAL_AND_EXP);
    //node->stype = STMT_DECL;
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
    TreeNode* node = new TreeNode($1->lineno, NODE_EQUALITY_EXP);
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("equality_exp LOP_EQ relational_exp\n");
}
| equality_Exp LOP_NOT_EQ relational_Exp{
    TreeNode* node = new TreeNode($1->lineno, NODE_NOT_EQUALITY_EXP);
    //node->stype = STMT_DECL;
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
    TreeNode* node = new TreeNode($1->lineno, NODE_RELATION_LESS_EXP);
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_LESS additive_Exp\n");
}
| relational_Exp LOP_GREATER additive_Exp{
    TreeNode* node = new TreeNode($1->lineno, NODE_RELATION_GREATER_EXP);
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_GREATER additive_Exp\n");
}
| relational_Exp LOP_LESS_EQ additive_Exp{
    TreeNode* node = new TreeNode($1->lineno, NODE_RELATION_LESS_EQ_EXP);
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_GREATER additive_Exp\n");
}
| relational_Exp LOP_GREATER_EQ additive_Exp{
    TreeNode* node = new TreeNode($1->lineno, NODE_RELATION_GREATER_EQ_EXP);
    //node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("relational_exp OP_GREATER_EQ additive_Exp\n");
}
;

assignment_Stmt
: unaryExp assignment_Operator additive_Exp{
    TreeNode* node = new TreeNode($1->lineno, NODE_ASSIGN_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
    printf("unaryExp assignment_Operator additive_Exp\n");
}
;


additive_Exp
: mult_Exp{
    $$=$1;
}
| additive_Exp LOP_ADD mult_Exp{
    TreeNode* node = new TreeNode($1->lineno,NODE_additive_Exp);
    //有可能加stype?
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
| additive_Exp LOP_SUB mult_Exp{
    TreeNode* node = new TreeNode($1->lineno,NODE_additive_Exp);
    //有可能加stype?
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
;


mult_Exp
: unaryExp{
    $$=$1;
}
| mult_Exp LOP_MULT unaryExp{
    TreeNode* node = new TreeNode($1->lineno,NODE_additive_Exp);
    //有可能加stype?
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
| mult_Exp LOP_DIV unaryExp{
    TreeNode* node = new TreeNode($1->lineno,NODE_additive_Exp);
    //有可能加stype?
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
| mult_Exp LOP_MOD unaryExp{
    TreeNode* node = new TreeNode($1->lineno,NODE_additive_Exp);
    //有可能加stype?
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    $$ = node;
}
;

unaryExp
: 
Id{
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
;

declaration
: T IDENTIFIER assignment_Operator additive_Exp{  // declare and init
    TreeNode* node = new TreeNode($1->lineno, NODE_DECL_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    $$ = node;
    printf("T IDENTIFIER LOP_ASSIGN expr\n");   
}
| T IDENTIFIER {
    TreeNode* node = new TreeNode($1->lineno, NODE_DECL_STMT);
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