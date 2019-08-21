int hitung=1;

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
#define steps 200*16/40 //step per mm

//servo
#include <Servo.h>
#define servo 11
Servo myservo; 

bool dir_x = false, dir_y = false;
int x,y;
int x1=0, y1=0;
/*
// Function : step . function: to control the direction of the stepper motor , the number of steps .
// Parameters : dir direction control , dirPin corresponding stepper motor DIR pin , stepperPin corresponding stepper motor " step " pin , Step number of step of no return value.
*/
void pen_up(){
  myservo.write(150);
}
void pen_down(){
  myservo.write(130);
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
  line_traj(x1, y1, x2, y2);
  //Serial.print("x_init : ");
  //Serial.print("move_to(");
  //Serial.println(");");
  //delay(1000);
  // ----- remember last "scaled" co-ordinate
  x = x2;
  y = y2;
  x1 = x2;
  y1 = y2;
}
void arc_cw_traj (float x2, float y2, float i, float j){
  x = x1;
  y = y1;
  
  float
  I = i,  //jarak x dari pusat lingkaran
  J = j,  //jarak y dari pusat lingkaran
  circleX = x + I,
  circleY = y + J,
  dx,
  dy,
  chord,
  radius,
  alpha,
  beta,
  arc,
  current_angle,
  next_angle,
  newX,
  newY;

  dx = x - x2;
  dy = y - y2;
  
  chord = sqrt(dx*dx + dy*dy);
  radius = sqrt(I*I + J*J); //jari - jari lingkaran
  alpha = 2*asin(chord/(2*radius)); //sudut dari pusat lingkaran
  arc = alpha*radius; //panjang bususr

  int segments = 1; //dibagi ke beberapa segmen biar lebih halus
  if(arc>arc_max){
    segments = (int)(arc/arc_max);
    beta = alpha/segments;
  }
  else{
    beta = alpha;
  }
  current_angle = atan2(-J,-I);
  if(current_angle<=0) current_angle += TWO_PI; //biar sudut ga negatif

  //----plot CW
  next_angle = current_angle;
  for (int segment = 1; segment < segments; segment++) {
      next_angle -= beta;                                   //move CW around circle
      if (next_angle < 0) next_angle += TWO_PI;             //check if angle crosses zero
        newX = circleX + radius * cos(next_angle);            //standard circle formula
        newY = circleY + radius * sin(next_angle);
//        Serial.print("newX ");
//        Serial.println(newX);
//        Serial.print("newY ");
//        Serial.println(newY);
        move_to(newX, newY);
  }
  // ----- draw sisa
//    float error_x=x2-newX;
//    float error_y=y2-newY;
//    float x_sisa=newX;
//    float y_sisa=newY;
//    float error_longest = max(abs(error_x),abs(error_y));
//    float bagi = (int)error_longest/error_max;
//    for(int seg = 1; seg<bagi; seg++){
//      x_sisa=x_sisa+(error_x)/bagi;
//      y_sisa=y_sisa+(error_y)/bagi;
//      move_to(x_sisa, y_sisa);
//    }
    //final line
    move_to(x2, y2);
}
void arc_ccw_traj (int x2, int y2, float i, float j){
  Serial.println("masuk");
  x = x1;
  y = y1;
  
  float
  I = i,  //jarak x dari pusat lingkaran
  J = j,  //jarak y dari pusat lingkaran
  circleX = x + I,
  circleY = y + J,
  dx,
  dy,
  chord,
  radius,
  alpha,
  beta,
  arc,
  current_angle,
  next_angle,
  newX,
  newY;

  dx = x - x2;
  dy = y - y2;
  
  chord = sqrt(dx*dx + dy*dy);
  radius = sqrt(I*I + J*J); //jari - jari lingkaran
  alpha = 2*asin(chord/(2*radius)); //sudut dari pusat lingkaran
  arc = alpha*radius; //panjang bususr

  int segments = 1; //dibagi ke beberapa segmen biar lebih halus
  if(arc>arc_max){
    segments = (int)(arc/arc_max);
    beta = alpha/segments;
  }
  else{
    beta = alpha;
  }
  current_angle = atan2(-J,-I);
  if(current_angle<=0) current_angle += TWO_PI; //biar sudut ga negatif

  //----plot CW
  next_angle = current_angle;
  for (int segment = 1; segment < segments; segment++) {
      next_angle += beta;                                   //move CW around circle
      if (next_angle > TWO_PI) next_angle -= TWO_PI;             //check if angle crosses zero
        newX = circleX + radius * cos(next_angle);            //standard circle formula
        newY = circleY + radius * sin(next_angle);
//        Serial.print("newX ");
//        Serial.println(newX);
//        Serial.print("newY ");
//        Serial.println(newY);
        move_to(newX, newY);
  }
  // ----- draw sisa
//    float error_x=x2-newX;
//    float error_y=y2-newY;
//    float x_sisa=newX;
//    float y_sisa=newY;
//    float error_longest = max(abs(error_x),abs(error_y));
//    float bagi = (int)error_longest/error_max;
//    for(int seg = 1; seg<bagi; seg++){
//      x_sisa=x_sisa+(error_x)/bagi;
//      y_sisa=y_sisa+(error_y)/bagi;
//      move_to(x_sisa, y_sisa);
//    }
    //final line
    move_to(x2, y2);
}
void line_traj(int x1, int y1, int x2, int y2) {          //these are "scaled" co-ordinates

  // ----- locals
  int
  x = x1,                               //current "scaled" X-axis position
  y = y1,                               //current "scaled" Y-axis position
  dy,                                   //line slope
  dx,
  slope,
  longest,                              //axis lengths
  shortest,
  maximum,
  error,                                //bresenham thresholds
  threshold;

  // ----- find longest and shortest axis
  dy = y2 - y1;                         //vertical distance
  dx = x2 - x1;                         //horizontal distance
  longest = max(abs(dy), abs(dx));      //longest axis
  shortest = min(abs(dy), abs(dx));     //shortest axis

  // ----- scale Bresenham values by 2*longest
  error = -longest;                     //add offset to so we can test at zero
  threshold = 0;                        //test now done at zero
  maximum = (longest << 1);             //multiply by two
  slope = (shortest << 1);              //multiply by two ... slope equals (shortest*2/longest*2)

  // ----- initialise the swap flag
  /*
     The XY axes are automatically swapped by using "longest" in
     the "for loop". XYswap is used to decode the motors.
  */
  bool XYswap = true;                    //used for motor decoding
  if (abs(dx) >= abs(dy)) XYswap = false;

  // ----- pretend we are always in octant 0

  for (int i = 0; i < longest; i++) {

    // ----- move left/right along X axis
    if (XYswap) {   //swap
      if (dy < 0) {
        y--;
        y_min();                         //move pen 1 step down
      } else {
        y++;
        y_plus();                           //move pen 1 step up
      }
    } else {    //no swap
      if (dx < 0) {
        x--;
        x_min();                         //move pen 1 step left
      } else {
        x++;
        x_plus();                        //move pen 1 step right
      }
    }

    // ----- move up/down Y axis
    error += slope;
    if (error > threshold) {
      error -= maximum;

      // ----- move up/down along Y axis
      if (XYswap) {  //swap
        if (dx < 0) {
          x--;
          x_min();                         //move pen 1 step left
        } else {
          x++;
          x_plus();                        //move pen 1 step right
        }
      } else {  //no swap
        if (dy < 0) {
          y--;
          y_min();                         //move pen 1 step down
        } else {
          y++;
          y_plus();                           //move pen 1 step up
        }
      }
    }
  }
}
//void line_traj (int x1,int y1,int x2,int y2){
//  
//  x = x1;
//  y = y1;
//  int dx = x2 - x1;
//  int dy = y2 - y1;
//  
//  int longest = max(abs(dy), abs(dx));      //longest axis
//  int shortest = min(abs(dy), abs(dx));     //shortest axis
//  //Serial.println(longest);
//  if(abs(dx)>abs(dy)){
//    float P = 2*dy - dx;
//    for(int i=0; i<longest; i++){
//      if(dx>0){
//        dir_x = false;
//        dir_y = false;
//        //Serial.println("F F X+");
//        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//        x++;
//      }
//      else{
//        dir_x = true;
//        dir_y = true;
//        //Serial.println("T T X-");
//        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//        x--;
//      }
//      if(P<0){
//        P = P + 2*dy;
//      }
//      else{
//        P = P + 2*dy - 2*dx;
//        if(dy>0){
//          dir_x = false;
//          dir_y = true;
//          //Serial.println("F T Y+");
//          step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//          y++;
//        }
//        else{
//          dir_x = true;
//          dir_y = false;
//          //Serial.println("T F Y-");
//          step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//          y--;
//        }
//      }
//    }
//  }
//  else{
//    float P = 2*dx - dy;
//    for(int i=0; i<longest; i++){
//      if(dy>0){
//        dir_x = false;
//        dir_y = true;
//        //Serial.println("F T Y+");
//        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//        y++;
//      }
//      else{
//        dir_x = true;
//        dir_y = false;
//        //Serial.println("T F Y-");
//        step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//        y--;
//      }
//      if(P<0){
//        P = P + 2*dx;
//      }
//      else{
//        P = P + 2*dx - 2*dy;
//          if(dx>0){
//            dir_x = false;
//            dir_y = false;
//            //Serial.println("F F X+");
//            step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//            x++;
//          }
//          else{
//            dir_x = true;
//            dir_y = true;
//            //Serial.println("T T X-");
//            step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
//            x--;
//          }
//      }
//    }
//  }
//}
void x_plus(){
  dir_x = false;
  dir_y = false;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
}
void x_min(){
  dir_x = true;
  dir_y = true;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
}
void y_plus(){
  dir_x = false;
  dir_y = true;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
}
void y_min(){
  dir_x = true;
  dir_y = false;
  step (dir_x, X_DIR, X_STP,dir_y, Y_DIR, Y_STP);
}
void step (boolean dir1, byte dirPin1, byte stepperPin1,boolean dir2, byte dirPin2, byte stepperPin2)
{
    
    digitalWrite (dirPin1, dir1);
    digitalWrite (dirPin2, dir2);
    delayMicroseconds(2);
    for (int i = 0; i <1; i++) 
    {
        digitalWrite (stepperPin1, HIGH);
        digitalWrite (stepperPin2, HIGH);
        delayMicroseconds (400);
        digitalWrite (stepperPin1, LOW);
        digitalWrite (stepperPin2, LOW);
        delayMicroseconds (600);
    }
}

