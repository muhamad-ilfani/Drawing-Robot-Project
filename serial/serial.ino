int hitung=1;

char c = ' ';
char d = ' ';

//Serial
#define STRING_SIZE 128             //string size
char
BUFFER[STRING_SIZE + 1],
INPUT_CHAR;
String
INPUT_STRING,
SUB_STRING;
int
INDEX = 0,                        //buffer index
START,                            //used for sub_string extraction
FINISH;

float
X,                                //gcode float values held here
Y,
I,
J;

// ----- plotter definitions
#define BAUD 9600
#define XOFF 0x13                   //pause transmission (19 decimal)
#define XON 0x11                    //resume transmission (17 decimal)
#define PEN 3

// ----- constants
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

//steper
#define EN 8 // stepper motor enable , active low
#define X_DIR 5 // X -axis stepper motor direction control
#define Y_DIR 6 // y -axis stepper motor direction control
#define X_STP 2 // x -axis stepper control
#define Y_STP 3 // y -axis stepper control

//trajektori
#define rotation 400
#define arc_max 0.2
#define error_max 2.0
#define SCALE_FACTOR 1
#define steps 200*2/40 //step per mm

//servo
#include <Servo.h>
#define servo 11
Servo myservo; 

bool dir_x = false, dir_y = false;
int x,y;
int x1=0, y1=0;
int x_now = 0;
int y_now = 0;
/*
// Function : step . function: to control the direction of the stepper motor , the number of steps .
// Parameters : dir direction control , dirPin corresponding stepper motor DIR pin , stepperPin corresponding stepper motor " step " pin , Step number of step of no return value.
*/
void pen_up(){
  myservo.write(150);
}
void pen_down(){
  myservo.write(138);
}

void x_plus(){
  dir_x = false;
  dir_y = false;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
  x_now++;
}
void x_min(){
  dir_x = true;
  dir_y = true;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
  x_now--;
}
void y_plus(){
  dir_x = false;
  dir_y = true;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
  y_now++;
}
void y_min(){
  dir_x = true;
  dir_y = false;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
  y_now--;
}

void step (boolean dir1, byte dirPin1, byte stepperPin1,boolean dir2, byte dirPin2, byte stepperPin2)
{
    
    digitalWrite (dirPin1, dir1);
    digitalWrite (dirPin2, dir2);
    delayMicroseconds(2);
    for (int i = 0; i <16; i++) 
    {
        digitalWrite (stepperPin1, HIGH);
        digitalWrite (stepperPin2, HIGH);
        delayMicroseconds (300);
        digitalWrite (stepperPin1, LOW);
        digitalWrite (stepperPin2, LOW);
        delayMicroseconds (300);
    }
}

void move_to(float x, float y) {                        //x,y are absolute co-ordinates

  // ----- apply scale factor
  int x2 = round(x * steps * SCALE_FACTOR);      //scale x and y
  int y2 = round(y * steps * SCALE_FACTOR);
  Serial.print(x2);
  Serial.print("\t");
  Serial.println(y2);
  //x = x;
  //y = y;
  x1 = x_now;
  y1 = y_now;
  line_traj(x1, y1, x2, y2);
  x_now = 0;
  y_now = 0;
  //Serial.print("x_init : ");
  //Serial.print("move_to(");
  //Serial.println(");");
  //delay(1000);
  // ----- remember last "scaled" co-ordinate
}

