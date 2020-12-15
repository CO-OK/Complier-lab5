%option nounput
%{
#include "common.h"
#include "main.tab.h"  // yacc header
#include <string.h>
//#include "layerNode.h"
int lineno=1;
extern layerNode* currentNode;
extern layerNode* layer_root;
extern layerNode* makeNode(layerNode* node);
extern int assignRefSymbolType(layerNode*node,Item*);
%}
BLOCKCOMMENT \/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/
LINECOMMENT \/\/[^\n]*
EOL	(\r\n|\r|\n)
WHILTESPACE [[:blank:]]


INTEGER [0-9]+
LOP_REFERENCE "&"
LOP_NOT "!"
LOP_ADD "+"
LOP_SUB "-"
LOP_MULT "*"
LOP_DIV "/"
LOP_MOD "%"
LOP_DOUBLE_OR "||" 
LOP_DOUBLE_AND "&&" 
LOP_EQ "==" 
LOP_NOT_EQ "!=" 
LOP_LESS "<" 
LOP_GREATER ">" 
LOP_LESS_EQ "<=" 
LOP_GREATER_EQ ">=" 


DOUBLE_ADD "++"
DOUBLE_SUB "--"
CHAR \'.?\'
STRING \".*\"
BOOL ("true"|"false")

L_Braces "{"
R_Braces "}"

IDENTIFIER [[:alpha:]_][[:alpha:][:digit:]_]* 
%%

{BLOCKCOMMENT}  /* do nothing */
{LINECOMMENT}  /* do nothing */


"int" return T_INT;
"bool" return T_BOOL;
"char" return T_CHAR;
"string" return T_STRING;

"=" return LOP_ASSIGN;
"|=" return LOP_ASSIGN_OR;
"&=" return LOP_ASSIGN_AND;
"^=" return LOP_ASSIGN_NOT;
"+=" return LOP_ASSIGN_ADD;
"-=" return LOP_ASSIGN_SUB;
"*=" return LOP_ASSIGN_MULT;
"/=" return LOP_ASSIGN_DIV;
"%=" return LOP_ASSIGN_MOD;


"(" return L_Small_Braces;
")" return R_Small_Braces;
"if" return IF;
"else" return ELSE;
"while" return WHILE;
"for" return FOR;

"continue" return CONTINUE;
"break" return BREAK;
"return" return RETURN;

"main" return MAIN;

";" return  SEMICOLON;
"," return COMMA;

{INTEGER} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_INT;
    node->int_val = atoi(yytext);
    node->layer_node=currentNode;
    yylval = node;
    
    return INTEGER;
}

{CHAR} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_CHAR;
    node->int_val = yytext[1];
    node->ch_val = yytext[1];
    node->layer_node=currentNode;
    yylval = node;
    return CHAR;
}

{STRING} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_STRING;
    node->str_val = yytext;
    node->layer_node=currentNode;
    yylval = node;
    return CHAR;
}

{BOOL} {
    TreeNode* node = new TreeNode(lineno, NODE_CONST);
    node->type = TYPE_BOOL;
    if(strcmp(yytext,"true")==0||atoi(yytext)==1)
    {
        node->b_val=1;
    }
    else{
        node->b_val=0;
    }
    //node->str_val = yytext;
    node->layer_node=currentNode;
    yylval = node;
    return BOOL;
}

{IDENTIFIER} {
    TreeNode* node = new TreeNode(lineno, NODE_VAR);
    node->var_name = string(yytext);
    node->layer_node=currentNode;
    node->type=TYPE_VOID;
    yylval = node;
    Item* item=new Item;
    item->name=node->var_name;
    item->symbol_type=SYMBOL_VAR;
    item->symbol_property=PROPERTY_REFE;
    item->tree_node=node;
    item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
    currentNode->section->section_table.push_back(item);
    //在符号表中寻找有没有之前定义过的，如果有，则将定义的那个的type赋给此变量
    /*code*/
    assignRefSymbolType(currentNode,item);
    return IDENTIFIER;
}

{LOP_ADD} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_ADD;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_ADD;
}

{LOP_SUB} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_SUB;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_SUB;
}

{LOP_MULT} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_MULT;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_MULT;
}

{LOP_DIV} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_DIV;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_DIV;
}

{LOP_MOD} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_MOD;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_MOD;
}



{LOP_DOUBLE_OR} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_DOUBLE_OR;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_DOUBLE_OR;    
}

{LOP_DOUBLE_AND} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_DOUBLE_AND;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_DOUBLE_AND; 
}

{LOP_EQ} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_EQ;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_EQ;   
}

{LOP_NOT_EQ} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_NOT_EQ;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_NOT_EQ;   
}

{LOP_LESS} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_LESS;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_LESS;
}

{LOP_GREATER} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_GREATER;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_GREATER;    
}

{LOP_LESS_EQ} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_LESS_EQ;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_LESS_EQ;     
}

{LOP_GREATER_EQ} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_GREATER_EQ;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_GREATER_EQ; 
}

{DOUBLE_ADD} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_DOUBLE_ADD;
    node->layer_node=currentNode;
    yylval = node;
    return DOUBLE_ADD;
}

{DOUBLE_SUB} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_DOUBLE_SUB;
    node->layer_node=currentNode;
    yylval = node;
    return DOUBLE_SUB;
}

{LOP_REFERENCE} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_UNARY_REFERENCE;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_REFERENCE;
}

{LOP_NOT} {
    TreeNode* node = new TreeNode(lineno, NODE_OPERATOR);
    node->optype=OP_UNARY_NOT;
    node->layer_node=currentNode;
    yylval = node;
    return LOP_NOT;
}


{L_Braces} {
    //yylval = currentNode;
    currentNode=makeNode(currentNode);
    return L_Braces;
}

{R_Braces} {
    currentNode=currentNode->prev;
    currentNode->accessTime++;
    return R_Braces;
}

{WHILTESPACE} /* do nothing */

{EOL} lineno++;

. {
    cerr << "[line "<< lineno <<" ] unknown character:" << yytext << endl;
}
%%