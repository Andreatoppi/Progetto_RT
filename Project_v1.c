#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define XWIN    640 //window x resolution
#define YWIN    480 //window y resolution
#define BKG     0   //background color
#define SMIN    0   //min sensor distance
#define SMAX    300 //max sensor distance
#define SSTEP   1   //sensor precision
#define BLUE    11  //colors
#define WHITE   15

int desired_level = 100;

struct tank_t {                 //tank data structure
    pthread_mutex_t mutex;
    pthread_cond_t  C_f, C_t;
    int level;
    int x1, y1, x2, y2, h, w, color;
    int sensor;
    bool    tap;
}tank;

void init_tank_t (struct tank_t *t){            //tank inizialization

    pthread_mutex_init (&t->mutex, NULL);       //mutex and condition variable init
    pthread_cond_init (&t->C_f, NULL);
    pthread_cond_init (&t->C_t, NULL);
    t->level = 0;
    t->tap = FALSE;
    t->x1 = 200;
    t->y1 = 100;
    t->x2 = t->y2 = 400;
    t->h = 300;
    t->w = 200;
    t->color = 15;
    t->sensor = 0;
}

//init of allegro

void init_allegro(void){
    allegro_init();
    install_mouse();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
    clear_to_color(screen, BKG);
    show_mouse(screen);
}

void create_tank(struct tank_t *t){
    rectfill(screen, t->x1, t->y1, t->x2, t->y2, t->color);
}

void create_button(){
    circle(screen, 100, 200, 50, 255);
    textout_centre_ex(screen, font, "Open", 100, 198, 255, -1);
}

int read_sensor(struct tank_t *t){       //sensor
    int c = 0;        // pixel value
    int x, y;       // sensor coordinates
    int d = SMIN;   // min sensor distance

    do{
        x = 300;
        y = 101 + d;
        c = getpixel(screen, x, y); //get pixel color
        d = d + SSTEP;
    }while (c == WHITE);

    return d-1;
}

void check_level(struct tank_t *t){                 //unlock the "refiller" if the level il low
    if (t->level < desired_level-1){
        pthread_cond_signal(&t->C_f);
        }
}

void check_tap(struct tank_t *t){                   //if tap is hold unlock "emptier"
    if (mouse_b == 1)
        t->tap = TRUE;
    else 
        t->tap = FALSE;

    if (t->tap && t->level>0)
        pthread_cond_signal(&t->C_t);
    }

int get_level(struct tank_t *t){                    //measure the level of liquid
    return t->level;
}

void fill_pixel(struct tank_t *t){
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, t->y2-t->level, BLUE);    //color line by line in blue
}

void empty_pixel(struct tank_t *t){
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, t->y2-t->level, WHITE);   //color line by line in white
}

void *th_tap(void *arg){            //thread that manage the tap task
    struct tank_t *t = &tank;

    while (1){
        // sleep (1);
        pthread_mutex_lock (&t->mutex);
        
        while (!t->tap)             //lock il tap is not click
            pthread_cond_wait (&t->C_t, &t->mutex);
        
        if (t->level>=0){
            empty_pixel(t);         //if tank is not empty do it
            t->level--;
        }
        pthread_mutex_unlock (&t->mutex);
    }
}

void *th_filler(void *arg){         //thread that manage fill task
    struct tank_t *t = &tank;

    while(1){
        // sleep(1);
        pthread_mutex_lock (&t->mutex);
        
        while(t->level > desired_level-1)       //if level is higher i'm not refill
            pthread_cond_wait(&t->C_f, &t->mutex);

        fill_pixel(t);      //refill
        t->level++;
        pthread_mutex_unlock (&t->mutex);
    }    
}

void *th_tank (void *arg){      //task tank to check the status of tank
    struct tank_t *t = &tank;
    
    while(1){
        // sleep(1);
        pthread_mutex_lock (&t->mutex);                 
        check_level (t);        //check liquid level
        check_tap (t);          //check if the tap is hold
        // printf("livello: %d\n", get_level(t))
        pthread_mutex_unlock (&t->mutex);
    }
}

void *th_sensor (void *args){       //sensor task to evaluate quantity of liquid
    struct tank_t *t = &tank;
    
    while(1){
        sleep(1/2);
        pthread_mutex_lock(&t->mutex);
        t->sensor = read_sensor(t);
        // printf("sensor: %d\n", t->sensor);   //print status
        pthread_mutex_unlock(&t->mutex);
    }
}

int main(){
    init_allegro();
    init_tank_t(&tank);      //init della struct

    create_tank(&tank);
    create_button();

    pthread_t tank, filler, tap, sensor;

    pthread_create (&tank, NULL, th_tank, NULL);
    pthread_create (&filler, NULL, th_filler, NULL);
    pthread_create (&tap, NULL, th_tap, NULL);
    pthread_create (&sensor, NULL, th_sensor, NULL);

    pthread_join (tank, NULL);
    pthread_join (filler, NULL);
    pthread_join (tap, NULL);

    allegro_exit();
    return 0;
}