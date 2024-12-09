#include "shell.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

env_var env_vars[MAX_ENV_VARS];
int env_var_count = 0;

void set_env_var(const char *name, const char *value) {
    for (int ix = 0; ix < env_var_count; ix++) {
        if (strcmp(env_vars[ix].name, name) == 0) {
            free(env_vars[ix].value);
            env_vars[ix].value = strdup(value);
            return;
        }
    }
    if (env_var_count < MAX_ENV_VARS) {
        env_vars[env_var_count].name = strdup(name);
        env_vars[env_var_count].value = strdup(value);
        env_var_count++;
    } else {
        fprintf(stderr, "Environment variable limit reached\n");
    }
}

void unset_env_var(const char *name) {
    for (int ix = 0; ix < env_var_count; ix++) {
        if (strcmp(env_vars[ix].name, name) == 0) {
            free(env_vars[ix].name);
            free(env_vars[ix].value);
            env_vars[ix] = env_vars[env_var_count - 1];
            env_var_count--;
            return;
        }
    }
}

char *get_env_var(const char *name) {
    for (int ix = 0; ix < env_var_count; ix++) {
        if (strcmp(env_vars[ix].name, name) == 0) {
            return env_vars[ix].value;
        }
    }
    return NULL;
}

void replace_env_var(char *cmd) {
    char buffer[1000];
    char *current = cmd;
    char *output = buffer;

    while (*current) {
        if (*current == '$' && isalnum(*(current + 1))) {
            current++;
            char variable_name[100];
            int name_length = 0;

            while (isalnum(*current) && name_length < sizeof(variable_name) - 1) {
                variable_name[name_length++] = *current++;
            }
            variable_name[name_length] = '\0';

            char *value = get_env_var(variable_name);
            if (value) {
                while (*value) {
                    *output++ = *value++;
                }
            }
        } else {
            *output++ = *current++;
        }
    }
    *output = '\0';
    strcpy(cmd, buffer);
}

void free_env_vars() {
    for (int ix = 0; ix < env_var_count; ix++) {
        free(env_vars[ix].name);
        free(env_vars[ix].value);
    }
    env_var_count = 0;
}