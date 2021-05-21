#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#define PORT 8080

void clear_buffer(char* b) 
{
    int i;
    for (i = 0; i < BUFSIZ ; i++)
        b[i] = '\0';
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int new_socket = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZ] = {0};
    if ((new_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(new_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1)
    {
        char buffer[BUFSIZ], command[BUFSIZ];
        printf("1. Login\n2. Register\nCommand :\n");
        scanf("%s", command);
        if(strcmp(command,"login")==0)
        {
            send(new_socket,"login", strlen("login"),0);
            char username[200];
            char password[100];
            printf("Username : ");
            scanf("%s", username);
            printf("Password : ");
            scanf("%s", password);
            strcat(username, ":");
            strcat(username, password);

            send(new_socket , username , strlen(username) , 0 );
            clear_buffer(buffer);
            valread = read(new_socket , buffer, BUFSIZ);
            printf("%s\n",buffer);

            if(strcmp(buffer,"Login Success")==0)
            {
                while(1)
                {
                    printf("\n1. Add Database\n2. Download\n3. Delete\n4. See\n5. Find\n6. Logout\nCommand : ");
                    char command2[20];
                    scanf("%s", command2);
                    if(strcmp(command2,"add")==0)
                    {
                        // printf("Ini masuk add");
                        char buffer[BUFSIZ];
                        int valread;
                        char pub[20];
                        char tahun_pub[10];
                        char path_file[50];
                        char add_data[110];
                        char len_data[20];
                        char temp_files[BUFSIZ];

                        send(new_socket, "New Data", strlen("New Data"), 0);

                        clear_buffer(buffer);
                        valread = read(new_socket, buffer, BUFSIZ);

                        printf("Publisher: ");
                        scanf("%s", pub);
                        printf("Tahun Publikasi: ");
                        scanf("%s", tahun_pub);
                        printf("Filepath: ");
                        scanf("%s", path_file);

                        sprintf(add_data, "%s:%s:%s", path_file, pub, tahun_pub);

                        send(new_socket, add_data, strlen(add_data), 0);
                        
                        clear_buffer(buffer);
                        valread = read(new_socket, buffer,BUFSIZ);
                        printf("ini buffer sebelum terakhir --> %s\n",buffer);
                        
                        send(new_socket, "berhasil",strlen("berhasil"),0);
                    
                        FILE *fd;
                        fd = fopen(path_file, "rb");

                        while(fgets(temp_files, BUFSIZ, fd) != NULL) 
                        {
                            send(new_socket, temp_files, strlen(temp_files), 0);
                            bzero(temp_files, BUFSIZ) ;
                        }
                        fclose(fd) ; 

                        clear_buffer(buffer);
                        valread = read(new_socket , buffer, BUFSIZ);

                        if (strcmp(buffer, "success") == 0) 
                        {
                            printf("Data added successfully\n");

                            FILE* log = fopen("running.log", "a") ;
                            fprintf(log, "Tambah : %s (%s)\n", path_file, username);
                            fclose(log) ;
                        } 
                        else 
                        {
                            printf("There's a problem adding data\n");
                        }

                    }
                    else if(strcmp(command2, "download")==0)
                    {
                        char buffer[BUFSIZ];
                        int valread;
                        char filename[50];
                        char local[100];

                        scanf("%s", filename);
                        send(new_socket,"DownloadFile", strlen("DownloadFile"),0);

                        clear_buffer(buffer);
                        valread = read(new_socket, buffer, BUFSIZ);
                        printf("buffer client --> %s\n", buffer);
                        
                        send(new_socket, filename, strlen(filename),0);
                        clear_buffer(buffer);
                        read(new_socket,buffer,BUFSIZ);
                        printf("Apakah ini down success? --> %s\n",buffer);

                        strcpy(local,filename);

                        send(new_socket,"ds diterima", strlen("ds diterima"),0);

                        if(strcmp(buffer,"Down Success")==0) 
                        {
                            printf("%s\n",buffer);
                            FILE* file = fopen(local, "w");
                            clear_buffer(buffer);
                            read(new_socket,buffer,BUFSIZ);
                            fprintf(file, "%s", buffer);
                            fclose(file);
                        }
                        else
                        {
                            printf("File not found.\n");
                            printf("%s\n",buffer);
                        }
                        
                        
                    }
                    else if(strcmp(command2,"see")==0)
                    {
                        char buffer[BUFSIZ];
                        int valread;

                        send(new_socket, "See", strlen("See"), 0);

                        clear_buffer(buffer);
                        valread = read(new_socket,buffer,BUFSIZ);
                        printf("%s\n",buffer);
                    }
                    else if (strcmp(command2,"delete")==0)
                    {
                        char buffer[BUFSIZ];
                        int valread;
                        char namafile[50];

                        send(new_socket,"Delete", strlen("Delete"),0);

                        clear_buffer(buffer);
                        valread = read(new_socket,buffer,BUFSIZ);

                        scanf("%s",namafile);

                        send(new_socket,namafile,strlen(namafile),0);

                        clear_buffer(buffer);
                        valread = read(new_socket,buffer,BUFSIZ);
                        if (strcmp(buffer,"Success")==0)
                        {
                            printf("Data deleted successfuly.\n");

                            FILE* dlog = fopen("running.log", "a") ;
                            fprintf(dlog, "Hapus : %s (%s)\n", namafile, username);
                            fclose(dlog);
                        }
                        else
                        {
                            printf("There's a problem deleting data\n");
                        }
                    }
                    else if(strcmp(command2,"find")==0)
                    {
                        char buffer[BUFSIZ];
                        int valread;
                        char findfile[50];

                        send(new_socket,"Find", strlen("Find"),0);

                        clear_buffer(buffer);
                        valread = read(new_socket,buffer,BUFSIZ);
                        clear_buffer(buffer);

                        scanf("%s",findfile);

                        send(new_socket,findfile,strlen(findfile),0);

                        clear_buffer(buffer);
                        valread = read(new_socket,buffer,BUFSIZ);
                        // printf("Buffer find --> %s\n",buffer);
                        if(strcmp(buffer,"Failed") == 0)
                        {
                            printf("%s\n",buffer);
                        }
                        else
                        {
                            printf("%s\n",buffer);
                        }
                    }
                    else if(strcmp(command2,"logout")==0)
                    {
                        break;
                    }
                }
            }   
        }
        else if(strcmp(command,"register")==0)
        {
            send(new_socket,"register", strlen("register"),0);
            char username[200];
            char password[100];
            printf("Username : ");
            scanf("%s", username);
            printf("Password : ");
            scanf("%s", password);
            strcat(username, ":");
            strcat(username, password);
            printf("%s\n",username);
            send(new_socket , username , strlen(username) , 0 );
            clear_buffer(buffer);
            valread = read(new_socket , buffer, BUFSIZ);
            printf("%s\n", buffer);
        }
    }   
    return 0;
}

