#include "../kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]){
	int parent_read[2];
	int child_read[2];
	pipe(parent_read);
	pipe(child_read);
	int pid = fork();
	if(pid == 0){
		// child process
		// close the read end of parent_read and the write of child_read;
		close(parent_read[0]);
		close(child_read[1]);
		char buf[4];
		// read from the read end of child_read
		if(read(child_read[0],buf,4) == 0){
			fprintf(2,"Some wrong happended while reading from pipe in child process.\n");
			close(parent_read[1]);
			close(child_read[0]);
			exit(1);
		}else{
			fprintf(1, "%d: received ping\n", getpid());
			// write the pong to parent
			if(write(parent_read[1], "pong", 4) < 4){
				fprintf(2, "Some wrong happened while writting to pipe in child process.\n");
				close(parent_read[1]);
				close(child_read[0]);
				exit(1);	
			}
			close(parent_read[1]);
			close(child_read[0]);
			exit(0);
		}

	}else if (pid > 0){
		// parent process
		// close the read end of child_read and the write end of the parent_read
		close(parent_read[1]);
		close(child_read[0]);	
		if(write(child_read[1], "ping", 4) < 4){
			fprintf(2, "Some wrong happened while writting to pipe in parent process.\n");
			close(parent_read[0]);
			close(child_read[1]);
			wait(&pid);
			exit(1);
		}
		char buf[4];
		if(read(parent_read[0], buf, 4) < 4){
			fprintf(2, "Some wrong happened while reading from the pipe in parent process.\n");
			close(parent_read[0]);
			close(child_read[1]);
			wait(&pid);
			exit(1);
		}else{
			fprintf(1, "%d: received pong\n", getpid());
		}		
		close(parent_read[0]);
		close(child_read[1]);
		wait(&pid);
		exit(0);
	}else{
		fprintf(2, "Fork error.\n");
		close(parent_read[0]);
		close(child_read[1]);
	}
	exit(1);
}
