#ifndef PROXY_H
#define PROXY_H

#include <errno.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cache.h"

#define MAX_USERS_COUNT 5
#define PORT 8080
#define BUFFER_SIZE 4096

struct FuncArgs {
    int client_socket;
    Cache* cache;
};

int   server_socket_init();
int   is_response_status_ok(char* buffer);
char* extract_url(char* request);
char* extract_host(const char* request, size_t max_host_len);

void  read_and_cache_rest(int dest_socket, CacheItem* item, size_t already_read);
void* fetch_and_cache_data(void* arg);
void  handle_client_request(void* args);

void  set_params(struct sockaddr_in* server_addr);
void  binding_and_listening(int server_socket, struct sockaddr_in* server_addr);
int   send_to(int socket, void* data, unsigned int size);

#endif //PROXY_H