void setup () {   // The stepper motor used in the IO pin is set to output
    pinMode (X_DIR, OUTPUT); pinMode (X_STP, OUTPUT);
    pinMode (Y_DIR, OUTPUT); pinMode (Y_STP, OUTPUT);
    pinMode (EN, OUTPUT);
    Serial.begin(9600);
    digitalWrite (EN, LOW);
    myservo.attach(servo);

    // ----- plotter setup
    memset(BUFFER, '\0', sizeof(BUFFER));     //fill with string terminators
    INPUT_STRING.reserve(STRING_SIZE);
    INPUT_STRING = "";
    // ----- flush the buffers
    Serial.flush();                           //clear TX buffer
    while (Serial.available()) Serial.read(); //clear RX buffer
    myservo.write(180);
}
void process(String string){
  // ----- convert string to upper case
  INPUT_STRING = string;
  INPUT_STRING.toUpperCase();

  // ----------------------------------
  // G00   linear move with pen_up
  // ----------------------------------
  if (INPUT_STRING.startsWith("G00")) {

    // ----- extract X
    START = INPUT_STRING.indexOf('X'); //Mencari index karakter X yaitu kalo sesuai format pada indeks ke-4 //kalo ga ada karakter X maka return -1
    if (!(START < 0)) {
      FINISH = START + 8;
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 1); //substring(start, lenght)
      X = SUB_STRING.toFloat();
    }

    // ----- extract Y
    START = INPUT_STRING.indexOf('Y');
    if (!(START < 0)) {
      FINISH = START + 8;
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 1);
      Y = SUB_STRING.toFloat();
    }

    pen_up();
    move_to(X, Y);
  }

  // ----------------------------------
  // G01   linear move with pen_down
  // ----------------------------------
  if (INPUT_STRING.startsWith("G01")) {

    // ----- extract X
    START = INPUT_STRING.indexOf('X');
    if (!(START < 0)) {
      FINISH = START + 8;
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 1);
      X = SUB_STRING.toFloat();
    }

    // ----- extract Y
    START = INPUT_STRING.indexOf('Y');
    if (!(START < 0)) {
      FINISH = START + 8;
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 1);
      Y = SUB_STRING.toFloat();
    }

    pen_down();
    move_to(X, Y);
  }

  // ----------------------------------
  // G02   clockwise arc with pen_down
  // ----------------------------------
  if (INPUT_STRING.startsWith("G02")) {

    // ----- extract X
    START = INPUT_STRING.indexOf('X');
    if (!(START < 0)) {
      FINISH = INPUT_STRING.indexOf('.', INPUT_STRING.indexOf('X'));
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 7);
      X = SUB_STRING.toFloat();
    }

    // ----- extract Y
    START = INPUT_STRING.indexOf('Y');
    if (!(START < 0)) {
      FINISH = INPUT_STRING.indexOf('.', INPUT_STRING.indexOf('Y'));
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 7);
      Y = SUB_STRING.toFloat();
    }

    // ----- extract I
    START = INPUT_STRING.indexOf('I');
    if (!(START < 0)) {
      FINISH = INPUT_STRING.indexOf('.', INPUT_STRING.indexOf('I'));
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 7);
      I = SUB_STRING.toFloat();
    }

    // ----- extract J
    START = INPUT_STRING.indexOf('J');
    if (!(START < 0)) {
      FINISH = INPUT_STRING.indexOf('.', INPUT_STRING.indexOf('J'));
      SUB_STRING = INPUT_STRING.substring(START + 1, FINISH + 7);
      J = SUB_STRING.toFloat();
    }

    pen_down();
    arc_cw_traj(X, Y, I, J);
  }
}

