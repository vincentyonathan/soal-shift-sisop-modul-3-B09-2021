#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

void *factorial (void* arg)
{
    int *data = (int *)arg;

    if (data[0]>=data[1]){
        
    }
}

int main()
{
    key_t key = 1234;
    int matrix[4][6]
    int (*value)[4][6];
    int c=4;
    int d=6;

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
        value = shmat(shmid, NULL, 0);

    int m, n;
    printf("Masukkan elemen matrix baru :\n");
    for (m=0; m<4; m++) {
        for (n=0; n<6; n++) {
            scanf("%d", &matrixBaru[m][n]);
            }
        }
    int sum = c * d;
    pthread_t *thr;
    thr = (pthread_t*)malloc(max * sizeof(pthread_t));

    for (int m=0; m<c; m++) {
            for (int n=0; n<d; n++) {
                data = (int *)malloc((max)*sizeof(int));
                data[0] = (*value)[c][d];
                data[1] = matrixBaru[c][d];
                pthread_create(&threads[count++], NULL, factorial, (void*)(data));
            }
        }



    return 0;
}