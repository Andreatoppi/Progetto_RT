#define N           4   // Number of tank
#define XWIN        150 // Window x resolution
#define YWIN        480 // Window y resolution
#define BKG         0   // Background color
#define Y0          100 // Y position of sensor
#define SMIN        0   // Min sensor distance
#define SMAX        300 // Max sensor distance
#define SSTEP       1   // Sensor precision
#define BLUE        11  // VGA code of blue
#define WHITE       15  // VGA code of white
#define MAXLEVEL    300 // Max quantity of liquid in tank
#define MINLEVEL    0   // Min quantity of liquid in tank
#define HTANK       300 // Tank hight
#define WTANK       100 // Tank width
#define X1TANK      150 // Position of tank
#define Y1TANK      100 // Position of tank
#define R           25  // Button radious 
#define DESLVL      100 // Desired level
#define COUNT       0   // Counter offset

struct tank_t {     // Tank data structure
    pthread_mutex_t mutex;
    pthread_cond_t  C_f, C_t;   // fcondition_variable_filler-tank
    int level;
    int x1, y1, x2, y2, h, w, color;    // tank properties
    int sensor;         // value read from sensor
    int xsensor;        // x position of sensor
    int tnum;           // id tank
    bool    tap;        // tap status
    int desired_level;  // desired level controlled by user
    int count;          // offset counter
}tank[N];

struct button {     // Button data structure
    int x, y, r;    // circle coordinates
    int txtcolor, bkgcolor;     // text and background color
    bool state;
}button[N];

struct input_field {
    int x1, y1, x2, y2; // input fields coordinates
    int color;
    bool inc, dec;  // increase and decrease button state
}input_field[N];

void init_tank_t(struct tank_t *t, int i);
void init_button(struct button *b, struct tank_t *t);
void init_input_field(struct input_field *i, struct tank_t *t);
void init_allegro(void);
void init_gui(void);
void create_tank(struct tank_t *t);
void create_button(struct button *b);
void create_input_field(struct input_field *i);
void read_sensor(struct tank_t *t);
void update_level(struct tank_t *t);
void check_level(struct tank_t *t, void *arg);
void change_button(struct button *b);
void check_tap(struct tank_t *t, struct button *b, void *arg);
void fill_pixel(struct tank_t *t);
void empty_pixel(struct tank_t *t);
int get_level(struct tank_t *t);
void show_status(struct tank_t *t);
void check_input(struct input_field *i, struct tank_t *t);
void increase_level(struct tank_t *t);
void decrease_level(struct tank_t *t);
void fill(struct tank_t *t);
void empty(struct tank_t *t);
void *th_tap(void *arg);
void *th_filler(void *arg);
void *th_tank(void *arg);
void *th_sensor(void *arg);
