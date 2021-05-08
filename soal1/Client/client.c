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
        printf("1. Login\n2. Register\n3. Exit\nCommand : ");
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
        else
        {
            printf("Command Not Found.");
        }
    }   
    return 0;
}

