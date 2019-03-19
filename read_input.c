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
    // sprintf(s, "Press SPACE to create a task");
    // textout_ex(screen, font, s, 10, 10, 14, BKG);
}

void get_keycodes(char *scan, char *ascii)
{
    int k;
    k = readkey();
    *ascii = k;
    *scan = k >> 8;
}

int main(){
    char str[20];
    int i=0;
    char ascii, scan, s[2];
    int x = 250;

    init();
    do{
        textout_ex(screen, font, "Premi ESC per uscire", textout_centre_ex+10, textout_centre_ex+10, 12, -1);
        textout_ex(screen, font, "Inserisci in nome del giocatore", x, 200, 12, -1);
        do {
            get_keycodes(&scan,&ascii);
            if (scan != KEY_ENTER) {
                str[i] = ascii;   // put ascii in s for echoing
                str[i+1] = '\0';
                clear_to_color(screen, BKG);
                textout_ex(screen, font, str, x, 220, 12, -1);    // echo
                // x = x + 8;
                i++;
            }       
        } while (scan!=KEY_ENTER);

        char nome[20];
        for (int j=0;j<20;j++)
            nome[j] = str[j];

        textout_ex(screen, font, "Il nome del giocatore e'", x, x+10, 12, -1);
        textout_ex(screen, font, nome, x, x+20, 12, -1);

    }while(!key[KEY_ESC]);
    allegro_exit();
}