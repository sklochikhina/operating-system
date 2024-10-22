#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int makeDir(const char* param) {
    if (mkdir(param, 0777) == -1) {
        perror("Cannot create a directory");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int readDir(char* param) {
    struct dirent** entry_list;
    int n;

    if ((n = scandir(param, &entry_list, 0, 0)) == -1) {
        perror("It's impossible to show the filling of the directory");
        return EXIT_FAILURE;
    }
    while (n--) {
        printf("%s\n", entry_list[n]->d_name);
        free(entry_list[n]);
    }
    
    free(entry_list);
    return EXIT_SUCCESS;
}

int removeDir(char* param) {
    if (rmdir(param) == -1) {
        perror("The directory cannot be removed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int createFile(char* param) {
    if (creat(param, S_IRWXU) == -1) {
        perror("Cannot create a file");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int readFile(char* param) {
    FILE* file;
    char buff;

    if ((file = fopen(param, "rb")) == NULL) {
        perror("Cannot open the file");
        return EXIT_FAILURE;
    }
    while(!feof(file)){
        fread(&buff, 1, 1, file);
        printf("%c", buff);
    }
    fclose(file);
    return EXIT_SUCCESS;
}

int removeFile(char* param) {
    if (remove(param) == -1) {
        perror("The file cannot be removed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int createSoftLink(char* param, char* name) {
    if (symlink(param, name) == -1) {
        perror("The symbolic link for the file cannot be created");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int readSoftLink(char* param) {
    errno = 0;
    char buff[4096];
    if (readlink(param, buff, 4096) == -1) {
        perror("The symbolic link cannot be read");
        return EXIT_FAILURE;
    }
    printf("%s\n", buff);
    return EXIT_SUCCESS;
}

int readSoftLinkFile(char* param) {
    readFile(param);
    return EXIT_SUCCESS;
}

int removeSoftLink(char* param) {
    if (unlink(param) == -1) {
        perror("The symbolic link cannot be removed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int createHardLink(char* param, char* name) {
    if (link(param, name) == -1) {
        perror("The hard link for the file cannot be created");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int removeHardLink(char* param) {
    removeFile(param);
    return EXIT_SUCCESS;
}

int showModeAndLinks(char* param) {
    struct stat fileStat;

    if (stat(param, &fileStat) == -1) {
        perror("It's impossible to read the file data");
        return EXIT_FAILURE;
    }

    printf("Mode of the file %s is %o and amount of hard links is %lu \n", param, fileStat.st_mode&0777, fileStat.st_nlink);

    return EXIT_SUCCESS;
}

int changeMode(char* param, char* mode) {
    if (chmod(param, strtol(mode, NULL, 8)) == -1) {
        perror("The file mode cannot be changed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./<action> <param> [mode]\n");
        exit(EXIT_FAILURE);
    }

    char* action = strrchr(argv[0], '/') + 1;
    char* param = argv[1];

    if (!strcmp(action, "make_dir"))            return makeDir(param);
    if (!strcmp(action, "read_dir"))            return readDir(param);
    if (!strcmp(action, "remove_dir"))          return removeDir(param);
    if (!strcmp(action, "create_file"))         return createFile(param);
    if (!strcmp(action, "read_file"))           return readFile(param);
    if (!strcmp(action, "remove_file"))         return removeFile(param);
    if (!strcmp(action, "read_soft_link"))      return readSoftLink(param);
    if (!strcmp(action, "read_soft_link_file")) return readSoftLinkFile(param);
    if (!strcmp(action, "remove_soft_link"))    return removeSoftLink(param);
    if (!strcmp(action, "remove_hard_link"))    return removeHardLink(param);
    if (!strcmp(action, "show_mode_and_links")) return showModeAndLinks(param);
    if (!strcmp(action, "create_hard_link") && argc == 3){
        char* name = argv[2];
        return createHardLink(param, name);
    }
    if (!strcmp(action, "create_soft_link") && argc == 3){
        char* name = argv[2];
        return createSoftLink(param, name);
    }
    if (!strcmp(action, "change_mode") && argc == 3) {
        char* mode = argv[2];
        return changeMode(param, mode);
    }
    else {
        perror("Invalid action\n");
        return EXIT_FAILURE;
    }
}