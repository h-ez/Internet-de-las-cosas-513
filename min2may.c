#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

int main(){
    char c;
    char may;
    int n = 0;
    int vocal_cont = 0;

    do{
        n = read(STDIN_FILENO, &c, 1);
        may = toupper(c);

        if(may == 'A' || may == 'E' || may == 'I' || may == 'I' || may == 'U' || may == 'O'){
            vocal_cont += 1;
        }

        write(STDOUT_FILENO, &may, 1);

    }while(n != 0);

    printf("Numero de vocales: ");
    printf("%d", vocal_cont);

    return 0;
}