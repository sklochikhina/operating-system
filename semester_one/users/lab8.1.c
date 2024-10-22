#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR (-1)
#define SUCCESS 0
#define SIZE 100

int main(int argc, char** argv) {
    printf("Real uid = %d\n", getuid());
    printf("Effective uid = %d\n\n", geteuid());

    if (setuid(getuid()) == ERROR) {
        perror("setuid");
        return ERROR;
    }

    FILE* file = fopen("./testfile.txt", "r");
    if (file == NULL)
        perror("fopen");

    fclose(file);

    return SUCCESS;
}