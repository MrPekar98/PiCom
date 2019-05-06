#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "server.h"
#define PORT 58091

// Globals
int sockfd;	// Socket descriptor.
int error = 0;	// Tells if error.

// Prototypes
static inline server_con init_sock(unsigned port);
static inline int connect_client(int sockfd);

// Main function
int main()
{
  printf("Initialising connection...\n");

  do
  {
    server_con socket = init_sock(PORT);

    if (socket.error)
    {
      socket.error = 0;
      continue;
    }

    int client_fd = connect_client(socket.sockfd);

    if (client_fd < 0)
    {
      printf("Connection to controller lost. Trying again...\n");
      continue;
    }
  }
  while (1);

  return 0;
}

// Initialises socket.
static inline server_con init_sock(unsigned port)
{
  server_con con;
  con.sockfd = socket(AF_INET, SOCK_STREAM, 0);;

  if (con.sockfd < 0)
  {
    con.error = 1;
    return con;
  }

  bzero((char *) &con.socket, sizeof(con.socket));
  con.socket.sin_family = AF_INET;
  con.socket.sin_addr.s_addr = INADDR_ANY;
  con.socket.sin_port = htons(port);

  if (bind(con.sockfd, (struct sockaddr *) &con.socket, sizeof(con.socket)) < 0)
    con.error = 1;

  return con;
}

// Connects socket to client.
static inline int connect_client(int sockfd)
{
  struct sockaddr_in client;
  listen(sockfd, 1);
  int client_size = sizeof(client);
  
  return accept(sockfd, (struct sockaddr *) &client, &client_size);
}
