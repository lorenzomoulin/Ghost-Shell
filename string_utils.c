#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
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
    if (ans == 's' || ans == 'S')exit(0);
    PROMPT
}

void sigchld_handler(int signum){
    while (waitpid(-1, NULL, WNOHANG) != -1){
        printf("kill = %d\n", kill(0, SIGKILL));
    }
}

int main(){
    srandom(time(NULL));
    signal(SIGINT, sigint_handler);
    // Stop killing the process
    // signal(SIGCHLD, sigchld_handler);
    while(1){
        int num = 0;
        //printf("rolling dice...\nyou've got number %ld\n", ROLL(2));
        while((num = prompt()) == 0);
        if (num == 1){
            tcommand cm = get_args(buffer[0]);
            int f1 = fork();
            if (f1 == 0){
                execvp(cm.argv[0], cm.argv);
                exit(0);
            }
            else
                waitpid(f1, NULL, 0); // Wait for the right PID, instead of any
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
                    execvp(cmd[i].argv[0], cmd[i].argv);
                    exit(0);
                }
                else{
                    all[i] = f;
                    if (first) pgid = f, first = 0;
                    setpgid(f, pgid);
                }
            }
        }
    }
    return 0;
}
