#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabsym.h"
#include "semantics.h"
int stuff = 0;
int exists = 0;

extern struct node* pai_de_todos;

void doSemantics(struct node* no){
	tabelas = new_table("Global");
	struct node* aux=no;
    /* Create Global Symbol Table (GST) */
	if(aux!=NULL){
		aux=aux->son;
		while(aux!=NULL){
			if(strcmp(aux->token,"FuncDecl")==0){
				doFuncHeader(aux->son);
			}else if(strcmp(aux->token,"VarDecl")==0){
		        // Add corresponding values to GST, if it does not exist
				if(!doVarParDecl(aux->son,"Global",0)){
					printf("Line %d, column %d: Symbol %s already defined\n",aux->son->brother->line, aux->son->brother->col, aux->son->brother->data);
				}
		    }
			aux = aux->brother;
		}
	}

	aux=no;
    if(aux!=NULL){
		aux=aux->son;
		while(aux!=NULL){
		    if(strcmp(aux->token,"FuncDecl")==0){
				//printf("CURRENT TABLE FUNC DECL: %s\n",aux->son->son->data);
				if(strcmp(aux->son->brother->token, "FuncBody")==0)
					doFuncBody(aux->son->brother->son,aux->son->son->data);
				else{
					aux->son->brother->token= strdup("FuncBody");
				}
			}
		    aux = aux->brother; // Go to next logical node
		}
	}
	checkIfsAndFors(pai_de_todos);
}

int doVarParDecl(struct node* no,char* id_table,int is_param){
	if(no!=NULL){
		return add_to_table(tabelas, id_table, no->brother->data, str_to_type(no->token), is_param);
	}
	return 0;
}

void doFuncHeader(struct node* no){
	struct node* aux;
	current_table = no->son->data;
	//printf("CURRENT TABLE FUNC HEADER: %s\n",current_table);
	if(add_to_table(tabelas,"Global",no->son->data,5,0)){
		add_table(tabelas, no->son->data);
		if(strcmp(no->son->brother->token,"FuncParams")==0){
      add_to_table(tabelas,current_table,"return",0,0);
			//add_to_table(tabelas,current_table,strdup("return"),0,0);
			aux=no->son->brother->son;
		}else{
			add_to_table(tabelas,current_table,"return",str_to_type(no->son->brother->token),0);
      //add_to_table(tabelas,current_table,strdup("return"),str_to_type(no->son->brother->token),0);
			aux=no->son->brother->brother->son;
		}

		while(aux!=NULL){
			if(!doVarParDecl(aux->son, current_table, 1)){
				// Dá erro!
				printf("Line %d, column %d: Symbol %s already defined\n",aux->son->brother->line, aux->son->brother->col, aux->son->brother->data);
			}
			aux=aux->brother;
		}
	}else{
		
		printf("Line %d, column %d: Symbol %s already defined\n",no->son->line, no->son->col, no->son->data);
		no->brother->token= "-";
    //no->brother->token= strdup("-");
	}
}

void doFuncBody(struct node* no, char* id_table){
	struct node* aux = no;
	struct node* before; // Apenas para evitar erros
	
	while(aux!=NULL){
		if(strcmp(aux->token,"VarDecl")==0){

			if(doVarParDecl(aux->son,id_table,0) != 1){
				// gerar erro de já existir na tabela
				semantics_error = 1;
				printf("Line %d, column %d: Symbol %s already defined\n",aux->son->brother->line, aux->son->brother->col, aux->son->brother->data);
			}else{
				exists=0;
				isUsed(aux,aux->son->brother->data); // conta o número de utilizações da variável
				if(exists==0){
					if(!aux->son->brother->error){
						printf("Line %d, column %d: Symbol %s declared but never used\n",aux->son->brother->line,aux->son->brother->col,aux->son->brother->data);
						aux->son->brother->error=1;
					}
				}
			}
		}else{
			//printf("[%d] It's a %s\n", stuff++, aux->token);
			doAnnotation(before, aux,id_table, 0);
		}
		before = aux;
		aux=aux->brother;
	}

}

