#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

/*
 * Runs a parsed pipeline.
 *   - a single built-in (cd, pwd, echo, exit) runs inside the shell itself
 *   - everything else is run with fork() + execvp() and waited for with waitpid()
 *   - supports  <  >  >>  |  and  &  (background)
 * Returns 1 if the shell should exit, 0 otherwise.
 */
int execute_pipeline(pipeline_t *pipeline);

/*
 * Installs the SIGCHLD handler that reaps finished background jobs so they
 * do not stay behind as zombie processes.  Call once at start-up.
 */
void setup_background_handler(void);

#endif
