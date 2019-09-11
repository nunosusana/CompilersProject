
struct _tab* tabelas;
char* current_table;
int semantics_error;

void doSemantics(struct node* no);
int doVarParDecl(struct node* no,char* id_table,int is_param);
void doFuncHeader(struct node * no);
void doFuncBody(struct node* no, char* id_table);
void doAnnotation(struct node* before, struct node* no, char* id_table, int lvl);
void printAST_annoted(struct node* no,int deep);

int verificaOctal(char* intlit);
int isUsed(struct node* n_aux, char* id);
void checkIfsAndFors(struct node * pai_de_todos);
