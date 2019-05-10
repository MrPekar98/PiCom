#include <stdio.h>
#include "server.h"
#include "pi_controller.h"
#define PORT 58091

const char *ip1 = "192.168.38.1", *ip2 = "192.168.8.104";

// Prototypes
server_con init_client(const char *hostname, int port);
void connect_client(server_con *connection);

// Main function.
int main()
{
  printf("Initialising connection...\n");

  while (1)
  {
    server_con client_con = init_client("127.0.0.1", PORT);

    if (client_con.error)
      continue;

    printf("Connection setup. Connecting to server...\n");
    connect_client(&client_con);
    printf("Connected to server.\n");
    
    // Start communicating.
    break;
  }

  return 0;
}

// Initialises client.
server_con init_client(const char *hostname, int port)
{
  server_con con = {.sockfd = socket(AF_INET, SOCK_STREAM, 0)};

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
  connection->error = connect(connection->sockfd, (struct sockaddr *) (connection.s_addr), sizeof(connection->s_addr));
}
