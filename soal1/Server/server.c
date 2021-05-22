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
#define _OPEN_THREADS

pthread_t tid[1000];

void clear_buffer(char* b) 
{
    int i;
    for (i = 0; i < BUFSIZ; i++)
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
    printf("Upload Proses");
    char buffer[BUFSIZ];
    int valread;
    ssize_t length;

    send(sockets,"Upload", strlen("Upload"),0);
    clear_buffer(buffer);
    read(sockets,buffer,BUFSIZ);
    printf("ini buffer upload --> %s\n",buffer);

    // printf("SEND UPLOADIN\n");
    // send(sockets,"upload in", strlen("upload in"),0);
    // printf("SELESAI SEND UPLOADIN\n");

    FILE *filedir;
    char file_path[100];
    sprintf(file_path, "FILES/%s", namafile);

    // length = recv(sockets,buffer,BUFSIZ,0);

    // send(sockets,"upload start", strlen("upload start"),0);
    filedir = fopen(file_path, "w");

    
    clear_buffer(buffer);
    read(sockets, buffer, BUFSIZ);
    fprintf(filedir, "%s", buffer);

    
    fclose(filedir);    
}

void delete_tsv(char name[], int linecount)
{
    FILE *fp, *fp_temp;
    char str[BUFSIZ];
    char file_name_temp[] = "temp_file.txt";
    int counter = 0;
    char ch;

    fp = fopen(name, "r");
    fp_temp = fopen(file_name_temp, "w"); 

    while (!feof(fp)) 
    {
        strcpy(str, "\0");
        fgets(str, BUFSIZ, fp);
        if (!feof(fp)) 
        {
            counter++;
            if (counter != linecount) 
            {
                fprintf(fp_temp, "%s", str);
            }
        }
    }
    fclose(fp);
    fclose(fp_temp);
    remove(name);
    rename(file_name_temp, name);
}