void line_traj (int x1,int y1,int x2,int y2){
  x = x1;
  y = y1;
  int dx = x2 - x1;
  int dy = y2 - y1;
  
  int longest = max(abs(dy), abs(dx));      //longest axis
  int shortest = min(abs(dy), abs(dx));     //shortest axis
  //Serial.println(longest);
  if(abs(dx)>abs(dy)){
    float P = 2*abs(dx) - abs(dy);
    for(int i=0; i<longest; i++){
      if(dx>0){
        dir_x = false;
        dir_y = false;
        //Serial.println("F F X+");
        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
        x++;
      }
      else{
        dir_x = true;
        dir_y = true;
        //Serial.println("T T X-");
        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
        x--;
      }
      if(P<0){
        P = P + 2*abs(dy);
      }
      else{
        P = P + 2*abs(dy) - 2*abs(dx);
        if(dy>0){
          dir_x = false;
          dir_y = true;
          //Serial.println("F T Y+");
          step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
          y++;
        }
        else{
          dir_x = true;
          dir_y = false;
          //Serial.println("T F Y-");
          step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
          y--;
        }
      }
    }
  }
  else{
    float P = 2*abs(dy) - abs(dx);
    for(int i=0; i<longest; i++){
      if(dy>0){
        dir_x = false;
        dir_y = true;
        //Serial.println("F T Y+");
        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
        y++;
      }
      else{
        dir_x = true;
        dir_y = false;
        //Serial.println("T F Y-");
        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
        y--;
      }
      if(P<0){
        P = P + 2*abs(dx);
      }
      else{
        P = P + 2*abs(dx) - 2*abs(dy);
          if(dx>0){
            dir_x = false;
            dir_y = false;
            //Serial.println("F F X+");
            step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
            x++;
          }
          else{
            dir_x = true;
            dir_y = true;
            //Serial.println("T T X-");
            step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
            x--;
          }
      }
    }
  }
}

void setup () {   // The stepper motor used in the IO pin is set to output
    pinMode (X_DIR, OUTPUT); pinMode (X_STP, OUTPUT);
    pinMode (Y_DIR, OUTPUT); pinMode (Y_STP, OUTPUT);
    pinMode (EN, OUTPUT);
    Serial.begin(9600);
    digitalWrite (EN, LOW);
    myservo.attach(servo);
    myservo.write(150);

    // ----- plotter setup
    memset(BUFFER, '\0', sizeof(BUFFER));     //fill with string terminators
    INPUT_STRING.reserve(STRING_SIZE);
    INPUT_STRING = "";
    // ----- flush the buffers
    Serial.flush();                           //clear TX buffer
    while (Serial.available()) Serial.read(); //clear RX buffer
}

void loop () {
  if(Serial.available() > 0){
    //String info = Serial.readStringUntil('\n');
    c = Serial.read();
    if(c == '1'){
      x_plus();
      Serial.print("OK");
    }
    if(c == '2'){
      x_min();
      Serial.print("OK");
    }
    if(c == '3'){
      y_plus();
      Serial.print("OK");
    }
    if(c == '4'){
      y_min();
      Serial.print("OK");
    }
    if(c == '5'){
      pen_up();
      delay(1000);
      Serial.print("OK");
    }
    if(c == '6'){
      pen_down();
      delay(1000);
      Serial.print("OK");
    }
    if(c == '7'){
      pen_up();
      move_to(0,0);
      Serial.print("DONE");
    }
    //info = "";
    //Serial.print("Command : ");
    //Serial.println(info);
    delay(50);
    Serial.flush();
    while(Serial.available())
      {  
        d = Serial.read(); //untuk membaca terus
      }
    //myservo.write(150);
    //delay(500);
    //myservo.write(138);
    //delay(500);
  }
  }
  // ----- get the next instruction
  //  while (Serial.available()) {
  //    INPUT_CHAR = Serial.read();               //read character
  //    //Serial.println (INPUT_CHAR);
  //    Serial.write(INPUT_CHAR);                 //echo character to the screen
  //    BUFFER[INDEX++] = INPUT_CHAR;             //add char to buffer
  //    if (INPUT_CHAR == '\n') {                 //check for line feed
  //      Serial.flush();                         //clear TX buffer
  //      Serial.write(XOFF);                     //pause transmission
  //      INPUT_STRING = BUFFER;                  //convert to string
  //      process(INPUT_STRING);                  //interpret string and perform task
  //      memset(BUFFER, '\0', sizeof(BUFFER));   //fill buffer with string terminators
  //      INDEX = 0;                              //point to buffer start
  //      INPUT_STRING = "";                      //empty string
  //      Serial.flush();                         //clear TX buffer
  //      Serial.write(XON);                      //resume transmission
  //    }
  //  }
