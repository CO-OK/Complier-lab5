#include "tree.h"
void TreeNode::addChild(TreeNode* child) {

    printf("add child\n");
    if(this->child==nullptr)
        this->child=child;
    else
    {
        TreeNode* tmp=this->child;
        while(tmp->sibling!=nullptr)
        {
            tmp=tmp->sibling;
        }
        tmp->sibling=child;
    }
    
}

void TreeNode::addSibling(TreeNode* sibling){
    printf("add sibling\n");
    if(this->sibling==nullptr)
        this->sibling=sibling;
    else
    {
        TreeNode* tmp=this;
        while(tmp->sibling!=nullptr)
        {
            tmp=tmp->sibling;
        }
        tmp->sibling=sibling;
    }  
}

TreeNode::TreeNode(int lineno, NodeType type) {
    this->lineno=lineno;
    this->nodeType=type;
}

void TreeNode::genNodeId() {
    //层级遍历
    list<TreeNode*> node_list;
    TreeNode*tmp=nullptr;
    node_list.push_back(this);
    int id=0;
    while(!node_list.empty())
    {
        tmp=node_list.front();
        node_list.pop_front();
        tmp->nodeID=id;
        id++;
        for(TreeNode*t=tmp->child;t!=nullptr;t=t->sibling)
        {
            node_list.push_back(t);
        }
    }
    printf("id:%d\n",id);
}

void TreeNode::printNodeInfo() {
    /*
        int nodeID; 
        int lineno;
        NodeType nodeType;
    */
    //printf("nodeID:%d\n",this->nodeID);
    cout<<"nodeID:"<<this->nodeID<<"    ";
    //printf("lineno:%d\n",this->lineno);
    cout<<"lineno:"<<this->lineno<<"    ";
    cout<<"NodeType:"<<this->nodeType2String(this->nodeType)<<"    ";
    if(this->nodeType==NODE_TYPE)
    {
        cout<<"TYPE_SPEC="<<this->type->getTypeInfo()<<"    ";
    }
}

void TreeNode::printChildrenId() {
    TreeNode*tmp=this->child;
    cout<<"child: ";
    while(tmp!=nullptr)
    {
        cout<<tmp->nodeID<<" ";
        tmp=tmp->sibling;
    }
    cout<<"    ";
}

void TreeNode::printAST() {
    //层级遍历
    list<TreeNode*> node_list;
    TreeNode*tmp=nullptr;
    node_list.push_back(this);
    while(!node_list.empty())
    {
        tmp=node_list.front();
        node_list.pop_front();
        //cout<<tmp->nodeID;
        tmp->printNodeInfo();
        tmp->printSpecialInfo();
        tmp->printChildrenId();
        cout<<endl;
        for(TreeNode*t=tmp->child;t!=nullptr;t=t->sibling)
        {
            node_list.push_back(t);
        }
    }
}


// You can output more info...
void TreeNode::printSpecialInfo() {
    switch(this->nodeType){
        case NODE_CONST:{
            /*
            TreeNode* node = new TreeNode(lineno, NODE_CONST);
            node->type = TYPE_INT;
            node->int_val = atoi(yytext);
            yylval = node;
            return INTEGER;
            */
            string tmp=type->getTypeInfo();
            cout<<"node->type="<<tmp<<" ";
            if(tmp=="char")
            {
                cout<<"ascii_value="<<this->int_val<<" "<<"character="<<this->ch_val<<" ";
            }
            else if(tmp=="int")
            {
                cout<<"value="<<this->int_val<<" ";
            }
            else if(tmp=="bool")
            {
                cout<<"value="<<this->b_val<<" ";
            }
            else if(tmp=="string")
            {
                cout<<"value="<<this->str_val<<" ";
            }
            break;
        }
        case NODE_VAR:{
            /*
            TreeNode* node = new TreeNode(lineno, NODE_VAR);
            node->var_name = string(yytext);
            yylval = node;
            return IDENTIFIER;
            */
           cout<<"var_name="<<this->var_name<<"    ";
           break;
        }
        case NODE_EXPR:
            break;
        case NODE_STMT:
            break;
        case NODE_TYPE:
            break;
        case NODE_OPERATOR:{
            cout<<"operator_type_= "<<this->opType2String(this->optype)<<"     ";
        }
        default:
            break;
    }
}

