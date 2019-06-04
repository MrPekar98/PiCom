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
void run_command_proc(char *proc, char *input);
char *proc_name(char *command);
char *proc_input(char *command);

// Menu for executing command.
char *pi_menu(pi_command command)
{
  switch (command.com)
  {
    case EXEC:
      break;

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
      run_command_proc(proc_name(command.data), proc_input(command.data));
      break;

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
  char *out = (char *) malloc(sizeof(char) * DATA_LEN);
  FILE *p = popen(com.data, "r");

  while (fgets(temp, DATA_LEN, p) != NULL)
  {
    strcat(out, temp);
    strcat(out, "\n");
  }

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

  free(temp);
  fclose(p);
  return out;
}

// TODO: Download Java server at make it open the program.
// Runs external bash script.
void run_command_proc(char *proc, char *input)
{
  const int pid = fork();

  if (pid == 0)
  {
    char *pname = (char *) malloc(sizeof(char) * (strlen(proc) + 3));
    sprintf(pname, "sudo bash %s.sh", proc);
    FILE *p = popen(pname, "w");

    fprintf(p, "%s\n", input);
    free(pname);
    fclose(p);	// TODO: Might have to not close file to make it run until it ends.
  }

  else
    wait(NULL);
}

// Returns process name of string command.
char *proc_name(char *command)
{
  const size_t len = strlen(command);
  char *res = (char *) malloc(sizeof(char) * len);
  unsigned i = 0;

  while (command[i] != ' ')
  {
    res[i] = command[i++];
  }

  return res;
}

// Returns input to process.
char *proc_input(char *command)
{
  unsigned counter = 0, i;
  const size_t len = strlen(command);
  char *res = (char *) malloc(sizeof(char) * len);

  for (i = 0; i < len; i++)
  {
    if (command[i] == ' ')
    {
      for (; i < len; i++)
      {
        res[counter++] = command[i];
      }

      return res;
    }
  }

  return NULL;
}
