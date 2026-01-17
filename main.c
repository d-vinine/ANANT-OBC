#include <stdio.h>

typedef enum {
    INIT,
    IDLE,
    TESTING,
    READY,
    LAUNCH,
    MAX_HEIGHT,
    DESCENT,
    PARA_DEPLOYMENT,
    LANDED,
    RECOVERY,
    SHUTDOWN,
} States;

typedef struct { 
    int PWR;
    int GUN;
    int TEL;
    int BEACON;
    int LOCATION;
    int DATA;
    int VID_REC;
    int PARA_TRIGGER;
    int TEST_MODE;
    int READY;
} Outputs;
void set_output(Outputs *out, int pwr, int gun, int tel, int beacon, int location, int data, int vid_rec, int para_trigger, int test_mode, int ready);
void apply_transition_outputs(States st, Outputs *out);

// Mock input prototypes 
int get_CMD_TEST();
int get_OK();
int get_TESTS();
int get_REBOOT();
int get_ACCL();
int get_ALTITUDE();
int get_MAX_ALT();
int get_PRESSURE();
int get_RECOVERED();
int get_POWER_OFF();

int main() {
    States state = INIT;
    Outputs current_out;

    apply_transition_outputs(state, &current_out);

    int run = 1;
    while (run) {
        States next_state = state;

        switch (state) {
            case INIT:
                if (get_OK() == 1) next_state = IDLE;
                break;

            case IDLE:
                if (get_CMD_TEST() == 1) next_state = TESTING;
                if (get_ACCL() == 1) next_state = LAUNCH;
                break;

            case TESTING:
                if (get_TESTS() == 1)  next_state = READY;
                if (get_TESTS() == 0) next_state = INIT;
                break;

            case READY:
                if (get_REBOOT() == 1) next_state = INIT;
                break;

            case LAUNCH:
                if (get_MAX_ALT() == 1) next_state = MAX_HEIGHT;
                break;

            case MAX_HEIGHT:
                if (get_MAX_ALT() == 1 && get_ACCL() == 1) next_state = DESCENT;
                break;

            case DESCENT:
                if (get_ALTITUDE() <= 200) next_state = PARA_DEPLOYMENT; 
                break;

            case PARA_DEPLOYMENT:
                if (get_PRESSURE() == 1) next_state = LANDED;
                break;

            case LANDED:
                if (get_RECOVERED() == 1) next_state = RECOVERY;
                break;

            case RECOVERY:
                if (get_POWER_OFF() == 1) next_state = SHUTDOWN;
                break;

            case SHUTDOWN:
                run = 0; 
                break;
        }

        if (next_state != state) {
            state = next_state;
            apply_transition_outputs(state, &current_out);
        }
    }

    return 0;
}

// --- Logic Implementation ---

void apply_transition_outputs(States st, Outputs *out) {
    switch (st) {
        case INIT:            set_output(out, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0); break;
        case IDLE:            set_output(out, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0); break;
        case TESTING:         set_output(out, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0); break;
        case READY:           set_output(out, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1); break;
        case LAUNCH:          set_output(out, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0); break;
        case MAX_HEIGHT:      set_output(out, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0); break;
        case DESCENT:         set_output(out, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0); break;
        case PARA_DEPLOYMENT: set_output(out, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0); break;
        case LANDED:          set_output(out, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0); break;
        case RECOVERY:        set_output(out, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0); break;
        case SHUTDOWN:        set_output(out, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); break;
    } 
}

void set_output(
    Outputs *out,
    int pwr, int gun, int tel, int beacon, int location, 
    int data, int vid_rec, int para_trigger, int test_mode, int ready
) {
    out->PWR          = pwr;
    out->GUN          = gun;
    out->TEL          = tel;
    out->BEACON       = beacon;
    out->LOCATION     = location;
    out->DATA         = data;
    out->VID_REC      = vid_rec;
    out->PARA_TRIGGER = para_trigger;
    out->TEST_MODE    = test_mode;
    out->READY        = ready;
}

int get_CMD_TEST()   { return 0; }
int get_OK()         { return 1; }
int get_TESTS()      { return 1; }
int get_REBOOT()     { return 1; }
int get_ACCL()       { return 1; }
int get_ALTITUDE()   { return 1; }
int get_MAX_ALT()    { return 1; }
int get_PRESSURE()   { return 1; }
int get_RECOVERED()  { return 1; }
int get_POWER_OFF()  { return 1; }
