#include <pthread.h>
#include <stdio.h>

int i = 0;

void *thread1(){
	for(int x = 0; x < 1000000; x++){
		i++;
	}
	return 0;
}

void *thread2(){
	for(int x = 0; x < 1000000; x++){
		i--;
	}
	return 0;
}

int main(){
	pthread_t myThread1;
	pthread_t myThread2;

	pthread_create(&myThread1, NULL, thread1, NULL);
	pthread_create(&myThread2, NULL, thread2, NULL);	
	
	pthread_join(myThread1, NULL);

	

	pthread_join(myThread2, NULL);
	
	printf("%i\n", i);

	return 0;
}
