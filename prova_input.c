#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(){
    int a;
    int i=0;
    char ascii, scan;

    allegro_init();
    install_keyboard();
    install_mouse();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
    show_mouse(screen);


    do {

        int ok = 0;
        if(ok == 0){
            a = readkey();
            clear_to_color(screen, 0);
            ascii = a & 0xFF;
            scan = a >> 8;
        }
        //printf("%d",a[i]);
        char s = a;
        textout_ex(screen, font, &scan, 300, 200, 12, -1);

        i++;
    } while (!key[KEY_ENTER]);

    allegro_exit();
}