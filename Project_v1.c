#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define XWIN        1000 // window x resolution
#define YWIN        480 // window y resolution
#define BKG         0   // background color
#define X0          250 // x position of sensor
#define Y0          100 // y position of sensor
#define SMIN        0   // min sensor distance
#define SMAX        300 // max sensor distance
#define SSTEP       1   // sensor precision
#define BLUE        11  // VGA code of blue
#define WHITE       15  // VGA code of white
#define MAXLEVEL    300 // max quantity of liquid in tank
#define MINLEVEL    0   // min quantity of liquid in tank
#define HTANK       300 // tank hight
#define WTANK       100 // tank width
#define X1TANK      200 // position of tank
#define Y1TANK      100 // position of tank

int desired_level = 150;

struct tank_t {     //tank data structure
    pthread_mutex_t mutex[2];
    pthread_cond_t  C_f[2], C_t[2];
    int level;
    int x1, y1, x2, y2, h, w, color;
    int sensor, xsensor;
    int tnum;
    bool    tap;
}tank[2];

void init_tank_t(struct tank_t *t, int i){        //tank inizialization

    pthread_mutex_init(&t->mutex[i-1], NULL);   //mutex and condition variable init
    pthread_cond_init(&t->C_f[i-1], NULL);
    pthread_cond_init(&t->C_t[i-1], NULL);
    t->level = MINLEVEL;
    t->tap  = FALSE;
    t->x1 = X1TANK * i;
    t->y1 = Y1TANK;
    t->x2 = X1TANK * i + WTANK;
    t->y2 = (Y1TANK + HTANK);
    t->h = HTANK;
    t->w = WTANK;
    t->color = WHITE;
    t->sensor = SMIN;
    t->xsensor = (X1TANK * i) +50;
    t->tnum = i;
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
    textout_centre_ex(screen, font, "O200200pen", 100, 198, 255, -1);
}

void read_sensor(struct tank_t *t){     //sensor
    int c = 0;                          // pixel value
    int y = Y0;                         // sensor coordinates
    int d = SMIN;                       // min sensor distance

    do{
        y = Y0 + d;
        c = getpixel(screen, t->xsensor, y);    //get pixel color
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
        pthread_cond_signal(&t->C_f[t->tnum-1]);
        }
}

void check_tap(struct tank_t *t){       //if tap is hold unlock "emptier"
    if (mouse_b == 1)
        t->tap = TRUE;
    else 
        t->tap = FALSE;

    if (t->tap && t->level>0)
        pthread_cond_signal(&t->C_t[t->tnum-1]);
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
    struct tank_t *t = &tank[(intptr_t)arg];

    while (1){
        usleep(500); 
        pthread_mutex_lock(&t->mutex[(intptr_t)arg]);
        
        while (!t->tap)             //lock il tap is not click
            pthread_cond_wait(&t->C_t[(intptr_t)arg], &t->mutex[(intptr_t)arg]);
        
        if (t->level>MINLEVEL){
            empty_pixel(t);         //if tank is not empty do it
            update_level(t);
            }
        t->tap = FALSE;
        pthread_mutex_unlock(&t->mutex[(intptr_t)arg]);
    }
}

void *th_filler(void *arg){         //thread that manage fill task
    struct tank_t *t = &tank[(intptr_t)arg];

    while (1){
        usleep(1000);
        pthread_mutex_lock(&t->mutex[(intptr_t)arg]);
        
        while (t->level > desired_level-1)       //if level is higher i'm not refill
            pthread_cond_wait(&t->C_f[(intptr_t)arg], &t->mutex[(intptr_t)arg]);

        if (t->level<MAXLEVEL){
            fill_pixel(t);      //refill
            update_level(t);
        }
        pthread_mutex_unlock(&t->mutex[(intptr_t)arg]);
    }    
}

void *th_tank(void *arg){      //task tank to check the status of tank
    struct tank_t *t = &tank[(intptr_t)arg];
    
    while (1){
        sleep(1/2);
        pthread_mutex_lock(&t->mutex[(intptr_t)arg]);
        check_tap(t);          //check if the tap is hold
        check_level(t);        //check liquid level
        
        // printf("livello: %d\t sensor: %d\n", get_level(t), t->sensor);

        pthread_mutex_unlock(&t->mutex[(intptr_t)arg]);
    }
}

void *th_sensor(void *arg){       //sensor task to evaluate quantity of liquid
    struct tank_t *t = &tank[(intptr_t)arg];
    
    while (1){
        sleep(1/2);
        pthread_mutex_lock(&t->mutex[(intptr_t)arg]);
        update_level(t);
        pthread_mutex_unlock(&t->mutex[(intptr_t)arg]);
    }
}

int main(){
    init_allegro();
    for (int i=0;i<3;i++){
        init_tank_t(&tank[i], i+1);      //init della struct
        create_tank(&tank[i]);
    }
    create_button();

    pthread_t tank[2], filler[2], tap[2], sensor[2];

    for (int i=0;i<3;i++){
        pthread_create(&tank[i], NULL, th_tank, (void *)(intptr_t)i);
        pthread_create(&sensor[i], NULL, th_sensor, (void *)(intptr_t)i);
        pthread_create(&filler[i], NULL, th_filler, (void *)(intptr_t)i);
        pthread_create(&tap[i], NULL, th_tap, (void *)(intptr_t)i);
    }

    for (int i=0;i<3;i++){
        pthread_join(tank[i], NULL);
        pthread_join(filler[i], NULL);
        pthread_join(tap[i], NULL);
    }

    allegro_exit();
    return 0;
}