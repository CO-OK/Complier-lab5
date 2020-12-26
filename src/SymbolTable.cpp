#include "tree.h"
extern void printLayer(layerNode*node);

void copyLayerDesc(int* lay1,int* lay2)
{
    for(int i=0;i<layerDescNum;i++)
        lay1[i]=lay2[i];
}
void setProperty(SymbolTableSection* section,TreeNode* tree_node,SymbolProperty type)
{
    for(list<Item*>::iterator i=section->section_table.begin();i!=section->section_table.end();i++)
    {
        if((*i)->tree_node==tree_node)
        {
            (*i)->symbol_property=type;
        }
    }
}

void correctSymbol(layerNode*node)
{
    if(node==nullptr)
        return;
    for(list<Item*>::iterator i=node->section->section_table.begin();i!=node->section->section_table.end();)
    {
        if((*i)->tree_node->layer_node!=node)
        {
            layerNode* tmp=(*i)->tree_node->layer_node;
            tmp->section->section_table.push_back(*i);
            i=node->section->section_table.erase(i);
        }
        else
        {
            i++;
        }
    }
    for(int i=0;i<node->nodeCount;i++)
    {
        correctSymbol(node->list[i]);
    }
}
void setSymbolType(SymbolTableSection* section,TreeNode* tree_node,SymbolType type)
{
    for(list<Item*>::iterator i=section->section_table.begin();i!=section->section_table.end();i++)
    {
        if((*i)->tree_node==tree_node)
        {
            (*i)->symbol_type=type;
        }
    }
}
void printSymbolTable(layerNode*node)
{
    if(node==nullptr)
        return;
    cout<<"layer:";
    printLayer(node);
    cout<<endl;
    for(list<Item*>::iterator i=node->section->section_table.begin();i!=node->section->section_table.end();i++)
    {
        cout<<"symbol:  "<<(*i)->name<<"    ";
        cout<<"ref_or_def:  "<<SymbolProperty2String((*i)->symbol_property)<<"     ";
        cout<<"symbol_type:  "<<SymbolType2String((*i)->symbol_type)<<"    ";
        cout<<endl;
    }
    cout<<endl;
    for(int i=0;i<node->nodeCount;i++)
    {
        printSymbolTable(node->list[i]);
    }
}
string SymbolProperty2String(SymbolProperty type)
{
    switch (type)
    {
        case PROPERTY_DEF:{
            return "PROPERTY_DEF";
        }
        case PROPERTY_REFE:{
            return "PROPERTY_REFE";
        }
        default:
            return "";
    }
}
string SymbolType2String(SymbolType type)
{
    switch(type)
    {
        case SYMBOL_FUNC:{
            return "SYMBOL_FUNC";
        }
        case SYMBOL_VAR:{
            return "SYMBOL_VAR";
        }
        default:
            return "";
    }
}

int assignRefSymbolType(layerNode*node,Item*item)
{
    if(node==nullptr)
        return 0;
    for(list<Item*>::iterator i=node->section->section_table.begin();i!=node->section->section_table.end();i++)
    {
        if((*i)->name==item->name&&(*i)->symbol_property==PROPERTY_DEF&&item!=(*i)&&(*i)->symbol_type==SYMBOL_VAR)
        {
            item->tree_node->type=(*i)->tree_node->type;//先设置类型相同
            //将引用的符号指向定义的符号
            item->def_pos=(*i)->def_pos;
            return 1;
        }
    }
    if(node->is_func)//如果是在函数体中寻找，则不再向上一级寻找
    {
        for(list<Item*>::iterator i=node->section->section_table.begin();i!=node->section->section_table.end();i++)
        {
            if((*i)->name==item->name&&(*i)->symbol_property==PROPERTY_DEF&&item!=(*i))
            {
                item->tree_node->type=(*i)->tree_node->type;//先设置类型相同
                //将引用的符号指向定义的符号
                item->def_pos=(*i)->def_pos;
                return 1;
            }
        }
        return 0;
    }
    /*
        在多级符号表中有可能需要向上一级作用域寻找
    */
    return assignRefSymbolType(node->prev,item);
}

void check_symbol_table(SymbolTableSection* section)
{
    //普通符号是否存在定义前引用
    for(list<Item*>::iterator i=section->section_table.begin();i!=section->section_table.end();i++)
    {
        if((*i)->symbol_type!=SYMBOL_FUNC)
        {
            if((*i)->def_pos==(*i)->tree_node&&(*i)->symbol_property==PROPERTY_REFE)
            {
                printf("var ref before def at line %d\n",(*i)->tree_node->lineno);
            }
        }
        //符号重定义
        if((*i)->symbol_property==PROPERTY_DEF)
        {
            for(list<Item*>::iterator j = i;j!=section->section_table.end();j++)
            {
                if((*j)->symbol_property==PROPERTY_DEF&&(*j)->name==(*i)->name&&*j!=*i)
                {
                    printf("Symbol Redefining at line %d, symbol is ",(*j)->tree_node->lineno);
                    cout<<(*j)->name<<endl;
                }
            }
        }
    }
}

Item*get_symbol_item(string name,layerNode*node)//找到name对应的item
{

    for(list<Item*>::iterator i=node->section->section_table.begin();i!=node->section->section_table.end();i++)
    {
        if((*i)->name==name&&(*i)->symbol_property==PROPERTY_DEF)
        {
            return (*i);
        }
    }
    Item*tmp=nullptr;
    //寻找子层
    for(int i=0;i<node->nodeCount;i++)
    {
        if((tmp=get_symbol_item(name,node->list[i]))!=nullptr)
            return tmp;
    }
    return tmp;
}

string get_stack_count(Item* item)
{
    if(item==nullptr)
        return "null";
    return to_string(item->stack_count);
}