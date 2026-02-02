#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void usage(const char *a) {
    fprintf(stderr,"Usage: %s <cmd> [args]\n",a); 
    exit(1);
}

static double d(struct timespec a, struct timespec b) {
    return (b.tv_sec - a.tv_sec) + (b.tv_nsec - a.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
	usage(argv[0]);
    }

    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // printf("Hello (PID: %d) You are the parent!\n", (int) getpid()); 
    int status;
    char *args[4];
    

    int len = 0;
    for (int i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1;
    }

    char *str = malloc(len + 1);
    str[0] = '\0';

    for (int i = 1; i < argc; i++) {
        strcat(str, argv[i]);
        if (i < argc - 1) {
            strcat(str, " ");
        }
    }

    pid_t rc = fork();


    if (rc < 0) {
	fprintf(stderr, "forked failed\n");
	exit(1);

    } else if (rc == 0) {
	// printf("Hello (PID: %d) You are the child!\n", (int) getpid());
	// printf("child rc: %d\n", rc);

	char *shell = getenv("SHELL");
	if (shell == NULL) shell = "/bin/sh";

	args[0] = shell;
	// printf("args[0]: %s\n", args[0]);
	
	args[1] = "-c";
	// printf("args[1]: %s\n", args[1]);

	args[2] = str;
	// printf("args[2]: %s\n", args[2]);

	args[3] = NULL;
	// printf("args[3]\n");

	execvp(args[0], args);

    } else {
	waitpid(rc, &status, 0);

	clock_gettime(CLOCK_MONOTONIC, &end);

	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
	    // printf("Child exit with code 0.\n");
	}

	printf("pid=%d elasped=%.3f exit=%d\n", rc, d(start, end), status);

    }

    free(str);
    return 0;
}
