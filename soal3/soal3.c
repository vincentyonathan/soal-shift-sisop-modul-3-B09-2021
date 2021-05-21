#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h> 
#include <libgen.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

pthread_t tid[5000]; int result;

void bikinDirektori(char *direktorinya) {
	struct stat fileaku = {0};

	if (stat(direktorinya, &fileaku) == -1) {
		mkdir(direktorinya, 0777);
	}
}

char *dapetinExt(char *namaFile) {
	if(namaFile[0] == '.') return "Hidden";
    char *temp = strchr(namaFile, '.');
    if(!temp) return "Unknown";
    if(temp[1] == '.') return "Hidden"; 
    return temp + 1;
}

void *pindahinajadeh(void *argv){
	char *pathnya, cwd[300], folderExt[300], temp1[300], nahini[300]; 
	
	pathnya = (char *)argv;	
	getcwd(cwd, sizeof(cwd));

	char* file = basename(pathnya);
	strcpy(folderExt, dapetinExt(file));
	
	int i;
	for(i = 0; i < strlen(folderExt); i++){
		if(folderExt[i] > 64 && folderExt[i] < 91){
			folderExt[i] += 32;
		}
	}
	
	strcpy(nahini,"");strcat(nahini, cwd);strcat(nahini, "/");strcat(nahini,folderExt);
	bikinDirektori(nahini);
	strcat(nahini, "/");strcat(nahini, file);
	
	result = rename(pathnya , nahini); 
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
    int x, y; 
	x = 2;
    if(strcmp(argv[1], "-f") == 0){
        while(argv[x] != NULL){
            pthread_create(&(tid[x-2]), NULL, pindahinajadeh, (void *)argv[x]);
            if(result==0){printf("FILE %d: Berhasil Dikategorikan\n", x-1);}
            else{printf("FILE %d: Sad, gagal :(\n", x-1);}
            x++;
        }
        for(y = 0; y < (x-1); y++){
            pthread_join(tid[y], NULL);
        }
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