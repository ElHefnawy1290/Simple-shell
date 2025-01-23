#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 100
#define clear() printf("\033[H\033[J")

void InitShell()
{
    clear();
    printf("\n\n\t\t\t ================================================================================== \n");
    printf("\t\t\t          ===================== WELCOME TO OUR SHELL ===================== \n\n");
    sleep(2);
    clear();
}

void PrintPrompt()
{
    char cwd[1024];
    char *username = getenv("USER");
    char hostname[1024];

    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, sizeof(cwd));

    printf("[%s@%s:%s]$ ", username, hostname, cwd);
    fflush(stdout);
}

void TakeInput(char *buffer, size_t size)
{
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
}

int ExecuteCommand(char *command)
{
    int pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        return -1;
    }

    if (pid == 0)
    {
        // Child process
        char *args[MAX_ARGS];
        char *token = strtok(command, " ");

        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (execvp(args[0], args) < 0)
        {
            perror("Execution failed");
            _exit(EXIT_FAILURE);
        }
    }

    // Parent process
    wait(NULL);
    return 0;
}

int HandlePiping(char *command)
{
    char *commands[2];
    char *token = strtok(command, "|");

    int i = 0;
    while (token != NULL && i < 2)
    {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }

    if (i < 2)
    {
        fprintf(stderr, "Invalid pipe command\n");
        return -1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("Pipe failed");
        return -1;
    }

    int pid1 = fork();
    if (pid1 == 0)
    {
        // First child process
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[0]); // Close unused read end
        close(pipefd[1]);
        ExecuteCommand(commands[0]);
        _exit(EXIT_FAILURE);
    }

    int pid2 = fork();
    if (pid2 == 0)
    {
        // Second child process
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[1]); // Close unused write end
        close(pipefd[0]);
        ExecuteCommand(commands[1]);
        _exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);

    return 0;
}

int HandleRedirection(char *command)
{
    char *args[MAX_ARGS];
    char *redirection = strstr(command, ">>") ? ">>" : ">";

    char *before_redirection = strtok(command, redirection);
    char *file = strtok(NULL, " ");

    if (!file)
    {
        fprintf(stderr, "Invalid redirection syntax\n");
        return -1;
    }

    int pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        return -1;
    }

    if (pid == 0)
    {
        int fd;
        if (strcmp(redirection, ">>") == 0)
            fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd < 0)
        {
            perror("File open failed");
            _exit(EXIT_FAILURE);
        }

        dup2(fd, STDOUT_FILENO); // Redirect stdout to the file
        close(fd);

        ExecuteCommand(before_redirection);
        _exit(EXIT_FAILURE);
    }

    wait(NULL);
    return 0;
}

int HandleCompoundCommands(char *command)
{
    char *separator = strstr(command, "&&") ? "&&" : ";";

    char *cmd1 = strtok(command, separator);
    char *cmd2 = strtok(NULL, "");

    if (!cmd2)
    {
        fprintf(stderr, "Invalid compound command syntax\n");
        return -1;
    }

    if (ExecuteCommand(cmd1) == 0 || strcmp(separator, ";") == 0)
    {
        ExecuteCommand(cmd2);
    }

    return 0;
}

int main()
{
    InitShell();

    while (1)
    {
        char command[MAX_COMMAND_LEN];

        PrintPrompt();
        TakeInput(command, sizeof(command));

        if (strcmp(command, "exit") == 0)
        {
            printf("Thank You For Using Our Shell\n");
            sleep(1);
            break;
        }

        if (strchr(command, '|'))
        {
            HandlePiping(command);
        }
        else if (strstr(command, ">") || strstr(command, ">>"))
        {
            HandleRedirection(command);
        }
        else if (strstr(command, ";") || strstr(command, "&&"))
        {
            HandleCompoundCommands(command);
        }
        else
        {
            ExecuteCommand(command);
        }
    }

    return 0;
}
