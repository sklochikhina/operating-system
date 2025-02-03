#include "cache.h"

void init_cache(Cache* cache) {
    for (int i = 0; i < MAX_CACHE_SIZE; i++) {
        memset(cache->cache[i].url, '\0', MAX_URL_LEN);

        cache->cache[i].data = (Data*)malloc(sizeof(Data));
        cache->cache[i].data->memory = NULL;

        cache->cache[i].last_access = 0;
        cache->cache[i].is_error = cache->cache[i].is_loading = cache->cache[i].is_size_exceeded = 0;

        pthread_rwlock_init(&cache->cache[i].sync_rw, NULL);
        pthread_mutex_init(&cache->cache[i].item_mutex, NULL);
    }
    printf(ANSI_COLOR_CYAN "Cache has been initialized successfully!\n" ANSI_COLOR_CYAN);
}

void destroy_cache(Cache* cache) {
    if (cache == NULL) return;

    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        pthread_mutex_lock(&cache->cache[i].item_mutex);
        if (cache->cache[i].data != NULL) {
            if (cache->cache[i].data->memory != NULL)
                free(cache->cache[i].data->memory);
            free(cache->cache[i].data);
        }
        pthread_mutex_unlock(&cache->cache[i].item_mutex);

        pthread_rwlock_destroy(&cache->cache[i].sync_rw);
        pthread_mutex_destroy(&cache->cache[i].item_mutex);
    }

    printf(ANSI_COLOR_CYAN "Cache has been destroyed successfully!\n" ANSI_COLOR_CYAN);

    free(cache);
}

void delete_item(const char* url, Cache* cache) {
    CacheItem* item = find_url_in_cache(cache, url);

    if (item == NULL) return;

    pthread_mutex_lock(&item->item_mutex);
    if (item->data->memory != NULL) {
        free(item->data->memory);
        item->data->memory = NULL;
    }
    memset(item->url, '\0', MAX_URL_LEN);
    pthread_mutex_unlock(&item->item_mutex);
}

CacheItem* find_url_in_cache(Cache* cache, const char* url) {
    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        pthread_mutex_lock(&cache->cache[i].item_mutex);
        if (strcmp(cache->cache[i].url, "") == 0) {
            pthread_mutex_unlock(&cache->cache[i].item_mutex);
            continue;
        }
        if (strcmp(cache->cache[i].url, url) == 0) {
            cache->cache[i].last_access = time(NULL);
            pthread_mutex_unlock(&cache->cache[i].item_mutex);
            return &cache->cache[i];
        }
        pthread_mutex_unlock(&cache->cache[i].item_mutex);
    }
    return NULL;
}

int find_empty_url_in_cache(Cache* cache) {
    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        pthread_mutex_lock(&cache->cache[i].item_mutex);
        if (strcmp(cache->cache[i].url, "") == 0)  {
            pthread_mutex_unlock(&cache->cache[i].item_mutex);
            return i;
        }
        pthread_mutex_unlock(&cache->cache[i].item_mutex);
    }
    return -1;
}

CacheItem* add_url_to_cache(Cache* cache, const char* url) {
    if (find_url_in_cache(cache, url) != NULL) {
        printf(ANSI_COLOR_YELLOW "Not add to cache url (already in) : |%s| \n" ANSI_COLOR_RESET, url);
        return NULL;
    }

    CacheItem* item = (CacheItem*)malloc(sizeof(CacheItem));

    // Ищем индекс элемента с пустым url
    int index = find_empty_url_in_cache(cache);

    if (index != -1) {
        printf(ANSI_COLOR_YELLOW "Add to cache url (count < max) : |%s| \n" ANSI_COLOR_RESET, url);

        pthread_mutex_lock(&cache->cache[index].item_mutex);

        strcpy(cache->cache[index].url, url);
        cache->cache[index].last_access = time(NULL);

        item = &cache->cache[index];

        pthread_mutex_unlock(&cache->cache[index].item_mutex);
    } else {
        printf(ANSI_COLOR_YELLOW "Add to cache url (delete) : |%s| \n" ANSI_COLOR_RESET, url);

        int minInd = 0;
        time_t min = 0;

        for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
            pthread_mutex_lock(&cache->cache[i].item_mutex);
            if (strcmp(cache->cache[i].url, "") == 0) {
                continue;
            }
            if (!i) {
                min = cache->cache[i].last_access;
                minInd = i;
                continue;
            }
            if (cache->cache[i].last_access < min) {
                min = cache->cache[i].last_access;
                minInd = i;
            }
            // не разблокируем элементы во избежание их изменения в другом месте
        }

        strcpy(cache->cache[minInd].url, url);
        free(cache->cache[minInd].data);                        // освобождаем старые данные
        cache->cache[minInd].last_access = time(NULL);

        item = &cache->cache[minInd];

        for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
            pthread_mutex_unlock(&cache->cache[i].item_mutex);
        }
    }

    return item;
}
