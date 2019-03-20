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

//init of allegro

void init(void){
    allegro_init();
    install_keyboard();
    install_mouse();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
    clear_to_color(screen, BKG);
    show_mouse(screen);
}

void create_tank(BITMAP *b){
    rectfill(b, 200, 100, 400, 400, 255);
}


void create_button(BITMAP *b){
    circle(b, 100, 200, 50, 255);
    textout_centre_ex(b, font, "Open", 100, 198, 255, -1);
}

int main(){

    BITMAP *buffer;
    int w = 100;
    int h = 100;

    buffer = create_bitmap(w, h);
    clear_bitmap(buffer);
    clear_to_color(buffer, 13);
    
    // init();
    create_tank(buffer);
    create_button(buffer);
    
    do{

    }
    while(!key[KEY_ESC]);

    allegro_exit();
    return 0;
}