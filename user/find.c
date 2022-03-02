#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
#include "../kernel/fs.h"
char* fmtname(char *path){
    static char buf[DIRSIZ+1];
    memset(buf,0,sizeof(buf));
    char *p;
    // Find first character after last slash
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    if(strlen(p)>=DIRSIZ)
        return p;
    memmove(buf,p,strlen(p));
    return buf;
}
void find(char* filename,char *path){
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;
    // first, open path
    if ((fd = open(path,0)) < 0){
        fprintf(2, "find: cannot open %s \n",path);
        return;
    }
    // get stat of path
    if (fstat(fd,&st) < 0){
        fprintf(2,"find: cannot stat %s \n",path);
        close(fd);
        return;
    }
    switch(st.type){
        case T_FILE:
            if(strcmp(fmtname(path),filename) == 0){
                fprintf(1,"%s\n",path);
            }
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
                fprintf(2,"**find: path too long [%s] and find will not check this directory!**\n",path);
                break;
            }
            strcpy(buf,path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd,&de,sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                if(strcmp(".",de.name) == 0 || strcmp("..",de.name) == 0){
                    // don't recurision in . and ..
                    // fprintf(1,"i will not recurison in %s\n",de.name);
                    continue;
                }
                // recursion in this dir
                memmove(p,de.name,DIRSIZ);
                p[DIRSIZ] = 0;
                find(filename,buf);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc == 1){
        fprintf(1,"Find: Usage find [FileName]\n");
        exit(0);
    }
    for(int i = 1;i<argc;i++){
        find(argv[i],".");
    }
    exit(0);
}