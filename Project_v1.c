#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define XWIN        640 // window x resolution
#define YWIN        480 // window y resolution
#define BKG         0   // background color
#define X0          300 // x position of sensor
#define Y0          100 // y position of sensor
#define SMIN        0   // min sensor distance
#define SMAX        300 // max sensor distance
#define SSTEP       1   // sensor precision
#define BLUE        11  // VGA code of blue
#define WHITE       15  // VGA code of white
#define MAXLEVEL    300 // max quantity of liquid in tank
#define MINLEVEL    0   // min quantity of liquid in tank
#define HTANK       300 // tank hight
#define WTANK       200 // tank width

int desired_level = 200;

struct tank_t {     //tank data structure
    pthread_mutex_t mutex;
    pthread_cond_t  C_f, C_t;
    int level;
    int x1, y1, x2, y2, h, w, color;
    int sensor;
    bool    tap;
}tank;

void init_tank_t(struct tank_t *t){        //tank inizialization

    pthread_mutex_init(&t->mutex, NULL);   //mutex and condition variable init
    pthread_cond_init(&t->C_f, NULL);
    pthread_cond_init(&t->C_t, NULL);
    t->level = MINLEVEL;
    t->tap  = FALSE;
    t->x1 = 200;
    t->y1 = 100;
    t->x2 = t->y2 = 400;
    t->h = HTANK;
    t->w = WTANK;
    t->color = WHITE;
    t->sensor = SMIN;
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

void read_sensor(struct tank_t *t){     //sensor
    int c = 0;                          // pixel value
    int y = Y0;                         // sensor coordinates
    int d = SMIN;                       // min sensor distance

    do{
        y = Y0 + d;
        c = getpixel(screen, X0, y);    //get pixel color
        d = d + SSTEP;
    }while (c == WHITE);
    t->sensor = d-2; 
}

void update_level(struct tank_t *t){
    read_sensor(t);
    t->level = MAXLEVEL - t->sensor;
}

void check_level(struct tank_t *t){     //unlock the "refiller" if the level il low
    if (t->level < desired_level){
        pthread_cond_signal(&t->C_f);
        }
}

void check_tap(struct tank_t *t){       //if tap is hold unlock "emptier"
    if (mouse_b == 1)
        t->tap = TRUE;
    else 
        t->tap = FALSE;

    if (t->tap && t->level>0)
        pthread_cond_signal(&t->C_t);
    }

int get_level(struct tank_t *t){        //measure the level of liquid
    update_level(t);
    return t->level;
}

void fill_pixel(struct tank_t *t){
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, t->y2-t->level, BLUE);  //color line by line in blue
}

void empty_pixel(struct tank_t *t){
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, (t->y2-t->level)+1, WHITE); //color line by line in white
}
 
void *th_tap(void *arg){            //thread that manage the tap task
    struct tank_t *t = &tank;

    while (1){
        usleep(500); 
        pthread_mutex_lock(&t->mutex);
        
        while (!t->tap)             //lock il tap is not click
            pthread_cond_wait(&t->C_t, &t->mutex);
        
        if (t->level>MINLEVEL){
            empty_pixel(t);         //if tank is not empty do it
            update_level(t);
            }
        t->tap = FALSE;
        pthread_mutex_unlock(&t->mutex);
    }
}

void *th_filler(void *arg){         //thread that manage fill task
    struct tank_t *t = &tank;

    while (1){
        usleep(1000);
        pthread_mutex_lock(&t->mutex);
        
        while (t->level > desired_level-1)       //if level is higher i'm not refill
            pthread_cond_wait(&t->C_f, &t->mutex);

        if (t->level<MAXLEVEL){
            fill_pixel(t);      //refill
            update_level(t);
        }
        pthread_mutex_unlock(&t->mutex);
    }    
}

void *th_tank(void *arg){      //task tank to check the status of tank
    struct tank_t *t = &tank;
    
    while (1){
        sleep(1/2);
        pthread_mutex_lock(&t->mutex);
        check_tap(t);          //check if the tap is hold
        check_level(t);        //check liquid level
        
        printf("livello: %d\t sensor: %d\n", get_level(t), t->sensor);

        pthread_mutex_unlock(&t->mutex);
    }
}

void *th_sensor(void *args){       //sensor task to evaluate quantity of liquid
    struct tank_t *t = &tank;
    
    while (1){
        sleep(1/2);
        pthread_mutex_lock(&t->mutex);
        update_level(t);
        pthread_mutex_unlock(&t->mutex);
    }
}

int main(){
    init_allegro();
    init_tank_t(&tank);      //init della struct

    create_tank(&tank);
    create_button();

    pthread_t tank, filler, tap, sensor;

    pthread_create(&tank, NULL, th_tank, NULL);
    pthread_create(&sensor, NULL, th_sensor, NULL);
    pthread_create(&filler, NULL, th_filler, NULL);
    pthread_create(&tap, NULL, th_tap, NULL);

    pthread_join(tank, NULL);
    pthread_join(filler, NULL);
    pthread_join(tap, NULL);

    allegro_exit();
    return 0;
}