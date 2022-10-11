#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int var = 1;

void holaMundo(int signalNumber){
   if(signalNumber == 10){
    printf("Senal 10\n");
    var = 2;
   } else{
    printf("Otra senal\n");
   }
}

void noKill(int sig){
    printf(" - DONT YOU DARE\n");
}

int main(){
    signal(12, holaMundo);
    signal(10, holaMundo);
    signal(2, noKill);

    while(con == 1){
        printf("Trabajando...\n");
        sleep(1);
        
    }
    printf("Nunca llega\n");
    return 0;
}