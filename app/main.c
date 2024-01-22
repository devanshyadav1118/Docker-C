#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUT 0
#define IN 1
#define BUFFER_SIZE 4096 // read and write ends of a pipe
// Usage: your_docker.sh run <image> <command> <arg1> <arg2> ...
int main(int argc, char *argv[])
{
	// Disable output buffering
	setbuf(stdout, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	// printf("Logs from your program will appear here!\n");

	int outpipe[2], errpipe[2]; // created to set up inter-process communication using pipes.
	pipe(outpipe);
	pipe(errpipe);

	char *command = argv[3];
	int child_pid = fork();
	if (child_pid == -1)
	{
		printf("Error forking!");
		return 1;
	}
	// Uncomment this block to pass the first stage
	//
	// char *command = argv[3];
	// int child_pid = fork();
	// if (child_pid == -1) {
	//     printf("Error forking!");
	//     return 1;
	// }
	//
	if (child_pid == 0)
	{
		dup2(outpipe[1], 1);
		dup2(errpipe[1], 2);

		close(outpipe[0]);
		close(errpipe[0]);
		// Replace current program with calling program
		execv(command, &argv[3]);
	}
	else
	{
		execv(command, &argv[3]);
	}
	else
	{
		close(outpipe[IN]);
		close(errpipe[IN]);
		char buf[BUFFER_SIZE];
		memset(buf, 0, sizeof(buf));
		int bytesRead = read(outpipe[0], buf, BUFFER_SIZE);
		buf[bytesRead] = '\0';
		write(STDOUT_FILENO, buf, bytesRead);
		memset(buf, 0, sizeof(buf));
		bytesRead = read(errpipe[0], buf, BUFFER_SIZE);
		buf[bytesRead] = '\0';
		1 write(STDERR_FILENO, buf, bytesRead);
		// We're in parent
		wait(NULL);
		// printf("Child terminated");
	}

	return 0;
}
