#include <sys/socket.h>
#define BUFFER 1024

typedef struct
{
  short error;
  int sockfd;
  struct sockaddr_in socket;
} server_con;
