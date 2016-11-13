
int soc_serv_wait_for_client(FILE *fplog,int sdsock,int port);
void soc_serv_wait_for_client_close(FILE *fplog,int sd);
int soc_serv_open(FILE *fplog,int port);
void soc_serv_close(FILE *fplog,int sd);
int soc_open(FILE *fplog,char *hostname, int port);
int soc_send(FILE *fplog,int sd,unsigned char *buf,int buflen);
int soc_recv(FILE *fplog,int sd,unsigned char *buf,int buflen);
void soc_close(FILE *fplog,int sd);
int soc_recv_timeout(FILE *fplog,int sd,unsigned char *buf,int buflen,int timeout_ms);
