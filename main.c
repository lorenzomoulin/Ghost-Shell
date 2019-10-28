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
#define ROLL(x) (rand() % x) //Macro que retorna 0 ou 1 a partir de um número aleatório
#define PROMPT printf("gsh> ");
#define NEWLINE '\n'
#define MAXNCOMM 10
#define MAXLENCOMM 100
#define MAXNARG 5
#define rep(i, a, b) for(int i = a; i < b; ++i)
char buffer[MAXNCOMM][MAXLENCOMM];
pid_t all[MAXNCOMM];

//Tipo comando que mantém os argumentos e a sua quantidade.
typedef struct tcommand{ 
    int argc;
    char **argv;
} tcommand;

ProcessManager* procList;

//Retorna a quantidade de comandos lidos e armazena os comandos na matriz buffer.
int prompt(){
    int i = 0;    
    char c = 0, temp[MAXLENCOMM];
    PROMPT
    while(scanf("%[^#\n]%c", temp, &c) == 2){
        ++i;
        int j = 0;
        while(temp[j] == ' ' && temp[j] != '\0') j++; 
        strcpy(buffer[i-1], temp+j);     
        if (c == '\n') break;
    }
    if (!i) getchar();
    return i;
}

//Separa os argumentos de um comando
tcommand get_args(char *command){
    int i = 0, pos = 0, final = 0;
    char temp[MAXNARG][MAXLENCOMM];
    while(command[i] != '\0'){
        int j = 0;
        while(command[i] != '\0' && command[i] != ' ')
            temp[pos][j++] = command[i++];        
        temp[pos++][j] = '\0';
        command[i] == ' ' ? i++ : i;      
    }
    tcommand cm;
    cm.argc = pos;
    cm.argv = malloc((pos+1)*sizeof *cm.argv);

    rep(i, 0, pos){
        cm.argv[i] = malloc(MAXLENCOMM*sizeof *cm.argv[i]);
        strcpy(cm.argv[i], temp[i]); 
    }
   
    cm.argv[pos] = NULL;
    return cm;
}

//Mudando o comportamento padrão do sinal SIGINT
void sigint_handler(int signum){
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    if (isEmpty(procList)) exit(0);
    printf("\n"); PROMPT printf("Realmente deseja fechar a ghost shell? s/n - ");
    char ans;
    scanf("%c%*c", &ans);
    if (ans == 's' || ans == 'S'){
        freeProcessManager(procList);
    } 
    PROMPT
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

//Mudando o comportamento padrão do sinal SIGCHLD
void sigchld_handler(int signum){
    pid_t pid;
    int   status;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) != -1)
    {
         if(pid != 0){
            Process* proc = searchProcess(procList, pid);

            if(proc)
                procList = removeProcess(procList, pid);
            else{
                kill(pid, SIGKILL);
                return;
            }

            if (!WIFEXITED(status))
                kill(-(proc->pgid), SIGKILL);
        
         }
         return;  
    }
}

//Mudando o comportamento padrão do sinal SIGSTP
void sigtstp_handler(int signum){
    printf("\nAll running process have been stopped\n");
    searchAndSuspend(procList);
}


int main(){
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    procList = createProcessManager();

    while(1){
        int num = 0;
              
        signal(SIGTSTP,sigtstp_handler);
        while((num = prompt()) == 0);

        if (num == 1){ //Processo em foreground
            tcommand cm = get_args(buffer[0]);
            
            
            if(!strcmp(cm.argv[0], "clean&die")) freeProcessManager(procList);
            if(!strcmp(cm.argv[0], "mywait")){
                myWait();
                continue;
            }

            int f1 = fork();

            if (f1 == 0){ //Código do filho (processo de foreground)
                signal(SIGINT, SIG_IGN);
                execvp(cm.argv[0], cm.argv);
                exit(0);
            }
            else{ //Código do pai (gsh)
                Process* proc = createProcess(f1, getpgid(f1));
                procList = insertProcess(procList, proc);

                int status;
                waitpid(f1, &status, WUNTRACED);
                rep(i, 0, cm.argc+1){
                    free(cm.argv[i]);
                }
                
                free(cm.argv);
                procList = removeProcess(procList, f1);
            }
            
        }
        else{ //Processos em background 
               
            tcommand cmd[num];
            int first = 1;
            pid_t pgid;
            
            rep(i, 0, num){
                cmd[i] = get_args(buffer[i]);
                pid_t f = fork();
                
                int createdGhost;

                if (!f){ //Código do filho (processo de background)
                     srand(getpid()); //Mudando a semente do número aleatório com o PID do processo
                     createdGhost = ROLL(2); //Se ROLL(2) igual a 1, o processo cria um ghost.
                     signal(SIGINT, SIG_IGN);

                    if (first) pgid = f, first = 0;
                    setpgid(f, pgid);

                    if(createdGhost){ //Criando o ghost que executará o mesmo código do pai
                        fork();
                    }
                    
                    execvp(cmd[i].argv[0], cmd[i].argv);
                    exit(0);
                }
                else{ //Código do pai (gsh)
                    all[i] = f;
                    if (first) pgid = f, first = 0;
                    setpgid(f, pgid);


                    Process* proc = createProcess(f, pgid);
                    procList = insertProcess(procList, proc);
        
                    rep(j, 0, cmd[i].argc){
                        free(cmd[i].argv[j]);
                    }
        
                    free(cmd[i].argv);
                }
            }
        }
    }
    return 0;
}
