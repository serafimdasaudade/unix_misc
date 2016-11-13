
pthread_t create_thread_detached(FILE *fplog, void *func(void*),void *param);
pthread_t create_thread_joinable(FILE *fplog, void *func(void*),void *param);