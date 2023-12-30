#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void copy_file(const char *source, const char *destination, int batchNumber) {
   FILE *src, *dst;
   char ch;
   struct stat st;
   stat(source, &st);
   pid_t pid;
   pid = getpid();
   printf("Process with pid = %d copies file %s of size %ld in batch %d\n", pid, source, st.st_size, batchNumber);
   src = fopen(source, "r");
   dst = fopen(destination, "w");
   while ((ch = fgetc(src)) != EOF)
       fputc(ch, dst);
   fclose(src);
   fclose(dst);
   chmod(destination, st.st_mode);
   chown(destination, st.st_uid, st.st_gid);
}

int main(int argc, char *argv[]) {
   pid_t pid = getpid();
   char *srcdir = argv[1];
   char *dstdir = argv[2];
   int n = atoi(argv[3]);
   int batchNumber = 0;
   DIR *d = opendir(srcdir);
    struct dirent *dir;
   int continueFlag = 0;
   while(continueFlag == 0){
printf("Batch %d started\n", batchNumber);
   for(int i = 0; i< n;i++){
   if((dir = readdir(d)) != NULL)
   {
	if(strcmp(dir->d_name, ".") == 0 ||strcmp(dir->d_name, "..") ==0)
	    continue;
        char srcpath[1024], dstpath[1024];
	sprintf(srcpath, "%s/%s", srcdir, dir->d_name);
	sprintf(dstpath, "%s/%s", dstdir, dir->d_name);
	struct stat src_st;
	if(stat(dstpath, &src_st)==0)
		continue;
	if(S_ISDIR(src_st.st_mode))
		continue;
        pid = fork();
	if(pid == 0){
		copy_file(srcpath, dstpath, batchNumber);
		return 0;
	}
    }
    else continueFlag = -1;
}
while(wait()>0){}
printf("Batch %d finished\n", batchNumber);
batchNumber++;
}
   return 0;
}
