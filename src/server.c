// Server send '1', if command has been terminated successfully, '0' otherwise.

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "server.h"
#include "pi_controller.h"
#define PORT 58091

// Prototypes
static inline server_con init_sock(unsigned port);
static inline int connect_client(int sockfd);
static void exec_read(int client_sock);
void *timer(void *p);

// Main function
int main()
{
  printf("Initialising connection...\n");

  do
  {
    server_con socket = init_sock(PORT);

    if (socket.error)
      continue;

    printf("Connecting...\n");
    int client_fd = connect_client(socket.sockfd);

    if (client_fd < 0)
    {
      printf("Connection to controller lost. Trying again...\n");
      continue;
    }

    printf("Connected.\n");
    exec_read(client_fd);
    printf("Lost connection. Trying again...\n");
  }
  while (1);

  return 0;
}

// Initialises socket.
static inline server_con init_sock(unsigned port)
{
  server_con con = {.error = 0};
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
  long timeout = 0;
  pthread_t thread;
  printf("ID\tMessage\n\n");
  pthread_create(&thread, NULL, timer, &timeout);

  do
  {
    error = read(client_sock, buffer, DATA_LEN + 10);

    if (error < 0 || strlen(buffer) <= 1)
      continue;

    printf("%d\t%s\n", client_sock, buffer);
    ans[0] = (char) pi_menu(parse_picommand(buffer)) + TO_NUM;
    error = write(client_sock, ans, 18);
  }
  while (error >= 0 && timeout <= 60);

  pthread_cancel(thread);
}

// Increments time value in seconds.
void *timer(void *p)
{
  time_t start = time(NULL);

  while (1)
  {
    long t = time(NULL) - start;
    p = (void *) &t;
  }
}
