#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH_LENGTH 1024

int getIndexOfLastSlash(const char* string, unsigned int length) {
    int result = (int) length - 1;
    while (result > -1 && string[result] != '/')
        result--;
    return result;
}

void swap(char* a, char* b) {
    char c = *a;
    *a = *b;
    *b = c;
}

void reverseString(char* str) {
    int length = (int)strlen(str);
    if (str[length - 1] == '/') {
        str[length - 1] = '\0';
        length--;
    }
    int last_slash = getIndexOfLastSlash(str, length);
    int count = length - last_slash - 1, start = last_slash + 1;

    for (unsigned int i = 0; i < count / 2; i++)
        swap(&str[start + i], &str[length - i - 1]);
}

void reverseContent(FILE* src, FILE* dest) {
    fseek(src, 0, SEEK_END);
    fseek(dest, 0, SEEK_SET);
    long pos = ftell(src);
    while (--pos >= 0) {
        fseek(src, pos, SEEK_SET);
        fputc(fgetc(src), dest);
        fseek(src, -1, SEEK_CUR);
    }
}

void reverseDirectory(const char* srcPath, const char* destPath) {
    DIR* dir = opendir(srcPath);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    mkdir(destPath, 0777);

    struct dirent* entry;

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char srcEntryPath[MAX_PATH_LENGTH];
        char destEntryPath[MAX_PATH_LENGTH];

        snprintf(srcEntryPath, MAX_PATH_LENGTH, "%s/%s", srcPath, entry->d_name);
        snprintf(destEntryPath, MAX_PATH_LENGTH, "%s/%s", destPath, entry->d_name);

        if (entry->d_type == DT_DIR) {
            reverseString(destEntryPath);
            reverseDirectory(srcEntryPath, destEntryPath);
        }
        else if (entry->d_type == DT_REG) {
            FILE* srcFile = fopen(srcEntryPath, "rb");
            if (!srcFile) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            reverseString(destEntryPath);

            FILE* destFile = fopen(destEntryPath, "wb");
            if (!destFile) {
                perror("fopen");
                fclose(srcFile);
                exit(EXIT_FAILURE);
            }

            reverseContent(srcFile, destFile);

            fclose(srcFile);
            fclose(destFile);
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* srcPath = argv[1];
    char destPath[MAX_PATH_LENGTH];

    strcpy(destPath, srcPath);
    reverseString(destPath);

    reverseDirectory(srcPath, destPath);

    return 0;
}
