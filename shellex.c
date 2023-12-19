/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 

int main(int argc, char *argv[]) 
{
    char cmdline[MAXLINE]; /* Command line */
    char option[MAXLINE];
    int flag = 0;
    if(argc > 1){
	    if(strcmp(argv[1], "-p")== 0){
		strcpy(option, argv[2]);
		flag++;
	    }
    }

    if(signal(SIGINT, sigint_handler) == SIG_ERR){
	exit(1);
    }

    while (1) {
	/* Read */
	if(flag == 0){
		printf("sh257>");
	}else{
		printf("%s>",option);
	}	
	Fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) {
    // ... (existing code)
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */
    
    if (!builtin_command(argv)) {
        if ((pid = fork()) == 0) { /* Child runs user job */
            if (execvp(argv[0], argv) < 0) {
                printf("Execution failed (in fork)\n");
                printf("%s: Command not found.\n", argv[0]);
                exit(1);
            }
        }

        /* Parent waits for foreground job to terminate */
        if (!bg) {
	    int status;
            if (waitpid(pid, &status, 0) < 0){
                unix_error("waitfg: waitpid error");
	    }
	    if(WIFEXITED(status)){
		    printf("Process exited with status code %d\n", WEXITSTATUS(status));
	    }else{
		    printf("Process exited with status code %d\n", WEXITSTATUS(status));
	    }

        } else
            printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "quit")) /* quit command */
	exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	return 1;
    if(!strcmp(argv[0], "cd")){
	cd_cmd(argv);
	return 1;
    }
    if(!strcmp(argv[0], "pid")){
        pid_cmd();
	return 1;
    }
    if(!strcmp(argv[0], "ppid")){
	ppid_cmd();
	return 1;
    }
    if(!strcmp(argv[0], "help")){
	help_cmd();
	return 1;
    }
    if(!strcmp(argv[0], "exit")){
	exit_cmd();
	return 1;
    }

    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */

/* $begin cd_cmd */
/* implementation of the cd command */
int cd_cmd(char **argv){
	if(argv[1] == NULL){
		fprintf(stderr, "cd: missing args\n");
	}else{
		if(chdir(argv[1]) != 0){
			perror("cd");
		}
	}
	return 0;
}
/* $end cd_cmd */

/* $begin pid_cmd */
/* implementation of the pid command */
int pid_cmd(){
	printf("pid: %d\n", getpid());
	return 1;
}
/* $end pid_cmd */

/* $begin ppid command */
/* implementation of the */
int ppid_cmd(){
	printf("ppid: %d\n", getppid());
	return 1; 
}
/* $end ppid_cmd */

/* $begin help command */
void help_cmd(){

   	printf("**********************************************************************\n");
	printf("A Custom Shell for CMSC 257\n");
	printf("   -  Amaris Young-Diggs\n");
	printf("Usage:\n"); 
	printf("   -  User can change prompt by typing '-p' followed by user input\n");
	printf("**********************************************************************\n\n");
	printf("BUILTIN COMMANDS:\n");
	printf("   -  cd:   changes the current directory to the directory specified by user 'cd <filepath>'\n");
	printf("   -  ppid: prints the PID of the parent process\n");
	printf("   -  pid:  prints the PID of the current process\n");
	printf("   -  help: shows information about the shell and the functionality of the built-in commands\n");
	printf("   -  exit: terminates the shell\n");
	printf("   -  quit: exits the shell\n");
	printf("SYSTEM COMMANDS:\n");
	printf("   -  To use system commands use man pages to access descriptions\n");
}
/* $end help command */

/* $begin exit command */
int exit_cmd(){
	return raise(SIGTERM);
}
/* $end exit command */

/* $begin sginal handler */
void sigint_handler(int signo){
	
}
/* $end signal handle */
