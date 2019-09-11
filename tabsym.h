
/*
 * type:
 * 		0 - none
 * 		1 - int
 * 		2 - real
 * 		3 - str
 *		4 - bool
 * 		5 - function
 */
typedef struct _sym{
	struct _sym* next;
	char* id;
	int type;
  int is_param;
	int found;
}symtab;

typedef struct _tab{
	char* id;
	struct _tab* next;
	symtab* st;
}tables;

symtab* new_symbol(char* id, int type, int is_param);

tables* new_table(char* id);

tables* add_table(tables* tab, char* id);

int add_to_table(tables* tab, char* id_table, char* id, int type, int is_param);

int add_symbol(tables *tab, char* id, int type, int is_param);

int search_symbol(symtab* st, char* id);

tables* search_table(tables* tab, char* id);

int compare_symbols(symtab* a, symtab* b);

void print_tables(tables* tab);

void print_symbols(tables* tab, symtab* st);

void print_params(symtab* st,int n);

char* type_to_str(int type);

int str_to_type(char* type);

void freeTabs(tables *tab);

void freeSymbols(symtab *st);
