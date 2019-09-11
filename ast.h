struct node{
    char* data;
    char* token;
    int line, col;
    struct node *son;
    struct node *brother;
    struct node *next;
    char* type;
    int error;
};

struct node* addMultipleId(struct node* pack, struct node* type);

struct node* packNodes(int num, ...);
struct node* addNode(char *name, int line, int col, int num, ...);
struct node* addNode2(struct node*, int num, ...);
struct node* addNodeData(char *name, char *data, int line, int col, int num, ...);
struct node* newNode(char*,char*, int, int);
struct node* addSon(struct node*,struct node*);
struct node* addBrother(struct node*,struct node*);
struct node* addNext(struct node* root, struct node* new_node);
int num_bros_diff_ignore(struct node* no,int count);
void printAST(struct node*,int);
void freeNode(struct node* no);
void freeAllNodes(struct node* no);



