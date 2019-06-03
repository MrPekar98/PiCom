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
static inline size_t cut_segment(char *buffer);
char *substring(char *str, unsigned end);

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

    printf("Connected.\n\n");
    exec_read(client_fd);
    close(socket.sockfd);
    close(client_fd);
    printf("\nSession ended. Starting over...\n");
  }
  while (1);

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
  listen(sockfd, 5);
  int client_size = sizeof(client);
  
  return accept(sockfd, (struct sockaddr *) &client, &client_size);
}

// Reads from client and executed command. Returns when an error occurs.
static void exec_read(int client_sock)
{
  char buffer[DATA_LEN + 10], ans[DATA_LEN];
  int error = 0;
  printf("ID\tMessage\n\n");

  while (error >= 0)
  {
    error = read(client_sock, buffer, DATA_LEN + 10);
 
    if (error < 0 || strlen(buffer) <= 1)
      return;

    const size_t buffer_len = cut_segment(buffer);
    buffer[buffer_len] = '\0';
    printf("%d\t%s\n", client_sock, buffer);
    strcpy(ans, pi_menu(parse_picommand(buffer)));
    error = write(client_sock, ans, 18);
    memset(buffer, 0, DATA_LEN + 10);
    memset(ans, 0, DATA_LEN);
  }
}

// Removes first segment of input and returns first segment as number.
static inline size_t cut_segment(char *buffer)
{
  unsigned i, counter = 0, met = 0;
  char *temp = (char *) malloc(strlen(buffer) * sizeof(char) - 1);

  for (i = 0; i < strlen(buffer); i++)
  {
    if (buffer[i] == ';' && !met)
      met = i;

    else if (met)
      temp[counter++] = buffer[i];
  }

  const size_t size = atoi(substring(buffer, met));
  sprintf(buffer, "%s", temp);
  return size;
}

// Substring function.
char *substring(char *str, unsigned end)
{
  char *res = (char *) malloc(sizeof(char) * end);
  unsigned i;

  for (i = 0; i < end; i++)
  {
    res[i] = str[i];
  }

  return res;
}
