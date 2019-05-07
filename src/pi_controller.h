#include <string.h>
#define DATA_LEN 30
#define TO_NUM 48
#define COM_1 0
#define COM_2 2
#define DATA 4

// Enum to specify commands.
// For example, 0;5;2;1;Hello, world.
// Say, the above example creates a file with a name.
enum level1 {CD = 1};
enum level2 {EXEC = 1, MKDIR, RMDIR, TOUCH, RM};

// Structure representing command passed through a network.
typedef struct
{
  enum level1 com1;
  enum level2 com2;
  char data[DATA_LEN];
} pi_command;

// Prototypes
pi_command parse_picommand(char *command);
char *tostring(pi_command com);

// Menu for executing command.
void pi_menu(pi_command command)
{

}

// Parses string to pi_command. Example: "2;6;1;4;Hello, world"
pi_command parse_picommand(char *command)
{
  const size_t str_length = strlen(command);
  unsigned i;
  pi_command com = {.level1 = command[COM_1] - TO_NUM, .level2 = command[COM_2] - TO_NUM};

  sprintf(com.data, "%s", command + DATA);
  return com;
}

// Converts pi_command into string.
char *tostring(pi_command com)
{
  char result[DATA_LEN + 5];
  sprintf(result, "%d;%d;%s", com.level1, com.level2, com.data);
  return result;
}
