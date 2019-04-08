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

//------------------------------------------------------------------------------
// STRUCT INITIALIZATION
//------------------------------------------------------------------------------
void init_tank_t(struct tank_t *t, int i){

    pthread_mutex_init(&t->mutex, NULL);
    pthread_cond_init(&t->C_f, NULL);
    pthread_cond_init(&t->C_t, NULL);
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
    t->desired_level = DESLVL;
}

void init_button(struct button *b, struct tank_t *t){
    b->x = t->xsensor;
    b->y = t->y1 - 50;
    b->r = R;
    b->txtcolor = BKG;
    b->bkgcolor = WHITE;
}

void init_input_field(struct input_field *i, struct tank_t *t){
    i->x1 = t->x1;
    i->x2 = t->x2;
    i->y1 = t->y2 + 55;
    i->y2 = t->y2 + 65;
    i->color = WHITE;
    i->inc = i->dec = FALSE;
}

//------------------------------------------------------------------------------
// GUI INITIALIZATION
//------------------------------------------------------------------------------
void init_allegro(void){
    allegro_init();
    install_mouse();
    install_keyboard();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
    clear_to_color(screen, BKG);
    show_mouse(screen);
}

void init_gui(void){
    for (int i=0;i<N;i++){
        init_tank_t(&tank[i], i+1);
        init_button(&button[i], &tank[i]);
        init_input_field(&input_field[i], &tank[i]);
        create_tank(&tank[i]);
        create_button(&button[i]);
        create_input_field(&input_field[i]);
    }
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

// Function that print button on the screen
void create_input_field(struct input_field *i){
    rectfill(screen, i->x1, i->y1, i->x2, i->y2, i->color);
    textout_centre_ex(screen, font, "+", i->x1+R, i->y2-8, BKG, WHITE);
    textout_centre_ex(screen, font, "-", i->x2-R, i->y2-8, BKG, WHITE);
}

//------------------------------------------------------------------------------
// LOGICAL FUNCTIONS
//------------------------------------------------------------------------------

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
    if (t->level < t->desired_level){
        pthread_cond_signal(&t->C_f);
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
        pthread_cond_signal(&t->C_t);
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

// Function thah print tank status on the screen
void show_status(struct tank_t *t){
    char lvl[12], d_lvl[14], nofluid[9], maxlvl[10], clear[12];
    sprintf(lvl," Lvl: %d ", t->level);
    sprintf(d_lvl, " Lvl des: %d ", t->desired_level);
    sprintf(nofluid, "No fluid");
    sprintf(maxlvl, "Max level");
    sprintf(clear, "         ");

    textout_centre_ex(screen, font, clear, t->xsensor, t->y2+15, BKG, BKG);
    
    if (t->level == MINLEVEL)
        textout_centre_ex(screen, font, nofluid, t->xsensor, t->y2+15, WHITE, BKG);
    
    if (t->level == MAXLEVEL)
        textout_centre_ex(screen, font, maxlvl, t->xsensor, t->y2+15, WHITE, BKG);
    

    textout_centre_ex(screen, font, lvl, t->xsensor, t->y2+35, BKG, BKG);
    textout_centre_ex(screen, font, lvl, t->xsensor, t->y2+35, WHITE, BKG);
    textout_centre_ex(screen, font, d_lvl, t->xsensor, t->y1-15, BKG, BKG);
    textout_centre_ex(screen, font, d_lvl, t->xsensor, t->y1-15, WHITE, BKG);

}

// Function that increase desiderated level
void increase_level(struct tank_t *t){
    t->desired_level++;
}

// Function that decrease desiderated level
void decrease_level(struct tank_t *t){
    t->desired_level--;
}

// Function that cack if user want to change desiderated lvl
void check_input(struct input_field *i, struct tank_t *t){
    int x, y;   //mouse coordinates
    x = mouse_x;
    y = mouse_y;

    if (i->x1 < x && x < i->x2-50 && i->y1 < y && y < i->y2 && mouse_b == 1)
        i->inc = TRUE;
    if (i->x1+50 < x && x < i->x2 && i->y1 < y && y < i->y2 && mouse_b == 1)
        i->dec = TRUE;

    if (i->inc){
        i->inc = FALSE;
        increase_level(t);
    }
    if (i->dec){
        i->dec = FALSE;
        decrease_level(t);
    }
}

//------------------------------------------------------------------------------
// THREAD
//------------------------------------------------------------------------------
void *th_tap(void *arg){        //tap sensor
    struct tank_t *t = &tank[(intptr_t)arg];

    while (1){
        usleep(10000); 
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

void *th_filler(void *arg){     //thread that manage fill task
    struct tank_t *t = &tank[(intptr_t)arg];

    while (1){
        usleep(20000);
        pthread_mutex_lock(&t->mutex);
        
        while (t->level > t->desired_level-1)       //if level is higher i'm not refill
            pthread_cond_wait(&t->C_f, &t->mutex);

        if (t->level<MAXLEVEL){
            fill_pixel(t);      //refill
            update_level(t);
        }

        pthread_mutex_unlock(&t->mutex);
    }    
}

void *th_tank(void *arg){       //task tank to check the status of tank
    struct tank_t *t = &tank[(intptr_t)arg];
    struct button *b = &button[(intptr_t)arg];  
    struct input_field *i = &input_field[(intptr_t)arg];  
    
    while (1){
        usleep(10000);
        pthread_mutex_lock(&t->mutex);
        check_tap(t, b, arg);       // check if the tap is hold
        check_level(t, arg);        // check liquid level
        show_status(t);             // show the current level of liquid
        check_input(i, t);
        // printf("livello: %d\t sensor: %d\t tank: %ld\n" , get_level(t), t->sensor, (intptr_t)arg);

        pthread_mutex_unlock(&t->mutex);
    }
}

void *th_sensor(void *arg){     //sensor task to evaluate quantity of liquid
    struct tank_t *t = &tank[(intptr_t)arg];
    
    while (1){
        usleep(10000);
        pthread_mutex_lock(&t->mutex);
        update_level(t);
        pthread_mutex_unlock(&t->mutex);
    }
}