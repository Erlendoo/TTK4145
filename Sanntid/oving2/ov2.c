#include <pthread.h>
#include <stdio.h>

//gcc -std=gnu99 -Wall -g -o helloworld_c helloworld_c.c -lpthread

int i = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread1(){
	for(int x = 0; x < 1000000; x++){
		pthread_mutex_lock(&mutex);
		i++;
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

void *thread2(){
	for(int x = 0; x < 1000000; x++){
		pthread_mutex_lock(&mutex);
		i--;
		pthread_mutex_unlock(&mutex);
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

