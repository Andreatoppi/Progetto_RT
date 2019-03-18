#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(){

    allegro_init();
    install_keyboard();
    install_mouse();

    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
    char a;
    int r, g, b;    // range [0, 255]
    int color, color2;
    r = 6; g = 42; b = 255;
    color = makecol(b, g, r);
    color2 = makecol(r, g, b);
    clear_to_color(screen, color);
    circlefill(screen, 320, 240, 50, color2);
    a = readkey();
    textout_ex(screen, font, "Ciaooooooo", 300, 200, 12, -1);
    
    readkey();
    allegro_exit();
    return 0;
}