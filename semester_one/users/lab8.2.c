#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR (-1)
#define SUCCESS 0
#define SIZE 100

void func(FILE* file){
    printf("Real uid = %d\n", getuid());
    printf("Effective uid = %d\n\n", geteuid());

    file = fopen("./testfile.txt", "r");
    if (file == NULL)
        perror("fopen");
}

int main(int argc, char** argv) {
    FILE* file = NULL;
    func(file);

    if (setuid(getuid()) == ERROR) {
        perror("setuid");
        return ERROR;
    }

    char command[SIZE] = "cat ";
    char* buff = strcat(command, argv[1]);

    if (system(buff) != 0) return ERROR;

    fclose(file);

    return SUCCESS;
}