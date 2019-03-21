#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define XWIN 640    //window x resolution
#define YWIN 480    //window y resolution
#define BKG 0       //background color

int desired_level = 10;

struct tank_t {
    pthread_mutex_t mutex;
    pthread_cond_t C_f;
    int level;

}tank;

void init_tank_t (struct tank_t *t){

    pthread_mutex_init (&t->mutex, NULL);       //inizializzo il mutex al valore di default 1
    pthread_cond_init (&t->C_f, NULL);
    t->level = 0;
}

//init of allegro

void init_allegro(void){
    allegro_init();
    install_keyboard();
    install_mouse();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
    clear_to_color(screen, BKG);
    show_mouse(screen);
}

void create_tank(){
    rectfill(screen, 200, 100, 400, 400, 255);
}


void create_button(){
    circle(screen, 100, 200, 50, 255);
    textout_centre_ex(screen, font, "Open", 100, 198, 255, -1);
}

void *th_filler(void *arg){
    struct tank_t *t = &tank;
    while(1){
        sleep(1);
        pthread_mutex_lock (&t->mutex);
        while(t->level > desired_level-1)
            pthread_cond_wait(&t->C_f, &t->mutex);
        t->level++;
        pthread_mutex_unlock (&t->mutex);
    }    
}

void check_level(struct tank_t *t){
    if (t->level < desired_level-1){
        pthread_cond_signal(&t->C_f);
        }
}

int get_level(struct tank_t *t){
    return t->level;
}

void *th_tank (void *arg){
    struct tank_t *t = &tank;
    int level;
    while(1){
        // sleep(1);
        pthread_mutex_lock (&t->mutex);                 
        
        check_level (&tank);
        level = get_level(t);
        printf("livello: %d\n",level);

        pthread_mutex_unlock (&t->mutex);
    }
}

int main(){

    init_allegro();
    init_tank_t(&tank);      //init della struct

    create_tank();
    create_button();

    pthread_t tank;
    pthread_t filler;

    pthread_create (&tank, NULL, th_tank, NULL);
    pthread_create (&filler, NULL, th_filler, NULL);

    srand(time(NULL));      //funzione di appoggio per numeri random
    
    // do{

    // }
    // while(!key[KEY_ESC]);

    pthread_join (tank, NULL);
    pthread_join (filler, NULL);

    allegro_exit();
    return 0;
}