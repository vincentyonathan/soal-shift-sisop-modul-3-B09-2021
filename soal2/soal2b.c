#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

int mx2[4][6], selisih;
bool isnull = false;

void input(){
	printf("Input matriks 4x6\n");
	for (int i = 0; i < 4; i++) {
    	for (int j = 0; j < 6; j++) {
      		scanf("%d", &mx2[i][j]);
   	 	}
  	}
}
long long factorial(int n) {
    if (n == 0) return 1;
    else return n * factorial(n - 1);
}
long long except(int n){
	if (n == selisih) return 1;
        else return n * except(n - 1);
}

void *kondisi(void* arg){
	long long angka = *(long long*)arg;
	if(isnull)//=0
		printf("0 ");
	else if(selisih<1) /// <1
    	printf("%lld ", factorial(angka));
	else//>1
		printf("%lld ", except(angka));
}

int main(){
	int angka;
	key_t key = 1234;
    int (*value)[6];
    int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);
    
    input();
    
    pthread_t tid[4*6];
    int index=0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 6; j++){
        	isnull=false;
            long long *val = malloc(sizeof(long long[4][6]));
            *val = value[i][j];
            selisih = value[i][j] - mx2[i][j];
            if(value[i][j]==0 || mx2[i][j]==0) isnull=true;
            pthread_create(&tid[index], NULL, &kondisi, val);
            sleep(1);
            index++;
        }
        printf("\n");
    }
    for (int i = 0; i < index; i++) {
        pthread_join(tid[i], NULL);
    }
    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}