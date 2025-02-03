#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_request.h"

#define MAX_CACHE_SIZE 3
#define MAX_URL_LEN 1024
#define CACHE_BUFFER_SIZE (500 * 1024 * 1024)  // 500 MB

typedef struct {
    char url[MAX_URL_LEN];
    Data* data;
    time_t last_access;
    int is_loading;
    int is_size_exceeded;
    int is_error;
    pthread_rwlock_t sync_rw;
    pthread_mutex_t item_mutex;
} CacheItem;

typedef struct {
    CacheItem cache[MAX_CACHE_SIZE];
} Cache;

void       init_cache(Cache* cache);

CacheItem* find_url_in_cache(Cache* cache, const char* url);

CacheItem* add_url_to_cache(Cache* cache, const char* url);
int        find_empty_url_in_cache(Cache* cache);
void       delete_item(const char* url, Cache* cache);
void       destroy_cache(Cache* cache);

#endif //CACHE_H
