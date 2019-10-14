#ifndef LISTA_SIMPLES
#define LISTA_SIMPLES

// typedef struct process Process;
// typedef struct process_manager ProcessManager;

typedef struct {
	pid_t pid;
    pid_t pgid;
} Process;
typedef struct process_manager {
	struct process_manager* prox;
	Process* item;
} ProcessManager;


Process* createProcess(pid_t pid, pid_t pgid);
void freeProcess(Process* item);
void printProcess(Process* item);

/* 
Cria uma Lista Vazia (com valor null)
*/
ProcessManager* createProcessManager();
ProcessManager* insertProcess(ProcessManager* lista, Process* item);
ProcessManager* removeProcess(ProcessManager* lista, pid_t pid);
Process* searchProcess(ProcessManager* lista, pid_t pid);
void printProcessManager(ProcessManager* lista);
int isEmpty(ProcessManager* lista);
void freeProcessManager(ProcessManager* lista);


#endif