#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    key_t key = 1234;
    int m, n, p, q, c, d, k, sum = 0;
    int first[4][3], second[3][6], (*value)[4][6];

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
        value = shmat(shmid, NULL, 0);

    printf("Masukkan elemen matrix pertama\n");
    m=4;
    n=3;
    for (  c = 0 ; c < m ; c++ )
        for ( d = 0 ; d < n ; d++ )
        scanf("%d", &first[c][d]);
 
    p=3;
    q=6;
    {
        printf("Masukkan elemen matrix kedua\n");
 
        for ( c = 0 ; c < p ; c++ )
            for ( d = 0 ; d < q ; d++ )
                scanf("%d", &second[c][d]);
 
        for ( c = 0 ; c < m ; c++ )
        {
            for ( d = 0 ; d < q ; d++ )
            {
                for ( k = 0 ; k < p ; k++ )
                {
                sum = sum + first[c][k]*second[k][d];
                }
 
            (*value)[c][d] = sum;
            sum = 0;
            }
        }
 
    printf("Hasil dari perkalian matrix adalah :-\n");
 
    for ( c = 0 ; c < m ; c++ )
        {
        for ( d = 0 ; d < q ; d++ )
            printf("%d\t", (*value)[c][d]);
 
        printf("\n");
        }
    }
    sleep(5);

    shmdt(value);
        shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
