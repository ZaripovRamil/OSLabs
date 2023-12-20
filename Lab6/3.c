#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void copy_file(const char *source, const char *destination) {
   FILE *src, *dst;
   char ch;
   src = fopen(source, "r");
   dst = fopen(destination, "w");
   while ((ch = fgetc(src)) != EOF)
       fputc(ch, dst);
   fclose(src);
   fclose(dst);
}

void split_copy_dir(char *source, char *destination, int thread, int totalThreads) {
    DIR *d = opendir(source);
    struct dirent *dir;
    int i = 0;
    while((dir = readdir(d)) != NULL)
    {
	if(strcmp(dir->d_name, ".") == 0 ||strcmp(dir->d_name, "..") ==0)
	    continue;
	struct stat st;
        char srcpath[1024], dstpath[1024];
	sprintf(srcpath, "%s/%s", source, dir->d_name);
	sprintf(dstpath, "%s/%s", destination, dir->d_name);
	struct stat src_st;
	stat(srcpath, &src_st);
	if(S_ISDIR(src_st.st_mode))
		continue;
        if((i%totalThreads) == thread)
	{
	    copy_file(srcpath, dstpath);
	}
	i+=1;
    }
}

int main(int argc, char *argv[]) {
   pid_t pid = getpid();
   char *srcdir = argv[1];
   char *dstdir = argv[2];
   int n = atoi(argv[3]);
   int i;
      for(i = 0; i<n;i++){
          pid = fork();
          if(pid == 0){
     		split_copy_dir(srcdir, dstdir, i, n);
		return 0;
          }
       }
   return 0;
}
