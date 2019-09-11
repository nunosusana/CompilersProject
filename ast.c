#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ast.h"
#include <stdarg.h>

extern struct node* first;

struct node* addMultipleId(struct node* pack, struct node* type){
    struct node* anterior;
    struct node* root=addNode("VarDecl", 0, 0, 2,newNode(type->token, type->data, type->line, type->col),newNode(pack->token, pack->data, pack->line, pack->col));
    anterior=pack;
    pack=pack->brother;
    while(pack!=NULL){
        addBrother(root,addNode("VarDecl", 0, 0, 2,newNode(type->token, type->data, type->line, type->col),newNode(pack->token, pack->data, pack->line, pack->col)));
        anterior=pack;
        pack=pack->brother;
    }
    return root;
}

struct node* packNodes(int num, ...){
    va_list args;
    va_start(args, num);
    struct node* root = va_arg(args, struct node*);
    num--;
    while(num){
        addBrother(root, va_arg(args, struct node*));
        num--;
    }
    return root;
}

struct node* addNode(char *name, int line, int col, int num, ...){
    va_list args;
    struct node* root = newNode(name, NULL, line, col);
    va_start(args, num);

    while(num){
        addSon(root, va_arg(args, struct node*));
        num--;
    }
    return root;
}

struct node* addNodeData(char *name, char *data, int line, int col, int num, ...){
    va_list args;
    struct node* root = newNode(name, data, line, col);
    va_start(args, num);

    while(num){
        addSon(root, va_arg(args, struct node*));
        num--;
    }
    return root;
}


struct node* addNode2(struct node* root, int num, ...){
    va_list args;
    va_start(args, num);

    while(num){
        addSon(root, va_arg(args, struct node*));
        num--;
    }
    return root;
}


struct node* newNode(char* token, char* data, int line, int col){
    struct node* no = (struct node*)malloc(sizeof(struct node));
    no->token=token;
    no->data=data;
    no->line = line;
    no->col = col;
    no->error = 0;
    no->type = NULL;
    no->son = NULL;
    no->brother = NULL;
    no->next = NULL;
    return(no);
}

struct node* addSon(struct node* no, struct node* new_node){
    if(no->son==NULL){
        no->son=new_node;
        return no->son;
    }else{
        return addBrother(no->son,new_node);
    }
}

struct node* addBrother(struct node* no, struct node* new_node){
    if(no->brother==NULL){
        no->brother=new_node;
        return no->brother;
    }else
        return addBrother(no->brother, new_node);
}

struct node* addNext(struct node* no, struct node* new_node){
    if(no!=NULL && no->next==NULL){
        no->next=new_node;
        return no->next;
    }else if(no!=NULL){
        return addNext(no->next, new_node);
    }

    return no;
}

int num_bros_diff_ignore(struct node* no,int count){
    if(no!=NULL)
        if(strcmp(no->token,"Braces")!=0)
            return num_bros_diff_ignore(no->brother,count+1);
        else
            return num_bros_diff_ignore(no->brother,count);
    else
        return count;
}

void printAST(struct node* no,int deep){
    int deep_son=deep+1;
    if(no!=NULL){
        if(strcmp(no->token,"Braces")!=0 && strcmp(no->token,"Error")!=0){
            if(strcmp(no->token,"IntLit")==0 || strcmp(no->token,"Id")==0 || strcmp(no->token,"RealLit")==0 || strcmp(no->token,"StrLit")==0){
                for(int i=0;i<deep;i++) printf("..");
                printf("%s(%s)\n",no->token,no->data);
            }else if(strcmp(no->token,"Block")==0 && strcmp(no->data,"1")==0){
                if(no->son!=NULL){
                    if(num_bros_diff_ignore(no->son,0)>1){
                        for(int i=0;i<deep;i++) printf("..");
                        printf("%s\n",no->token);
                    }else{
                        deep_son--;
                    }
                }
            }else{
                for(int i=0;i<deep;i++) printf("..");
                printf("%s\n",no->token);
                deep_son=deep+1;
            }
        }
        if(no->son!=NULL) printAST(no->son,deep_son);
        if(no->brother!=NULL) printAST(no->brother,deep);
    }
}

void freeNode(struct node* no){
    if(no!=NULL){
        if(no->data!=NULL && no->token!=NULL && strcmp(no->data,"")!=0 && strcmp(no->token,"Block")!=0)
            free(no->data);
        //if(no->type!=NULL)
            //free(no->type);
        free(no);
    }
}


void freeAllNodes(struct node* no){
    if(no->next!=NULL)
        freeAllNodes(no->next);
    freeNode(no);
}



/* int main(){
    struct node *n1 = newNode("Um nó","1");
    
    struct node *n2_1 = newNode("Nó n2_1","2_1");
    struct node *n2_2 = newNode("Nó n2_2","2_2");
    
    struct node *n2 = addNode("Outro Nó", 2, n2_1, n2_2);

    struct node *root = addNode("Cenas", 2, n1, n2);

    printAST(root,0);
    return 0;
} */

