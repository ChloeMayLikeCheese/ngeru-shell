#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>

// global variable totrack the shells current working directory
char cwd[PATH_MAX];

char *get_homedir(void){
    return getenv("HOME");
}

void startup() {
    char* fastfetch = "~/shell/fastfetch-runner.sh";
    system(fastfetch);
    char *homedir = get_homedir();
    chdir(homedir);

    printf(" ／l、\n（ﾟ､ ｡７ \n l、ﾞ~ヽ \n じしf_,)ノ\n");

}
// function to ignore SIGINT in the parent process (shell)
void handle_sigint(int sig) {
    // do nothing
}
void update_cwd() {
    // update the global cwd variable to the current directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd failed");
    }
}

void run_command(char *input) {
    char *args[100];
    char *token = strtok(input, " ");
    int i = 0;

    // tokenize the input into command
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // null terminate the array of arguments

    // handle the "cd" command directly in the parent process
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL) {
            // aattempt to change the directory
            if (chdir(args[1]) == -1) {
                perror("cd error");
            } else {
                // update the global cwd variable
                update_cwd();
            }
        } else {
            printf("cd: missing argument\n");
        }
        return;
    }

    // handle the "exit" command in the parent process
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting...\n");
        exit(0);
    }

    // fork and execute commands using execvp in the child process
    pid_t pid = fork();
    if (pid == 0) {
        // in the child process, run the command

        // reset the signal handler for the child process to default
        signal(SIGINT, SIG_DFL);

        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(1);  // make sure the child process exits if execvp fails
    } else if (pid > 0) {
        // wait for the child to finish
        wait(NULL);
    } else {
        // fork fail
        perror("Fork failed");
    }
}

int main() {
    
    startup();
    
    char input[100];
    // set up signal handler
    signal(SIGINT, handle_sigint);



    while (1) {
        printf("%s 🐈/>  ",cwd);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;  // remove newline from input

            // if input is true, run it
            if (strlen(input) > 0) {
                run_command(input);
            }
            // if not, dont
        } else {
            printf("Error reading input.\n");
            break;
        }
    }

    return 0;
}
