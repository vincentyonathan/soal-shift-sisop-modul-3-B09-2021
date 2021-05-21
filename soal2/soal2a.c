#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int mx1[4][3], mx2[3][6];
void matriksAwal(){
	printf("Input matriks 4x3\n");
	for (int i = 0; i < 4; i++) {
    	for (int j = 0; j < 3; j++) {
      		scanf("%d", &mx1[i][j]);
   	 	}
  	}
  	printf("Input matriks 3x6\n");
	for (int i = 0; i < 3; i++) {
    	for (int j = 0; j < 6; j++) {
      		scanf("%d", &mx2[i][j]);
   	 	}
  	}
}

void main() {
	matriksAwal();
	
	key_t key = 1234;
  	int (*value)[6];
  	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
  	value = shmat(shmid, NULL, 0);
  	
  	printf("Hasil Perkalian Matriks :\n");
  	for (int i = 0; i < 4; i++) {
    	for (int j = 0; j < 6; j++) {
    		for (int k = 0; k< 3; k++){
      			value[i][j] += mx1[i][k] * mx2[k][j];
			}
      		printf("%d ",value[i][j]);
   	 	}
    	printf("\n");
  	}
  	shmdt(value);
}