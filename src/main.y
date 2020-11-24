%{
    #include "common.h"
    #define YYSTYPE TreeNode *  
    TreeNode* root;
    extern int lineno;
    int yylex();
    int yyerror( char const * );
%}
%token T_CHAR T_INT T_STRING T_BOOL 

%token LOP_ASSIGN 

%token SEMICOLON

%token IDENTIFIER INTEGER CHAR BOOL STRING

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
;

declaration
: T IDENTIFIER LOP_ASSIGN paperConst{  // declare and init
    TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    $$ = node;
    printf("T IDENTIFIER LOP_ASSIGN expr\n");   
}
| T IDENTIFIER LOP_ASSIGN Id{  // declare and init
    TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
    node->stype = STMT_DECL;
    node->addChild($1);
    node->addChild($2);
    node->addChild($4);
    $$ = node;
    printf("T IDENTIFIER LOP_ASSIGN expr\n");   
}
| T IDENTIFIER {
    TreeNode* node = new TreeNode($1->lineno, NODE_STMT);
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