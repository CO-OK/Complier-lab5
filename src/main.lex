%option nounput
%{
#include "common.h"
#include "main.tab.h"  // yacc header
int lineno=1;
%}
BLOCKCOMMENT \/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/
LINECOMMENT \/\/[^\n]*
EOL	(\r\n|\r|\n)
WHILTESPACE [[:blank:]]

INTEGER [0-9]+
LOP_ADD "+"
LOP_SUB "-"
LOP_MULT "*"
LOP_DIV "/"
LOP_MOD "%"

CHAR \'.?\'
STRING \".*\"

IDENTIFIER [[:alpha:]_][[:alpha:][:digit:]_]* 
%%

{BLOCKCOMMENT}  /* do nothing */
{LINECOMMENT}  /* do nothing */

"{" return L_Braces;
"}" return R_Braces;
"int" return T_INT;
"bool" return T_BOOL;
"char" return T_CHAR;
"string" return T_STRING;

"=" return LOP_ASSIGN;
"|=" return LOP_ASSIGN_OR;
"&=" return LOP_ASSIGN_AND;
"^=" return LOP_ASSIGN_NOT;

";" return  SEMICOLON;

{INTEGER} {
    printf("INTEGER\n");
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_INT;
    node->int_val = atoi(yytext);
    yylval = node;
    return INTEGER;
}

{CHAR} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_CHAR;
    node->int_val = yytext[1];
    node->ch_val = yytext[1];
    yylval = node;
    return CHAR;
}

{STRING} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_STRING;
    node->str_val = yytext;
    yylval = node;
    return CHAR;
}

{IDENTIFIER} {
    TreeNode* node = new TreeNode(lineno, NODE_VAR);
    node->var_name = string(yytext);
    yylval = node;
    return IDENTIFIER;
}

{LOP_ADD} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_ADD;
    yylval = node;
    return LOP_ADD;
}

{LOP_SUB} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_SUB;
    yylval = node;
    return LOP_SUB;
}

{LOP_MULT} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_MULT;
    yylval = node;
    return LOP_MULT;
}

{LOP_DIV} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_DIV;
    yylval = node;
    return LOP_DIV;
}

{LOP_MOD} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_MOD;
    yylval = node;
    return LOP_MOD;
}



{WHILTESPACE} /* do nothing */

{EOL} lineno++;

. {
    cerr << "[line "<< lineno <<" ] unknown character:" << yytext << endl;
}
%%