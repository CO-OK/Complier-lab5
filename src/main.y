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

%token SEMICOLON

%token IDENTIFIER INTEGER CHAR BOOL STRING

%token L_Braces R_Braces

%left LOP_EQ

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
;

compound_Stmt
: L_Braces statement R_Braces{
    $$=$2;
}

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
: T IDENTIFIER assignment_Operator paperConst{  // declare and init
    TreeNode* node = new TreeNode($1->lineno, NODE_DECL_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($3);
    node->addChild($4);
    $$ = node;
    printf("T IDENTIFIER LOP_ASSIGN expr\n");   
}
| T IDENTIFIER assignment_Operator Id{  // declare and init
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