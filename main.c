// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h> 
// #include <sys/types.h>

// int main(){

//   while(1){
//     printf("gsh> ");
//     char cmd[100];
//     scanf("%[^\n]%*c",cmd);
    
//     pid_t pid = fork();

//     if(pid == 0){
//       system(cmd);
//     }
//   }

//   return 0;
// }