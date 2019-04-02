#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "mylib.h"

int main(){
    init_allegro();
    for (int i=0;i<N;i++){
        init_tank_t(&tank[i], i+1);
        init_button(&button[i], &tank[i]);
        create_tank(&tank[i]);
        create_button(&button[i]);
    }

    pthread_t tank[N-1], filler[N-1], tap[N-1], sensor[N-1];

    for (int i=0;i<N;i++){
        pthread_create(&tank[i], NULL, th_tank, (void *)(intptr_t)i);
        pthread_create(&sensor[i], NULL, th_sensor, (void *)(intptr_t)i);
        pthread_create(&filler[i], NULL, th_filler, (void *)(intptr_t)i);
        pthread_create(&tap[i], NULL, th_tap, (void *)(intptr_t)i);
        usleep(250000);    
    }
    
    for (int i=0;i<N;i++){
        pthread_join(tank[i], NULL);
        pthread_join(sensor[i], NULL);
        pthread_join(filler[i], NULL);
        pthread_join(tap[i], NULL);
    }

    allegro_exit();

    return 0;
}