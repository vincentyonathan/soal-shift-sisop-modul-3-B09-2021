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
            // printf("Ini kalo dia regis\n");  
            char buffers[1024];
            clear_buffer(buffers);
            valread = read(socketfd,buffers,1024);
            // printf("%s\n", buffers);
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
//                 printf("%s",temp);
                if((strstr(temp, buffers)) != NULL) 
                {
                    flag = 1;
                    char kata[20] = "Login Success";
                    printf("Login Berhasil");
                    send(socketfd,kata, strlen(kata),0);
                    break;
                }                
            }
//             printf("bisa ga print");
            if(flag == 0)
            {
                char kata[20] = "Login Failed";
                printf("Login Gagal");
                send(socketfd,kata, strlen(kata),0);
            }
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
