#ifndef PROCESS_MANAGER
#define PROCESS_MANAGER

//Estrutura de um processo que contém seu PID e PGID
typedef struct {
	pid_t pid;
    pid_t pgid;
} Process;

//Estrutura de uma lista encadeada cujos itens são processos
typedef struct process_manager {
	struct process_manager* prox;
	Process* item;
} ProcessManager;

//Cria um processo com pid e pgid passados
Process* createProcess(pid_t pid, pid_t pgid);

//Libera um processo da memória
void freeProcess(Process* item);

//Cria uma Lista Vazia (com valor null)
ProcessManager* createProcessManager();

//Insere um processo na lista de processos
ProcessManager* insertProcess(ProcessManager* lista, Process* item);

//Remove um processo da lista de processos
ProcessManager* removeProcess(ProcessManager* lista, pid_t pid);

//Procura e retorna o processo encontrado com pid passado.
//Caso o processo não exista, NULL é retornado
Process* searchProcess(ProcessManager* lista, pid_t pid);

void printProcessManager(ProcessManager* lista);

//Retorna true se a lista de processos estiver vazia
int isEmpty(ProcessManager* lista);

//Libera a lista de processos da memória
//e mata os processos existentes
void freeProcessManager(ProcessManager* lista);

//Implementa o comando interno mywait cujo objetivo
//é liberar processos descendentes da shell no estado “Zombie”
void myWait();

//Suspende os processos da lista de processos
void searchAndSuspend(ProcessManager* lista);


#endif