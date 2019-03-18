#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(){

    allegro_init();
    int col = 14;
    int x, y;

    install_keyboard();
    install_mouse();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
    show_mouse(screen);

    do {
        if (mouse_b == 0) {
        x = mouse_x;
        y = mouse_y;
        scare_mouse();
        putpixel(screen, x, y, col);
        unscare_mouse();
        }
        if (mouse_b == 1) {
        x = mouse_x;
        y = mouse_y;
        scare_mouse();
        putpixel(screen, x, y, 100);
        unscare_mouse();
        }
        if (mouse_b == 2) {
        x = mouse_x;
        y = mouse_y;
        scare_mouse();
        putpixel(screen, x, y, 0);
        unscare_mouse();
        }
    } while (!key[KEY_ESC]);

    allegro_exit();
    return 0;
}