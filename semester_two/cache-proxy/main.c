#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include "proxy.h"

int server_socket;
int server_is_on = 1;
Cache* cache;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf(ANSI_COLOR_RED "Received SIGINT, closing socket...\n" ANSI_COLOR_RED);
        close(server_socket);
        server_is_on = 0;

        destroy_cache(cache);
        exit(signum);
    }
}

int main() {
    signal(SIGINT, signal_handler);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = server_socket_init();
    set_params(&server_addr);
    binding_and_listening(server_socket, &server_addr);

    cache = (Cache*)malloc(sizeof(Cache));
    init_cache(cache);

    while (server_is_on) {
        printf(ANSI_COLOR_GREEN "Waiting for connection...\n" ANSI_COLOR_RESET);

        // Ожидаем клиентов
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Формируем аргументы для потока
        struct FuncArgs* args = malloc(sizeof(struct FuncArgs));
        args->client_socket = client_socket;
        args->cache = cache;

        // Запускаем поток для обработки соединения
        pthread_t tid;
        int err = pthread_create(&tid, NULL, (void* (*)(void *))handle_client_request, args);
        if (err) {
            perror("Error creating thread");
            close(server_socket);
            destroy_cache(cache);
            exit(EXIT_FAILURE);
        }
        pthread_detach(tid); // Отрываем поток, чтобы он завершился сам
    }

    close(server_socket);
    destroy_cache(cache);

    return 0;
}