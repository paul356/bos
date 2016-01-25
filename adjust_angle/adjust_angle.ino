#define NSTEPPERS 3
#define NSTEPS 50
#define HALF_PULSE 50
#define MARK_SIZE 2

#define PULSE_DEGREE_RATIO_BASE ((long)8000L*6.75)
#define PULSE_DEGREE_RATIO_FIRST ((long)8000L*15.375)
#define PULSE_DEGREE_RATIO_SECOND ((long)8000L*11.25)

long pulse_degree_ratio [NSTEPPERS] = {PULSE_DEGREE_RATIO_BASE, PULSE_DEGREE_RATIO_FIRST, PULSE_DEGREE_RATIO_SECOND};
long real_angles        [NSTEPPERS] = {0*PULSE_DEGREE_RATIO_BASE/360., 0*PULSE_DEGREE_RATIO_FIRST/360., 0*PULSE_DEGREE_RATIO_SECOND/360.};

float degree_limits     [NSTEPPERS][2] = {{-90., 90.}, {0., 90.}, {0., 135.}};

const int enable_pin    [NSTEPPERS] = {55, 5, 25};
const int direction_pin [NSTEPPERS] = {60, 54, 27};
const int step_pin      [NSTEPPERS] = {56, 4, 26};

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

long angle_to_pulse(float deg, int idx)
{
    return (long)((pulse_degree_ratio[idx] / 360.0) * deg);
    //return (long)(deg * pulse_degree_ratio[idx]);
}

int serial_read_sync()
{
    while (1) {
        if (Serial.available() > 0) {
            return Serial.read();
        }
    }
}

int read_angles(long angles[])
{
    float degs[NSTEPPERS];

    // Get angle from serial port
    // 1. Find the start of a set of angles
    while (serial_read_sync() != 'a') {
        ;
    }
    for (int i=0; i<NSTEPPERS; i++) {
        if (i != 0) {
            serial_read_sync();
        }
        serial_read_sync();
        degs[i] = Serial.parseFloat();
    }

    int ret = 0;
    for (int i=0; i<NSTEPPERS; i++) {
        if (degs[i] < degree_limits[i][0]) {
            degs[i] = degree_limits[i][0];
            ret = i+1;
        }
        if (degs[i] > degree_limits[i][1]) {
            degs[i] = degree_limits[i][1];
            ret = i+1;
        }
        angles[i] = angle_to_pulse(degs[i], i);
    }

    return ret;
}

void pulse_stepper(int idx, long delta, int dir)
{
    digitalWrite(enable_pin[idx], LOW);
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
    // Finish remainders
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

void echo_done (int ret)
{
    Serial.print('0' + ret);
}

void dump_real_angles ()
{
    char prefix[] = {'a', 'b', 'c'};
    for (int j=0; j<NSTEPPERS; j++) {
        Serial.print(prefix[j]);
        Serial.print('=');
        Serial.print(real_angles[j]);
        Serial.print(',');
    }
    Serial.print('\n');
}

/*
   void change_to_degree(float a, float b, float c)
   {
   float degs[NSTEPPERS];
   long angles[NSTEPPERS];

   degs[0] = a; degs[1] = b; degs[2] = c;

   for (int i=0; i<NSTEPPERS; i++) {
   angles[i] = angle_to_pulse(degs[i], i);
   }

   change_to_angle(&angles[0]);
   }
   */

void loop()
{
    long angles[NSTEPPERS];

    int out_of_range =  read_angles(&angles[0]);

    change_to_angle(&angles[0]);

    echo_done(out_of_range);
    dump_real_angles();
}

