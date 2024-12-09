#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <fcntl.h> 
#include <unistd.h>

#define COMMAND_SEPARATOR "|"
#define READ_SIDE 0
#define WRITE_SIDE 1
#define MAX_ENV_VARS 100

typedef struct {
    char *name;
    char *value;
} env_var;

extern env_var env_vars[MAX_ENV_VARS];
extern int env_var_count;

void add_character_to_string(char *string, char character);
void split(char *cmd, char *words[], char delimiter);
bool find_absolute_path(char *cmd, char *absolute_path);
void execute_command(char *cmd, char *args[], int input_fd, int output_fd, bool background);
void set_env_var(const char *name, const char *value);
void unset_env_var(const char *name);
char *get_env_var(const char *name);
void replace_env_var(char *cmd);
void free_env_vars();

void process_input(char *input); 
void handle_redirections(char *args[], int *input_fd);
void handle_internal_command(char *args[]);
bool check_if_background(char *args[]);
bool internal_commands(char *cmd);

#endif
