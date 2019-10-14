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
#define ROLL(x) (random() % x)
#define PROMPT printf("gsh> ");
#define NEWLINE '\n'
#define MAXNCOMM 10
#define MAXLENCOMM 100
#define MAXNARG 5
#define rep(i, a, b) for(int i = a; i < b; ++i)
char buffer[MAXNCOMM][MAXLENCOMM];
pid_t all[MAXNCOMM];

typedef struct tcommand{ //tipo comando que mantém os argumentos e a sua quantidade.
    int argc;
    char **argv;
} tcommand;

ProcessManager* procList;

//retorna a quantidade de comandos lidos e armazena os comandos na matriz buffer.
int prompt(){
    int i = 0;    
    char c = 0, temp[MAXLENCOMM];
    PROMPT
    while(scanf("%[^#\n]%c", temp, &c) == 2){
        ++i;
        int j = 0;
        while(temp[j] == ' ' && temp[j] != '\0') j++; //retira espaços do começo da string
        strcpy(buffer[i-1], temp+j);
        //printf("comando %d = %s\n", i, buffer[i-1]);     
        if (c == '\n') break;
    }
    if (!i) getchar();
    return i;
}

//separa os argumentos de um comando
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
    cm.argv = malloc(pos*sizeof *cm.argv);
    rep(i, 0, pos){
        cm.argv[i] = malloc(MAXLENCOMM*sizeof *cm.argv[i]);
        strcpy(cm.argv[i], temp[i]);
    }
    return cm;
}

void sigint_handler(int signum){
    printf("\n"); PROMPT printf("Realmente deseja fechar a ghost shell? s/n - ");
    char ans;
    scanf("%c", &ans);
    if (ans == 's' || ans == 'S') exit(0);
    PROMPT
}

void sigchld_handler(int signum){
    pid_t pid;
    int   status;
    while ((pid = waitpid(-1, &status, WNOHANG)) != -1)
    {
     //   unregister_child(pid, status);   // Or whatever you need to do with the PID
         if(pid != 0){
            Process* proc = searchProcess(procList, pid);
            procList = removeProcess(procList, pid);
            printf("SIGCHLD HANDLER: ");
            printProcess(proc);
            kill(-(proc->pgid), SIGKILL);
         }
         return;  
    }
}

int main(){
    srandom(time(NULL));
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    procList = createProcessManager();

    while(1){
        int num = 0;
        //printf("rolling dice...\nyou've got number %ld\n", ROLL(2));
        while((num = prompt()) == 0);
        if (num == 1){
            tcommand cm = get_args(buffer[0]);

            if(!strcmp(cm.argv[0], "psx")) printProcessManager(procList);

            int f1 = fork();
            if (f1 == 0){
                
                signal(SIGINT, SIG_DFL); //restore original Ctrl-C behavior
                execvp(cm.argv[0], cm.argv);
                exit(0);
            }
            else{
                Process* proc = createProcess(f1, getpgid(f1));
                procList = insertProcess(procList, proc);
                // printProcessManager(procList);

                signal(SIGINT, SIG_IGN);
                waitpid(f1, NULL, 0);
                procList = removeProcess(procList, f1);
                signal(SIGINT, sigint_handler);
            }
            rep(i, 0, cm.argc) free(cm.argv[i]);
            free(cm.argv);
        }
        else{
            //rodar os processos em background    
            tcommand cmd[num];
            int first = 1;
            pid_t pgid;
            
            rep(i, 0, num){
                cmd[i] = get_args(buffer[i]);
                pid_t f = fork();
                if (!f){
                    signal(SIGINT, SIG_IGN);

                    if (first) pgid = f, first = 0;
                    setpgid(f, pgid);
                    
                    execvp(cmd[i].argv[0], cmd[i].argv);
                    exit(0);
                }
                else{
                    all[i] = f;
                    if (first) pgid = f, first = 0;
                    setpgid(f, pgid);


                    Process* proc = createProcess(f, pgid);
                    procList = insertProcess(procList, proc);
                    // printProcessManager(procList);
                }
            }
        }
    }
    return 0;
}
