
int socket_join_multicast(int sock, char *addr);
#ifdef KLK_SOURCE
int socket_leave_multicast(int sock, char *addr);
#endif
int socket_set_ttl(int sock, int ttl);
int socket_set_nonblock(int sock);
int socket_open(char *laddr, int port);
void socket_close(int sock);
int socket_connect(int sock, char *addr, int port);
