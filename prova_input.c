#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

//-------------------------------------------------------------
// GLOBAL CONSTANTS
//-------------------------------------------------------------
#define XWIN 640
// window x resolution
#define YWIN 480
// window y resolution
#define XBASE 40
// X start for the message
#define YBASE 50
// Y level for the first task
#define YINC 30
// Y increment for the other tasks
#define BKG 0
// background color
//-------------------------------------------------------------
#define MAXT 10
// max number of tasks
#define LEN 80
// max message length
#define PER 30
// base period
#define PINC 20
// period increment

void init(void){
    char s[LEN];
    allegro_init();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
    clear_to_color(screen, BKG);
    install_keyboard();
    install_mouse();
    show_mouse(screen);
    // ptask_init(SCHED_FIFO);
    sprintf(s, "Press SPACE to create a task");
    textout_ex(screen, font, s, 10, 10, 14, BKG);
}

int main(){
    int a;
    int i=0;
    char ascii, scan;

    init();

    do {
     } while (!key[KEY_ESC]);

    allegro_exit();
}