#include "../kernel/types.h"
#include "user.h"

void primes(int pipeReadInterface){
    int prime;
    int otherNum;
    int p[2];
    pipe(p);
    if(read(pipeReadInterface, &prime,sizeof(int)) != 0){
        fprintf(1, "prime %d\n",prime);
    }else{
        close(p[0]);
        close(p[1]);
        close(pipeReadInterface);
        exit(0);
    }
    int pid = fork();
    if(pid == 0){
        // child process
        close(pipeReadInterface);
        close(p[1]);
        primes(p[0]);
    }else if(pid > 0){
        close(p[0]);
        for(;read(pipeReadInterface,&otherNum,sizeof(int)) != 0;){
            if(otherNum % prime != 0){
                write(p[1],&otherNum,sizeof(int));
            }
        }
        close(pipeReadInterface);
        close(p[1]);
        wait(&pid);
        exit(0);
    }else{
        fprintf(2, "Fork Error.\n");
    }
    exit(1);
}

int main(){
    int p[2];
    pipe(p);
    for(int i = 2; i <= 35; i++){
        write(p[1],&i,sizeof(int));
    }
    close(p[1]);    
    primes(p[0]);
    exit(0);
}

