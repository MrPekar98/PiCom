#include <string.h>
#define DATA_LEN 50
#define TO_NUM 48
#define COM 0
#define DATA 2

// Commands
enum command {EXEC = 1, MKDIR, RMDIR, TOUCH, RM, LS, RAW};

// Structure representing command passed through a network.
typedef struct
{
  enum command com;
  char data[DATA_LEN];
} pi_command;

// Prototypes
pi_command parse_picommand(char *command);
char *tostring(pi_command com);

// Menu for executing command.
char *pi_menu(pi_command command)
{
  return "ANSWER!";
}

// Parses string to pi_command. Example: "2;Hello, world"
pi_command parse_picommand(char *command)
{
  const size_t str_length = strlen(command);
  unsigned i;
  pi_command com = {.com = command[COM] - TO_NUM};

  sprintf(com.data, "%s", command + DATA);
  return com;
}

// Converts pi_command into string.
char *tostring(pi_command com)
{
  static char result[DATA_LEN + 3];
  sprintf(result, "%d;%s", com.com, com.data);
  return result;
}
