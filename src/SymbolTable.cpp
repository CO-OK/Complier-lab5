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