void *client(void *arg)
{
    printf("Masuk fungsi client\n");
    int socketfd = *(int *)arg;
    int valread;
    char command[BUFSIZ];
    char userpass[100];

    clear_buffer(command);
    valread = read(socketfd , command, BUFSIZ);
    fprintf(stdout, "Receive connection handshake %s\n", command);

    send(socketfd, "SUCCESS", strlen("SUCCESS"), 0);

    clear_buffer(command);
    valread = read(socketfd , command, BUFSIZ);
    printf("ini buffer sukses lagi --> %s \n",command);

    while(1)
    {    
        // printf("Ini ngeprint while client\n");
        FILE *fdir;
        fdir = fopen("akun.txt","a+");    
        clear_buffer(command);
        valread = read(socketfd, command, BUFSIZ);
        // printf("%s-%ld",command,strlen(command));
        if(strcmp(command,"register")==0)
        { 
            char buffers[BUFSIZ];
            clear_buffer(buffers);
            valread = read(socketfd,buffers,BUFSIZ);
            // printf("%s\n", buffers);
            printf("Registrasi Sukses\n"); 
            fprintf(fdir,"%s\n",buffers);
            //pesan yang ditampilkan ke client
            send(socketfd,"Register Success",strlen("Register Success"),0);
        }
        else if (strcmp(command,"login")==0)
        {
            char temp[BUFSIZ];
            char userada[10];
            // printf("Ini kalo dia login\n");  
            char buffers[BUFSIZ];
            int flag = 0;
            clear_buffer(buffers);
            valread = read(socketfd,buffers,BUFSIZ);
            strcpy(userpass,buffers);
            while(fgets(temp, BUFSIZ, fdir) != NULL) 
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
            char buffers[BUFSIZ];
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
            valread = read(socketfd, buffers, BUFSIZ);

            char filename[50];
            char pub[50];
            char tahun_pub[10];            

            ekstrak(buffers, filename, pub, tahun_pub);

            char data[200];
            sprintf(data, "FILES/%s\t%s\t%s", filename, pub, tahun_pub);

            upload(socketfd,filename);

            printf("buffer co --> %s\n",buffers);
            send(socketfd, "success", strlen("success"), 0);

            //fprintf(fdirc, "%s\n", buffers);
            fprintf(fdirc, "%s\n", data);

            FILE* log = fopen("running.log", "a") ;
            fprintf(log, "Tambah : %s (%s)\n", filename, userpass);
            fclose(log) ;
            
            fclose(fdirc);

        }
        else if (strcmp(command,"DownloadFile")==0)
        {
            char buffers[BUFSIZ];
            int exist;
            char baris[50];
            char iterationb[30];
            //untuk saat found
            char datat[BUFSIZ];
            printf("Download File\n");

            send(socketfd, "Success Download", strlen("Success Download"),0);

            clear_buffer(buffers);
            valread = read(socketfd, buffers, BUFSIZ);
            printf("buffer server --> %s\n", buffers);
            
            char file_path[100] = "FILES/";
            strcat(file_path,buffers);

            FILE *fdi;
            fdi = fopen("file.tsv","r");
            while (fgets(baris, sizeof(baris), fdi)) 
            {
                int index = 0;
                pisah(baris, iterationb, '\t', &index);
                printf("%s -- %s\n", iterationb, file_path);
                if (strcmp(iterationb, file_path) == 0)
                {
                    exist = 1;
                    break;
                }
            }
            fclose(fdi);
            if (exist)
            {
                send(socketfd, "Down Success", strlen("Down Success"),0 );
                // printf("buffer ketemu --> %s\n",buffers);
                // printf("Ada ni filenya\n");
                clear_buffer(buffers);
                read(socketfd,buffers,BUFSIZ);
                fdi = fopen(file_path,"rb");
                while(fgets(datat, BUFSIZ, fdi) != NULL) 
                {
                    send(socketfd, datat, strlen(datat), 0);
                    bzero(datat,BUFSIZ);
                }
                fclose(fdi);
                bzero(buffers,BUFSIZ);
            }
            else
            {
                send(socketfd, "Down Failed", strlen("Down Failed"),0);
            } 
        }
        else if (strcmp(command,"See")==0)
        {
            int valread;
            char buffers[BUFSIZ];
            char files[300];
            char data[BUFSIZ];
            char datafix[BUFSIZ];

            FILE *fi;
            fi = fopen("file.tsv","r");

            clear_buffer(datafix);
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
                sprintf(data, "Nama: %s\nPublisher: %s\nTahun publishing: %s\nEkstensi file: %s\nFilepath: %s\n\n", nama, publ, tahun_pub, file_ext, file_path);

                //printf(data, "Nama: %s\nPublisher: %s\nTahun publishing: %s\nEkstensi file: %s\nFilepath: %s\n\n", nama, publ, tahun_pub, file_ext, file_path);
                strcat(datafix, data);   
                printf("strcat buffer di while %s\n",data);     
            }
            printf("strcat buffer data %s\n",datafix);  
            send(socketfd,datafix,strlen(datafix),0);
            fclose(fi);
        }
        else if(strcmp(command,"Delete")==0)
        {
            char temp[] = "file.tsv";
            char namatemp[] = "FILES/";
            char buffers[BUFSIZ];
            int valread;
            char iterasi[30];
            char baris[50];
            char namafile[50];
            int exist;
            int line;

            send(socketfd,"Success",strlen("Success"),0);

            //untuk nerima nama file
            clear_buffer(buffers);
            valread = read(socketfd,buffers,BUFSIZ);

            strcpy(namafile,buffers);
            strcat(namatemp,namafile);

            FILE *fdel;
            fdel = fopen("file.tsv","r");
            while (fgets(baris, sizeof(baris), fdel)) 
            {
                line++;
                int index = 0;
                pisah(baris, iterasi, '\t', &index);
                printf("%s -- %s\n", iterasi, namatemp);
                if (strcmp(iterasi, namatemp) == 0)
                {
                    exist = 1;
                    break;
                }
            }
            fclose(fdel);
            if(exist)
            {
                char path_file[100];
                sprintf(path_file,"FILES/%s",namafile);

                char new_name[100];
                sprintf(new_name,"FILES/old-%s",namafile);

                delete_tsv(temp,line);

                rename(path_file, new_name);

                send(socketfd,"Success", strlen("Success"),0);

                FILE* dlog = fopen("running.log", "a") ;
                fprintf(dlog, "Hapus : %s (%s)\n", namafile, userpass);
                fclose(dlog);
            }
            else
            {
                send(socketfd,"Failed", strlen("Failed"),0);
            }

        }
        else if (strcmp(command,"Find")==0)
        {
            char buffers[BUFSIZ];
            int valread;
            char files[300];
            char data[BUFSIZ];
            char fordata[BUFSIZ];
            int flag = 0;

            send(socketfd,"Success",strlen("Success"),0);

            clear_buffer(buffers);
            valread = read(socketfd,buffers,BUFSIZ);

            char filename[50];
            strcpy(filename,buffers);

            FILE* ffile;
            ffile = fopen("file.tsv","r");

            while (fgets(files, sizeof files, ffile)!= NULL) 
            {
                char file_exist[50];
                char delim = '\t';
                int idxs = 0;
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

                    flag = 1;

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
                    //printf("Ini FIND =----\nIni buffers dan fordata --> %s\n", buffers);
                }
            }
            if(flag == 1)
            {
                send(socketfd, buffers, strlen(buffers),0 );
            }
            else
            {
                send(socketfd,"Failed", strlen("Failed"),0);
            }
            
            fclose(ffile);
        }
        
        fclose(fdir);

        if(valread == 0)
        {
            return 0;
        }
    }
}

int main(int argc, char const *argv[]) {
    printf("Halo ini server");
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFSIZ] = {0};
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

        printf("Buat Thread Baru --\n");
        pthread_create(&(tid[total]), NULL, &client, &new_socket);
        printf("Buat Thread Join --\n");
        pthread_join(tid[total],NULL);
        total = total+1;   
    }  

    return 0;
}
