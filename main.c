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
    init_gui();
    
    pthread_t tank[N-1], filler[N-1], tap[N-1], sensor[N-1];

    // Thread init
    for (int i=0;i<N;i++){
        pthread_create(&tank[i], NULL, th_tank, (void *)(intptr_t)i);
        pthread_create(&sensor[i], NULL, th_sensor, (void *)(intptr_t)i);
        pthread_create(&filler[i], NULL, th_filler, (void *)(intptr_t)i);
        pthread_create(&tap[i], NULL, th_tap, (void *)(intptr_t)i);
    }
    
    do{
        
    }while(!key[KEY_ESC]);

    allegro_exit();

    // Thread join
    for (int i=0;i<N;i++){
        pthread_join(tank[i], NULL);
        pthread_join(sensor[i], NULL);
        pthread_join(filler[i], NULL);
        pthread_join(tap[i], NULL);
    }

    return 0;
}