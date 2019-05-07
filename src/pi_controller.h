#include <string.h>
#define DATA 30

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
  char data[DATA];
} pi_command;

// Prototype
pi_command parse_picommand(char *command);

// Menu for executing command.
void pi_menu(pi_command command)
{

}

// Parses string to pi_command. Example: "2;6;1;4;Hello, world"
pi_command parse_picommand(char *command)
{
  return (pi_command) {.com1 = CD};
}
