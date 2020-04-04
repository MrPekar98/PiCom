#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
char *run_command(pi_command com);
char *run_command_arg(char *command, char *arg);
void run_command_proc(char *proc);

// Menu for executing command.
char *pi_menu(pi_command command)
{
  switch (command.com)
  {
    case EXEC:
      run_command_proc(command.data);
      return " ";

    case MKDIR:
      return run_command_arg("mkdir", command.data);

    case RMDIR:
      return run_command_arg("rmdir", command.data);

    case TOUCH:
      return run_command_arg("touch", command.data);

    case RM:
      return run_command_arg("rm", command.data);

    case LS:
      return run_command_arg("ls", command.data);

    case RAW:
      return run_command(command);

    default:
      return "Invalid command.";
  }
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

// Returns output of command.
char *run_command(pi_command com)
{
  char *temp = (char *) malloc(sizeof(char) * DATA_LEN);
  char *out = (char *) malloc(sizeof(char) * DATA_LEN * 10);
  FILE *p = popen(com.data, "r");

  while (fgets(temp, DATA_LEN, p) != NULL)
  {
    strcat(out, temp);
    strcat(out, "\n");
  }

  strcat(out, " \0")
  free(temp);
  fclose(p);
  return out;
}

// Returns output of command given arguments of command.
char *run_command_arg(char *command, char *arg)
{
  char *temp = (char *) malloc(sizeof(char) * DATA_LEN);
  char *out = (char *) malloc(sizeof(char) * DATA_LEN * 10);
  char *com = (char *) malloc(sizeof(char) * (strlen(command) + strlen(arg) + 2));
  sprintf(com, "%s %s", command, arg);
  FILE *p = popen(com, "r");
  free(com);

  while (fgets(temp, DATA_LEN, p) != NULL)
  {
    strcat(out, temp);
    strcat(out, "\n");
  }

  strcat(out, " \0");
  free(temp);
  fclose(p);
  return out;
}

// Runs external bash script.
void run_command_proc(char *proc)
{
  const int pid = fork();

  if (pid == 0)
  {
    char *command = (char *) malloc(sizeof(char) * strlen(proc) + 9);
    sprintf(command, "xdg-open %s.out", proc);
    system(command);
    free(command);
  }
}
