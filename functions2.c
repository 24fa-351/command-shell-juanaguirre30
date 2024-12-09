#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool internal_commands(char *cmd) {
    return (strcmp(cmd, "cd") == 0 || 
            strcmp(cmd, "pwd") == 0 || 
            strcmp(cmd, "set") == 0 || 
            strcmp(cmd, "unset") == 0);
}

void process_input(char *input) {
    replace_env_var(input);
    char *commands[100];
    split(input, commands, '|');

    int num_commands = 0;
    while (commands[num_commands] != NULL) {
        num_commands++;
    }

    int input_fd = STDIN_FILENO;
    int pipe_fd[2];

    for (int ix = 0; ix < num_commands; ix++) {
        char *args[100];
        split(commands[ix], args, ' ');

        handle_redirections(args, &input_fd);

        if (internal_commands(args[0])) {
            handle_internal_command(args);
        } else {
            if (ix < num_commands - 1) {
                pipe(pipe_fd);
                execute_command(args[0], args, input_fd, pipe_fd[WRITE_SIDE], false);
                close(pipe_fd[WRITE_SIDE]);
                input_fd = pipe_fd[READ_SIDE];
            } else {
                bool background = check_if_background(args);
                execute_command(args[0], args, input_fd, STDOUT_FILENO, background);
            }
        }
    }

    if (input_fd != STDIN_FILENO) {
        close(input_fd); 
    }
}

void handle_redirections(char *args[], int *input_fd) {
    for (int jx = 0; args[jx] != NULL; jx++) {
        if (strcmp(args[jx], "<") == 0) {
            if (args[jx + 1] == NULL) {
                fprintf(stderr, "Error: No file specified for input redirection\n");
                return;
            }
            *input_fd = open(args[jx + 1], O_RDONLY);
            if (*input_fd < 0) {
                perror("Error opening input file");
                return;
            }
            args[jx] = NULL;
        } else if (strcmp(args[jx], ">") == 0) {
            if (args[jx + 1] == NULL) {
                fprintf(stderr, "Error: No file specified for output redirection\n");
                return;
            }
            int output_fd = open(args[jx + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd < 0) {
                perror("Error opening output file");
                return;
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            args[jx] = NULL;
        }
    }
}

void handle_internal_command(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: Missing argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd");
        }
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
    } else if (strcmp(args[0], "set") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            set_env_var(args[1], args[2]);
        } else {
            fprintf(stderr, "Usage: set <var> <value>\n");
        }
    } else if (strcmp(args[0], "unset") == 0) {
        if (args[1] != NULL) {
            unset_env_var(args[1]);
        } else {
            fprintf(stderr, "Usage: unset <var>\n");
        }
    }
}

bool check_if_background(char *args[]) {
    int index = 0;
    while (args[index] != NULL) {
        index++;
    }

    if (index > 0 && strcmp(args[index - 1], "&") == 0) {
        args[index - 1] = NULL; 
        return true;
    }
    return false;
}

