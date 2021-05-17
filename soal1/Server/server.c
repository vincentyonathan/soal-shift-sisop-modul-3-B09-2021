#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <ctype.h> 
#include <limits.h>
#define PORT 8080  

pthread_t tid[1000];

void clear_buffer(char* b) 
{
    int i;
    for (i = 0; i < 1024; i++)
        b[i] = '\0';
}

void pisah(char str[], char fileex[], char pemisah, int *idx)
{
    int index_ex = 0;
    while(*idx < strlen(str) && str[*idx] != pemisah) {
        fileex[index_ex] = str[*idx];
        *idx += 1;
        index_ex++;    
    }
    fileex[index_ex] = '\0';
    *idx += 1;
}

void ekstrak(char buffers[], char filename[], char pub[], char tahun_pub[])
{
    int idx = 0;
    int len = strlen(buffers);

    pisah(buffers, filename, ':', &idx);
    pisah(buffers, pub , ':', &idx);
    pisah(buffers, tahun_pub, ':', &idx);
}


void upload(int sockets, char namafile[])
{
    char buffer[1024];
    int valread;
    ssize_t length;

    send(sockets,"Upload", strlen("Upload"),0);
    clear_buffer(buffer);
    read(sockets,buffer,1024);
    
    FILE *filedir;
    char file_path[100];
    sprintf(file_path, "FILES/%s", namafile);

    length = read(sockets, buffer, 1024);

    filedir = fopen(file_path, "w");
    if (filedir == NULL) 
    {
        perror("No File");
        exit(EXIT_FAILURE);
    }
    // *prt, *size byte per kata, number of elements ,tujuan
    fwrite(buffer, sizeof(char),length,filedir);

    fclose(filedir);    
}

