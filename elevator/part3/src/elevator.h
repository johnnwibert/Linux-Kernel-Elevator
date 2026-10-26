#define MIN_FLOOR 1
#define MAX_FLOOR 5
#define MAX_PETS 5
#define MAX_WEIGHT 50

// 5 elevator movement states 
enum elevator_state { OFFLINE, IDLE, LOADING, UP, DOWN };

struct pet
{
    struct list_head list;
    int type, weight, start_floor, dest_floor;
};

struct elevator
{
    enum elevator_state state;
    int current_floor, load_weight, load_count;
    bool stopping; // set by stop_elevator
    struct list_head riders; // pets on board
    struct task_struct *thread;
    struct mutex lock;
};

// tells every file that includes elevator.h that a variable named elev exists somewhere else
extern struct elevator elev;

int elevator_thread_fn(void *data);