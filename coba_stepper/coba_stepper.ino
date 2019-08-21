// ----- motor definitions 
#define STEPS_PER_MM 200*2/40      //200steps/rev; 16 x microstepping; 40mm/rev
#define NUDGE STEPS_PER_MM*5        //move pen 5mm (change number to suit)
#define DIR1 5                      //arduino ports
#define DIR2 6
#define STEP1 2
#define STEP2 3

// ----- Bit set/clear/check/toggle macros
#define SET(x,y) (x |=(1<<y)) 
#define CLR(x,y) (x &= (~(1<<y)))
#define CHK(x,y) (x & (1<<y))
#define TOG(x,y) (x^=(1<<y))

bool
CW = true,                          //flag ... does not affect motor direction
CCW = false,                        //flag ... does not affect motor direction
DIRECTION1,                         //motor directions can be changed in step_motors()
DIRECTION2;

long
PULSE_WIDTH = 2,                    //easydriver step pulse-width (uS)
DELAY = 250; 

void setup() {
  // put your setup code here, to run once:
  // ----- initialise motor1
  pinMode(DIR1, OUTPUT);
  pinMode(STEP1, OUTPUT);
  digitalWrite(DIR1, CW);
  delayMicroseconds(PULSE_WIDTH);
  digitalWrite(STEP1, LOW);

  // ----- initialise motor2
  pinMode(DIR2, OUTPUT);
  pinMode(STEP2, OUTPUT);
  digitalWrite(DIR2, CW);
  delayMicroseconds(PULSE_WIDTH);
  digitalWrite(STEP2, LOW);

  Serial.begin(9600);
}
void step_motors() {

  // ----- locals
  enum {dir1, step1, dir2, step2};                                //define bit positions
  byte pattern = PORTB;                                           //read current state PORTB

  // ----- set motor directions
  //(DIRECTION1 == CW) ? SET(pattern, dir1) : CLR(pattern, dir1); //normal motor direction
  //(DIRECTION2 == CW) ? SET(pattern, dir2) : CLR(pattern, dir2); //normal motor direction
  (DIRECTION1 == CCW) ? SET(pattern, dir1) : CLR(pattern, dir1);  //motor windings reversed
  (DIRECTION2 == CCW) ? SET(pattern, dir2) : CLR(pattern, dir2);  //motor windings reversed
  PORTB = pattern;
  delayMicroseconds(PULSE_WIDTH);                                 //wait for direction lines to stabilise

  // ----- create leading edge of step pulse(s)
  pattern = SET(pattern, step1);                                  //prepare step pulse
  pattern = SET(pattern, step2);
  PORTB = pattern;                                                //step the motors
  delayMicroseconds(PULSE_WIDTH);                                 //mandatory delay

  // ----- create trailing-edge of step-pulse(s)
  pattern = CLR(pattern, step1);
  pattern = CLR(pattern, step2);
  PORTB = pattern;

  // ----- determines plotting speed
  delayMicroseconds(DELAY);   
}
void loop() {
  // put your main code here, to run repeatedly:

}
