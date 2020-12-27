#include "tree.h"
#include "SymbolTable.h"
#include "type.h"
#include <string>
extern int temp_count;
extern TreeNode *root;
extern int str_count;
extern Item* global_str_list[1000];
extern list<ThreeAdCodeItem*> *code_list;
void get_all_arg(TreeNode* id_list,list<TreeNode*>*arg_list);
void printLayer(layerNode*node);
string gen_expr_asm(TreeNode* node);
string get_location_or_value(TreeNode* node);
string get_code(TreeNode*);
string opType2_String (OperatorType type);
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
    this->label.begin_label=nullptr;
    this->label.next_label=nullptr;
    this->label.false_label=nullptr;
    this->label.true_label=nullptr;
    this->code=nullptr;
    this->item=nullptr;
    this->ThreeAD=nullptr;
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
    temp->total_count=0;
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
        if(this->get_child(0)->type->type==VALUE_CHAR&&this->get_child(2)->type->type==VALUE_INT)
            return 1;
        if(this->get_child(0)->type->type==VALUE_INT&&this->get_child(2)->type->type==VALUE_CHAR)
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
                if(this->get_child(0)->type->type==TYPE_CHAR->type&&this->get_child(2)->type->type==TYPE_CHAR->type)
                    return 1;
                printf("NODE_MULT_EXP type error at line %d\n",this->lineno);
                return 0;
            }
            case NODE_additive_Exp:
            {
                //cout<<this->type->getTypeInfo()<<endl;
                if(this->get_child(0)->type->type==TYPE_INT->type&&this->get_child(2)->type->type==TYPE_INT->type)
                    return 1;//两边必须都是整形才可以计算
                if(this->get_child(0)->type->type==TYPE_CHAR->type&&this->get_child(2)->type->type==TYPE_CHAR->type)
                    return 1;
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

int TreeNode:: has_ID()
{
    if(this->nodeType==NODE_VAR)
        return 1;
    TreeNode*tmp=this->child;
    int a=0;
    while(tmp!=nullptr&&a==0)
    {
        a=tmp->has_ID();
        tmp=tmp->sibling;
    }
    return a;
}

void TreeNode:: gen_label(TreeNode*node)//生成label
{
    if(node==nullptr)
        return;
    int flag=0;
    if(node->nodeType==NODE_PROG)
    {
        flag=1;
    }
    else if(node->nodeType==NODE_MAIN)
    {
        flag=1;
        if(node->label.begin_label==nullptr)
        {
            node->label.begin_label=new string("label_main_begin");
            *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
        }
        if(node->label.next_label==nullptr)
        {
            node->label.next_label=new string("label_main_next");
            *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
        }
        node->child->sibling->label.begin_label=node->label.next_label;//main函数入口的话应该是孩子的开始是爹的next
    }
    else if(node->nodeType==NODE_SELECTION_STMT)//if
    {
        flag=1;
        if(node->label.begin_label==nullptr)
        {
            node->label.begin_label=new string("label_if_begin");
            *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
        }
        if(node->label.next_label==nullptr)
        {
            node->label.next_label=new string("label_if_next");
            *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
        }
        if(this->child_num()==2)//没有else
        {
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //if条件的true_label即为if语句块的begin
            node->get_child(0)->label.true_label=node->get_child(1)->label.begin_label=new string("label_if_true");
            *node->get_child(0)->label.true_label+=to_string(node->get_child(0)->nodeID);
            //if为真语句块的next即为if的next
            node->get_child(1)->label.next_label=node->label.next_label;
            //if条件的false_label即为if语句块的next
            node->get_child(0)->label.false_label=node->label.next_label;
        }
        else if(this->child_num()==3)//有else
        {
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //if条件的true_label即为if语句块的begin
            node->get_child(0)->label.true_label=node->get_child(1)->label.begin_label=new string("label_if_true");
            *node->get_child(0)->label.true_label+=to_string(node->get_child(0)->nodeID);
            //if条件的false_label即为else语句块的begin
            node->get_child(0)->label.false_label=node->get_child(2)->label.begin_label=new string("label_if_false");
            *node->get_child(0)->label.false_label+=to_string(node->get_child(0)->nodeID);
            //if为真语句块的next即为if的next
            node->get_child(1)->label.next_label=node->label.next_label;
            //if为假语句块的next即为if的next
            node->get_child(2)->label.next_label=node->label.next_label;
        }
    }
    else if(node->nodeType==NODE_DECL_STMT)//定义语句
    {
        flag=1;
        if(node->label.begin_label==nullptr)
        {
            node->label.begin_label=new string("decl_begin");
            *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
        }
        if(node->label.next_label==nullptr)
        {
            node->label.next_label=new string("decl_next");
            *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
        }
        //兄弟节点的begin即为node的next
        if(node->sibling!=nullptr)
            node->sibling->label.begin_label=node->label.next_label;
    }
    else if(node->nodeType==NODE_DECL_STMT_LIST)//定义列表
    {
        flag=1;
        if(node->label.begin_label==nullptr)
        {
            node->label.begin_label=new string("decl_list_begin");
            *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
        }
        if(node->label.next_label==nullptr)
        {
            node->label.next_label=new string("decl_list_next");
            *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
        }
        //兄弟节点的begin即为node的next
        if(node->sibling!=nullptr)
            node->sibling->label.begin_label=node->label.next_label;
    }
    else if(node->nodeType==NODE_ASSIGN_STMT)//赋值语句，带逗号
    {
        flag=1;
        if(node->label.begin_label==nullptr)
        {
            node->label.begin_label=new string("assign_stmt_begin");
            *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
        }
        if(node->label.next_label==nullptr)
        {
            node->label.next_label=new string("assign_stmt_next");
            *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
        }
        //兄弟节点的begin即为node的next
        if(node->sibling!=nullptr)
            node->sibling->label.begin_label=node->label.next_label;
    }
    else if(node->nodeType==NODE_ASSIGN_EXPR)//赋值语句不带逗号
    {
        flag=1;
        if(node->label.begin_label==nullptr)
        {
            node->label.begin_label=new string("assign_expr_begin");
            *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
        }
        if(node->label.next_label==nullptr)
        {
            node->label.next_label=new string("assign_expr_next");
            *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
        }
        //兄弟节点的begin即为node的next
        if(node->sibling!=nullptr)
            node->sibling->label.begin_label=node->label.next_label;
    }
    else if(node->nodeType==NDOE_ITERATION_STMT)//循环语句
    {
        flag=1;
        if(node->iterationtype==ITERATION_WHILE)
        {
            TreeNode*condition=node->get_child(0);
            TreeNode*loop_body=node->get_child(1);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("while_begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("while_next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //循环体的下一条语句————循环的开始(继续循环)
            loop_body->label.next_label=node->label.begin_label;
            //循环体的开始标号即为循环条件的真值的标号
            loop_body->label.begin_label=condition->label.true_label=new string("while_body_begin"+to_string(loop_body->nodeID)+":");
            //循环条件的假值标号即为循环的下一条语句标号
            condition->label.false_label=node->label.next_label;
        }
        else if(node->iterationtype==ITERATION_FOR_EEE)
        {
            TreeNode*condition=node->get_child(1);
            TreeNode*prev=node->get_child(0);
            TreeNode*after=node->get_child(2);
            TreeNode*loop_body=node->get_child(3);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for_e_e_e_begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for_e_e_e_next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //for循环第一个位置的begin即为for节点的begin
            prev->label.begin_label=node->label.begin_label;
            //for循环第一个位置的next应该是一个新的next
            prev->label.next_label=new string("for_first_pos_next"+to_string(prev->nodeID)+":");
            //循环体的下一条语句应该是for最后一个位置的begin
            loop_body->label.next_label=after->label.begin_label=new string("for_loop_body_next"+to_string(loop_body->lineno)+":");
            //循环体的开始语句应该是prev的next
            loop_body->label.begin_label=prev->label.next_label;
            //for循环最后一个位置的next应该是循环体的开始语句
            after->label.next_label=loop_body->label.begin_label;
            //condition的真值标号应该是循环体的begin
            condition->label.true_label=loop_body->label.begin_label;
            //condition的假值标号应该是for的next
            condition->label.false_label=node->label.next_label;
        }
        else if(node->iterationtype==ITERATION_FOR_EE_)
        {
            TreeNode*prev=node->get_child(0);
            TreeNode*condition=node->get_child(1);
            TreeNode*loop_body=node->get_child(2);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for_e_e__begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for_e_e__next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //for循环第一个位置的begin即为for节点的begin
            prev->label.begin_label=node->label.begin_label;
            //for循环第一个位置的next应该是一个新的next
            prev->label.next_label=new string("for_first_pos_next"+to_string(prev->nodeID)+":");
            //循环体的开始应该是第一个位置的next
            loop_body->label.begin_label=prev->label.next_label;
            //循环体的next应该是第一个位置的next
            loop_body->label.next_label=prev->label.next_label;
            //condition的真值是循环的开始
            condition->label.true_label=loop_body->label.begin_label;
            //condition的假值是node的next
            condition->label.false_label=node->label.next_label;
        }
        else if(node->iterationtype==ITERATION_FOR_E_E)
        {
            TreeNode*prev=node->get_child(0);
            TreeNode*after=node->get_child(1);
            TreeNode*loop_body=node->get_child(2);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for_e__e_begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for_e__e_next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //for循环第一个位置的begin即为for节点的begin
            prev->label.begin_label=node->label.begin_label;
            //for循环第一个位置的next应该是一个新的next
            prev->label.next_label=new string("for_first_pos_next"+to_string(prev->nodeID)+":");
            //循环体的开始应该是第一个位置的next
            loop_body->label.begin_label=prev->label.next_label;
            //循环体的next应该是after的开始
            loop_body->label.next_label=after->label.begin_label=new string("for_loop_body_next"+to_string(loop_body->lineno)+":");
            //after的next应该是loop_body的begin
            after->label.next_label=loop_body->label.begin_label;
        }
        else if(node->iterationtype==ITERATION_FOR_E__)
        {
            TreeNode*prev=node->get_child(0);
            TreeNode*loop_body=node->get_child(1);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for_e___begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for_e___next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //prev的begin即为for节点的begin
            prev->label.begin_label=node->label.begin_label;
            //prev的next是一个新的next
            prev->label.next_label=new string("for_first_pos_next"+to_string(prev->nodeID)+":");
            //loop_body的begin是prev的next
            loop_body->label.begin_label=prev->label.next_label;
            //loop_body的next是prev的next
            loop_body->label.next_label=prev->label.next_label;
        } 
        else if(node->iterationtype==ITERATION_FOR__EE)
        {
            TreeNode*condition=node->get_child(0);
            TreeNode*after=node->get_child(1);
            TreeNode*loop_body=node->get_child(2);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for__e_e_begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for__e_e_next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //loop_body的begin是for的begin
            loop_body->label.begin_label=node->label.begin_label;
            //loop_body的next是after的begin
            loop_body->label.next_label=after->label.begin_label=new string("for_loop_body_next"+to_string(loop_body->lineno)+":");
            //after的next是loop_body的begin
            after->label.next_label=loop_body->label.begin_label;
            //condition的true是循环体的开始
            condition->label.true_label=loop_body->label.begin_label;
            //condition的false是for的下一条
            condition->label.false_label=node->label.next_label;
        }
        else if(node->iterationtype==ITERATION_FOR__E_)
        {
            TreeNode*condition=node->get_child(0);
            TreeNode*loop_body=node->get_child(1);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for__e__begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for__e__next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //loop_body的begin即为node的begin
            loop_body->label.begin_label=node->label.begin_label;
            //loop_body的next即为node的begin
            loop_body->label.next_label=loop_body->label.begin_label;
            //condition的true即为node的begin
            condition->label.true_label=node->label.begin_label;
            //condition的false即为node的next
            condition->label.false_label=node->label.next_label;
        }
        else if(node->iterationtype==ITERATION_FOR___E)
        {
            TreeNode*after=node->get_child(0);
            TreeNode*loop_body=node->get_child(1);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for___e_begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for___e_next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //loop_body的begin即为node的begin
            loop_body->label.begin_label=node->label.begin_label;
            //loop_body的next即为after的begin
            loop_body->label.next_label=after->label.begin_label=new string("for_loop_body_next"+to_string(loop_body->lineno)+":");
            //after的next即为for的begin
            after->label.next_label=node->label.begin_label;
        }
        else if(node->iterationtype==ITERATION_FOR____)
        {
            TreeNode*loop_body=node->get_child(0);
            if(node->label.begin_label==nullptr)
            {
                node->label.begin_label=new string("for____begin");
                *node->label.begin_label=*node->label.begin_label+to_string(node->nodeID)+":";
            }
            if(node->label.next_label==nullptr)
            {
                node->label.next_label=new string("for____next");
                *node->label.next_label=*node->label.next_label+to_string(node->nodeID)+":";
            }
            //兄弟节点的begin即为node的next
            if(node->sibling!=nullptr)
                node->sibling->label.begin_label=node->label.next_label;
            //loop_body的begin即为node的begin
            loop_body->label.begin_label=node->label.begin_label;
            //loop_body的next即为node的begin
            loop_body->label.next_label=node->label.begin_label;
        }
    }
    else if(node->nodeType==NODE_EXPR)
    {
        if(node->exprtype==NODE_LOGICAL_OR_EXP)
        {
            
            flag=1;
            TreeNode*first_op=node->get_child(0);
            TreeNode*seconde_op=node->get_child(2);
            if(node->label.true_label==nullptr)
            {
                node->label.true_label=new string("expr_or_true_label"+to_string(node->nodeID)+":");
            }
            if(node->label.false_label==nullptr)
            {
                node->label.false_label=new string("expr_or_false_label"+to_string(node->nodeID)+":");
            }
            first_op->label.true_label=node->label.true_label;
            first_op->label.false_label=new string("E1_false_label"+to_string(first_op->nodeID)+":");
            seconde_op->label.true_label=node->label.true_label;
            seconde_op->label.false_label=node->label.false_label;
        }
        else if(node->exprtype==NODE_LOGICAL_AND_EXP)
        {
            flag=1;
            TreeNode*first_op=node->get_child(0);
            TreeNode*seconde_op=node->get_child(2);
            if(node->label.true_label==nullptr)
            {
                node->label.true_label=new string("expr_and_true_label"+to_string(node->nodeID)+":");
            }
            if(node->label.false_label==nullptr)
            {
                node->label.false_label=new string("expr_and_false_label"+to_string(node->nodeID)+":");
            }
            first_op->label.true_label=new string("E1_true_label"+to_string(first_op->nodeID)+":");
            first_op->label.false_label=node->label.false_label;
            seconde_op->label.true_label=node->label.true_label;
            seconde_op->label.false_label=node->label.false_label;
        }
        else if(node->exprtype==NODE_UNARY_EXP&&node->get_child(0)->optype==OP_UNARY_NOT)
        {
            flag=1;
            TreeNode*E1=node->get_child(1);
            if(node->label.true_label==nullptr)
            {
                node->label.true_label=new string("expr_and_true_label"+to_string(node->nodeID)+":");
            }
            if(node->label.false_label==nullptr)
            {
                node->label.false_label=new string("expr_and_false_label"+to_string(node->nodeID)+":");
            }
            E1->label.true_label=node->label.false_label;
            E1->label.false_label=node->label.true_label;
        }
    }
    if(flag)
    {
        TreeNode *tmp = node->child;
        while (tmp != nullptr)
        {
            gen_label(tmp);
            tmp = tmp->sibling;
        }
    }
}
int find_str(list<string*>*str_list,string*str)
{
    if(str_list->empty())
        return 0;
    for(list<string*>::iterator i = str_list->begin();i!=str_list->end();i++)
    {
        if(**i==*str)
        {
            return 1;
        }
    }
    return 0;
}
void TreeNode:: print_label(TreeNode*root,list<string*>*str_list)
{
    /*if(root==nullptr)
        return;
    if(root->label.begin_label!=nullptr&&!find_str(str_list,root->label.begin_label))
    {
        cout<<*root->label.begin_label<<endl<<endl;
        str_list->push_back(root->label.begin_label);
    }
    TreeNode*tmp=root->child;
    if(root->code!=nullptr)
        root->print_code();
    while(tmp!=nullptr)
    {
        print_label(tmp,str_list);
        tmp=tmp->sibling;
    }
    if(root->label.next_label!=nullptr&&!find_str(str_list,root->label.next_label))
    {
        cout<<*root->label.next_label<<endl<<endl;
        str_list->push_back(root->label.next_label);
    }*/
    if(root->nodeType==NODE_PROG)
    {
        root->print_code();
        root->print_label(root->child,str_list);
    }
        
    else if(root->nodeType==NODE_MAIN)
    {
        root->print_code();
        TreeNode*tmp=root->child;
        while(tmp!=nullptr)
        {
            if(tmp->code!=nullptr)
                tmp->print_code();
            tmp=tmp->sibling;
        }
    }
}

void  TreeNode:: allocate_stack_space(TreeNode*node)//分配栈空间
{
    switch (node->nodeType)
    {
    case NODE_PROG:{//prog主要是存汇编的头
        node->code=new string("    .bss\n    .align 4\n    .section    .rodata\n");
        break;
    }
    case NODE_MAIN:{//main存main函数的开头
        node->code=new string("    .text\n    .global main\n    .type main, @function\n");
        *node->code+=("main:\n    pushl %ebp\n    movl %esp, %ebp\n");
        //*node->code+=("    subl    $"+to_string(node->layer_node->root->total_count+4)+", %esp\n");
        break;
    }
    case NODE_DECL_STMT:{
        if(node->child_num()==3)//带初值的赋值语句
        {
            TreeNode*child0=node->get_child(0);
            TreeNode*var=node->get_child(1);
            TreeNode*value=node->get_child(2);
            //如果是interger则在栈中分配空间
            switch(child0->type->type)//当前只有int和char
            {
                case VALUE_INT:{
                    //遍历整个符号表，找到该变量所在的定义位置
                    Item* symbol_item=get_symbol_item(var->var_name, node->layer_node->root);
                    if(symbol_item==nullptr)
                    {
                        cout<<"wrong when try to get a symbol\n";
                        break;
                    }
                    var->type=TYPE_INT;
                    var->item=symbol_item;
                    //首先标明这个变量在栈中的位置
                    symbol_item->stack_count=node->layer_node->root->total_count;
                    //栈中的临时栈顶
                    node->layer_node->root->total_count+=4;
                    //更新last_size
                    node->layer_node->root->last_size=4;
                    break;
                }
                case VALUE_CHAR:{
                    Item* symbol_item=get_symbol_item(var->var_name, node->layer_node->root);
                    if(symbol_item==nullptr)
                    {
                        cout<<"wrong when try to get a symbol\n";
                        break;
                    }
                    var->type=TYPE_CHAR;
                    var->item=symbol_item;
                    //首先标明这个变量在栈中的位置
                    symbol_item->stack_count=node->layer_node->root->total_count;
                    //栈中的临时栈顶
                    node->layer_node->root->total_count+=1;
                    //更新last_size
                    node->layer_node->root->last_size=1;
                    break;
                }
                case VALUE_STRING:{
                    Item* symbol_item=get_symbol_item(var->var_name, node->layer_node->root);
                    if(symbol_item==nullptr)
                    {
                        cout<<"wrong when try to get a symbol\n";
                        break;
                    }
                    var->str_val=value->str_val;
                    var->type=TYPE_STRING;
                    var->item=symbol_item;
                    //将这个字符串加入global_str_list;
                    global_str_list[str_count]=symbol_item;
                    //在程序最后再全局产生这个字符串变量即可
                }
                default:{
                    break;
                }
            }  
        }
        else//不带初值
        {

            TreeNode*child0=node->get_child(0);
            TreeNode*var=node->get_child(1);
            //如果是interger则在栈中分配空间
            switch(child0->type->type)//当前只有int和char
            {
                case VALUE_INT:{
                    //遍历整个符号表，找到该变量所在的定义位置
                    Item* symbol_item=get_symbol_item(var->var_name, node->layer_node->root);
                    if(symbol_item==nullptr)
                    {
                        cout<<"wrong when try to get a symbol\n";
                        break;
                    }
                    //首先标明这个变量在栈中的位置
                    symbol_item->stack_count=node->layer_node->root->total_count;
                    //栈中的临时栈顶
                    node->layer_node->root->total_count+=4;
                    //更新last_size
                    node->layer_node->root->last_size=4;
                    node->code=new string("    movl    $0, -"+to_string(symbol_item->stack_count)+"(%ebp)\n");                 
                    break;
                }
                case VALUE_CHAR:{
                    Item* symbol_item=get_symbol_item(var->var_name, node->layer_node->root);
                    if(symbol_item==nullptr)
                    {
                        cout<<"wrong when try to get a symbol\n";
                        break;
                    }
                    //首先标明这个变量在栈中的位置
                    symbol_item->stack_count=node->layer_node->root->total_count;
                    //栈中的临时栈顶
                    node->layer_node->root->total_count+=1;
                    //更新last_size
                    node->layer_node->root->last_size=1;
                    node->code=new string("    movl    $0, -"+to_string(symbol_item->stack_count)+"(%ebp)\n");
                    break;
                }
                default:{
                    break;
                }
            }  
        }
        break;
    }
    case NODE_EXPR:{
        switch(node->exprtype)//目前只对算数表达式分配临时空间
        {
            case NODE_additive_Exp:{
                //创建临时变量
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=4;
                node->layer_node->root->last_size=4;
                node->item=item;
                break;
          
            }
            case NODE_MULT_EXP:{
                //创建临时变量
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=4;
                node->layer_node->root->last_size=4;
                node->item=item;
                break;
            }
            case NODE_RELATION_LESS_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_RELATION_GREATER_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_RELATION_LESS_EQ_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_RELATION_GREATER_EQ_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_EQUALITY_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_NOT_EQUALITY_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_LOGICAL_OR_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_LOGICAL_AND_EXP:{
                Item* item=new Item;
                item->symbol_type=SYMBOL_TEMP;
                item->tree_node=node;
                item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                item->index=temp_count;
                temp_count++;
                //加入符号表
                node->layer_node->section->section_table.push_back(item);
                //在栈中分配空间目前只支持int
                item->stack_count=node->layer_node->root->total_count;
                node->layer_node->root->total_count+=1;
                node->layer_node->root->last_size=1;
                node->item=item;
                break;
            }
            case NODE_UNARY_EXP:{
                TreeNode*op=node->get_child(0);
                if(op->optype==OP_UNARY_NOT)
                {
                    Item* item=new Item;
                    item->symbol_type=SYMBOL_TEMP;
                    item->tree_node=node;
                    item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                    item->index=temp_count;
                    temp_count++;
                    //加入符号表
                    node->layer_node->section->section_table.push_back(item);
                    //在栈中分配空间目前只支持int
                    item->stack_count=node->layer_node->root->total_count;
                    node->layer_node->root->total_count+=1;
                    node->layer_node->root->last_size=1;
                    node->item=item;
                    break;
                }
                else if(op->optype==OP_SUB)
                {
                    Item* item=new Item;
                    item->symbol_type=SYMBOL_TEMP;
                    item->tree_node=node;
                    item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                    item->index=temp_count;
                    temp_count++;
                    //加入符号表
                    node->layer_node->section->section_table.push_back(item);
                    //在栈中分配空间目前只支持int
                    item->stack_count=node->layer_node->root->total_count;
                    node->layer_node->root->total_count+=4;
                    node->layer_node->root->last_size=4;
                    node->item=item;
                    break;
                }
                else if(op->optype==OP_UNARY_REFERENCE)
                {
                    TreeNode*var=node->get_child(1);
                    Item* symbol_item=get_symbol_item(var->var_name, node->layer_node->root);
                    if(symbol_item==nullptr)
                    {
                        cout<<"wrong when try to get a symbol in OP_UNARY_REFERENCE\n";
                        break;
                    }
                    Item* item=new Item;
                    item->symbol_type=SYMBOL_TEMP;
                    item->tree_node=node;
                    item->def_pos=node;//先假定这是一个定义而非引用，指向自己，后面在检查时如果有指向自己且是一个引用的则报错
                    item->index=temp_count;
                    temp_count++;
                    //加入符号表
                    node->layer_node->section->section_table.push_back(item);
                    //在栈中分配空间目前只支持int
                    item->stack_count=node->layer_node->root->total_count;
                    node->layer_node->root->total_count+=4;
                    node->layer_node->root->last_size=4;
                    node->item=item;
                    break;

                }
            }
        }
        break;
    }
    case NODE_VAR:{//使引用变量指向它所定义的那个符号的栈空间
        Item* symbol_item=get_symbol_item(node->var_name, node->layer_node->root);
        node->item=symbol_item;
        break;
    }
    default:{
        //cout<<"gen code shouldn't be here\n";
        break;
    }

    }
    TreeNode*tmp=node->child;
    while(tmp!=nullptr)
    {
        tmp->allocate_stack_space(tmp);
        tmp=tmp->sibling;
    }
}


string TreeNode:: gen_ASM_code(TreeNode*node)//产生全部汇编码
{
    switch(node->nodeType)
    {
        case NODE_PROG:{
            TreeNode*tmp=node->child;
            while(tmp!=nullptr)
            {
                tmp->gen_ASM_code(tmp);
                tmp=tmp->sibling;
            }
            break;
        }
        case NODE_MAIN:{
            TreeNode*tmp=node->child;//在整个程序的层面上
            while(tmp!=nullptr)
            {
                tmp->gen_ASM_code(tmp);
                tmp=tmp->sibling;
            }
            break;
        }
        case NODE_BLOCK_FLAG:{
            TreeNode*tmp=node->child;
            while(tmp!=nullptr)
            {
                tmp->gen_ASM_code(tmp);
                tmp=tmp->sibling;
            }
            break;
        }
        case NODE_DECL_STMT:{//变量声明
            if(node->code!=nullptr)
                break;
            if(node->child_num()==3){
                if(node->get_child(0)->type->type==TYPE_INT->type)//int
                {
                    //node->code= child3->code+赋值
                    TreeNode*var=node->get_child(1);
                    TreeNode*value=node->get_child(2);
                    if(value->nodeType==NODE_CONST)
                    {
                        /*
                        movl value  var
                        */
                        node->code=new string("");
                        *node->code+="    movl    "+get_location_or_value(value)+",-"+to_string(var->item->stack_count)+"(%ebp)\n";
                    }
                    else
                    {
                        node->code=new string("");
                        *node->code+=get_code(value);
                        *node->code+="    xorl    %ebx, %ebx\n";
                        *node->code+="    movl    -"+to_string(value->item->stack_count)+"(%ebp), %ebx\n"+"    movl    %ebx, -"+to_string(var->item->stack_count)+"(%ebp)\n";
                    }
                }
                if(node->get_child(0)->type->type==TYPE_CHAR->type)//char
                {
                    //与int相比，需要加入值的限制
                    TreeNode*var=node->get_child(1);
                    TreeNode*value=node->get_child(2);
                    if(value->nodeType==NODE_CONST)
                    {
                        /*
                        movl value  var
                        */
                        node->code=new string("");
                        //限制值
                        while(value->int_val>=128)
                            value->int_val-=256;
                        while(value->int_val<=-129)
                            value->int_val+=256;
                        *node->code+="    movb    "+get_location_or_value(value)+",-"+to_string(var->item->stack_count)+"(%ebp)\n";
                    }
                    else
                    {
                        //在这种情况下还未支持对值的限制
                        node->code=new string("");
                        *node->code+=get_code(value);
                        *node->code+="    xorl    %ebx, %ebx\n";
                        *node->code+="    movb    -"+to_string(value->item->stack_count)+"(%ebp), %bl\n"+"    movb    %bl, -"+to_string(var->item->stack_count)+"(%ebp)\n";
                    }
                }
            }
            else{/*不带初值的汇编码在分配栈空间的时候已经产生*/}
            break;
        }
        case NODE_ASSIGN_EXPR:{//赋值语句= += -= ++ --
            if(node->code!=nullptr)
                break;
            node->code=new string("");
            if(node->child_num()==3)
            {
                TreeNode*var=node->get_child(0);
                TreeNode*op=node->get_child(1);
                TreeNode*value=node->get_child(2);
                
                switch(op->optype)
                {
                    case OP_ASSIGN_EQ:{// =
                        /*
                        movl value eax
                        movl eax var
                        */
                    if(var->type->type==TYPE_INT->type)
                    {
                            *node->code+=get_code(value);
                            *node->code+="    xorl    %eax, %eax\n";
                            *node->code+="    movl    "+get_location_or_value(value)+", %eax\n";
                            *node->code+="    movl    %eax, "+get_location_or_value(var)+"\n";
                    }
                    else if(var->type->type==TYPE_CHAR->type)
                    {
                            *node->code+=get_code(value);
                            *node->code+="    xorl    %eax, %eax\n";
                            *node->code+="    movb    "+get_location_or_value(value)+", %al\n";
                            *node->code+="    movb    %al, "+get_location_or_value(var)+"\n";
                    }
                        break;
                    }
                    case OP_ADD_EQ:{// +=
                        /*
                        movl var eax
                        movl value ebx
                        addl eax ebx
                        movl eax var
                        */
                    if(var->type->type==TYPE_INT->type)
                    {
                            *node->code+=get_code(value);
                            *node->code+="    xorl    %eax, %eax\n    xorl    %ebx, %ebx\n";
                            *node->code+="    movl    "+get_location_or_value(var)+", %eax\n";
                            *node->code+="    movl    "+get_location_or_value(value)+", %ebx\n";
                            *node->code+="    addl    %eax, %ebx\n";
                            *node->code+="    movl    %ebx, "+get_location_or_value(var)+"\n";
                    }
                    else if(var->type->type==TYPE_CHAR->type)
                    {
                            *node->code+=get_code(value);
                            *node->code+="    xorl    %eax, %eax\n    xorl    %ebx, %ebx\n";
                            *node->code+="    movb    "+get_location_or_value(var)+", %al\n";
                            *node->code+="    movb    "+get_location_or_value(value)+", %bl\n";
                            *node->code+="    addb    %al, %bl\n";
                            *node->code+="    movb    %bl, "+get_location_or_value(var)+"\n";
                    }
                        break;
                    }
                    case OP_SUB_EQ:{// -=
                        /*
                        movl var eax
                        movl value ebx
                        subl eax ebx
                        movl eax var
                        */
                    if(var->type->type==TYPE_INT->type)
                    {
                            *node->code+=get_code(value);
                            *node->code+="    xorl    %eax, %eax\n    xorl    %ebx, %ebx\n";
                            *node->code+="    movl    "+get_location_or_value(var)+", %eax\n";
                            *node->code+="    movl    "+get_location_or_value(value)+", %ebx\n";
                            *node->code+="    subl    %ebx, %eax\n";
                            *node->code+="    movl    %eax, "+get_location_or_value(var)+"\n";
                    }
                    else if(var->type->type==TYPE_CHAR->type)
                    {
                            *node->code+=get_code(value);
                            *node->code+="    xorl    %eax, %eax\n    xorl    %ebx, %ebx\n";
                            *node->code+="    movb    "+get_location_or_value(var)+", %al\n";
                            *node->code+="    movb    "+get_location_or_value(value)+", %bl\n";
                            *node->code+="    subb    %bl, %al\n";
                            *node->code+="    movb    %al, "+get_location_or_value(var)+"\n";
                    }
                    break;
                    }
                }
            }
            else if(node->child_num()==2)
            {
                TreeNode*var=node->get_child(0);
                TreeNode*op=node->get_child(1);
                if(op->optype==OP_DOUBLE_ADD)
                {
                    *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(var)+", %eax\n";
                    *node->code+="    addl    $1, %eax\n";
                    *node->code+="    movl    %eax, "+get_location_or_value(var)+"\n";
                }
                else if(op->optype==OP_DOUBLE_SUB)
                {
                    *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(var)+", %eax\n";
                    *node->code+="    subl    $1, %eax\n";
                    *node->code+="    movl    %eax, "+get_location_or_value(var)+"\n";
                }
            }
            
            break;
        }
        case NODE_FUNCTION_CALL:{//函数调用
            /*if(node->code!=nullptr)
                break;*/
            node->code=new string("");
            if(node->child_num()==2)//带参数
            {
                TreeNode*id_list=node->get_child(1);
                TreeNode*func_name=node->get_child(0);
                //先产生压入参数的汇编
                //先定义一个所有参数组成的列表
                list<TreeNode*>*arg_list=new list<TreeNode*>;
                //遍历id_list
                get_all_arg(id_list,arg_list);
                //参数入栈
                int temp_stack_count=0;//临时堆栈计数
                for(list<TreeNode*>::iterator i=arg_list->begin();i!=arg_list->end();i++)
                {
                    if(((*i)->nodeType==NODE_CONST||(*i)->nodeType==NODE_VAR)&&(*i)->type->type==TYPE_INT->type)
                    {
                        *node->code+="    pushl    "+get_location_or_value(*i)+"\n";
                        temp_stack_count+=4;
                    }
                    if(((*i)->nodeType==NODE_CONST||(*i)->nodeType==NODE_VAR)&&(*i)->type->type==TYPE_CHAR->type)
                    {
                        *node->code+="    pushl    "+get_location_or_value(*i)+"\n";
                        temp_stack_count+=4;
                    }
                    if((*i)->nodeType==NODE_CONST&&(*i)->type->type==TYPE_STRING->type)
                    {
                        //如果是string则将其先存在全局变量中再调用
                        Item* item=new Item;
                        item->symbol_type=SYMBOL_TEMP;
                        item->tree_node=(*i);
                        item->def_pos=(*i);
                        //加入符号表
                        (*i)->layer_node->section->section_table.push_back(item);
                        //加入global_str_list
                        global_str_list[str_count]=item;
                        str_count++;
                        *node->code+="    pushl    $STR"+to_string(str_count-1)+"\n";
                        temp_stack_count+=4;
                    }
                    if((*i)->nodeType==NODE_EXPR)
                    {
                        if((*i)->exprtype==NODE_additive_Exp||(*i)->exprtype==NODE_MULT_EXP||(*i)->exprtype==NODE_UNARY_EXP)
                        {
                            *node->code+=get_code(*i);//先产生子表达式的代码
                            *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(*i)+", %eax\n";
                            *node->code+="    pushl    %eax\n";
                            temp_stack_count+=4;
                        }
                    }
                }
                *node->code+="    call    "+func_name->var_name+"\n";
                *node->code+="    addl    $"+to_string(temp_stack_count)+" ,%esp\n";
            }
            break;
        }
        case NODE_SELECTION_STMT:{
            if(node->code!=nullptr)
                break;
            TreeNode*condition=node->get_child(0);
            node->code=new string("");
            *node->code+=*condition->code;
            *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(condition)+" , %al\n";
            *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
            *node->code+="    cmpb    %al, %cl\n";
            *node->code+="    je    NODE_SELECTION_STMT_false"+to_string(node->nodeID)+"\n";
            if(node->child_num()==2)
            {
                node->gen_ASM_code(node->get_child(1));
            }
            else
            {
                node->gen_ASM_code(node->get_child(1));
                node->gen_ASM_code(node->get_child(2));
            }
            break;
        }
        case NDOE_ITERATION_STMT:{
            if(node->code!=nullptr)
                break;
            node->code=new string("");
            if(node->iterationtype==ITERATION_WHILE)
            {
                TreeNode*condition=node->get_child(0);
                TreeNode*body=node->get_child(1);
                *node->code+="ITERATION_WHILE_LOOP"+to_string(node->nodeID)+":\n";
                *node->code+=*condition->code;
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(condition)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    ITERATION_WHILE_LOOP_NEXT"+to_string(node->nodeID)+"\n";
                //下面应该是循环体，在打印代码时拼接
                node->gen_ASM_code(body);
            }
            else if(node->iterationtype==ITERATION_FOR_EEE)
            {
                TreeNode*condition=node->get_child(1);
                TreeNode*after=node->get_child(2);
                TreeNode*body=node->get_child(3);
                //先产生最开始的语句
                TreeNode*first=node->get_child(0);
                node->gen_ASM_code(first);
                //把最开始的语句加入这个节点的代码中，之后在完成整个汇编时不再加入即可
                *node->code+=*first->code;
                *node->code+="ITERATION_FOR_EEE_LOOP"+to_string(node->nodeID)+":\n";
                *node->code+=*condition->code;
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(condition)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    ITERATION_FOR_EEE_LOOP_NEXT"+to_string(node->nodeID)+"\n";
                //产生循环体和after的语句，最后生成时加上
                node->gen_ASM_code(body);
                node->gen_ASM_code(after);
            }
            else if(node->iterationtype==ITERATION_FOR_EE_)
            {
                TreeNode*condition=node->get_child(1);
                TreeNode*first=node->get_child(0);
                TreeNode*body=node->get_child(2);
                //先产生最开始的语句
                node->gen_ASM_code(first);
                //把最开始的语句加入这个节点的代码中，之后在完成整个汇编时不再加入即可
                *node->code+=*first->code;
                *node->code+="ITERATION_FOR_EE__LOOP"+to_string(node->nodeID)+":\n";
                *node->code+=*condition->code;
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(condition)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    ITERATION_FOR_EE__LOOP_NEXT"+to_string(node->nodeID)+"\n";
                //产生循环体的语句
                node->gen_ASM_code(body);
            }
            else if(node->iterationtype==ITERATION_FOR_E_E)
            {
                TreeNode*first=node->get_child(0);
                TreeNode*after=node->get_child(1);
                TreeNode*body=node->get_child(2);
                //先产生最开始的语句
                node->gen_ASM_code(first);
                //把最开始的语句加入这个节点的代码中，之后在完成整个汇编时不再加入即可
                *node->code+=*first->code;
                *node->code+="ITERATION_FOR_E_E_LOOP"+to_string(node->nodeID)+":\n";
                //没有condition直接产生循环体和after
                node->gen_ASM_code(body);
                node->gen_ASM_code(after);
            }
            else if(node->iterationtype==ITERATION_FOR_E__)
            {
                TreeNode*first=node->get_child(0);
                TreeNode*body=node->get_child(1);
                //先产生最开始的语句
                node->gen_ASM_code(first);
                //把最开始的语句加入这个节点的代码中，之后在完成整个汇编时不再加入即可
                *node->code+=*first->code;
                *node->code+="ITERATION_FOR_E__LOOP"+to_string(node->nodeID)+":\n";
                //没有condition直接产生body
                node->gen_ASM_code(body);
            }
            else if(node->iterationtype==ITERATION_FOR__EE)
            {
                TreeNode*condition=node->get_child(0);
                TreeNode*after=node->get_child(1);
                TreeNode*body=node->get_child(2);
                *node->code+="ITERATION_FOR__EE_LOOP"+to_string(node->nodeID)+":\n";
                *node->code+=*condition->code;
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(condition)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    ITERATION_FOR__EE_LOOP_NEXT"+to_string(node->nodeID)+"\n";
                node->gen_ASM_code(body);
                node->gen_ASM_code(after);
            }
            else if(node->iterationtype==ITERATION_FOR__E_)
            {
                TreeNode*condition=node->get_child(0);
                TreeNode*body=node->get_child(1);
                *node->code+="ITERATION_FOR__E__LOOP"+to_string(node->nodeID)+":\n";
                *node->code+=*condition->code;
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(condition)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    ITERATION_FOR__E__LOOP_NEXT"+to_string(node->nodeID)+"\n";
                node->gen_ASM_code(body);
            }
            else if(node->iterationtype==ITERATION_FOR___E)
            {
                TreeNode*after=node->get_child(0);
                TreeNode*body=node->get_child(1);
                *node->code+="ITERATION_FOR___E_LOOP"+to_string(node->nodeID)+":\n";
                node->gen_ASM_code(after);
                node->gen_ASM_code(body);
            }
            else if(node->iterationtype==ITERATION_FOR____)
            {
                TreeNode*body=node->get_child(0);
                *node->code+="ITERATION_FOR_____LOOP"+to_string(node->nodeID)+":\n";
                node->gen_ASM_code(body);
            }
            break;

        }
    }
    return "";
}



string opType2_String (OperatorType type)
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


string gen_expr_asm(TreeNode*node)//目前产生算数表达式的汇编码
{
    if(node->nodeType==NODE_EXPR)
    {
        switch(node->exprtype)
        {
            case NODE_additive_Exp:{//加和减
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*op=node->get_child(1);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                if(first->nodeType==NODE_CONST&&second->nodeType==NODE_CONST)
                {
                    /*
                    movl 值，node
                    */
                    int total_value=first->int_val+second->int_val;
                    *node->code+="    movl    $"+to_string(total_value)+", -"+to_string(node->item->stack_count)+"(%ebp)\n";
                }
                else
                {
                    
                    /*
                        movl first eax
                        op  second eax
                        movl eax  node
                    */
                    *node->code+=gen_expr_asm(first);
                    *node->code+=gen_expr_asm(second);
                    *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                    if(op->optype==OP_ADD)
                    {
                        *node->code+="    addl    "+get_location_or_value(second)+", %eax\n";
                    }
                    else
                    {
                        *node->code+="    subl    "+get_location_or_value(second)+", %eax\n";
                    }
                    *node->code+="    movl    %eax, "+get_location_or_value(node)+"\n";
                }
                return *node->code;
                break;
            }
            case NODE_MULT_EXP:{//乘和除
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*op=node->get_child(1);
                TreeNode*second=node->get_child(2);
                
                node->code=new string("");
                if(first->nodeType==NODE_CONST&&second->nodeType==NODE_CONST)
                {
                    /*
                    movl 值，node
                    */
                    int total_value=first->int_val*second->int_val;
                    *node->code+="    movl    $"+to_string(total_value)+", -"+to_string(node->item->stack_count)+"(%ebp)\n";
                }
                else
                {
                    
                    /*
                        movl first eax
                        op  second eax
                        movl eax  node
                    */
                   //先产生两个字表达式的代码才能正确运算
                    *node->code+=gen_expr_asm(first);
                    *node->code+=gen_expr_asm(second);
                    
                    if(op->optype==OP_MULT)
                    {
                        
                        //如果oprd是32位，则将其与eax相乘。将结果送到edx含高32位，eax含低32位
                        *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";//先移第一个操作数到eax
                        *node->code+="    xorl    %ebx, %ebx\n    movl    "+get_location_or_value(second)+", %ebx\n";
                        *node->code+="    mull    %ebx\n";
                        *node->code+="    movl    %eax, "+get_location_or_value(node)+"\n";//现在就只把低32位传给结果
                    }
                    else if(op->optype==OP_DIV)
                    {
                        /*
                        如果oprd是32位
                        则edx含有被除数高32位，eax含有被除数低32位
                        结果的商送到eax，结果的余数送到edx中
                        */
                        *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";//被除数
                        *node->code+="    xorl    %ebx, %ebx\n    movl    "+get_location_or_value(second)+", %ebx\n";//除数
                        *node->code+="    xorl    %edx, %edx\n";
                        *node->code+="    divl    %ebx\n";
                        *node->code+="    movl    %eax, "+get_location_or_value(node)+"\n";//现在就只把低32位传给结果
                    }
                    else if(op->optype==OP_MOD)
                    {
                        *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";//被除数
                        *node->code+="    xorl    %ebx, %ebx\n    movl    "+get_location_or_value(second)+", %ebx\n";//除数
                        *node->code+="    xorl    %edx, %edx\n";
                        *node->code+="    divl    %ebx\n";
                        *node->code+="    movl    %edx, "+get_location_or_value(node)+"\n";//现在就只把低32位传给结果
                    }
                    
                }
                return *node->code;
                break;
            }
            case NODE_RELATION_LESS_EXP:{// <
                //对于真假值的表达式，这个表达式的值只有1或0
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                *node->code+="    xorl    %ecx, %ecx\n    movl    "+get_location_or_value(second)+", %ecx\n";
                *node->code+="    cmpl    %ecx, %eax\n";
                *node->code+="    jl    NODE_RELATION_LESS_EXP_true"+to_string(first->nodeID)+"\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_RELATION_LESS_EXP_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_RELATION_LESS_EXP_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_RELATION_LESS_EXP_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_RELATION_GREATER_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                *node->code+="    xorl    %ecx, %ecx\n    movl    "+get_location_or_value(second)+", %ecx\n";
                *node->code+="    cmpl    %ecx, %eax\n";
                *node->code+="    jg    NODE_RELATION_GREATER_EXP_true"+to_string(first->nodeID)+"\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_RELATION_GREATER_EXP_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_RELATION_GREATER_EXP_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_RELATION_GREATER_EXP_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_RELATION_LESS_EQ_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                *node->code+="    xorl    %ecx, %ecx\n    movl    "+get_location_or_value(second)+", %ecx\n";
                *node->code+="    cmpl    %ecx, %eax\n";
                *node->code+="    jle    NODE_RELATION_LESS_EQ_true"+to_string(first->nodeID)+"\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_RELATION_LESS_EQ_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_RELATION_LESS_EQ_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_RELATION_LESS_EQ_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_RELATION_GREATER_EQ_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                *node->code+="    xorl    %ecx, %ecx\n    movl    "+get_location_or_value(second)+", %ecx\n";
                *node->code+="    cmpl    %ecx, %eax\n";
                *node->code+="    jge    NODE_RELATION_GREATER_EQ_true"+to_string(first->nodeID)+"\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_RELATION_GREATER_EQ_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_RELATION_GREATER_EQ_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_RELATION_GREATER_EQ_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_EQUALITY_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                *node->code+="    xorl    %ecx, %ecx\n    movl    "+get_location_or_value(second)+", %ecx\n";
                *node->code+="    cmpl    %eax, %ecx\n";
                *node->code+="    je    NODE_EQUALITY_EXP_true"+to_string(first->nodeID)+"\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_EQUALITY_EXP_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_EQUALITY_EXP_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_EQUALITY_EXP_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_NOT_EQUALITY_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+", %eax\n";
                *node->code+="    xorl    %ecx, %ecx\n    movl    "+get_location_or_value(second)+", %ecx\n";
                *node->code+="    cmpl    %eax, %ecx\n";
                *node->code+="    jne    NODE_NOT_EQUALITY_EXP_true"+to_string(first->nodeID)+"\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_NOT_EQUALITY_EXP_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_NOT_EQUALITY_EXP_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_NOT_EQUALITY_EXP_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_LOGICAL_OR_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(first)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $1, %cl\n";
                *node->code+="    cmpb    %cl, %al\n";
                *node->code+="    jge    NODE_LOGICAL_OR_EXP_true"+to_string(first->nodeID)+"\n";//为真直接跳出
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(second)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $1, %cl\n";
                *node->code+="    cmpb    %cl, %al\n";
                *node->code+="    jge    NODE_LOGICAL_OR_EXP_true"+to_string(first->nodeID)+"\n";//为真直接跳出
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_LOGICAL_OR_EXP_true_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_LOGICAL_OR_EXP_true"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_LOGICAL_OR_EXP_true_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_LOGICAL_AND_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*first=node->get_child(0);
                TreeNode*second=node->get_child(2);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                *node->code+=gen_expr_asm(second);
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(first)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    NODE_LOGICAL_AND_EXP_false"+to_string(first->nodeID)+"\n";//为假直接跳出
                *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(second)+", %al\n";
                *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                *node->code+="    cmpb    %al, %cl\n";
                *node->code+="    je    NODE_LOGICAL_AND_EXP_false"+to_string(first->nodeID)+"\n";//为假直接跳出
                *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                *node->code+="    jmp    NODE_LOGICAL_AND_EXP_false_next"+to_string(first->nodeID)+"\n";
                *node->code+="NODE_LOGICAL_AND_EXP_false"+to_string(first->nodeID)+":\n";
                *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                *node->code+="NODE_LOGICAL_AND_EXP_false_next"+to_string(first->nodeID)+":\n";
                break;
            }
            case NODE_UNARY_EXP:{
                if(node->code!=nullptr)
                    break;
                TreeNode*op=node->get_child(0);
                TreeNode*first=node->get_child(1);
                node->code=new string("");
                *node->code+=gen_expr_asm(first);
                if(op->optype==OP_UNARY_NOT)
                {
                    *node->code+="    xorl    %eax, %eax\n    movb    "+get_location_or_value(first)+" ,%al\n";
                    *node->code+="    xorl    %ecx, %ecx\n    movb    $0, %cl\n";
                    *node->code+="    cmpb    %al, %cl\n";
                    *node->code+="    je    NODE_UNARY_EXP_NOT"+to_string(node->nodeID)+"\n";
                    *node->code+="    movb    $0, "+get_location_or_value(node)+"\n";
                    *node->code+="    jmp    NODE_UNARY_EXP_NOT_NEXT"+to_string(node->nodeID)+"\n";
                    *node->code+="NODE_UNARY_EXP_NOT"+to_string(node->nodeID)+":\n";
                    *node->code+="    movb    $1, "+get_location_or_value(node)+"\n";
                    *node->code+="NODE_UNARY_EXP_NOT_NEXT"+to_string(node->nodeID)+":\n";
                }
                else if(op->optype==OP_SUB)
                {

                    *node->code+="    xorl    %eax, %eax\n    movl    "+get_location_or_value(first)+" ,%eax\n";
                    *node->code+="    negl    %eax\n";
                    *node->code+="    movl    %eax, "+get_location_or_value(node)+"\n";
                }
                else if(op->optype==OP_UNARY_REFERENCE)
                {
                    *node->code+="    xorl    %eax, %eax\n    leal    "+get_location_or_value(first)+" ,%eax\n";
                    *node->code+="    movl    %eax, "+get_location_or_value(node)+"\n";
                }
            }
        }
    }
    //递归
    TreeNode*tmp=node->child;//在算数表达式的层面上
    while(tmp!=nullptr)
    {
        gen_expr_asm(tmp);
        tmp=tmp->sibling;
    }
    if(node->code!=nullptr)
        return *node->code;
    return "";
}

string get_location_or_value(TreeNode* node)
{
    switch(node->nodeType)
    {
        case NODE_VAR:{
            string temp="-"+to_string(node->item->stack_count)+"(%ebp)";
            return temp;
        }
        case NODE_CONST:{
            string temp="$"+to_string(node->int_val);
            return temp;
        }
    }
    return "-"+to_string(node->item->stack_count)+"(%ebp)";;
}

void TreeNode:: print_code()
{
    if(this->nodeType==NODE_PROG)
    {
        if(this->code!=nullptr)
            cout<<*this->code;
        TreeNode*tmp=this->get_child(this->child_num()-1);
        tmp->print_code();
    }
    else if(this->nodeType==NODE_MAIN)
    {
        if(this->code!=nullptr)
            cout<<*this->code;
        TreeNode*temp=root->child;
        while(temp!=this)
        {
            temp->print_code();
            temp=temp->sibling;
        }
        TreeNode*tmp=this->child;
        while(tmp!=nullptr)
        {
            tmp->print_code();
            tmp=tmp->sibling;
        }
    }
    else if(this->nodeType==NODE_DECL_STMT)
    {
        if(this->code!=nullptr)
            cout<<*this->code;
        TreeNode*tmp=this->child;
        while(tmp!=nullptr)
        {
            tmp->print_code();
            tmp=tmp->sibling;
        }
    }
    else if(this->nodeType==NODE_ASSIGN_EXPR)
    {
        if(this->code!=nullptr)
            cout<<*this->code;
        TreeNode*tmp=this->child;
        while(tmp!=nullptr)
        {
            tmp->print_code();
            tmp=tmp->sibling;
        }
    }
    else if(this->nodeType==NODE_FUNCTION_CALL)
    {
        if(this->code!=nullptr)
            cout<<*this->code;
        TreeNode*tmp=this->child;
        while(tmp!=nullptr)
        {
            tmp->print_code();
            tmp=tmp->sibling;
        }
    }
    else if(this->nodeType==NODE_SELECTION_STMT)
    {
        if(this->child_num()==2)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
     
            TreeNode*tmp=this->get_child(1);
            tmp->print_code();
            cout<<"NODE_SELECTION_STMT_false"+to_string(this->nodeID)+":\n";
            *this->code+="NODE_SELECTION_STMT_false"+to_string(this->nodeID)+":\n";
        }
        else if(this->child_num()==3)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
          
            TreeNode*true_body=this->get_child(1);
            TreeNode*false_body=this->get_child(2);
            true_body->print_code();
            cout<<"NODE_SELECTION_STMT_false"+to_string(this->nodeID)+":\n";
            *this->code+="NODE_SELECTION_STMT_false"+to_string(this->nodeID)+":\n";
            false_body->print_code();
        }
    }
    else if(this->nodeType==NDOE_ITERATION_STMT)
    {
        if(this->iterationtype==ITERATION_WHILE)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(1);
            body->print_code();
            cout<<"    jmp    ITERATION_WHILE_LOOP"+to_string(this->nodeID)+"\n";
            cout<<"ITERATION_WHILE_LOOP_NEXT"+to_string(this->nodeID)+":\n";
        }
        else if(this->iterationtype==ITERATION_FOR_EEE)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(3);
            TreeNode*after=this->get_child(2);
            body->print_code();
            after->print_code();
            cout<<"    jmp    ITERATION_FOR_EEE_LOOP"+to_string(this->nodeID)+"\n";
            cout<<"ITERATION_FOR_EEE_LOOP_NEXT"+to_string(this->nodeID)+":\n";
        }
        else if(this->iterationtype==ITERATION_FOR_EE_)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(2);
            body->print_code();
            cout<<"    jmp    ITERATION_FOR_EE__LOOP"+to_string(this->nodeID)+"\n";
            cout<<"ITERATION_FOR_EE__LOOP_NEXT"+to_string(this->nodeID)+":\n";
        }
        else if(this->iterationtype==ITERATION_FOR_E_E)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(2);
            TreeNode*after=this->get_child(1);
            body->print_code();
            after->print_code();
            cout<<"    jmp    ITERATION_FOR_E_E_LOOP"+to_string(this->nodeID)+"\n";
        }
        else if(this->iterationtype==ITERATION_FOR_E__)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(1);
            body->print_code();
            cout<<"    jmp    ITERATION_FOR_E__LOOP"+to_string(this->nodeID)+"\n";
        }
        else if(this->iterationtype==ITERATION_FOR__EE)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(2);
            TreeNode*after=this->get_child(1);
            body->print_code();
            after->print_code();
            cout<<"    jmp    ITERATION_FOR__EE_LOOP"+to_string(this->nodeID)+"\n";
            cout<<"ITERATION_FOR__EE_LOOP_NEXT"+to_string(this->nodeID)+":\n";
        }
        else if(this->iterationtype==ITERATION_FOR__E_)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(1);
            body->print_code();
            cout<<"    jmp    ITERATION_FOR__E__LOOP"+to_string(this->nodeID)+"\n";
            cout<<"ITERATION_FOR__E__LOOP_NEXT"+to_string(this->nodeID)+":\n";
        }
        else if(this->iterationtype==ITERATION_FOR___E)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*after=this->get_child(0);
            TreeNode*body=this->get_child(1);
            body->print_code();
            after->print_code();
            cout<<"    jmp    ITERATION_FOR___E_LOOP"+to_string(this->nodeID)+"\n";
        }
        else if(this->iterationtype==ITERATION_FOR____)
        {
            if(this->code!=nullptr)
                cout<<*this->code;
            TreeNode*body=this->get_child(0);
            body->print_code();
            cout<<"    jmp    ITERATION_FOR_____LOOP"+to_string(this->nodeID)+"\n";
        }
    }
    else if(this->nodeType==NODE_BLOCK_FLAG)
    {
        TreeNode*tmp=this->child;
        while(tmp!=nullptr)
        {
            tmp->print_code();
            tmp=tmp->sibling;
        }
    }
}
string get_code(TreeNode*node)
{
    if(node->code==nullptr)
        return "";
    return *node->code;
}

void get_all_arg(TreeNode* id_list,list<TreeNode*>*arg_list)
{
    if(id_list->nodeType==NODE_EXPR)
    {
        switch(id_list->exprtype)
        {
            case NODE_additive_Exp:{
                arg_list->push_back(id_list);
                break;
            }
            case NODE_MULT_EXP:{
                arg_list->push_back(id_list);
                break;
            }
            case NODE_UNARY_EXP:{
                arg_list->push_back(id_list);
                break;
            }
        }
        return;
    }
    if(id_list->nodeType==NODE_VAR||id_list->nodeType==NODE_CONST)
    {
        arg_list->push_back(id_list);
        
    }
    TreeNode*tmp=id_list->child;
    while(tmp!=nullptr)
    {
        get_all_arg(tmp,arg_list);
        tmp=tmp->sibling;
    }    
}
//