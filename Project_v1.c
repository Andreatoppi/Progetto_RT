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

void create_tank(){
    rectfill(screen, 200, 100, 400, 400, 255);
}


void create_button(){
    circle(screen, 100, 200, 50, 255);
    textout_centre_ex(screen, font, "Open", 100, 198, 255, -1);
}

int main(){

    init();
    create_tank();
    create_button();
    do{

    }
    while(!key[KEY_ESC]);

    allegro_exit();
    return 0;
}