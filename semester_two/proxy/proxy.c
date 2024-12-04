#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define PORT 8080 // Порт прокси
#define BUFFER_SIZE 8192
#define CACHE_SIZE 100

// Структура для кэшированных данных
typedef struct cache_entry {
    char* url;
    char* data;
    size_t size;
    struct cache_entry* next;
} cache_entry_t;

cache_entry_t* cache = NULL;
pthread_mutex_t cache_lock;

void cache_store(char* url, char* data, size_t size) {
    pthread_mutex_lock(&cache_lock);

    // Сохраняем данные в кэш
    cache_entry_t* new_entry = malloc(sizeof(cache_entry_t));
    new_entry->url = strdup(url);
    new_entry->data = malloc(size);
    memcpy(new_entry->data, data, size);
    new_entry->size = size;
    new_entry->next = cache;
    cache = new_entry;

    pthread_mutex_unlock(&cache_lock);
}

cache_entry_t* cache_find(char* url) {
    pthread_mutex_lock(&cache_lock);
    cache_entry_t* entry = cache;

    while (entry != NULL) {
        if (strcmp(entry->url, url) == 0) {
            pthread_mutex_unlock(&cache_lock);
            return entry;
        }
        entry = entry->next;
    }

    pthread_mutex_unlock(&cache_lock);
    return NULL;
}

int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int connect_to_remote(char* host, int port) {
    struct sockaddr_in server_addr;
    struct hostent* server;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    if ((server = gethostbyname(host)) == NULL) {
        perror("gethostbyname");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return -1;
    }

    return sockfd;
}

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_sock, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        close(client_sock);
        return NULL;
    }

    buffer[bytes_read] = '\0';

    // Извлекаем URL из запроса
    char method[BUFFER_SIZE], url[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, url, version);

    printf("Client requested URL: %s\n", url);

    // Проверяем кэш
    cache_entry_t* cached_page = cache_find(url);
    if (cached_page) {
        // Если страница есть в кэше, возвращаем её клиенту
        send(client_sock, cached_page->data, cached_page->size, 0);
        close(client_sock);
        return NULL;
    }

    // Парсим хост из URL
    char host[BUFFER_SIZE], path[BUFFER_SIZE];
    sscanf(url, "http://%[^/]%s", host, path);

    // Подключаемся к удалённому серверу
    int remote_sock = connect_to_remote(host, 80);
    if (remote_sock < 0) {
        close(client_sock);
        return NULL;
    }

    // Отправляем запрос на удалённый сервер
    snprintf(buffer, sizeof(buffer), "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
    send(remote_sock, buffer, strlen(buffer), 0);

    // Получаем данные с удалённого сервера и кэшируем их
    char* response = malloc(BUFFER_SIZE);
    int total_size = 0;
    while ((bytes_read = recv(remote_sock, buffer, sizeof(buffer), 0)) > 0) {
        response = realloc(response, total_size + bytes_read);
        memcpy(response + total_size, buffer, bytes_read);
        total_size += bytes_read;
        send(client_sock, buffer, bytes_read, 0); // Отправляем данные клиенту
    }

    // Кэшируем страницу
    cache_store(url, response, total_size);

    free(response);
    close(remote_sock);
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock = create_server_socket(PORT);
    pthread_mutex_init(&cache_lock, NULL);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        // Создаём поток для обработки клиента
        pthread_t tid;
        int* client_sock_ptr = malloc(sizeof(int));
        *client_sock_ptr = client_sock;
        pthread_create(&tid, NULL, handle_client, client_sock_ptr);
        pthread_detach(tid);
    }

    close(server_sock);
    pthread_mutex_destroy(&cache_lock);
    return 0;
}
