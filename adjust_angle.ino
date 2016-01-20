#define NSTEPPERS 3
#define PULSE_DEGREE_RATIO 8000
#define NSTEPS 200
#define HALF_PULSE 100
#define MARK_SIZE 2

int  base_degrees  [NSTEPPERS] = {0, 90, 180};
long real_angles   [NSTEPPERS] = {0, 90, 180}; // set it based on reference point
const int enable_pin    [NSTEPPERS] = {0, 0, 0};
const int direction_pin [NSTEPPERS] = {0, 0, 0};
const int step_pin      [NSTEPPERS] = {0, 0, 0};

void setup()
{
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    for (int i=0; i<NSTEPPERS; i++) {
        pinMode(enable_pin[i], OUTPUT);
    }

    for (int i=0; i<NSTEPPERS; i++) {
        pinMode(direction_pin[i], OUTPUT);
    }

    for (int i=0; i<NSTEPPERS; i++) {
        pinMode(step_pin[i], OUTPUT);
    }
}

long angle_to_pulse(float deg)
{
    return (long)(deg * PULSE_DEGREE_RATIO);
}

void read_angles(long angles[])
{
    float degrees[NSTEPPERS];
    int nbytes = 0;

    // Get angle from serial port
    // 1. Find the start of a set of angles
    while (1) {
        if (Serial.available() > 0) {
            int b = Serial.read();
            if (b == 'a') {
                nbytes ++;
                break;
            }
        }
    }

    while (nbytes < (MARK_SIZE + NSTEPPERS*sizeof(float))) {
        if (Serial.available() > 0) {
            int b = Serial.read();
            nbytes ++;
            if (nbytes >= MARK_SIZE) {
            }
        }
    }

    for (int i=0; i<NSTEPPERS; i++) {
        angles[i] = angle_to_pulse(degrees[i] - base_degrees[i]);
    }
}

void pulse_stepper(int idx, long delta, int dir)
{
    digitalWrite(enable_pin[idx]);
    digitalWrite(direction_pin[idx], dir);

    for (long i=0; i<delta; i++) {
        digitalWrite(step_pin[idx], HIGH);
        delayMicroseconds(HALF_PULSE);
        digitalWrite(step_pin[idx], LOW);
        delayMicroseconds(HALF_PULSE);
    }
}

void change_to_angle(long angles[])
{
    long pulse_done[NSTEPPERS] = {0, 0, 0};

    long pulse_delta[NSTEPPERS];
    int  pulse_dir[NSTEPPERS];

    for (int j=0; j<NSTEPPERS; j++) {
        if (angles[j] >= real_angles[j]) {
            pulse_dir[j] = HIGH;
        } else {
            pulse_dir[j] = LOW;
        }
        pulse_delta[j] = abs(angles[j] - real_angles[j]);
    }

    for (int i=0; i<NSTEPS; i++) {
        for (int j=0; j<NSTEPPERS; j++) {
            long interim = map(i, 0, NSTEPS, 0, pulse_delta[j]);
            if (interim > pulse_done[j]) {
                pulse_stepper(j, interim - pulse_done[j], pulse_dir[j]);
                pulse_done[j] = interim;
            }
        }
    }
    // Finish remainers
    for (int j=0; j<NSTEPPERS; j++) {
        if (pulse_delta[j] > pulse_done[j]) {
            pulse_stepper(j, pulse_delta[j] - pulse_done[j], pulse_dir[j]);
        }
    }

    // Save new angles
    for (int j=0; j<NSTEPPERS; j++) {
        real_angles[j] = angles[j];
    }
}

void loop()
{
    long angles[NSTEPPERS];

    read_angles(&angles[0]);

    change_to_angle(&angles[0]);
}
