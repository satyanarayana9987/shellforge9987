#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"
#include "builtin.h"

/* Apply  < file  and  > file / >> file  to this process.
   Returns 0 on success, -1 on error (message already printed). */
static int apply_redirections(const command_t *cmd)
{
    if (cmd->input[0] != '\0') {
        int fd = open(cmd->input, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "shellforge: %s: %s\n", cmd->input, strerror(errno));
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("shellforge: dup2");
            close(fd);
            return -1;
        }
        close(fd);
    }

    if (cmd->output[0] != '\0') {
        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
        int fd = open(cmd->output, flags, 0644);
        if (fd < 0) {
            fprintf(stderr, "shellforge: %s: %s\n", cmd->output, strerror(errno));
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("shellforge: dup2");
            close(fd);
            return -1;
        }
        close(fd);
    }

    return 0;
}

/* A built-in must run in the shell process itself (so `cd` really changes
   the shell's directory).  If it has a redirection, redirect temporarily. */
static int run_builtin_in_shell(command_t *cmd)
{
    int redirected = (cmd->input[0] != '\0' || cmd->output[0] != '\0');
    int saved_in = -1, saved_out = -1;
    int result = 0;

    if (redirected) {
        fflush(stdout);
        saved_in  = dup(STDIN_FILENO);
        saved_out = dup(STDOUT_FILENO);

        if (apply_redirections(cmd) < 0) {
            if (saved_in  >= 0) { dup2(saved_in,  STDIN_FILENO);  close(saved_in);  }
            if (saved_out >= 0) { dup2(saved_out, STDOUT_FILENO); close(saved_out); }
            return 0;
        }
    }

    result = execute_builtin(cmd);

    if (redirected) {
        fflush(stdout);
        if (saved_in  >= 0) { dup2(saved_in,  STDIN_FILENO);  close(saved_in);  }
        if (saved_out >= 0) { dup2(saved_out, STDOUT_FILENO); close(saved_out); }
    }

    return result;
}

/* Code that runs in the child process. Never returns. */
static void run_in_child(command_t *cmd)
{
    if (apply_redirections(cmd) < 0) {
        _exit(1);
    }

    if (is_builtin(cmd->argv[0])) {
        execute_builtin(cmd);
        fflush(stdout);
        _exit(0);
    }

    execvp(cmd->argv[0], cmd->argv);

    /* execvp only returns if it failed */
    if (errno == ENOENT) {
        fprintf(stderr, "shellforge: %s: command not found\n", cmd->argv[0]);
        _exit(127);
    }
    fprintf(stderr, "shellforge: %s: %s\n", cmd->argv[0], strerror(errno));
    _exit(126);
}

static void wait_for(pid_t pid)
{
    int status;
    while (waitpid(pid, &status, 0) < 0 && errno == EINTR) {
        /* interrupted by a signal: try again */
    }
}

int execute_pipeline(pipeline_t *pipeline)
{
    int n = pipeline->command_count;

    /* collect finished background jobs so they don't stay as zombies */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
    }

    if (n < 1) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        if (pipeline->commands[i].argc == 0) {
            if (n > 1) {
                fprintf(stderr, "shellforge: syntax error: missing command near '|'\n");
            }
            return 0;
        }
    }

    /* Single built-in: run inside the shell, no fork */
    if (n == 1 && is_builtin(pipeline->commands[0].argv[0])) {
        return run_builtin_in_shell(&pipeline->commands[0]);
    }

    /* Everything else: fork + execvp (one child per command, joined by pipes) */
    pid_t pids[MAX_COMMANDS];
    int started = 0;
    int prev_read = -1;

    fflush(NULL);   /* so buffered output isn't copied into the children */

    for (int i = 0; i < n; i++) {
        int fd[2] = { -1, -1 };

        if (i < n - 1 && pipe(fd) < 0) {
            perror("shellforge: pipe");
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("shellforge: fork");
            if (fd[0] >= 0) close(fd[0]);
            if (fd[1] >= 0) close(fd[1]);
            break;
        }

        if (pid == 0) {
            if (prev_read >= 0) {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }
            if (fd[1] >= 0) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            if (fd[0] >= 0) {
                close(fd[0]);
            }
            run_in_child(&pipeline->commands[i]);
        }

        /* parent */
        pids[started++] = pid;
        if (prev_read >= 0) close(prev_read);
        if (fd[1] >= 0)     close(fd[1]);
        prev_read = fd[0];
    }

    if (prev_read >= 0) {
        close(prev_read);
    }

    if (pipeline->commands[n - 1].background) {
        if (started > 0) {
            printf("[background] pid %d\n", (int)pids[started - 1]);
        }
    } else {
        for (int i = 0; i < started; i++) {
            wait_for(pids[i]);
        }
    }

    return 0;
}
