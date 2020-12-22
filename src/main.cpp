#include "common.h"
#include <fstream>
#include <string>

extern TreeNode *root;
extern FILE *yyin;
extern int yyparse();
extern layerNode* currentNode;
extern layerNode* layer_root;
extern list<string*>*str_list;
using namespace std;
int main(int argc, char *argv[])
{
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
    currentNode->total_count=0;
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
    if(root != NULL) {
        root->genNodeId();
        correctSymbol(layer_root);
        root->printAST();
        printSymbolTable(layer_root);
        check_section(layer_root);
        root->gen_label(root);
        root->gen_code(root);
        *root->child->code+=("    subl    $"+to_string(root->layer_node->root->total_count+4)+", %esp\n");
        root->print_label(root, str_list);
        string end=("    addl    $"+to_string(root->layer_node->root->total_count+4+4)+", %esp\n    movl    $0, %eax\n    ret");
        cout<<end;
    }
    return 0;
}