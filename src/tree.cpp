#include "tree.h"
#include "type.h"
void printLayer(layerNode*node);
void TreeNode::addChild(TreeNode* child) {

    //printf("add child\n");
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
    //printf("add sibling\n");
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
    this->change_field.is_func_field=0;
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
    if(node->change_field.is_func_field)
    {
        node->layer_node->is_func=1;//表示这个layer_node是一个函数体的layer
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
            break;
        }
        case NODE_FUNCTION_DEF:{
            if(this->func_info->func_name!=nullptr)
                cout<<"func_name="<<this->func_info->func_name->var_name<<"     ";
            if(this->func_info->return_value!=nullptr)
                cout<<"return_value="<<this->func_info->return_value->type->getTypeInfo()<<"    ";
            if(this->func_info->decl_list!=nullptr)
                cout<<"func_args_at_node:"<<this->func_info->decl_list->nodeID<<"     ";
            if(this->func_info->func_body!=nullptr)
                cout<<"func_body_at_node:"<<this->func_info->func_body->nodeID<<"     ";
            if(this->func_info->arg_list!=nullptr)
                cout<<"func_args_at_node:"<<this->func_info->arg_list->nodeID<<"      ";
            break;
        }
        case NODE_FUNCTION_CALL:{
            if(this->func_info->func_name!=nullptr)
                cout<<"func_name="<<this->func_info->func_name->var_name<<"     ";
            if(this->func_info->arg_list!=nullptr)
                cout<<"func_args_at_node:"<<this->func_info->arg_list->nodeID<<"      ";
            break;
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
        case NODE_ASSIGN_EXP_WITH_COMMA:
        {
            return "NODE_ASSIGN_EXP_WITH_COMMA";
        }
        case NODE_MULT_EXP:{
            return "NODE_MULT_EXP";
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
        case NODE_ASSIGN_EXPR:
        {
            return "NODE_ASSIGN_EXPR";
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
        case NODE_DECL_STMT_LIST:{
            return "NODE_DECL_STMT_LIST";
        }
        case NODE_FUNCTION_DEF:{
            return "NODE_FUNCTION_DEF";
        }
        case NODE_MAIN:{
            return "NODE_MAIN";
        }
        case NODE_ID_LIST:{
            return "NODE_ID_LIST";
        }
        case NODE_FUNCTION_CALL:{
            return "NODE_FUNCTION_CALL";
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
    temp->is_func=0;
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
    temp->section=new SymbolTableSection;
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

TreeNode* findFuncDef(string func_name,list<TreeNode*> func_list)
{
    for(list<TreeNode*>::iterator i=func_list.begin();i!=func_list.end();i++)
    {
        if((*i)->func_info->func_name->var_name==func_name)
            return *i;
    }
    return nullptr;
}


int TreeNode:: check_type()
{
    if(this->nodeType==NODE_DECL_STMT&&this->child_num()==3)
    {
        if((this->get_child(0)->type->type)==(this->get_child(2)->type->type))
            return 1;
        else
        {
            printf("NODE_DECL_STMT type error at line %d\n",this->lineno);
            return 0;
        }
    }
    if(this->nodeType==NODE_EXPR)
    {
        switch (this->exprtype)
        {
            case NODE_POSTFIX_EXP://后缀表达式必须是整形才可以运算
            {
                if(this->get_child(0)->type->type!=TYPE_INT->type)
                {
                    printf("NODE_POSTFIX_EXP type error at line %d,",this->lineno);
                    cout<<"type = "<<this->get_child(0)->type->getTypeInfo()<<", should be int"<<endl;
                    return 0;
                }
                else
                    return 1;  
            }
            case NODE_UNARY_EXP://前缀表达式
            {
                if(this->get_child(0)->optype==OP_UNARY_NOT&&this->get_child(1)->type->type==TYPE_BOOL->type)
                    return 1;//!后面必须跟bool型
                if(this->get_child(0)->optype==OP_UNARY_REFERENCE&&this->get_child(1)->nodeType==NODE_VAR)
                    return 1;//引用后面必须跟一个变量
                if(this->get_child(0)->optype==OP_UNARY_NOT&&this->get_child(1)->type->type==TYPE_INT->type)
                    return 1;//负号后面必须跟一个整形
                printf("UNARY_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_MULT_EXP:
            {
                if(this->get_child(0)->type->type==TYPE_INT->type&&this->get_child(2)->type->type==TYPE_INT->type)
                    return 1;//两边必须都是整形才可以计算
                printf("NODE_MULT_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_additive_Exp:
            {
                //cout<<this->type->getTypeInfo()<<endl;
                if(this->get_child(0)->type->type==TYPE_INT->type&&this->get_child(2)->type->type==TYPE_INT->type)
                    return 1;//两边必须都是整形才可以计算
                printf("NODE_additive_Exp type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_ASSIGN_EXP:
            {
                if(this->get_child(0)->type->type==this->get_child(2)->type->type)
                    return 1;//两边类型相同才能赋值
                printf("NODE_ASSIGN_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_RELATION_LESS_EXP:
            {
                //两边类型相同且不是string时才可以比较
                if((this->get_child(0)->type->type==this->get_child(2)->type->type)&&this->get_child(0)->type->type!=TYPE_STRING->type)
                {
                    return 1;
                }
                printf("NODE_RELATION_LESS_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_RELATION_GREATER_EXP:
            {
                //两边类型相同且不是string时才可以比较
                if((this->get_child(0)->type->type==this->get_child(2)->type->type)&&this->get_child(0)->type->type!=TYPE_STRING->type)
                {
                    return 1;
                }
                printf("NODE_RELATION_GREATER_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_RELATION_LESS_EQ_EXP:
            {
                //两边类型相同且不是string时才可以比较
                if((this->get_child(0)->type->type==this->get_child(2)->type->type)&&this->get_child(0)->type->type!=TYPE_STRING->type)
                {
                    return 1;
                }
                printf("NODE_RELATION_LESS_EQ_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_RELATION_GREATER_EQ_EXP:
            {
                //两边类型相同且不是string时才可以比较
                if((this->get_child(0)->type->type==this->get_child(2)->type->type)&&this->get_child(0)->type->type!=TYPE_STRING->type)
                {
                    return 1;
                }
                printf("NODE_RELATION_GREATER_EQ_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_EQUALITY_EXP:
            {
                //两边类型相同时才可以比较
                if(this->get_child(0)->type->type==this->get_child(2)->type->type)
                {
                    return 1;
                }
                printf("NODE_EQUALITY_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_NOT_EQUALITY_EXP:
            {
                //两边类型相同时才可以比较
                if(this->get_child(0)->type->type==this->get_child(2)->type->type)
                {
                    return 1;
                }
                printf("NODE_NOT_EQUALITY_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_LOGICAL_OR_EXP:
            {
                //两边都是bool型
                if(this->get_child(0)->type->type==TYPE_BOOL->type&&this->get_child(2)->type->type==TYPE_BOOL->type)
                {
                    return 1;
                }
                printf("NODE_LOGICAL_OR_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_LOGICAL_AND_EXP:
            {
                //两边都是bool型
                if(this->get_child(0)->type->type==TYPE_BOOL->type&&this->get_child(2)->type->type==TYPE_BOOL->type)
                {
                    return 1;
                }
                printf("NODE_LOGICAL_AND_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            default:
                return 1;
        }
    }
    if(this->nodeType==NDOE_ITERATION_STMT)
    {
        if(this->iterationtype==ITERATION_FOR___E)
        {
            //for的最后一项必须是表达式
            NodeType type=this->get_child(0)->nodeType;
            if(type!=NODE_EXPR&&type!=NODE_ASSIGN_STMT&&type!=NODE_ASSIGN_EXPR)
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, last pos must be an expr\n",get_child(0)->lineno);
                return 0;
            }
            return 1;
        }
        if(this->iterationtype==ITERATION_FOR__E_)
        {
            //中间必须是bool类型表达式
            if(!(this->get_child(0)->nodeType==NODE_EXPR&&this->get_child(0)->type->type==TYPE_BOOL->type))
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, middle pos must be an bool expr\n",this->lineno);
                return 0;
            }
            return 1;
        }
        if(this->iterationtype==ITERATION_FOR__EE)
        {
            int flag=1;
            //for的最后一项必须是表达式
            NodeType type=this->get_child(1)->nodeType;
            if(type!=NODE_EXPR&&type!=NODE_ASSIGN_STMT&&type!=NODE_ASSIGN_EXPR)
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, last pos must be an expr\n",get_child(1)->lineno);
                flag=0;
            }
             //中间必须是bool类型表达式
            if(!(this->get_child(0)->nodeType==NODE_EXPR&&this->get_child(0)->type->type==TYPE_BOOL->type))
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, middle pos must be an bool expr\n",get_child(0)->lineno);
                flag=0;
            }
            return flag;
        }
        if(this->iterationtype==ITERATION_FOR_E__)
        {
            //目前for的第一个表达式可以是以下类型
            NodeType type=get_child(0)->nodeType;
            if(type==NODE_ASSIGN_EXPR||type==NODE_ASSIGN_STMT)//赋值语句
                return 1;
            if(type==NODE_EXPR)//表达式算数或bool
                return 1;
            if(type==NODE_DECL_STMT||type==NODE_DECL_STMT_LIST)//声明
                return 1;
            printf("NDOE_ITERATION_STMT FOR error at line %d, first pos type error\n",get_child(0)->lineno);
            return 0;
        }
        if(this->iterationtype==ITERATION_FOR_E_E)
        {
            int flag1=0;
            //目前for的第一个表达式可以是以下类型
            NodeType type1=get_child(0)->nodeType;
            if(type1==NODE_ASSIGN_EXPR||type1==NODE_ASSIGN_STMT)//赋值语句
            {
                flag1=1;
            }
            if(type1==NODE_EXPR)//表达式算数或bool
            {
                flag1=1;
            }
            if(type1==NODE_DECL_STMT||type1==NODE_DECL_STMT_LIST)//声明
            {
                flag1=1;
            }
            if(!flag1)
                printf("NDOE_ITERATION_STMT FOR error at line %d, first pos type error\n",get_child(0)->lineno);
            //for的最后一项必须是表达式
            int flag2=1;
            NodeType type3=this->get_child(1)->nodeType;
            if(type3!=NODE_EXPR&&type3!=NODE_ASSIGN_STMT&&type3!=NODE_ASSIGN_EXPR)
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, last pos must be an expr\n",get_child(1)->lineno);
                flag2=0;
            }
            return flag1*flag2;
        }
        if(this->iterationtype==ITERATION_FOR_EE_)
        {
            int flag1=0;
            //目前for的第一个表达式可以是以下类型
            NodeType type1=get_child(0)->nodeType;
            if(type1==NODE_ASSIGN_EXPR||type1==NODE_ASSIGN_STMT)//赋值语句
            {
                flag1=1;
            }
            if(type1==NODE_EXPR)//表达式算数或bool
            {
                flag1=1;
            }
            if(type1==NODE_DECL_STMT||type1==NODE_DECL_STMT_LIST)//声明
            {
                flag1=1;
            }
            if(!flag1)
                printf("NDOE_ITERATION_STMT FOR error at line %d, first pos type error\n",get_child(0)->lineno);
             //中间必须是bool类型表达式
            int flag2=1;
            if(!(this->get_child(1)->nodeType==NODE_EXPR&&this->get_child(1)->type->type==TYPE_BOOL->type))
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, middle pos must be an bool expr\n",get_child(1)->lineno);
                flag2=0;
            }
            return flag1*flag2;
        }
        if(this->iterationtype==ITERATION_FOR_EEE)
        {
            int flag1=0;
            //目前for的第一个表达式可以是以下类型
            NodeType type1=get_child(0)->nodeType;
            if(type1==NODE_ASSIGN_EXPR||type1==NODE_ASSIGN_STMT)//赋值语句
            {
                flag1=1;
            }
            if(type1==NODE_EXPR)//表达式算数或bool
            {
                flag1=1;
            }
            if(type1==NODE_DECL_STMT||type1==NODE_DECL_STMT_LIST)//声明
            {
                flag1=1;
            }
            if(!flag1)
                printf("NDOE_ITERATION_STMT FOR error at line %d, first pos type error\n",get_child(0)->lineno);
            //中间必须是bool类型表达式
            int flag2=1;
            if(!(this->get_child(1)->nodeType==NODE_EXPR&&this->get_child(1)->type->type==TYPE_BOOL->type))
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, middle pos must be an bool expr\n",get_child(1)->lineno);
                flag2=0;
            }
            //for的最后一项必须是表达式
            int flag3=1;
            NodeType type2=this->get_child(2)->nodeType;
            if(type2!=NODE_EXPR&&type2!=NODE_ASSIGN_STMT&&type2!=NODE_ASSIGN_EXPR)
            {
                printf("NDOE_ITERATION_STMT FOR error at line %d, last pos must be an expr\n",get_child(2)->lineno);
                flag3=0;
            }
            return flag1*flag2*flag3;
        }
        if(this->iterationtype==ITERATION_WHILE)
        {
            //while条件必须是bool表达式
            if(!(this->get_child(0)->nodeType==NODE_EXPR&&this->get_child(0)->type->type==TYPE_BOOL->type))
            {
                printf("NDOE_ITERATION_STMT WHILE error at line %d, middle pos must be an bool expr\n",get_child(0)->lineno);
                return 0;
            }
            return 1;
        }
    }
    if(this->nodeType==NODE_SELECTION_STMT)
    {
        if(!(this->get_child(0)->nodeType==NODE_EXPR&&this->get_child(0)->type->type==TYPE_BOOL->type))
            {
                printf("NODE_SELECTION_STMT IF error at line %d,  must be an bool expr\n",get_child(0)->lineno);
                return 0;
            }
        return 1;
    }
    return 1;
}

TreeNode* TreeNode:: get_child(int child_num)
{
    TreeNode* tmp=this->child;
    for(int i=0;i<child_num;i++)
    {
        tmp=tmp->sibling;
    }
    return tmp;
}

int TreeNode:: child_num()
{
    int count=0;
    TreeNode*tmp=this->child;
    while(tmp!=nullptr)
    {
        tmp=tmp->sibling;
        count++;
    }
    return count;
}
void check_section(layerNode* node)
{
    check_symbol_table(node->section);
    for(int i=0;i<node->nodeCount;i++)
    {
        check_section(node->list[i]);
    }
}