#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

static void run(char *temp, char **argv, int argc){
	int status=0;
	int waitret=0;
	pid_t pid;
	char **args;

	pid = fork();
	if(pid==0){
		if((args = malloc(sizeof(*args)*(argc+3)))==NULL)
			return;
		memcpy(args,argv,sizeof(*argv)*argc);
		args[argc] = temp;
		args[argc+1] = NULL;
		execvp(args[0],args);
	} else if(pid>0){
		do{
			waitret=wait(&status);
		}while(waitret!=pid && !(waitret==-1 && errno==ECHILD));

		if(WIFEXITED(status))
			;//fprintf(stderr,"%s E%d\n",temp,WEXITSTATUS(status));
		else if(WIFSIGNALED(status))
			;//fprintf(stderr,"%s S%d\n",temp,WEXITSTATUS(status));

	} else {
		return; // error. oh well
	}
}

int main(int argc, char **argv){
	char deftemplate[20]="/tmp/farg.XXXXXX";
	char *template = deftemplate;
	char *tfile;
	int fd;
	int argstart=1;
	int ret;
	char buf[100];

	if(argc>2 && strcmp(argv[1],"-t")==0){
		template = argv[2];
		argstart += 2;
	}

	if((tfile = mktemp(template))==NULL)
		return 1;
	if((fd = open(tfile,O_CREAT|O_WRONLY,0700))<0)
		return 1;


	while((ret=read(0,buf,100))>0){
		if(write(fd,buf,ret)<0)
			break;
	}
	close(fd);

	run(tfile,argv+argstart,argc-argstart);

	unlink(tfile);

	return 0;
}
