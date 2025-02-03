#include "proxy.h"

#include <pthread.h>
#include <semaphore.h>

typedef struct {
    char* request;
    Cache* cache;
    int client_socket;
} ThreadArgs;

int server_socket_init() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    int option = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    return server_socket;
}

void set_params(struct sockaddr_in* server_addr) {
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;          // IPv4
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

void binding_and_listening(int server_socket, struct sockaddr_in* server_addr) {
    if (bind(server_socket, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_USERS_COUNT) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf(ANSI_COLOR_GREEN "Proxy server started. Listening on port %d...\n" ANSI_COLOR_RESET, PORT);
}

void read_and_cache_rest(int dest_socket, CacheItem* item, size_t already_read) {
    char buffer[BUFFER_SIZE] = {0};
    size_t bytes_read, all_bytes_read = already_read;

    while ((bytes_read = read(dest_socket, buffer, BUFFER_SIZE)) > 0) {
        // При превышении размера кэша ставим флаг, что данные превысили размер
        if (all_bytes_read + bytes_read > CACHE_BUFFER_SIZE) {
            printf(ANSI_COLOR_RED "Data size exceeds CACHE_BUFFER_SIZE! Not saving to cache. Closing connection...\n" ANSI_COLOR_RESET);

            pthread_mutex_lock(&item->item_mutex);
            item->is_size_exceeded = 1;
            pthread_mutex_unlock(&item->item_mutex);

            break;
        }

        // Записываем часть ответа в кэш
        pthread_rwlock_wrlock(&item->sync_rw);

        memcpy(item->data->memory + all_bytes_read, buffer, bytes_read);
        item->data->size += bytes_read;

        pthread_rwlock_unlock(&item->sync_rw);

        all_bytes_read += bytes_read;
    }
}

void* fetch_and_cache_data(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Cache* cache      = args->cache;
    char* request     = args->request;
    int client_socket = args->client_socket;

    // Извлекаем хост и url из запроса
    char* host = extract_host(request, 50);
    char* url  = extract_url(request);

    // Начинаем работать с новым элементом кэша
    CacheItem* item    = add_url_to_cache(cache, url);
    pthread_mutex_lock(&item->item_mutex);
    item->is_loading   = 1;
    item->data->memory = (char*)calloc(CACHE_BUFFER_SIZE, sizeof(char));
    item->data->size   = 0;
    pthread_mutex_unlock(&item->item_mutex);

    // Подключаемся к целевому серверу
    int dest_socket = connect_to_remote(host);
    if (dest_socket == -1) {
        printf(ANSI_COLOR_RED "Destiny socket error\n" ANSI_COLOR_RESET);

        close(client_socket);
        free(host);
        free(item->data->memory);
        free(item->data);
        free(request);

        return NULL;
    }
    printf(ANSI_COLOR_GREEN "Created new connection with remote server %s\n" ANSI_COLOR_RESET, host);

    // Отправляем серверу запрос, полученный от клиента
    ssize_t bytes_sent = send_to(dest_socket, request, strlen(request));
    if (bytes_sent == -1) {
        printf(ANSI_COLOR_RED "Error while sending request to remote server\n" ANSI_COLOR_RESET);

        free(item->data->memory);
        free(item->data);
        free(host);
        close(client_socket);
        close(dest_socket);

        return NULL;
    }
    printf(ANSI_COLOR_GREEN "Sent request to remote server, len = %lld\n" ANSI_COLOR_RESET, bytes_sent);

    char buffer[BUFFER_SIZE] = {0}; // буфер для считывания данных по блокам
    ssize_t bytes_read, all_bytes_read = 0;

    while ((bytes_read = read(dest_socket, buffer, BUFFER_SIZE)) > 0) {
        // При превышении размера кэша прекращаем чтение и удаляем запись
        if (all_bytes_read + bytes_read > CACHE_BUFFER_SIZE) {
            printf(ANSI_COLOR_RED "Data size exceeds CACHE_BUFFER_SIZE! Not saving to cache. Closing connection...\n" ANSI_COLOR_RESET);

            pthread_mutex_lock(&item->item_mutex);
            item->is_size_exceeded = 1;
            pthread_mutex_unlock(&item->item_mutex);

            close(client_socket);
            close(dest_socket);

            break;
        }

        // Отправляем считанные данные клиенту
        bytes_sent = send_to(client_socket, buffer, bytes_read);
        if (bytes_sent == -1) {
            if (errno == EPIPE)
                printf(ANSI_COLOR_YELLOW "Broken pipe - client disconnected. Closing connection...\n" ANSI_COLOR_RESET);
            else
                printf(ANSI_COLOR_RED "Error while sending data to client. Closing connection...\n" ANSI_COLOR_RESET);

            close(client_socket);

            printf(ANSI_COLOR_MAGENTA "Reading and caching rest of the data..." ANSI_COLOR_RESET);
            read_and_cache_rest(dest_socket, item, all_bytes_read);

            close(dest_socket);

            break;
        }

        if (all_bytes_read == 0 && !is_response_status_ok(buffer)) {
            printf(ANSI_COLOR_RED "Server returned error, not saving to cache. Sending to client...\n" ANSI_COLOR_RESET);

            send_to(client_socket, item->data->memory, item->data->size);

            pthread_mutex_lock(&item->item_mutex);
            item->is_error = 1;
            pthread_mutex_unlock(&item->item_mutex);

            close(client_socket);
            close(dest_socket);

            break;
        }

        // Записываем часть ответа в кэш
        pthread_rwlock_wrlock(&item->sync_rw);

        memcpy(item->data->memory + all_bytes_read, buffer, bytes_read);
        item->data->size += bytes_read;

        pthread_rwlock_unlock(&item->sync_rw);

        all_bytes_read += bytes_read;

        fflush(stdout);
    }

    pthread_mutex_lock(&item->item_mutex);
    item->is_loading = 0;
    pthread_mutex_unlock(&item->item_mutex);

    pthread_mutex_lock(&item->item_mutex);
    if (item->is_size_exceeded || item->is_error) {
        pthread_mutex_unlock(&item->item_mutex);

        printf(ANSI_COLOR_RED "Error appeared while reading the response, freeing memory...\n" ANSI_COLOR_RESET);

        delete_item(url, cache);

        free(url);
        free(host);

        return NULL;
    }

    item->data->size = all_bytes_read;
    printf(ANSI_COLOR_GREEN "Data sent to client.\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_GREEN "Data added to cache with size %llu\n\n" ANSI_COLOR_RESET, item->data->size);

    pthread_mutex_unlock(&item->item_mutex);

    close(client_socket);
    close(dest_socket);
    free(url);
    free(host);
    free(request);

    return NULL;
}

int send_to(int socket, void* data, unsigned int size) {
    return write(socket, data, size);
}

// Обработка запроса клиента
void handle_client_request(void* args) {
    struct FuncArgs* arg = (struct FuncArgs*)args;
    int client_socket = arg->client_socket;
    Cache* cache      = arg->cache;

    printf(ANSI_COLOR_YELLOW "Handling client request...\n" ANSI_COLOR_RESET);
    char* buffer = calloc(BUFFER_SIZE, sizeof(char));

    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_read < 0) {
        perror("Error reading from socket");
        close(client_socket);
        free(buffer);
        return;
    }

    char* url = extract_url(buffer);

    if (url != NULL) {
        CacheItem* item = find_url_in_cache(cache, url);
        if (item != NULL) {
            printf(ANSI_COLOR_GREEN "Data found in cache!\n" ANSI_COLOR_RESET);

            pthread_mutex_lock(&item->item_mutex);
            if (item->is_loading) {
                pthread_mutex_unlock(&item->item_mutex);

                printf(ANSI_COLOR_BLUE "Data is loading by other client right now. Starting to catch...\n" ANSI_COLOR_RESET);

                size_t sent = 0;

                while (1) {
                    pthread_rwlock_rdlock(&item->sync_rw);
                    // Отправляем клиенту доступные данные
                    size_t to_send = item->data->size - sent;
                    if (to_send > 0) {
                        const int written = send_to(client_socket, item->data->memory + sent, to_send);
                        if (written == -1) {
                            pthread_rwlock_unlock(&item->sync_rw);
                            if (errno == EPIPE) {
                                printf(ANSI_COLOR_YELLOW "Broken pipe - client disconnected. Closing connection...\n" ANSI_COLOR_RESET);
                            } else {
                                printf(ANSI_COLOR_RED "Error while sending data to client. Closing connection...\n" ANSI_COLOR_RESET);
                            }

                            close(client_socket);
                            free(buffer);
                            free(url);
                            return;
                        }
                        sent += written;
                    }

                    pthread_rwlock_unlock(&item->sync_rw);

                    pthread_mutex_lock(&item->item_mutex);
                    if (!item->is_loading && sent == item->data->size) {
                        pthread_mutex_unlock(&item->item_mutex);
                        break;
                    }
                    pthread_mutex_unlock(&item->item_mutex);

                    usleep(1000);
                }
            } else {
                send_to(client_socket, item->data->memory, item->data->size);
                pthread_mutex_unlock(&item->item_mutex);
            }

            close(client_socket);
        }
        else {
            printf(ANSI_COLOR_BLUE "Data not found in cache. Fetching from remote server...\n" ANSI_COLOR_RESET);

            pthread_t tid;
            ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
            args->cache = cache;
            args->request = strdup(buffer);
            args->client_socket = client_socket;

            printf(ANSI_COLOR_BLUE "Initializing new downloading thread\n" ANSI_COLOR_RESET);

            int err = pthread_create(&tid, NULL, &fetch_and_cache_data, args);
            if (err != 0) {
                fprintf(stderr, "Error creating thread: %s\n", strerror(err));
                free(args->request);
                free(args);
                free(buffer);
                free(url);
                close(client_socket);
                return;
            }
            pthread_detach(tid);
        }
    } else {
        printf(ANSI_COLOR_RED "URL is NULL. Sending default response...\n" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_YELLOW "Closing client socket!...\n" ANSI_COLOR_RESET);
        close(client_socket);
    }

    free(url);
}

// Извлекаем URL из HTTP-запроса
char* extract_url(char* request) {
    // Ищем начало строки запроса
    const char* method_end = strstr(request, " "); // Поиск пробела после метода (например, "GET")
    if (!method_end) {
        return NULL;
    }

    // Ищем конец URL (следующий пробел после URL)
    const char* url_end = strstr(method_end + 1, " ");
    if (!url_end) {
        return NULL;
    }

    // Вычисляем длину URL
    size_t url_length = url_end - (method_end + 1);
    if (url_length >= MAX_URL_LEN) {
        return NULL;
    }

    char* url = (char*)malloc(url_length + 1);
    strncpy(url, method_end + 1, url_length);
    url[url_length] = '\0';

    return url;
}

// Извлекаем Host из HTTP-запроса
char* extract_host(const char* request, size_t max_host_len) {
    // Ищем строку "Host: " в запросе
    const char* host_start = strstr(request, "Host: ");
    if (!host_start) {
        return NULL;
    }

    // Ищем конец строки с Host (поиск символа новой строки)
    const char* host_end = strstr(host_start, "\r\n");
    if (!host_end) {
        return NULL;
    }

    // Пропустить "Host: " (длина 6 символов)
    host_start += 6;

    // Вычислить длину Host
    size_t host_length = host_end - host_start;
    if (host_length >= max_host_len) {
        return NULL;
    }

    // Копируем Host в выходной буфер
    char* host = (char*)malloc(host_length + 1);
    strncpy(host, host_start, host_length);
    host[host_length] = '\0';

    return host;
}

int is_response_status_ok(char* buffer) {
    return strstr(buffer, "HTTP/1.0 200 OK") || strstr(buffer, "HTTP/1.1 200 OK");
}