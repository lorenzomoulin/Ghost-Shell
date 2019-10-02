#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#define PROMPT printf("gsh> ");
#define NEWLINE '\n'
#define MAXNCOMM 10
#define MAXLENCOMM 100
#define MAXNARG 5
#define rep(i, a, b) for(int i = a; i < b; ++i)
char buffer[MAXNCOMM][MAXLENCOMM];

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
        while(temp[j] == ' ' && temp[j] != '\0') j++;
        strcpy(buffer[i-1], temp+j);
        printf("comando %d = %s\n", i, buffer[i-1]);     
        if (c == '\n') break;
    }
    if (!i) getchar();
    return i;
}

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


int main(){  
    while(1){
        int num = 0;
        while((num = prompt()) == 0);
        if (num == 1){
            tcommand cm = get_args(buffer[0]);
            int f1 = fork();
            if (f1 == 0)
                execvp(cm.argv[0], cm.argv);
            else
                printf("pai\n");
            rep(i, 0, cm.argc) free(cm.argv[i]);
            free(cm.argv);
        }
        else{
            //rodar os processos em background    
        }
    }
    return 0;
}
