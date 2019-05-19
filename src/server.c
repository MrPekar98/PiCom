// Server send '1', if command has been terminated successfully, '0' otherwise. If command 'ls' is called, then the output is sent.

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "server.h"
#include "pi_controller.h"
#define PORT 58091

// Prototypes
static inline server_con init_sock(unsigned port);
static inline int connect_client(int sockfd);
static void exec_read(int client_sock);
void timer(int *p, unsigned limit);

// Main function
int main()
{
  printf("Initialising connection...\n");
  server_con socket;

  do
  {
    socket = init_sock(PORT);

    if (socket.error)
    {
      close(socket.sockfd);
      continue;
    }

    printf("Connecting...\n");
    int client_fd = connect_client(socket.sockfd);

    if (client_fd < 0)
    {
      printf("Connection to controller lost. Trying again...\n");
      close(socket.sockfd);
      close(client_fd);
      continue;
    }

    printf("Connected.\n");
    exec_read(client_fd);
    close(socket.sockfd);
    close(client_fd);
    printf("\nSession ended. Starting over...\n");
  }
  while (1);

  close(socket.sockfd);
  return 0;
}

// Initialises socket.
static inline server_con init_sock(unsigned port)
{
  server_con con;
  con.sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (con.sockfd < 0)
  {
    con.error = 1;
    return con;
  }

  bzero((char *) &con.s_addr, sizeof(con.s_addr));
  con.s_addr.sin_family = AF_INET;
  con.s_addr.sin_addr.s_addr = INADDR_ANY;
  con.s_addr.sin_port = htons(port);

  if (bind(con.sockfd, (struct sockaddr *) &con.s_addr, sizeof(con.s_addr)) < 0)
    con.error = 1;

  con.error = 0;
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

// Reads from client and executed command. Returns when an error occurs.
static void exec_read(int client_sock)
{
  char buffer[DATA_LEN + 10], ans[2];
  int error = 0;
  printf("ID\tMessage\n\n");

  do
  {
    error = read(client_sock, buffer, DATA_LEN + 10);

    if (error < 0 || strlen(buffer) <= 1)
      continue;

    buffer[strlen(buffer)] = '\0';
    printf("%d\t%s\n", client_sock, buffer);
    ans[0] = (char) pi_menu(parse_picommand(buffer)) + TO_NUM;
    error = write(client_sock, ans, 18);
  }
  while (error >= 0 && strlen(buffer) <= 1);
}

// Increments time value in seconds.
void timer(int *p, unsigned limit)
{
  time_t start = time(NULL);

  while (*p <= limit)
  {
    *p = time(NULL) - start;
  }
}