void doAnnotation(struct node* before, struct node* no, char* id_table, int lvl){
	// recursively do annotation
    if(no!=NULL){
		//printf("\tdoAnnotation called with table %s\n", id_table);
		if(strcmp(no->token,"VarDecl")!=0){
			if(no->son!=NULL) doAnnotation(no, no->son,id_table, 1);
			if(lvl){
				if(no->brother!=NULL) doAnnotation(no, no->brother,id_table, 1);
			}else{
				if(no->son != NULL && no->son->brother!=NULL) doAnnotation(no->son, no->son->brother,id_table, 1);
			}

		  if(strcmp(no->token,"IntLit")==0){
          no->type="int";
          //no->type=strdup("int");
				  if(!verificaOctal(no->data)){
					  if(!(no->error)){
						  printf("Line %d, column %d: Invalid octal constant: %s\n",no->line, no->col, no->data);
					  }
					  no->error = 1;
				  }

		    if(strcmp(no->token,"IntLit")==0){
          		//char *str_int = "int";
				if(verificaOctal(no->data)==0){
					if(!(no->error)){
						printf("Line %d, column %d: Invalid octal constant: %s\n",no->line, no->col, no->data);
					}
					no->error = 1;
				}
				
				no->type=strdup("int");
				
			}else if(strcmp(no->token,"RealLit")==0){
        no->type="float32";
				//no->type=strdup("float32");
			}else if(strcmp(no->token,"StrLit")==0){
				no->type="string";
        //no->type=strdup("string");
			}else if(strcmp(no->token,"ParseArgs")==0){
				if(no->son && no->son->brother){
					if(strcmp(no->son->type,"int")==0 && strcmp(no->son->brother->type,"int")==0){
            no->type=no->son->type;
						//no->type =strdup(no->son->type);
					}else{
						semantics_error = 1;
						if(!(no->error)){
							printf("Line %d, column %d: Operator strconv.Atoi cannot be applied to types %s, %s\n",no->line, no->col, no->son->type, no->son->brother->type);
							no->type="undef";
              //no->type=strdup("undef");
						}
						no->error = 1;
					}
		  		}
			}else if(strcmp(no->token,"Id")==0){
				// get id type - if type -1 on id_table, search on Global, if -1 on Global -> error
				// if is_function (return_type) else type
				//printf("In id for: %s in table %s\n", no->data, id_table);
				tables* tmp = search_table (tabelas, id_table);
				//printf("Found table -> %s for symbol %s on line %d and column %d\n", tmp->id, no->data, no->line, no->col);
				int type;
				if(tmp!=NULL){
					type = search_symbol(tmp->st, no->data);
					//printf("\tType is %d\n", type);
					if(type==5){
            no->type=type_to_str(search_symbol(search_table(tabelas, id_table)->st,"return"));
						//no->type=strdup(type_to_str(search_symbol(search_table(tabelas, id_table)->st,"return")));
					}else if(type==-1){
						tmp = search_table (tabelas, "Global");
						//printf("\t\tIts the table %s\n", tmp->id);
						if(tmp!=NULL){
							type = search_symbol(tmp->st, no->data);
							//printf("\t\tAnd has type: %d, no->data %s\n", type, no->data);
							if(type==5){
                no->type=type_to_str(search_symbol(search_table(tabelas, no->data)->st,"return"));
								//no->type=strdup(type_to_str(search_symbol(search_table(tabelas, no->data)->st,"return")));
							}else if(type==-1){
								if(!(no->error)){
                  no->type="undef";
									//no->type=strdup("undef");
									semantics_error = 1;
									if(strcmp(before->token,"Call")==0){

										char *str = malloc(sizeof(char)*1024);
										memset(str, 0, sizeof(char)*1024);
										strcat(str,"");
										struct node *aux = no->brother;
										while(aux){
											if(aux->type!=NULL)
												strcat(str,aux->type);
											aux = aux->brother;
											if(aux)
												strcat(str,",");
										}
										printf("Line %d, column %d: Cannot find symbol %s(%s)\n",no->line, no->col, no->data, str);
                    free(str);
									}else{
										printf("Line %d, column %d: Cannot find symbol %s\n",no->line, no->col, no->data);
									}
								}
								no->error = 1;
							}else{
                no->type=type_to_str(type);
								//no->type=strdup(type_to_str(type));
							}
						}
					}else{
            no->type=type_to_str(type);
						//no->type=strdup(type_to_str(type));
					}
				}

			}else if(strcmp(no->token, "Return") == 0){
        char *type = type_to_str(search_symbol(search_table(tabelas, id_table)->st,"return"));
				//char *type = strdup(type_to_str(search_symbol(search_table(tabelas, id_table)->st,"return")));
				if(!(no->error)){
					if((no->son != NULL && strcmp(type, no->son->type) != 0)){
						printf("Line %d, column %d: Incompatible type %s in return statement\n",no->son->line, no->son->col,no->son->type);
					}else if(no->son == NULL && strcmp(type, "none") != 0){
						printf("Line %d, column %d: Incompatible type none in return statement\n",no->line,no->col);
					}
					no->error = 1;
				}
        //free(type);
			}else if(strcmp(no->token,"Eq")==0||strcmp(no->token,"Ne")==0){
				// temos que verificar se são dois iguais de forma a poder dar o erro
			  	if(no->son!=NULL && no->son->brother!=NULL){
					if(strcmp(no->son->type, no->son->brother->type) == 0 && ((strcmp(no->son->type, "string") == 0) || (strcmp(no->son->type,"bool")==0) || (strcmp(no->son->type,"int")==0) || (strcmp(no->son->type,"float32")==0))){
            no->type="bool";
						//no->type=strdup("bool");
					}else{
						if(!(no->error)){
							semantics_error = 1;
								printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
								no->type="undef";
                //no->type=strdup("undef");
						}
						no->error = 1;
					}
				}
		  }else if(strcmp(no->token,"Or")==0||strcmp(no->token,"And")==0){
			  	// temos que verificar se são dois iguais de forma a poder dar o erro
			  	if(no->son!=NULL && no->son->brother!=NULL){
					if(strcmp(no->son->type, no->son->brother->type) == 0 && (strcmp(no->son->type,"bool"))==0){
              no->type=no->son->type;
			  			//no->type=strdup("bool");
					}else{
						if(!(no->error)){
							semantics_error = 1;
							//if((strcmp(no->son->type,"undef")!=0) || (strcmp(no->son->brother->type,"undef")!=0)){
								printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
							//}
              no->type="bool";
							//no->type=strdup("bool");
						}
						no->error = 1;
					}
				}
		  }else if(strcmp(no->token,"Lt")==0||strcmp(no->token,"Gt")==0||strcmp(no->token,"Le")==0||strcmp(no->token,"Ge")==0){
				if(no->son && no->son->brother){
					if((strcmp(no->son->type, no->son->brother->type) == 0) && ((strcmp(no->son->type, "string") == 0) || (strcmp(no->son->type, "int") == 0) || (strcmp(no->son->type, "float32") == 0))){
						no->type="bool";
            //no->type = strdup("bool");
					}else{
						if(!(no->error)){
							semantics_error = 1;
							//if((strcmp(no->son->type,"undef")!=0) || (strcmp(no->son->brother->type,"undef")!=0)){
								printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
							//}
              no->type="bool";
							//no->type=strdup("bool");
						}
						no->error=1;
					}
				}
		  }else if(strcmp(no->token,"Sub")==0||strcmp(no->token,"Mul")==0||strcmp(no->token,"Div")==0){
			  	stuff++;
			  	// assume o tipo dos dois filhos se estes forem do mesmo tipo e forem int ou float32
				if(no->son && no->son->brother){
					if((strcmp(no->son->type, no->son->brother->type) == 0) && ((strcmp(no->son->type, "int") == 0) || (strcmp(no->son->type, "float32") == 0))){
						no->type=no->son->type;
            //no->type = strdup(no->son->type);
					}else{
						if(!(no->error)){
							semantics_error = 1;
							if((strcmp(no->son->type,"undef")!=0) || (strcmp(no->son->brother->type,"undef")!=0)){
								printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
							}
              no->type="undef";
							//no->type=strdup("undef");
						}
						no->error=1;
					}
				}

		 }else if(strcmp(no->token,"Add")==0){
			 
			 if(no->son && no->son->brother){
					if((strcmp(no->son->type, no->son->brother->type) == 0) && ((strcmp(no->son->type, "int") == 0) || (strcmp(no->son->type, "float32") == 0) || (strcmp(no->son->type, "string") == 0))){
						no->type=no->son->type;
            //no->type = strdup(no->son->type);
					}else{
						if(!(no->error)){
							semantics_error = 1;
							//if((strcmp(no->son->type,"undef")!=0) || (strcmp(no->son->brother->type,"undef")!=0)){
								printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
							//}
              no->type="undef";
							//no->type=strdup("undef");
						}
						no->error=1;
					}
				}

		  }else if(strcmp(no->token,"Mod")==0){
			  if(no->son && no->son->brother){
					if((strcmp(no->son->type, no->son->brother->type) == 0) && (strcmp(no->son->type, "int") == 0)){
            no->type=no->son->type;
						//no->type = strdup(no->son->type);
					}else{
						if(!(no->error)){
							semantics_error = 1;
							if((strcmp(no->son->type,"undef")!=0) || (strcmp(no->son->brother->type,"undef")!=0)){
								printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
							}
              no->type="undef";
							//no->type=strdup("undef");
						}
						no->error=1;
					}
				}
		  }else if(strcmp(no->token,"Not")==0||strcmp(no->token,"Minus")==0||strcmp(no->token,"Plus")==0){
			  	// assume o tipo do único filho
				if(no->son!=NULL){
					if((strcmp(no->token,"Not")==0) && (strcmp(no->son->type,"bool")==0))
            no->type=no->son->type;
						//no->type=strdup("bool");
					else if((strcmp(no->token,"Minus")==0||strcmp(no->token,"Plus")==0) && (strcmp(no->son->type,"int")==0||strcmp(no->son->type,"float32")==0))
						no->type=no->son->type;
            //no->type=strdup(no->son->type);
					else{
						if(!(no->error)){
							semantics_error = 1;
							if((strcmp(no->son->type,"undef")!=0)){
								printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",no->line, no->col, no->data, no->son->type);
							}
							if(strcmp(no->token,"Not")==0){
                no->type="bool";
								//no->type=strdup("bool");
							}else{
                no->type="undef";
								//no->type=strdup("undef");
							}
						}
						no->error = 1;
					}
				}

		  }else if(strcmp(no->token,"Assign")==0){
			  	
				// se tem dois filhos do mesmo tipo assume o tipo dos filhos
				
				if(no->son && no->son->brother){
					//printf("%s - %s\n",no->son->type, no->son->brother->type);
					if((strcmp(no->son->type, no->son->brother->type) == 0)){
            no->type = no->son->type;
						//no->type = strdup(no->son->type);

					}else{
						semantics_error = 1;

						if(!(no->error)){
							//if((strcmp(no->son->type,"undef")!=0) || (strcmp(no->son->brother->type,"undef")!=0)){
							printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",no->line, no->col, no->data, no->son->type, no->son->brother->type);
							  no->type="undef";
                //no->type=strdup("undef");
							//}else{
							//	no->type=strdup("undef");
							//}
						}
						no->error = 1;
					}	
				}
				

		  }else if(strcmp(no->token,"Call")==0){
				//printf("In Call for: %s\n", no->data);
			 	// assume o tipo do retorno da função
				if(no->son){
					if(no->son->type){
						if(strcmp(no->son->type,"none")==0){ // check se tem tipo de retorno, senão não atribui tipo ao Call
							no->type=NULL;
            }else{
							no->type = no->son->type; //no->type = strdup(no->son->type);
            }
					}else{
						no->type=NULL;
					}

					if(no->son->error!=1 || 1){
						struct node* n_aux = no->son->brother;
						char *str_aux = malloc(sizeof(char)*1024);
						memset(str_aux, 0, sizeof(char)*1024);
						strcat(str_aux,"");
						while(n_aux!=NULL){
							strcat(str_aux,n_aux->type);
							if(n_aux->brother)
								strcat(str_aux,",");
							n_aux=n_aux->brother;
						}

						tables* t_aux = search_table(tabelas, no->son->data);
						symtab* st_aux;
						char *str_aux2 = malloc(sizeof(char)*1024);
						memset(str_aux2, 0, sizeof(char)*1024);
						if(t_aux){
							st_aux = t_aux->st;
							strcat(str_aux2,"");
						}else{
							st_aux = NULL;
							strcat(str_aux2,"-1");
						}

						while(st_aux!=NULL){
							if(st_aux->is_param){
                char *auxaux = type_to_str(st_aux->type);
                //char *auxaux = strdup(type_to_str(st_aux->type));
								strcat(str_aux2,auxaux);
                //free(auxaux);
								if(st_aux->next)
									if(st_aux->next->is_param)
										strcat(str_aux2,",");
							}
							st_aux=st_aux->next;
						}

						if(strcmp(str_aux,str_aux2)!=0){

							if(!(no->son->error)){
								printf("Line %d, column %d: Cannot find symbol %s(%s)\n",no->son->line, no->son->col, no->son->data,str_aux);
							}
							no->son->error = 1;
						}
            free(str_aux); free(str_aux2);
					}
				}
			}

		  }
    }

}

int verificaOctal(char* intlit){
	int i = 1;
	if(intlit[0]=='0'){
		while(intlit[i]!='\0'){
			if(intlit[i]<'0' || intlit[i]>'7'){
				return 0;
			}
			i++;
		}
	}
	return 1;
}

int isUsed(struct node* n_aux,char* id){
	if(n_aux->brother!=NULL) exists=isUsed(n_aux->brother,id);
	if(strcmp(n_aux->token,"VarDecl")!=0 && strcmp(n_aux->token,"FuncHeader")!=0){
		if(n_aux->son!=NULL) exists=isUsed(n_aux->son,id);
		//printf("%s --- %s\n",n_aux->token, n_aux->data);
		if(strcmp(n_aux->token,"Id")==0 && strcmp(n_aux->data,id)==0){
			exists++;
		}
	}
	return exists;
}

void checkIfsAndFors(struct node * n_aux){
	if(n_aux!=NULL){
		checkIfsAndFors(n_aux->son);
		checkIfsAndFors(n_aux->brother);
		if(strcmp(n_aux->token,"If")==0 || strcmp(n_aux->token,"For")==0){
			if(n_aux->son != NULL && n_aux->son->type!= NULL && strcmp(n_aux->son->type,"bool")!=0 && strcmp(n_aux->son->type,"undef")!=0){
				printf("Line %d, column %d: Incompatible type %s in %s statement\n",n_aux->son->line,n_aux->son->col,n_aux->son->type,n_aux->data);
			}
		}
	}
}

void checkErrors(struct node* no){

	if(no->son) checkErrors(no->son);
}


void printAST_annoted(struct node* no,int deep){
    int deep_son=deep+1;
    if(no!=NULL){
        if(strcmp(no->token,"Braces")!=0 && strcmp(no->token,"Error")!=0){
            if(strcmp(no->token,"IntLit")==0 || strcmp(no->token,"Id")==0 || strcmp(no->token,"RealLit")==0 || strcmp(no->token,"StrLit")==0){
                for(int i=0;i<deep;i++) printf("..");
                printf("%s(%s)",no->token,no->data);
            }else if(strcmp(no->token,"Block")==0 && strcmp(no->data,"1")==0){
                if(no->son!=NULL){
                    if(num_bros_diff_ignore(no->son,0)>1){
                        for(int i=0;i<deep;i++) printf("..");
                        printf("%s",no->token);
                    }else{
                        deep_son--;
                    }
                }
            }else{
                for(int i=0;i<deep;i++) printf("..");
                printf("%s",no->token);
                deep_son=deep+1;
            }
        }
		if(no->type!=NULL){
			  tables* tab_aux = search_table(tabelas,no->data);
			  if(tab_aux!=NULL){
				  //printf(" - (%s)\n", no->type); // print do return type
				  printf(" - (");
				  print_params (tab_aux->st,0);
				  printf(")\n");
			  }else
				  printf(" - %s\n", no->type);
		}else{
			printf("\n");
		}
        if(no->son!=NULL) printAST_annoted(no->son,deep_son);
        if(no->brother!=NULL) printAST_annoted(no->brother,deep);
    }
}

