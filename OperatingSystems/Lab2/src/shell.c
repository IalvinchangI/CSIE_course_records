#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../include/command.h"
#include "../include/builtin.h"

// ======================= requirement 2.3 =======================
/**
 * @brief 
 * Redirect command's stdin and stdout to the specified file descriptor
 * If you want to implement ( < , > ), use "in_file" and "out_file" included the cmd_node structure
 * If you want to implement ( | ), use "in" and "out" included the cmd_node structure.
 *
 * @param p cmd_node structure
 * 
 */
void redirection(struct cmd_node *p){
	// redirect
	if (p->in_file != NULL) {  // change stdin
		int input_fd = open(p->in_file, O_RDONLY, 0644);
		// p.s. By default, the new file descriptor is set to remain open across an execve
		// 		(i.e., the FD_CLOEXEC file descriptor flag described in fcntl is initially disabled)
		if (input_fd == -1) {  // error
			// file does not exist
			perror(p->in_file);
		}
		else {
			if (dup2(input_fd, STDIN_FILENO) == -1) {  // error
				perror("dup2");
			}
		}
	}
	if (p->out_file != NULL) {  // change stdout
		int output_fd = open(p->out_file, O_CREAT | O_WRONLY, 0644);
		if (dup2(output_fd, STDOUT_FILENO) == -1) {  // error
			perror("dup2");
		}
	}

	// pipe
	if (p->in != STDIN_FILENO) {  // change stdin
		if (dup2(p->in, STDIN_FILENO) == -1) {  // error
			perror("dup2");
		}
	}
	if (p->out != STDOUT_FILENO) {  // change stdout
		if (dup2(p->out, STDOUT_FILENO) == -1) {  // error
			perror("dup2");
		}
	}
}
// ===============================================================

// ======================= requirement 2.2 =======================
/**
 * @brief 
 * Execute external command
 * The external command is mainly divided into the following two steps:
 * 1. Call "fork()" to create child process
 * 2. Call "execvp()" to execute the corresponding executable file
 * @param p cmd_node structure
 * @return int 
 * Return execution status
 */
int spawn_proc(struct cmd_node *p)
{
	int pid = fork();
	if (pid == -1) {				// error handling
		perror("fork");
	}
	else if (pid == 0) {			// child
		redirection(p);
		execvp(p->args[0], p->args);
	}
	else {
		int status;
		waitpid(pid, &status, 0);  	// wait for child
		// p.s. 0 means waits only for terminated children

		if (WIFEXITED(status) != 1) {  // error handling
			perror(p->args[0]);
		}
	}
  	return 1;
}
// ===============================================================


// ======================= requirement 2.4 =======================
/**
 * @brief 
 * Use "pipe()" to create a communication bridge between processes
 * Call "spawn_proc()" in order according to the number of cmd_node
 * @param cmd Command structure  
 * @return int
 * Return execution status 
 */
int fork_cmd_node(struct cmd *cmd)
{
	int *pids = (int*)malloc(cmd->pipe_num * sizeof(int));

	// setup in/out
	struct cmd_node *current = cmd->head;
	int pipe_fd[2] = {};
	int previous_pipe_fd0 = -1;
	for (int i = 0; current != NULL; i++, current = current->next) {
		// setup pipe
		if (current->next != NULL) {
			if (pipe(pipe_fd) == -1) {
				perror("pipe");
			}
			current->out = pipe_fd[1];
		}

		// new process
		pids[i] = fork();
		if (pids[i] == -1) {				// error handling
			perror("fork");
		}
		else if (pids[i] == 0) {			// child (current)
			if (current->next != NULL) {  // child close none use pipe (for child)
				close(pipe_fd[0]);
			}
			redirection(current);
			execvp(current->args[0], current->args);
		}
		// parent
		if (previous_pipe_fd0 != -1) {  // parent close none use pipe (for further usage)
			close(previous_pipe_fd0);
		}
		if (current->next != NULL) {
			// transfer input of pipe to next process
			current->next->in = pipe_fd[0];
			previous_pipe_fd0 = pipe_fd[0];
			// parent close none use pipe (for further usage)
			close(pipe_fd[1]);
		}
	}

	// wait pid
	current = cmd->head;
	for (int i = 0; current != NULL; i++, current = current->next) {
		int status;
		waitpid(pids[i], &status, 0);  	// wait for child
		// p.s. 0 means that it waits only for terminated children

		if (WIFEXITED(status) != 1) {  // error handling
			perror(current->args[0]);
			// close all process behind
		}
	}
	free(pids);
	return 1;
}
// ===============================================================


void shell()
{
	while (1) {
		// char *current_working_directory = get_current_dir_name();
		printf(">>> $ ");
		char *buffer = read_line();
		if (buffer == NULL)
			continue;

		struct cmd *cmd = split_line(buffer);
		
		int status = -1;
		// only a single command
		struct cmd_node *temp = cmd->head;
		
		if(temp->next == NULL){
			status = searchBuiltInCommand(temp);
			if (status != -1){
				int in = dup(STDIN_FILENO), out = dup(STDOUT_FILENO);
				if( in == -1 || out == -1)
					perror("dup");
				redirection(temp);
				status = execBuiltInCommand(status,temp);

				// recover shell stdin and stdout
				if (temp->in_file)  dup2(in, 0);
				if (temp->out_file){
					dup2(out, 1);
				}
				close(in);
				close(out);
			}
			else{
				//external command
				status = spawn_proc(cmd->head);
			}
		}
		// There are multiple commands ( | )
		else{
			
			status = fork_cmd_node(cmd);
		}
		// free space
		while (cmd->head) {
			
			struct cmd_node *temp = cmd->head;
      		cmd->head = cmd->head->next;
			free(temp->args);
   	    	free(temp);
   		}
		free(cmd);
		free(buffer);
		
		if (status == 0)
			break;
	}
}
