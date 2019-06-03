#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include "pi_controller.h"
#define PORT 58091

const char *ip1 = "192.168.38.1", *ip2 = "192.168.8.104";

// Prototypes
pi_command prompt_command();
server_con init_client(const char *hostname, int port);
void connect_client(server_con *connection);
char *com_server(pi_command command, server_con connection);

// Main function.
int main()
{
  printf("Connecting...\n");
  server_con client_con = init_client("127.0.0.1", PORT);

  if (client_con.error)
  {
    printf("Could not setup connection.\n\n");
    close(client_con.sockfd);
    return 1;
  }

  connect_client(&client_con);

  if (client_con.error)
  {
    printf("Could not connect to server.\n\n");
    close(client_con.sockfd);
    return 1;
  }

  printf("Connected to server.\n");

  // Start communicating.
  while (1)
  {    
    const pi_command com = prompt_command();
    char *response = com_server(com, client_con);

    if (response == NULL)
    {
      printf("Error communicating. Trying again...\n");
      close(client_con.sockfd);
      continue;
    }

    printf("\n-> %s\n\n", response);
    sleep(1);
  }

  close(client_con.sockfd);
  return 0;
}

// Prints all oppotunities.
pi_command prompt_command()
{
  pi_command c;
  printf("Options\n1) - exec\n2) - mkdir\n3) - rmdir\n4) - touch\n5) - rm\n6) - ls\n7) - raw command\n\n: ");
  scanf("%d", &c.com);
  printf("Data: ");
  scanf(" %[A-Za-z -\".',/\\-]", c.data);

  return c;
}

// Initialises client.
server_con init_client(const char *hostname, int port)
{
  server_con con = {.sockfd = socket(AF_INET, SOCK_STREAM, 0), .error = 0};

  if (con.sockfd < 0)
  {
    con.error = 1;
    return con;
  }

  struct hostent *server = gethostbyname(hostname);

  if (server == NULL)
  {
    con.error = 1;
    return con;
  }

  bzero((char *) &con.s_addr, sizeof(con.s_addr));
  con.s_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr, (char *) &con.s_addr.sin_addr.s_addr, server->h_length);
  con.s_addr.sin_port = htons(port);

  return con;
}

// Connects to server.
void connect_client(server_con *connection)
{
  if (connect(connection->sockfd, (struct sockaddr *) &(connection->s_addr), sizeof(connection->s_addr)) < 0)
    connection->error = 1;
}

// Communicates to server.
char *com_server(pi_command command, server_con connection)
{
  char *buffer = (char *) malloc(sizeof(char) * (DATA_LEN + 1));
  sprintf(buffer, "%d;%s", strlen(tostring(command)), tostring(command));

  if (write(connection.sockfd, buffer, strlen(buffer)) < 0)
    return NULL;

  bzero(buffer, (DATA_LEN + 1));

  if (read(connection.sockfd, buffer, DATA_LEN) < 0)
    return NULL;

  return buffer;
}
