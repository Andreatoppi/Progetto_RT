#include <allegro.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(){

    allegro_init();     //inizializzazione Allegro strutture dati
    int col = 14;       //colore sfondo
    int x, y;           //valori posizione

    install_keyboard(); //supporto tastiera
    install_mouse();    //supporto mouse
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);  //opening window
    show_mouse(screen); //show mouse pointer

    do {
        if (mouse_b == 0) {     //Print dei pixel in base al tasto schiacciato
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
    } while (!key[KEY_ESC]);    //chiusura della windows al tasto ESC

    allegro_exit();     //Uscita Allegro
    return 0;
}