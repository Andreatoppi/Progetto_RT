#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define XWIN 640    //window x resolution
#define YWIN 480    //window y resolution
#define BKG 0       //background color

int desired_level = 100;

struct tank_t {                 //tank data structure
    pthread_mutex_t mutex;
    pthread_cond_t C_f, C_t;
    int level;
    bool tap;
    int x1, y1, x2, y2, h, w, color;
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
    t->color = 255;
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

void check_level(struct tank_t *t){
    if (t->level < desired_level-1){
        pthread_cond_signal(&t->C_f);
        }
}

void check_tap(struct tank_t *t){
    if (mouse_b == 1)
        t->tap = TRUE;
    else 
        t->tap = FALSE;
    if (t->tap && t->level>0)
        pthread_cond_signal(&t->C_t);
    }

int get_level(struct tank_t *t){
    return t->level;
}

void fill_pixel(struct tank_t *t){
    int color_blue = makecol(0,255,255);
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, t->y2-t->level, color_blue);
}

void empty_pixel(struct tank_t *t){
    int color_white = makecol(255,255,255);
    for (int j=t->x1;j<t->x2+1;j++)
        putpixel(screen, j, t->y2-t->level, color_white);
}

void *th_tap(void *arg){
    struct tank_t *t = &tank;
    while (1){
        sleep (9/10);
        pthread_mutex_lock (&t->mutex);
        while (!t->tap)
            pthread_cond_wait (&t->C_t, &t->mutex);
        if (t->level>0){
            empty_pixel(t);
            t->level--;}
        pthread_mutex_unlock (&t->mutex);
    }
}

void *th_filler(void *arg){
    struct tank_t *t = &tank;
    while(1){
        sleep(9/10);
        pthread_mutex_lock (&t->mutex);
        while(t->level > desired_level-1)
            pthread_cond_wait(&t->C_f, &t->mutex);
        fill_pixel(t);
        t->level++;
        pthread_mutex_unlock (&t->mutex);
    }    
}

void *th_tank (void *arg){
    struct tank_t *t = &tank;
    int level;
    while(1){
        // sleep(1);
        pthread_mutex_lock (&t->mutex);                 
        
        check_level (&tank);
        check_tap (&tank);
        // level = get_level(t);
        printf("livello: %d\n",get_level(t));

        pthread_mutex_unlock (&t->mutex);
    }
}

int main(){
    init_allegro();
    init_tank_t(&tank);      //init della struct

    create_tank(&tank);
    create_button();

    pthread_t tank;
    pthread_t filler;
    pthread_t tap;

    pthread_create (&tank, NULL, th_tank, NULL);
    pthread_create (&filler, NULL, th_filler, NULL);
    pthread_create (&tap, NULL, th_tap, NULL);

    srand(time(NULL));      //funzione di appoggio per numeri random
    
    // do{

    // }
    // while(!key[KEY_ESC]);

    pthread_join (tank, NULL);
    pthread_join (filler, NULL);
    pthread_join (tap, NULL);

    allegro_exit();
    return 0;
}