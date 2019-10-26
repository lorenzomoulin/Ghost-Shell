#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include "process_manager.h"


Process* createProcess(pid_t pid, pid_t pgid){
	Process* obj = (Process*) malloc(sizeof(Process));
	obj->pid = pid;
    obj->pgid = pgid;
	
	return obj;
}

void freeProcess(Process* item){
	free(item);
}

void printProcess(Process* item){
	if(item == NULL) printf("Processo inexistente\n");
	printf("PID:%d / GPID=%d\n", item->pid, item->pgid);
}

ProcessManager* createProcessManager(){
	return NULL;
}

ProcessManager* insertProcess(ProcessManager* lista, Process* item){
	ProcessManager* novo = (ProcessManager*) malloc(sizeof(ProcessManager));
	novo->prox = lista;
	novo->item = item;
	return novo;
}


ProcessManager* removeProcess(ProcessManager* lista, pid_t pid){
	ProcessManager* ant = NULL;
	ProcessManager* p = lista;
	while(p != NULL && p->item->pid != pid){
		ant = p;
		p = p->prox;
	}
	

	if(p == NULL){
		printf("Elemento nao encontrado\n");
		return NULL;
	}
	if(ant == NULL){
		lista = p->prox;
	}
	else{
		ant->prox = p->prox;
	}
	return lista;

}

Process* searchProcess(ProcessManager* lista, pid_t pid){
	ProcessManager* ant = NULL;
	ProcessManager* p = lista;
	while(p != NULL && p->item->pid != pid){
		ant = p;
		p = p->prox;
	}
	
	if(p == NULL){
		printf("Elemento nao encontrado\n");
		return NULL;
	}
	return p->item;

}
void printProcessManager(ProcessManager* lista){
	if(lista == NULL){
		printf("Nenhum processo!\n");
		return;
	}
	
	printf("*******************************\n");
	printf("Lista de Processos:\n\n");
	while(lista != NULL){
		printProcess(lista->item);
		lista = lista->prox;
	}
	printf("*******************************\n");
}

int isEmpty(ProcessManager* lista){
	return (lista==NULL);
}

void freeProcessManager(ProcessManager* lista){
	ProcessManager* aux = lista;
	signal(SIGCHLD,SIG_DFL);
	while(lista != NULL){
		aux = lista;
		lista = lista->prox;
		kill(-(aux->item->pgid), SIGKILL);
		freeProcess(aux->item);
		free(aux);
	}
	exit(0);
}

void myWait(){
	int x;
	while ((x = waitpid(-1, NULL, WNOHANG)) > 0);
}

void searchAndSuspend(ProcessManager* lista){
 	signal(SIGTSTP, SIG_IGN);
	while(lista!=NULL){
		ProcessManager* aux = lista;
		lista = lista->prox;
		//signal(SIGTSTP, SIG_DFL);
		kill(-(aux->item->pgid), SIGTSTP);
	}
	//signal(SIGTSTP, sigtstp_handler);
}