void loop () {
  delay(1000);
  myservo.write(138);
  
  // manual
  //step (false, X_DIR, X_STP,true, Y_DIR, Y_STP);
  //delay(2000);
  //step (true, X_DIR, X_STP,false, Y_DIR, Y_STP);
  //delay(2000);
  //step (true, X_DIR, X_STP,true, Y_DIR, Y_STP);
  //delay(1000);
  //step (false, X_DIR, X_STP,false, Y_DIR, Y_STP);

  //coba kotak
//    move_to(0,20);
  //  delay(1000);
//    move_to(20,20);
//    move_to(20,0);
//    move_to(0,0);
//    while(1){}

  //coba arc
//  while(1){
  //arc_cw_traj(10,10,10,0);
  //delay(1000);
  //arc_ccw_traj(0,0,0,-10);
//  arc_ccw_traj(-10,-10,0,-10);
//  delay(1000);
//  arc_cw_traj(0,0,10,0);
//  delay(1000);
//  }
  
  process(F("G00 X51.309849 Y6.933768"));
  process(F("G01 X7.893822 Y50.349788"));
  process(F("G00 X7.893823 Y50.349788"));
  process(F("G01 X51.309852 Y50.349788"));
  process(F("G01 X51.309852 Y6.933760"));
  process(F("G01 X7.893823 Y6.933760"));
  process(F("G01 X7.893823 Y50.349788"));
  process(F("G00 X43.948985 Y28.588440"));
  process(F("G02 X39.778044 Y18.518899 I-14.240483 J0.000001"));
  process(F("G02 X29.708503 Y14.347958 I-10.069542 J10.069542"));
  process(F("G02 X19.638962 Y18.518898 I-0.000000 J14.240483"));
  process(F("G02 X15.468020 Y28.588440 I10.069542 J10.069543"));
  process(F("G02 X16.552012 Y34.038037 I14.240483 J0.000001"));
  process(F("G02 X19.638961 Y38.657983 I13.156491 J-5.449596"));
  process(F("G02 X24.258906 Y41.744932 I10.069543 J-10.069542"));
  process(F("G02 X29.708503 Y42.828924 I5.449597 J-13.156491"));
  process(F("G02 X39.778045 Y38.657982 I-0.000001 J-14.240483"));
  process(F("G02 X43.948985 Y28.588440 I-10.069543 J-10.069541"));
  process(F("G01 X43.948985 Y28.588440"));
  process(F("G00 X51.309849 Y50.349788"));
  process(F("G01 X7.893822 Y6.933768"));
  process(F("G00 X0.0000 Y0.0000"));
  while(1){
    
  }

  // ----- get the next instruction
    while (Serial.available()) {
      INPUT_CHAR = Serial.read();               //read character
      Serial.println (INPUT_CHAR);
      Serial.write(INPUT_CHAR);                 //echo character to the screen
      BUFFER[INDEX++] = INPUT_CHAR;             //add char to buffer
      if (INPUT_CHAR == '\n') {                 //check for line feed
        Serial.flush();                         //clear TX buffer
        Serial.write(XOFF);                     //pause transmission
        INPUT_STRING = BUFFER;                  //convert to string
        process(INPUT_STRING);                  //interpret string and perform task
        memset(BUFFER, '\0', sizeof(BUFFER));   //fill buffer with string terminators
        INDEX = 0;                              //point to buffer start
        INPUT_STRING = "";                      //empty string
        Serial.flush();                         //clear TX buffer
        Serial.write(XON);                      //resume transmission
      }
    }
} 
