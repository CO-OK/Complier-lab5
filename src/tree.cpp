#include "tree.h"
void printLayer(layerNode*node);
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
    this->layer_node=nullptr;
    this->change_field.accessTime=0;
    this->change_field.needChange=0;
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
        change_Field(tmp);
        for(TreeNode*t=tmp->child;t!=nullptr;t=t->sibling)
        {
            node_list.push_back(t);
        }
    }
    printf("id:%d\n",id);
}
void TreeNode::change_Child_Field(TreeNode* node)
{
    if(node->child==nullptr)
        return;
    TreeNode*tmp=node->child;
    while(tmp!=nullptr)
    {
        if(tmp->nodeType!=NODE_BLOCK_FLAG)
        {
            tmp->layer_node=node->layer_node;
            change_Child_Field(tmp);
        }    
        tmp=tmp->sibling;   
    }
}
void TreeNode:: change_Field(TreeNode* node)
{
    if(node->change_field.needChange)
    {
        node->layer_node=node->layer_node->list[node->change_field.accessTime];
        change_Child_Field(node);
    }
}
void TreeNode::printNodeInfo() {
    /*
        int nodeID; 
        int lineno;
        NodeType nodeType;
    */
    //printf("nodeID:%d\n",this->nodeID);
    cout<<"nodeID: "<<this->nodeID<<"    ";
    //printf("lineno:%d\n",this->lineno);
    cout<<"lineno: "<<this->lineno<<"    ";
    cout<<"NodeType: "<<this->nodeType2String(this->nodeType)<<"    ";
    cout<<"stype: "<<this->sType2String(this->stype)<<"    ";
    if(this->nodeType==NODE_TYPE)
    {
        cout<<"TYPE_SPEC="<<this->type->getTypeInfo()<<"    ";
    }
    if(this->nodeType==NODE_EXPR)
    {
        cout<<"expr_type="<<this->exprType2String(this->exprtype)<<"    ";
    }
    if(this->nodeType==NDOE_ITERATION_STMT)
    {
        cout<<"iteration_type="<<this->IterationType2String(this->iterationtype)<<"    ";
    }
    if(this->nodeType==NODE_JUMP_STMT)
    {
        cout<<"jumptype="<<this->JumpStmtType2String(this->jumptype)<<"    ";
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
        cout<<"layer:";
        printLayer(tmp->layer_node);
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
        case STMT_ASSIGN:{
            return "STMT_ASSIGN";
        }
        case STMT_SELECTION:{
            return "STMT_SELETION";
        }
        
        default:
            return "null";
    }
}


string TreeNode::IterationType2String(IterationType type)
{
    switch(type)
    {
        case ITERATION_WHILE:{
            return "STMT_ITERATION_WHILE";
        }
        case ITERATION_FOR_EEE:{
            return "STMT_ITERATION_FOR_EEE";
        }
        case ITERATION_FOR_EE_:{
            return "ITERATION_FOR_EE_";
        }
        case ITERATION_FOR_E_E:{
            return "ITERATION_FOR_E_E";
        }
        case ITERATION_FOR_E__:{
            return "ITERATION_FOR_E__";
        }
        case ITERATION_FOR__EE:{
            return "ITERATION_FOR__EE";
        }
        case ITERATION_FOR__E_:{
            return "ITERATION_FOR__E_";
        }
        case ITERATION_FOR___E:{
            return "ITERATION_FOR___E";
        }
        case ITERATION_FOR____:{
            return "ITERATION_FOR____";
        }
        default:
            return "null";
    }
}

string TreeNode::exprType2String(ExprType type){
    switch(type)
    {
        case NODE_additive_Exp:{
            return "NODE_additive_Exp";
        }
        case NODE_ASSIGN_EXP:{
            return "NODE_ASSIGN_EXP";
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
            return "NODE_RELATION_GREATER_EQ_EXP";
        }
        case NODE_POSTFIX_EXP:{
            return "NODE_POSTFIX_EXP";
        }
        case NODE_UNARY_EXP:{
            return "NODE_UNARY_EXP";
        }
        default:
            return "null";
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
        case NODE_BLOCK_FLAG:{
            return "NODE_BLOCK_FLAG";
        }
        case NODE_SELECTION_STMT:{
            return "NODE_SELECTION_STMT";
        }
        case NDOE_ITERATION_STMT:{
            return "NDOE_ITERATION_STMT";
        }
        case NODE_JUMP_STMT:{
            return "NODE_JUMP_STMT";
        }
        case NODE_EMPTY:{
            return "NODE_EMPTY";
        }
        default:
            return "null";
    }
}
string TreeNode::JumpStmtType2String(JumpStmtType type)
{
    switch(type)
    {
        case JUMP_CONTINUE:{
            return "JUMP_CONTINUE";
        }
        case JUMP_BREAK:{
            return "JUMP_BREAK";
        }
        case JUMP_RETURN_EXP:{
            return "JUMP_RETURN_EXP";
        }
        case JUMP_RETURN:{
            return "JUMP_RETURN";
        }
        default:
            return "null";
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
        case OP_ADD_EQ:{
            return "OP_ADD_EQ";
        }
        case OP_SUB_EQ:{
            return "OP_SUB_EQ";
        }
        case OP_MULT_EQ:{
            return "OP_MULT_EQ";
        }
        case OP_DIV_EQ:{
            return "OP_DIV_EQ";
        }
        case OP_MOD_EQ:{
            return "OP_MOD_EQ";
        }
        case OP_DOUBLE_ADD:{
            return "OP_DOUBLE_ADD";
        }
        case OP_DOUBLE_SUB:{
            return "OP_DOUBLE_SUB";
        }
        case OP_UNARY_REFERENCE:{
            return "OP_UNARY_REFERENCE";
        }
        case OP_UNARY_NOT:{
            return "OP_UNARY_NOT";
        }
        default:
            return "null";
    }
}


layerNode* makeNode(layerNode* node)
{
    layerNode* temp = node->list[node->nodeCount]=new layerNode;
    temp->nodeCount=0;
    temp->prev=node;
    temp->accessTime=0;
    //process layerDesc
    int flag=0;
    for(int i=0;i<layerDescNum;i++)
    {
        if(node->layerDesc[i]!=-1)
        {
            temp->layerDesc[i]=node->layerDesc[i];
            flag++;
        }
        else
        {
            temp->layerDesc[i]=-1;
        }
        
    }
    temp->layerDesc[flag]=node->nodeCount;
    node->nodeCount++;
    return temp;
}
void printLayer(layerNode*node)
{
    if(node==nullptr)
    {
        cout<<"null"<<"     ";
        return ;
    }
    for(int i=0;i<layerDescNum&&node->layerDesc[i]!=-1;i++)
    {
        if(i+1<layerDescNum&&node->layerDesc[i+1]==-1)
        {
            cout<<node->layerDesc[i];
        }
        else
        {   
            cout<<node->layerDesc[i]<<"-";
        }
    }
    cout<<"    ";
}