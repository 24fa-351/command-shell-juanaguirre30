#include "shell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

void add_character_to_string(char *string, char character) {
    int length = strlen(string);
    string[length] = character;
    string[length + 1] = '\0';
}

void split(char *cmd, char *words[], char delimiter) {
    int word_count = 0;
    char *next_char = cmd;
    char current_word[1000];
    current_word[0] = '\0'; 

    while (*next_char != '\0') {
        if (*next_char == delimiter) {
            if (strlen(current_word) > 0) { 
                words[word_count++] = strdup(current_word);
                current_word[0] = '\0'; 
            }
        } else {
            add_character_to_string(current_word, *next_char);
        }
        next_char++;
    }

    if (strlen(current_word) > 0) { 
        words[word_count++] = strdup(current_word);
    }

    words[word_count] = NULL; 
}


bool find_absolute_path(char *cmd, char *absolute_path) {
    char *directories[1000];
    split(getenv("PATH"), directories, ':');

    for (int ix = 0; directories[ix] != NULL; ix++) {
        char path[1000];
        strcpy(path, directories[ix]);
        strcat(path, "/");
        strcat(path, cmd);

        if (access(path, X_OK) == 0) {
            strcpy(absolute_path, path);
            return true;
        }
    }
    return false;
}

void execute_command(char *cmd, char *args[], int input_fd, int output_fd, bool background) {
    int child_pid = fork();

    if (child_pid == 0) {
        if (input_fd != STDIN_FILENO) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        char absolute_path[1000];
        if (!find_absolute_path(cmd, absolute_path)) {
            fprintf(stderr, "Command not found: %s\n", cmd);
            _exit(1);
        }

        execv(absolute_path, args);

        fprintf(stderr, "Error executing command: %s\n", cmd);
        _exit(1);
    } else {
        if (!background) {
            int status;
            waitpid(child_pid, &status, 0);
        } else {
            printf("Process running in background with PID %d\n", child_pid);
        }
    }
}