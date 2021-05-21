# soal-shift-sisop-modul-3-B09-2021


* Vincent Yonathan    - 05111940000186
* Aprilia Annisa      - 05111940000199
* Pramudityo Prabowo  - 05111940000210
---
### Soal Modul 3
[Soal Shift Modul 3](https://docs.google.com/document/d/1ud1JyncoSDAo5hA03wPvjn7QBHPUeUG1eBJ8ETtq2dQ/edit)

---

### Daftar isi
* [Soal 1](#soal-1)
* [Soal 2](#soal-2)
* [Soal 3](#soal-3)

---
### Soal 1
*Praktikan* diminta membantu mengerjakan suatu proyek dimana proyek tersebut meminta untuk membuat server database buku
#### Soal 1.a)
*Praktikan* diminta untuk membuat register dan login yang sinkron antara Client dengan Server. Ketika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server.&nbsp; 
User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Data berisi akun dan password tersimpan dalam **akun.txt**.&nbsp

#### Source Code
##### Client :
```c
...
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
            ...
            ...
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
```
##### Server :
```c
...
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
     ...
```

#### Thread pada Server 
```c
...
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_create(&(tid[total]), NULL, &client, &new_socket);
        pthread_join(tid[total],NULL);
        total++;   
    }
  ...
```
Untuk kedua fungsi ini. Server akan diinisialisasi dengan membuka akun.txt
```c
FILE *fdir;
fdir = fopen("akun.txt","a+"); 
```
##### Register
- Fungsi Register pertama akan mengirim dengan `send` yang akan diterima oleh server via `read` yang berisi "register" agar server dapat mengetahui perintah apa yang sedang diminta oleh user.
- Kemudian, server akan membaca `buffer` yang berisi register tersebut dan mengirimkan `send` kepada client bahwa perintah diterima.
- Server juga akan melakukan perintah ``
- Client kemudian akan meminta input dari user berupa username dan password. 
- Dengan format yang diminta soal yaitu (username:password) maka akan dilakukan strlen agar sesuai.
- Kemudian format data yang telah dibuat Client di `send` kepada Server
- Ketika diterima di server, server akan langsung menjalankan perintah untuk menulis data (username:password) ke **akun.txt** menggunakan `fprintf(fdir,"%s\n",buffers);`
- Terakhir Server akan melakukan `send` kepada Client untuk memberitahu bahwa registrasi telah berhasil dimana berparameter "Register Success" yang kemudian akan di `read` oleh Client dan ditampilkan ke user.
---
##### Login 
- Fungsi Login pertama tentu akan mengirim dengan `send` yang akan diterima oleh server via `read` yang berisi "login" agar server mengetahui apa yang harus dilakukan.
- Ketika sudah menerima perintah dari client, server juga akan memberikan `send` kepada client bahwa `command` telah sukses diterima.
- Client kemudian akan memproses input yang diterima dari user dimana input tersebut merupakan `username` dan `password`.
- Karena input tersebut akan dibandingkan dengan user yang ada pada **akun.txt** (dimana formatnya adalah username:password). Maka akan dilakukan strcat untuk membuat format yang sama.
- Kemudian akan dilakukan iterasi sebanyak line yang ada pada **akun.txt** dan akan dibandingkan satu persatu. Antara filename yang dicari dengan yang ada.
- Apabila ada baris yang mengandung isi data yang sama dengan yang dicari, maka program Server akan melakukan `send` dimana "Login Success" merupakan parameternya agar dapat di print di Client.
- Setelah menemukan data yang sama dengan yang dicari, kemudian program akan di `break` agar keluar dari looping dan tidak membuang banyak waktu.
- Apabila data yang dicari tidak ditemukan hasil yang sama pada **akun.txt** maka program akan melakukan `send` dengan parameter "Login Failed" untuk memberi tahu bahwa ada kesalahan yang dilakukan oleh user.
- Apabila login berhasil, program Client kemudian akan menampilkan menu utama dimana menu tersebut menerima input `command` untuk melakukan : Add Data, Download Data, Delete Data, See, Find, dan Logout.
Dimana berarti user hanya akan dapat melakukan kegiatan ini ketika sudah berhasil login.
- Karena diminta aga server dapat menghandle banyak connections. Maka dibuat thread untuk menghandle setiap koneksi. 
- Untuk itu, Digunakan fungsi pthread_join untuk menunggu thread sampai selesai baru membuat thread baru lagi untuk membuat koneksi dengan client baru.

