#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR 1
#define SUCCESS 0
#define SIZE 100

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return ERROR;
    }

    printf("Real uid = %d\n", getuid());
    printf("Effective uid = %d\n\n", geteuid());

    char command[SIZE] = "cat ";
    char* buff = strcat(command, argv[1]);

    if (system(buff) != 0) return ERROR;

    return SUCCESS;
}