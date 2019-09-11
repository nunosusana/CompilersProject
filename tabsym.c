#include "tabsym.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

/*
 * função que cria um novo simbolo para posteriormente adicioná-lo à
 * respectiva tabela
 */
symtab* new_symbol(char* id, int type, int is_param){
	symtab* tmp = (symtab*)malloc(sizeof(symtab));
	tmp->type=type;
	tmp->id=id;
	tmp->found = 0;
	tmp->is_param=is_param;
	tmp->next=NULL;
	return tmp;
}

/*
 * função que cria um nova tabela para posteriormente adicioná-la à lista
 * de tabelas
 */
tables* new_table(char* id){
	tables* tmp = (tables*)malloc(sizeof(tables));
	tmp->id = id;
	
	tmp->st = NULL;
	tmp->next = NULL;
	return tmp;
}

/*
 * função para adicionar uma nova tabela de simbolos
 */
tables* add_table(tables* tab, char* id){
	tables *aux = tab;
	int diff = 1;
	while(aux->next && (aux = aux->next) && (diff = strcmp(id, aux->id)));
	
	if(diff)
		aux->next = new_table(id);

	return aux->next;
	//printf("Tab->next = %s\n", tab->next->name);
}

/*
 * função para adicionar um novo simbolo a uma tabela dado o id da tabela
 */
int add_to_table(tables* tab, char* id_table, char* id, int type, int is_param){
	if(tab==NULL)
		return 0;
	else{
		if(strcmp(tab->id,id_table)==0){
			return add_symbol(tab, id, type, is_param);
		}else{
			return add_to_table(tab->next, id_table, id, type, is_param);
		}
	}
	return 0;
}

/*
 * função para adicionar simbolos à tabela de simbolos
 * retorna -1 caso o simbolo já exista na tabela
 * retorna 1 caso consiga adicionar o simbolo à tabela com sucesso
 */
int add_symbol(tables *tab, char* id, int type, int is_param){
	

	if(!tab->st){
		tab->st = new_symbol(id, type, is_param);
		return 1;
	}

	symtab *aux_pred;
	symtab *aux = tab->st;

	while(aux){
		if(strcmp(aux->id, id)==0){			
			return 0;
		}
		aux_pred = aux;
		aux = aux->next;
	}

	aux_pred->next = new_symbol(id, type, is_param);
	return 1;
}

/*
 * função que procura pela existência dum simbolo e retorna
 * o type do simbolo ou -1 caso não encontre o simbolo
 */
int search_symbol(symtab* st, char* id){
	if(st!=NULL){
		if(strcmp(st->id,id)==0)
			return st->type;
		else
			return search_symbol(st->next, id);
	}
	return -1;
}

/*
 * função que procura pela existência duma tabela dado o seu id e retorna
 * a tabela ou NULL caso não encontre a tabela
 */
tables* search_table(tables* tab, char* id){
	if(tab!=NULL && id !=NULL){
		if(strcmp(tab->id,id)==0){
			return tab;
		}else{
			return search_table(tab->next, id);
		}
	}
	return NULL;
}

/*
 * função que compara dois simbolos, retorna o simbolo caso sejam compativeis
 * ou -1 caso não sejam compatíveis
 */
int compare_symbols(symtab* a, symtab* b){
	if(a->type==b->type)
		return a->type;
	else
		return -1;
}

/*
 * função para imprimir o cabeçalho de cada tabela e chamar a print_symbols
 */
void print_tables(tables* tab){
	if(tab!=NULL){
		if(strcmp(tab->id,"Global")==0){
			printf("===== %s Symbol Table =====\n",tab->id);
		}else{
			printf("===== Function %s(",tab->id);
			print_params(tab->st,0);
			printf(") Symbol Table =====\n");
		}
		print_symbols(tab,tab->st);
		printf("\n");
		print_tables(tab->next);
	}
}

/*
 * função para imprimir todos os simbolos de uma tabela
 */
void print_symbols(tables* tab, symtab* st){
	char *type;
	if(st!=NULL){
		if(st->type==5){		// se for uma função
			printf("%s\t(",st->id);
			print_params(search_table(tab,st->id)->st, 0);
      type = strdup(type_to_str(search_symbol(search_table(tab,st->id)->st,"return")));
			printf(")\t%s",type );
		}else{
			type = strdup(type_to_str(st->type));
			printf("%s\t\t%s",st->id,type);
			if(st->is_param)	// caso seja parametro
				printf("\tparam");
		}
    free(type);
		printf("\n");
		print_symbols(tab,st->next);
	}
	
}

/*
 * função para imprimir os tipos dos parâmetros, separados por vírgulas
 */
void print_params(symtab* st,int n){
	if(st!=NULL){
		if(st->is_param==1){
			if(n!=0){
        char *aux = strdup(type_to_str(st->type));
				printf(",%s",aux);
        free(aux);
				print_params(st->next,n);
			}else{
        char * aux = strdup(type_to_str(st->type));
				printf("%s",aux);
        free(aux);
				print_params(st->next,n+1);
			}
		}else{
			print_params(st->next,n);
		}
	}
}

/*
 * função para converter o type de inteiro para string
 */
char* type_to_str(int type){
	switch(type){
		case(0):
			return("none");
			break;
		case(1):
			return("int");
			break;
		case(2):
			return("float32");
			break;
		case(3):
			return("string");
			break;
		case(4):
			return("bool");
			break;
		case(5):
			return("function");
			break;
		default:
			return("undef");
			break;
	}
}

/*
 * função para converter o type de string para inteiro
 */
int str_to_type(char* type){
	if(strcmp("none",type)==0)
		return(0);
	else if(strcmp("Int",type)==0)
		return 1;
	else if(strcmp("Float32",type)==0)
		return 2;
	else if(strcmp("String",type)==0)
		return 3;
	else if(strcmp("Bool",type)==0)
		return 4;
	else if(strcmp("Function",type)==0)
		return 5;
	else
		return 6;
}

void freeTabs(tables *tab){
	if(tab){
    //if(tab->id)
		  //free(tab->id);
    freeSymbols(tab->st);
    freeTabs(tab->next);
		free(tab);
	}
}

void freeSymbols(symtab *st){
	if(st){
		freeSymbols(st->next);
    //if(st->id)
		  //free(st->id);
		free(st);
	}
}



