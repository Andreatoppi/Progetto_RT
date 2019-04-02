#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

//------------------------------------------------------------------------------
// GLOBAL CONSTANTS
//------------------------------------------------------------------------------
#define N           4  //number of tank
#define XWIN        850 // window x resolution
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
#define X1TANK      150 // position of tank
#define Y1TANK      100 // position of tank
#define R           25  // button radious 

int desired_level = 100;

//------------------------------------------------------------------------------
// STRUCT DEFINITION. TANK AND BUTTON
//------------------------------------------------------------------------------
struct tank_t {     //tank data structure
    pthread_mutex_t mutex[N];
    pthread_cond_t  C_f[N], C_t[N];
    int level;
    int x1, y1, x2, y2, h, w, color;    // tank properties
    int sensor;     // value read from sensor
    int xsensor;    // x position of sensor
    int tnum;       // id tank
    bool    tap;    // tap status
}tank[N];

struct button {
    int x, y, r;    //circle coordinates
    int txtcolor, bkgcolor;     //text and background color
}button[N];

//------------------------------------------------------------------------------
// STRUCT INITIALIZATION
//------------------------------------------------------------------------------
void init_tank_t(struct tank_t *t, int i){

    pthread_mutex_init(&t->mutex[i-1], NULL);
    pthread_cond_init(&t->C_f[i-1], NULL);
    pthread_cond_init(&t->C_t[i-1], NULL);
    t->level = MINLEVEL;
    t->tap  = FALSE;
    t->x1 = X1TANK * i;
    t->y1 = Y1TANK;
    t->x2 = (X1TANK * i) + WTANK;
    t->y2 = (Y1TANK + HTANK);
    t->h = HTANK;
    t->w = WTANK;
    t->color = WHITE;
    t->sensor = SMIN;
    t->xsensor = (X1TANK * i) + (WTANK / 2);
    t->tnum = i;
}

void init_button(struct button *b, struct tank_t *t){
    b->x = t->xsensor;
    b->y = t->y1 - 50;
    b->r = R;
    b->txtcolor = BKG;
    b->bkgcolor = WHITE;
}

void init_allegro(void){
    allegro_init();
    install_mouse();
    install_keyboard();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
    clear_to_color(screen, BKG);
    show_mouse(screen);
}

// Function that print tank on the screen
void create_tank(struct tank_t *t){
    rectfill(screen, t->x1, t->y1, t->x2, t->y2, t->color);
}

// Function that print circle button on the screen
void create_button(struct button *b){
    circle(screen, b->x, b->y, b->r, b->bkgcolor);
    textout_centre_ex(screen, font, "Open", b->x, b->y, b->bkgcolor, b->txtcolor);
}

// Function used by sensor thread to evaluate proximity of liquid
void read_sensor(struct tank_t *t){
    int c = 0;      // pixel value
    int y = Y0;     // y sensor coordinates
    int d = SMIN;   // min sensor distance

    do{
        y = Y0 + d;
        c = getpixel(screen, t->xsensor, y);    //get pixel color
        d = d + SSTEP;
    }while (c == WHITE);
    t->sensor = d-2;    // update sensor value
}

// Function that read sensor and deduct the level
void update_level(struct tank_t *t){
    read_sensor(t);
    t->level = MAXLEVEL - t->sensor;
}

// Function that unloch the refill if liquid in low
void check_level(struct tank_t *t, void *arg){
    if (t->level < desired_level){
        pthread_cond_signal(&t->C_f[(intptr_t)arg]);
        }
}

// Function that check mouse status, so the tap status
void check_tap(struct tank_t *t, struct button *b, void *arg){
    int x, y;   //mouse coordinates
    x = mouse_x;
    y = mouse_y;

    if (mouse_b == 1)
        if (b->x-b->r < x && x < b->x+b->r && b->y-b->r < y && y < b->y+b->r)
            t->tap = TRUE;
        else
            t->tap = FALSE;       
    else 
        t->tap = FALSE;

    if (t->tap && t->level>0)
        pthread_cond_signal(&t->C_t[(intptr_t)arg]);
    }

int get_level(struct tank_t *t){
    update_level(t);
    return t->level;
}

// Function that put blue pixel on the screen
void fill_pixel(struct tank_t *t){
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, t->y2-t->level, BLUE);
}

// Function that put white pixel on the screen
void empty_pixel(struct tank_t *t){
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, (t->y2-t->level)+1, WHITE);
}

//------------------------------------------------------------------------------
// THREAD
//------------------------------------------------------------------------------
void *th_tap(void *arg){            //tap sensor
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
    struct button *b = &button[(intptr_t)arg];   
    
    while (1){
        usleep(500);
        pthread_mutex_lock(&t->mutex[(intptr_t)arg]);
        check_tap(t, b, arg);       //check if the tap is hold
        check_level(t, arg);        //check liquid level
        
        // printf("livello: %d\t sensor: %d\t tank: %ld\n" , get_level(t), t->sensor, (intptr_t)arg);

        pthread_mutex_unlock(&t->mutex[(intptr_t)arg]);
    }
}

void *th_sensor(void *arg){       //sensor task to evaluate quantity of liquid
    struct tank_t *t = &tank[(intptr_t)arg];
    
    while (1){
        usleep(500);
        pthread_mutex_lock(&t->mutex[(intptr_t)arg]);
        update_level(t);
        pthread_mutex_unlock(&t->mutex[(intptr_t)arg]);
    }
}

//------------------------------------------------------------------------------
// MAIN FUNCTION
//------------------------------------------------------------------------------
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