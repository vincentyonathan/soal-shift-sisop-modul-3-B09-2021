#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

void clear_buffer(char* b) 
{
    int i;
    for (i = 0; i < 1024 ; i++)
        b[i] = '\0';
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int new_socket = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
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
        char buffer[1024], command[1024];
        printf("1. Login\n2. Register\n3. Database\n4. Download\n5. Delete\n6. See\n7. Find\n8. Exit\nCommand : ");
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
            valread = read(new_socket , buffer, 1024);
            printf("%s\n",buffer);

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
            valread = read(new_socket , buffer, 1024);
            printf("%s\n", buffer);
        }
        else if(strcmp(command,"add")==0)
        {
            // printf("Ini masuk add");
            char buffer[1024];
            int valread;
            char pub[20];
            char tahun_pub[10];
            char path_file[50];
            char add_data[110];

            send(new_socket, "New Data", strlen("New Data"), 0);

            clear_buffer(buffer);
            valread = read(new_socket, buffer, 1024);

            printf("Publisher: ");
            scanf("%s", pub);
            printf("Tahun Publikasi: ");
            scanf("%s", tahun_pub);
            printf("Filepath: ");
            scanf("%s", path_file);

            sprintf(add_data, "%s:%s:%s", path_file, pub, tahun_pub);

            send(new_socket, add_data, strlen(add_data), 0);
            
            clear_buffer(buffer);
            valread = read(new_socket , buffer, 1024);

            if (strcmp(buffer, "success") == 0) 
            {
                printf("Data added successfully\n");
            } 
            else 
            {
                printf("There's a problem adding data\n");
            }

        }
        else if(strcmp(command, "download")==0)
        {
            char buffer[1024];
            int valread;
            char filename[50];

            scanf("%s", filename);
            send(new_socket,"DownloadFile", strlen("DownloadFile"),0);

            clear_buffer(buffer);
            valread = read(new_socket, buffer, 1024);
            printf("buffer client --> %s\n", buffer);
            send(new_socket, filename, strlen(filename),0);
            
        }
        else if(strcmp(command,"see")==0)
        {
            char buffer[1024];
            int valread;

            send(new_socket, "See", strlen("See"), 0);

            clear_buffer(buffer);
            valread = read(new_socket,buffer,1024);
            printf("%s\n",buffer);
        }
        else if (strcmp(command,"delete")==0)
        {
            char buffer[1024];
            int valread;
            char namafile[50];

            send(new_socket,"Delete", strlen("Delete"),0);

            clear_buffer(buffer);
            valread = read(new_socket,buffer,1024);

            scanf("%s",namafile);

            send(new_socket,namafile,strlen(namafile),0);

            clear_buffer(buffer);
            valread = read(new_socket,buffer,1024);
            if (strcmp(buffer,"Success"))
            {
                printf("Data deleted successfuly.\n");
            }
            else
            {
                printf("There's a problem deleting data\n");
            }
        }
        else if(strcmp(command,"find")==0)
        {
            char buffer[1024];
            int valread;
            char findfile[50];

            send(new_socket,"Find", strlen("Find"),0);

            clear_buffer(buffer);
            valread = read(new_socket,buffer,1024);

            scanf("%s",findfile);

            send(new_socket,findfile,strlen(findfile),0);

            clear_buffer(buffer);
            valread = read(new_socket,buffer,1024);
            if(strlen(buffer) > 0)
            {
                printf("%s",buffer);
            }
            else
            {
                printf("%s",buffer);
            }
        }
        
        else
        {
            printf("Command Not Found.\n");
        }
    }   
    return 0;
}

