#include "common.h"
#include <fstream>
#include <string>
extern int check_flag;
extern TreeNode *root;
extern FILE *yyin;
extern int yyparse();
extern layerNode* currentNode;
extern layerNode* layer_root;
extern list<string*>*str_list;
int temp_count;
list<ThreeAdCodeItem*> *code_list;//存储整个文件的三地址码
int str_count;//对字符串进行计数
Item* global_str_list[1000];//存储符号表中所有字符串
string gen_expr_asm(TreeNode*node);
using namespace std;
int main(int argc, char *argv[])
{
    check_flag=0;
    str_count=0;
    for(int i=0;i<1000;i++)
        global_str_list[i]=nullptr;
    temp_count=0;
    code_list=new list<ThreeAdCodeItem*>;
    if (argc == 2)
    {
        FILE *fin = fopen(argv[1], "r");
        if (fin != nullptr)
        {
            yyin = fin;
        }
        else
        {
            cerr << "failed to open file: " << argv[1] << endl;
        }
    }
    str_list=new list<string*>;
    currentNode=new layerNode;
    currentNode->prev=NULL;
    currentNode->root=currentNode;
    currentNode->total_count=4;
    currentNode->last_size=0;
    layer_root=currentNode;
    layer_root->nodeCount=0;
    layer_root->layerDesc[0]=0;
    layer_root->accessTime=0;
    currentNode->section=new SymbolTableSection;
    for(int i=1;i<layerDescNum;i++)
    {
        layer_root->layerDesc[i]=-1;
    }
    yyparse();
    if(check_flag==1)
        exit(1);
    correctSymbol(layer_root);
    if(check_section(layer_root)==1)
    {
        check_flag=1;
    }
    if(check_flag==1)
        exit(1);
    if(root != NULL) {
        root->genNodeId();
        
        //root->printAST();
        //printSymbolTable(layer_root);
        
        //cout<<"-----------------------------------------------------------------------------"<<endl;
        //root->gen_label(root);
        
        root->allocate_stack_space(root);//分配栈空间
        
  
        gen_expr_asm(root);//生成表达式的汇编
        
   
        root->gen_ASM_code(root);//生成所有汇编
         
        //产生全局字符串
        for(int i=0;global_str_list[i]!=nullptr;i++)
        {
            (*root->code)+=("STR"+to_string(i)+":\n");
            (*root->code)+=("    .string "+global_str_list[i]->tree_node->str_val+"\n");
        }
        /**/
        //找到main节点
        TreeNode*temp=root->child;
        while(temp!=nullptr&&temp->nodeType!=NODE_MAIN)
        {
            temp=temp->sibling;
        }
        *temp->code+=("    subl    $"+to_string(root->layer_node->root->total_count)+", %esp\n");
        root->print_code();
        //string end=("    addl    $"+to_string(root->layer_node->root->total_count+4)+", %esp\n    movl    $0, %eax\n    ret\n");
        //cout<<end;
    }
    return 0;
}