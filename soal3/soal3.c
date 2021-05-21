#include <libgen.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h> 

pthread_t tid[5000]; int result;

void bikinDirektori(char *direktorinya) {
	struct stat fileaku = {0};

	if (stat(direktorinya, &fileaku) == -1) {
		mkdir(direktorinya, 0777);
	}
}

char dapetinExt(char *namaFile, char *extension) {
    char *temp = strchr(namaFile, '.');
    if(!temp) {strcpy(extension,"Unknown");}
    else if(temp[1] == '.'){strcpy(extension,"Hidden"); }
	else{strcpy(extension,temp+1);}
}

int checkFile(char *basePath)
{
    struct stat buffer;
    int exist = stat(basePath,&buffer);
    if(exist == 0)
    {
        if(S_ISREG(buffer.st_mode)) 
           return 1;
        else 
	   return 0;
    }
    else{
		return 0;
	}     
}

void *pindahinajadeh(void *arg)
{
    char basePath[PATH_MAX];
    strcpy(basePath,(char *) arg);

    if(checkFile(basePath))
    {
        char *i,*b;
        char fullPath[PATH_MAX];
        strcpy(fullPath,(char *) arg);

        char namaFile[100];

        for(i=strtok_r(fullPath,"/",&b); i!=NULL; i=strtok_r(NULL,"/",&b)) 
		{
            memset(namaFile,0,sizeof(namaFile));
            strcpy(namaFile,i);
        }

        char extension[PATH_MAX];
        dapetinExt(namaFile, extension);

        if(strcmp(extension,"Hidden") != 0 && strcmp(extension,"Unknown") != 0)
        {
            for(int i = 0; i<strlen(extension); i++)
            {
                if(extension[i]>64 && extension[i]<91){
					extension[i]+=32;
				}
            }
        }
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
		{
            perror("getcwd() error");
            return (void *) 0;
        }

       
        char destDir[PATH_MAX];
        sprintf(destDir,"%s/%s",cwd,extension);
        
        bikinDirektori(destDir);

        char destination[PATH_MAX];
        sprintf(destination,"%s/%s/%s",cwd,extension,namaFile);
       
        rename(basePath,destination);
        
        return (void *) 1;
    }

    else return (void *) 0;
}

void fungsinyagaes(char *argv){
	int it=-1; struct dirent *direk;
   	DIR *direktorinya = opendir(argv);
	
	while ((direk = readdir(direktorinya)) != NULL) {
		char pathnya[300];
		
		if (strcmp(direk->d_name, ".") != 0 && strcmp(direk->d_name, "..") != 0) {
			if(direk->d_type == DT_REG) {
				
				strcpy(pathnya,"");strcat(pathnya, argv);strcat(pathnya, "/");strcat(pathnya, direk->d_name);
				pthread_create(&tid[++it], NULL, pindahinajadeh, (void *) pathnya);
				pthread_join(tid[it], NULL);
			}
			else if(direk->d_type == DT_DIR) {
				struct dirent *direk1;
   				DIR *direktorinya1 = opendir(argv);
				char pathnya0[300];
				strcpy(pathnya0,"");strcat(pathnya0, argv);strcat(pathnya0, "/");strcat(pathnya0, direk->d_name);
				fungsinyagaes(pathnya0);
   				closedir(direktorinya1);
			}
		}
	}
	closedir(direktorinya);
}

int main(int argc, char* argv[])
{
    if(strcmp(argv[1],"-f")==0) 
    {
	int i=0;
		for(int j=2;j<argc;j++)
		{
		    pthread_create(&(tid[i]), NULL, pindahinajadeh, (void*) argv[j]);
		    i++;
		}
		for (int j=0;j<(i);j++)
		{
		    long dicek;
		    void *st;
		    pthread_join( tid[j], &st);
		    dicek = (long) st;
			// printf("%ld", dicek);
		    if(dicek == 1) 
			printf("File %d : Berhasil Dikategorikan\n", j+1);
		    else 
			printf("File %d : Sad, gagal :(\n", j+1);
		}
        
        return 0;
    }
	
	else if(strcmp(argv[1], "-d") == 0){
			if( ENOENT != errno ) {
				fungsinyagaes(argv[2]);
				printf("Direktori sukses disimpan!\n");
			}
			else {
				printf("Yah, gagal disimpan :(\n");
			}
	}

	else if (strcmp(argv[1], "*") == 0) {
		char cwd[300];
		getcwd(cwd, sizeof(cwd));
		fungsinyagaes(cwd);
	}
	return 0;
}