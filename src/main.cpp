#include "common.h"
#include <fstream>

extern TreeNode *root;
extern FILE *yyin;
extern int yyparse();
extern layerNode* currentNode;
extern layerNode* layer_root;
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
    currentNode=new layerNode;
    currentNode->prev=NULL;
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
    }
    return 0;
}