#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#define BUFFER 1024

typedef struct
{
  short error;
  int sockfd;
  struct sockaddr_in s_addr;
} server_con;
