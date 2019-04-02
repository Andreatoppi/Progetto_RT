#define N           4  //number of tank
#define XWIN        850 // window x resolution
#define YWIN        480 // window y resolution
#define BKG         0   // background color
#define X0          250 // x position of sensor
#define Y0          100 // y position of sensor
#define SMIN        0   // min sensor distance
#define SMAX        300 // max sensor distance
#define SSTEP       1   // sensor precision
#define BLUE        11  // VGA code of blue
#define WHITE       15  // VGA code of white
#define MAXLEVEL    300 // max quantity of liquid in tank
#define MINLEVEL    0   // min quantity of liquid in tank
#define HTANK       300 // tank hight
#define WTANK       100 // tank width
#define X1TANK      150 // position of tank
#define Y1TANK      100 // position of tank
#define R           25  // button radious 
#define DESLVL      100 // desired level


struct tank_t {     //tank data structure
    pthread_mutex_t mutex[N];
    pthread_cond_t  C_f[N], C_t[N];
    int level;
    int x1, y1, x2, y2, h, w, color;    // tank properties
    int sensor;     // value read from sensor
    int xsensor;    // x position of sensor
    int tnum;       // id tank
    bool    tap;    // tap status
}tank[N];

struct button {
    int x, y, r;    //circle coordinates
    int txtcolor, bkgcolor;     //text and background color
}button[N];

void create_tank(struct tank_t *t);
void create_button(struct button *b);
void read_sensor(struct tank_t *t);
void update_level(struct tank_t *t);
void check_level(struct tank_t *t, void *arg);
void check_tap(struct tank_t *t, struct button *b, void *arg);
int get_level(struct tank_t *t);
void fill_pixel(struct tank_t *t);
void empty_pixel(struct tank_t *t);
void init_tank_t(struct tank_t *t, int i);
void init_button(struct button *b, struct tank_t *t);
void init_allegro(void);
void *th_tap(void *arg);
void *th_filler(void *arg);
void *th_tank(void *arg);
void *th_sensor(void *arg);