void *client(void *arg)
{
    int socketfd = *(int *)arg;
    int valread;
    char command[1024];

    while(1)
    {    
        // printf("Ini ngeprint while client\n");
        FILE *fdir;
        fdir = fopen("akun.txt","a+");    
        clear_buffer(command);
        valread = read(socketfd, command, 1024);
        // printf("%s-%ld",command,strlen(command));
        if(strcmp(command,"register")==0)
        { 
            char buffers[1024];
            clear_buffer(buffers);
            valread = read(socketfd,buffers,1024);
            // printf("%s\n", buffers);
            printf("Registrasi Sukses\n"); 
            fprintf(fdir,"%s\n",buffers);
            //pesan yang ditampilkan ke client
            send(socketfd,"Register Success",strlen("Register Success"),0);
        }
        else if (strcmp(command,"login")==0)
        {
            char temp[1024];
            char userada[10];
            // printf("Ini kalo dia login\n");  
            char buffers[1024];
            int flag = 0;
            clear_buffer(buffers);
            valread = read(socketfd,buffers,1024);
            while(fgets(temp, 1024, fdir) != NULL) 
            {
                // printf("%s",temp);
                if((strstr(temp, buffers)) != NULL) 
                {
                    printf("Login Berhasil\n");
                    flag = 1;
                    char kata[20] = "Login Success";
                    send(socketfd,kata, strlen(kata),0);
                    break;
                }                
            }
            // printf("bisa ga print");
            if(flag == 0)
            {
                printf("Login Gagal\n");
                char kata[20] = "Login Failed";
                send(socketfd,kata, strlen(kata),0);
            }
        }
        else if(strcmp(command,"New Data")==0)
        {
            char buffers[1024];
            printf("Adding to Database :\n");

            send(socketfd, "success", strlen("success"), 0);
            FILE *fdirc;
            fdirc = fopen("file.tsv", "a+");
            if (fdirc == NULL) 
            {
                perror("No File");
                exit(EXIT_FAILURE);
            }
            clear_buffer(buffers);
            valread = read(socketfd, buffers, 1024);

            char filename[50];
            char pub[50];
            char tahun_pub[10];            

            ekstrak(buffers, filename, pub, tahun_pub);

            char data[200];
            sprintf(data, "FILE/%s\t%s\t%s", filename, pub, tahun_pub);

            upload(socketfd,filename);

            send(socketfd, "success", strlen("success"), 0);

            //fprintf(fdirc, "%s\n", buffers);
            fprintf(fdirc, "%s\n", data);

            FILE* flog;
            flog = fopen("running.log","a+");

            //fprintf(fp_log, "Tambah: %s (%s)\n", filename, user_auth);

            fclose(fp_log);
            
            fclose(fdirc);

        }
        else if (strcmp(command,"DownloadFile")==0)
        {
            char buffers[1024];
            int exist;
            char baris[50];
            char iterationb[30];
            printf("Download File\n");

            send(socketfd, "Success Download", strlen("Success Download"),0);

            clear_buffer(buffers);
            valread = read(socketfd, buffers, 1024);
            printf("buffer server --> %s\n", buffers);
            
            char file_path[100];
            strcpy(file_path,buffers);

            FILE *fdi;
            fdi = fopen("FILE/file.tsv","r");
            while (fgets(baris, sizeof(baris), fdi)) 
            {
                int index = 0;
                pisah(baris, iterationb, '\t', &index);
                if (strcmp(iterationb, file_path) == 0)
                {
                    exist = 1;
                    break;
                }
            }
            fclose(fdi);
            if (exist)
            {
                // printf("Ada ni filenya\n");
                send(socketfd, "Down Success", strlen("Down Success"),0 );

            }
            else 
                // printf("File not found.\n");
                send(socketfd, "Down Failed", strlen("Failed"),0);
        }
        else if (strcmp(command,"See")==0)
        {
            printf("See Files:\n");
            int valread;
            char buffers[1024];
            char files[300];
            char data[1024];

            FILE *fi;
            fi = fopen("file.tsv","r");

            while (fgets(files, sizeof files, fi)) 
            {
                printf("Masuk While\n");
                int idx = 0;
                char publ[50];
                char nama[20];
                char file_ext[10];
                char tahun_pub[10];
                char file_path[100];
                char header_path[50];

                pisah(files, header_path, '/', &idx);
                pisah(files, nama, '.', &idx);
                pisah(files, file_ext, '\t', &idx);

                // printf("Ini header_path %s\n", header_path);
                // printf("Ini file_ext %s\n", file_ext);
                // printf("Ini nama %s\n", nama);
                sprintf(file_path, "%s/%s.%s", header_path, nama, file_ext);
                
                pisah(files, publ, '\t', &idx);
                pisah(files, tahun_pub, '\n', &idx);
                // printf("Ini publ %s\n", publ);
                // printf("Ini tahun_pub %s\n", tahun_pub);
                sprintf(data, "Nama: %s\nPublisher: %s\nTahun publishing: %s\nEkstensi file: %s\nFilepath: %s\n\n", nama, publ, tahun_pub, file_ext, file_path);\

                strcat(buffers, data);
                send(socketfd, data, strlen(data),0 );       
                        
            }
            send(socketfd, data, strlen(data), 0);

            fclose(fi);
        }
        else if(strcmp(command,"Delete")==0)
        {
            char buffers[1024];
            int valread;

            send(socketfd,"Success",strlen("Success"),0);

            clear_buffer(buffers);
            valread = read(socketfd,buffers,1024);

        }
        else if (strcmp(command,"Find")==0)
        {
            char buffers[1024];
            int valread;
            char files[300];
            char data[1024];

            send(socketfd,"Success",strlen("Success"),0);

            clear_buffer(buffers);
            valread = read(socketfd,buffers,1024);

            char filename[50];
            strcpy(filename,buffers);

            FILE* ffile;
            ffile = fopen("file.tsv","r");

            while (fgets(files, sizeof files, ffile)!= NULL) 
            {
                char file_exist[50];
                char delim = '\t';
                int idxs = 0;
                char fordata[1024];
                pisah(files, file_exist, delim, &idxs);
                if (strstr(file_exist, filename))
                {   
                    int idx = 0;
                    char publ[50];
                    char nama[20];
                    char file_ext[10];
                    char tahun_pub[10];
                    char file_path[100];
                    char header_path[50];

                    pisah(files, header_path, '/', &idx);
                    pisah(files, nama, '.', &idx);
                    pisah(files, file_ext, '\t', &idx);

                    // printf("Ini header_path %s\n", header_path);
                    // printf("Ini file_ext %s\n", file_ext);
                    // printf("Ini nama %s\n", nama);
                    sprintf(file_path, "%s/%s.%s", header_path, nama, file_ext);
                    
                    pisah(files, publ, '\t', &idx);
                    pisah(files, tahun_pub, '\n', &idx);
                    // printf("Ini publ %s\n", publ);
                    // printf("Ini tahun_pub %s\n", tahun_pub);
                    sprintf(fordata, "Nama: %s\nPublisher: %s\nTahun publishing: %s\nEkstensi file: %s\nFilepath: %s\n\n", nama, publ, tahun_pub, file_ext, file_path);\

                    strcat(buffers, fordata);
                }
            send(socketfd, fordata, strlen(fordata),0 );
            }
            // send(socketfd, fordata, strlen(fordata),0 );
            fclose(ffile);
        }
        
        fclose(fdir);
    }
}

int main(int argc, char const *argv[]) {
    // printf("Halo ini server");
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int total = 0;

    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_create(&(tid[total]), NULL, &client, &new_socket);
        total++;   
    }

    return 0;
}
