#include<stdio.h>
#include<pthread.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/stat.h>
#include<dirent.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[5000];char *helloDir, *placeDir;

void pindahinfilenya(char asal[], char tujuan[]){
    int heyyo;FILE *piw1, *piw2;
    piw1 = fopen(asal, "r");piw2 = fopen(tujuan, "w");

    if(!piw1){fclose(piw2);return ;}
    if(!piw2){return;}
    while((heyyo = fgetc(piw1)) != EOF) {fputc(heyyo, piw2);}

    fclose(piw1);fclose(piw2);remove(asal);
    return ;
}

char* namafile(char str[]){
    char *hellow, *hasil;
    hellow = strchr(str,'/');
    if(hellow == NULL){return str;}
    while(hellow != NULL) {hasil = hellow+1;hellow = strchr(hellow+1,'/');}
    return hasil;
}

char* dapetinExt(char str[]){
    char* hasil = strchr(namafile(str), '.');
    if(hasil == NULL) return NULL;
    else return (hasil+1);
}

void *pindahinajadeh(void *arg){
    pthread_t idnya = pthread_self();char *folderExt;char tujuanFolder[5000];
    folderExt = dapetinExt((char *)arg);

    if(folderExt == NULL){strcpy(tujuanFolder, "Unknown");}
    else{
        strcpy(tujuanFolder, folderExt);
        for(int i = 0; i < strlen(tujuanFolder); i++){
            if(tujuanFolder[i] > 64 && tujuanFolder[i] < 91) {tujuanFolder[i] += 32;}
        }
    }
    if(mkdir(tujuanFolder, 0777) == -1);
    char tujuanPathnya[5000]; char asalPathnya[5000];
    snprintf(asalPathnya, 5000, "%s/%s", helloDir, (char *)arg);
    snprintf(tujuanPathnya, 5000, "%s/%s/%s", helloDir, tujuanFolder, namafile((char *)arg)); pindahinfilenya((char *)arg, tujuanPathnya);
}

void *pindahinajadeh2(void *arg){
    pthread_t idnya = pthread_self();char *folderExt;char tujuanFolder[5000];
    folderExt = dapetinExt((char *)arg);

    if(folderExt == NULL){strcpy(tujuanFolder, "Unknown");}
    else{
        strcpy(tujuanFolder, folderExt);
        for(int i = 0; i < strlen(tujuanFolder); i++){
            if(tujuanFolder[i] > 64 && tujuanFolder[i] < 91) {tujuanFolder[i] += 32;}
        }
    }
    if(mkdir(tujuanFolder, 0777) == -1);
    char tujuanPathnya[5000]; char asalPathnya[5000];
    snprintf(asalPathnya, 5000, "%s/%s", placeDir, (char *)arg);
    snprintf(tujuanPathnya, 5000, "%s/%s/%s", helloDir, tujuanFolder, namafile((char *)arg)); pindahinfilenya((char *)arg, tujuanPathnya);
}

void fungsinyagaes(char **argv){
    char tralal[1000]; 
    helloDir = getcwd(tralal, 1000);
    int x, y, a; x = 2;
    DIR *direktorinya; struct dirent *direk; struct stat fileaku;
    direktorinya = opendir(".");

    if(direktorinya){
        while((direk = readdir(direktorinya)) != NULL){
            if(stat(direk->d_name, &fileaku) < 0);
            else if(!S_ISDIR(fileaku.st_mode)){ 
                 a = pthread_create(&(tid[x-2]), NULL, &pindahinajadeh, (void *)direk->d_name);x++;
                 if(a==0){printf("DIrektori sukses disimpan");}
                 else{printf("Yah, gagal disimpan :(");}
                } 
            else;  
        }
        for(y = 0; y < (x-1); y++){pthread_join(tid[y],NULL);} 
    }
}

int main(int argc, char **argv){
    char tralal[1000];
    helloDir = getcwd(tralal, 1000);
    int x, y, a; x = 2;
    if(strcmp(argv[1], "-f") == 0){
        while(argv[x] != NULL){
            a = pthread_create(&(tid[x-2]), NULL, &pindahinajadeh, (void *)argv[x]);
            if(a==0){printf("\n FILE %d: Berhasil Dikategorikan", x-1);}
            else{printf("\n FILE %d: Sad, gagal :(", x-1);}
            x++;
        }
        for(y = 0; y < (x-1); y++){
            pthread_join(tid[y], NULL);
        }
    }
    else if(strcmp(argv[1], "-d") == 0){
        chdir(argv[2]);placeDir = getcwd(tralal, 1000);
        DIR *direktorinya;struct dirent *direk;
        struct stat fileaku;
        direktorinya = opendir(".");
        if(direktorinya){
            while((direk = readdir(direktorinya)) != NULL){
                if (stat(direk->d_name, &fileaku) < 0);
                else if (!S_ISDIR(fileaku.st_mode)){pthread_create(&(tid[x-2]), NULL, &pindahinajadeh2, (void *)direk->d_name); x++;} else;
            }
            for(y = 0; y < (x-1); y++)
                pthread_join(tid[y],NULL);
        }
    }
    else if(strcmp(argv[1], "*") == 0){
        fungsinyagaes("/Users/macbook/soal3");
    }  
}