string TreeNode::sType2String(StmtType type) {
    switch(type)
    {
        case STMT_SKIP:{
            //cout<<"StmtType: STMT_SKIP"<<endl;
            return "STMT_SKIP";
        }
        case STMT_DECL:{
            //cout<<"StmtType: STMT_DECL"<<endl;
            return "STMT_DECL";
        }
        default:
            return "";
    }
}


string TreeNode::nodeType2String (NodeType type){
    switch(type)
    {
        case NODE_CONST:{
            //cout<<"NodeType: NODE_CONST"<<endl;
            return "NODE_CONST";
        }
        case NODE_VAR:{
            //cout<<"NodeType: NODE_VAR"<<endl;
            return "NODE_VAR";
        }
        case NODE_EXPR:{
            //cout<<"NodeType: NODE_EXPR"<<endl;
            return "NODE_EXPR";
        }
        case NODE_TYPE:{
            //cout<<"NodeType: NODE_TYPE"<<endl;
            return "NODE_TYPE";
        }
        case NODE_STMT:{
            //cout<<"NodeType: NODE_STMT"<<endl;
            return "NODE_STMT";
        }
        case NODE_PROG:{
            //cout<<"NodeType: NODE_PROG"<<endl;
            return "NODE_PROG";
        }
        case NODE_DECL_STMT:{
            return "NODE_DECL_STMT";
        }
        case NODE_OPERATOR:{
            return "NODE_OPERATOR";
        }
        case NODE_ASSIGN_STMT:{
            return "NODE_ASSIGN_STMT";
        }
        case NODE_additive_Exp:{
            return "NODE_additive_Exp";
        }
        case NODE_BLOCK_FLAG:{
            return "NODE_BLOCK_FLAG";
        }
        case NODE_SELECTION_STMT:{
            return "NODE_SELECTION_STMT";
        }
        case NODE_LOGICAL_OR_EXP:{
            return "NODE_LOGICAL_OR_EXP";
        }
        case NODE_LOGICAL_AND_EXP:{
            return "NODE_LOGICAL_AND_EXP";
        }
        case NODE_EQUALITY_EXP:{
            return "NODE_EQUALITY_EXP";
        }
        case NODE_NOT_EQUALITY_EXP:{
            return "NODE_NOT_EQUALITY_EXP";
        }
        case NODE_RELATION_LESS_EXP:{
            return "NODE_RELATION_LESS_EXP";
        }
        case NODE_RELATION_GREATER_EXP:{
            return "NODE_RELATION_GREATER_EXP";
        }
        case NODE_RELATION_LESS_EQ_EXP:{
            return "NODE_RELATION_LESS_EQ_EXP";
        }
        case NODE_RELATION_GREATER_EQ_EXP:{
            return "NODE_RELATION_GREATER_EXP";
        }
        default:
            return "";
    }
}

string TreeNode::opType2String (OperatorType type)
{
    switch(type)
    {
        case OP_EQ:{
            //cout<<"OperatorType: OP_EQ"<<endl;
            return "OP_EQ";
        }
        case OP_ASSIGN_EQ:{
            return "OP_ASSGIN_EQ";
        }
        case OP_OR_EQ:{
            return "OP_OR_EQ";
        }
        case OP_AND_EQ:{
            return "OP_AND_EQ";
        }
        case OP_ASSIGN_NOT_EQ:{
            return "OP_ASSIGN_NOT_EQ";
        }
        case OP_ADD:{
            return "OP_ADD";
        }
        case OP_SUB:{
            return "OP_SUB";
        }
        case OP_MULT:{
            return "OP_MULT";
        }
        case OP_DIV:{
            return "OP_DIV";
        }
        case OP_MOD:{
            return "OP_MOD";
        }
        case OP_DOUBLE_OR:{
            return "OP_DOUBLE_OR";
        }
        case OP_DOUBLE_AND:{
            return "OP_DOUBLE_AND";
        }
        case OP_NOT_EQ:{
            return "OP_NOT_EQ";
        }
        case OP_LESS:{
            return "OP_LESS";
        }
        case OP_GREATER:{
            return "OP_GREATER";
        }
        case OP_LESS_EQ:{
            return "OP_LESS_EQ";
        }
        case OP_GREATER_EQ:{
            return "OP_GREATER_EQ";
        }
        default:
            return "";
    }
}