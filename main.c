#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char input[1000];

    while (true) {
        printf("xsh# ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            break;
        }

        process_input(input);
    }

    free_env_vars();
    return 0;
}
