#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *a) {
    fprintf(stderr,"Usage: %s <pid>\n",a); 
    exit(1);
}

static int isnum(const char *s) {
    for(;*s; s++) { 
	if(!isdigit(*s)) {
	    return 0;
	}
    }
    return 1;
}

// retrieve /proc/<pid>/stat info
int proc_stats(char *pid, char arr[15][20]);

// retrieve /proc/<pid>/cmdline info
int proc_cmdline(char *pid, char *cmdline, int size);

// retrieve /proc/<pid>/status info
int proc_status(char *pid, char *VmRSS);


int main(int argc, char *argv[]) {
 
    if (argc!=2||!isnum(argv[1])) {
	usage(argv[0]);
    }

    // stat args 
    char args[6][20] = {0};
    
    char *pid = argv[1];
    // printf("(PID: %s)\n", pid);

    char stats[39][20] = {0};
    proc_stats(pid, stats);
    //for (int i = 0; i < 39; i++) printf("%d: %s\n", i+1, stats[i]);

    char VmRSS[10] = "";
    proc_status(pid, VmRSS);
    //for (int i = 0; i < strlen(VmRSS); i++) printf("%d. %c\n", i+1, VmRSS[i]);

    char cmdline[20] = "";
    proc_cmdline(pid, cmdline, sizeof(cmdline));

    // (1) PID
    strcpy(args[0], stats[0]);
    printf("PID:%s\n", args[0]);

    // (3) STATE
    strcpy(args[1], stats[2]);
    printf("State:%s\n", args[1]);

    // (4) PPID
    strcpy(args[2], stats[3]);
    printf("PPID:%s\n", args[2]);
    
    // CMD
    printf("Cmd:%s\n", cmdline);

    // Last Executed CPU | CPU Time (User + System)
    strcpy(args[3], stats[38]);
    
    long utime = atol(stats[12]); 
    long stime = atol(stats[13]);
    long sysHz = sysconf(_SC_CLK_TCK);

    float cpu_time = (float)(utime + stime) / sysHz;
    printf("CPU:%s %.3f\n", args[3], cpu_time);
    
    // VmRSS
    printf("VmRSS:%s\n", VmRSS);

    return 0;
}


int proc_status(char *pid, char *VmRSS){
    // syntax for command
    const char *pre = "/proc/";
    const char *suffix = "/status";
    int len = strlen(pre) + strlen(pid) + strlen(suffix);

    // str for /proc/<pid>/stat
    char cmd[len+1];
    cmd[0] = '\0';
    strcat(cmd, pre);
    strcat(cmd, pid);
    strcat(cmd, suffix);

    FILE *pFile = fopen(cmd, "r");
    char buffer[1024] = {0};

    if (pFile == NULL) {
	perror("fopen");
	return 1;
    }

    for (int i = 0; i < 23; i++) {
	fgets(buffer, sizeof(buffer), pFile);
    }

    // printf("Buffer: %s\n", buffer);
    
    int idx = 0;
    len = strlen(buffer);
    for (int j = 0; j < len; j++) {
	if (isdigit(buffer[j])) {
	    VmRSS[idx] = buffer[j];
	    idx++;
	}
    }
    
    fclose(pFile);
    
    return 0; 
}

int proc_stats(char *pid, char arr[15][20]) {
    // syntax for command
    const char *pre = "/proc/";
    const char *suffix = "/stat";
    const int len = strlen(pre) + strlen(pid) + strlen(suffix);

    // str for /proc/<pid>/stat
    char cmd[len+1];
    cmd[0] = '\0';
    strcat(cmd, pre);
    strcat(cmd, pid);
    strcat(cmd, suffix);

    // printf("str: %s | len: %ld\n", pre, strlen(pre));
    // printf("str: %s | len: %ld\n", suffix, strlen(suffix));
    // printf("str: %s | len: %ld\n", pid, strlen(pid));
    // printf("str: %s | len: %d\n", cmd, len);

    FILE *pFile = fopen(cmd, "r");
    char buffer[1024] = {0};

    if (pFile == NULL) {
	perror("fopen");
	return 1;
    }

    fgets(buffer, sizeof(buffer), pFile);
    char *tok = strtok(buffer, " ");
    strcpy(arr[0], tok);

    for (int i = 1; i < 39; i++) {
	tok = strtok(NULL, " ");
	strcpy(arr[i], tok);
    }

    fclose(pFile);

    return 0;
}

// retrieve /proc/<pid>/cmdline info 
int proc_cmdline(char *pid, char *cmdline, int size) {

    // syntax for command
    const char *pre = "/proc/";
    const char *suffix = "/cmdline";
    int len = strlen(pre) + strlen(pid) + strlen(suffix);

    // str for /proc/<pid>/stat
    char cmd[len+1];
    cmd[0] = '\0';
    strcat(cmd, pre);
    strcat(cmd, pid);
    strcat(cmd, suffix);

    FILE *pFile = fopen(cmd, "r");
    char buffer[1024] = {0};

    if (pFile == NULL) {
	perror("fopen");
	return 1;
    }

    fgets(buffer, sizeof(buffer), pFile);
    strncpy(cmdline, buffer, size - 1);
    cmdline[size-1] = '\0';
   
    fclose(pFile);

    return 0;
}

