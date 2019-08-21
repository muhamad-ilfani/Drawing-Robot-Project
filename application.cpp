#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include "SerialPort.h"	// Library described above

#define SCREEN_HEIGHT 1080	
#define SCREEN_WIDTH 1920
#define ARC_MAX 0.2
#define TWO_PI 6.28318
#define error_max 2.0

GLfloat pointVertex[2000000];
GLfloat *point;
int count = 0;
int count_vertex = 0;
int send_count = 0;
float x_past = 0, y_past = 0;
float x_pos, y_pos;
bool dir_x = false;
bool dir_y = false;
int xa, ya, xb, yb,xc,yc;
int index = 0;
bool draw = false;
int START, FINISH;
int x_now, y_now = 0;
bool pen_state = false;
int count_check = 0;
int type = 0;
std::string outgoingData = "";


void line_traj(float x1, float y1, float x2, float y2);
void line_traj_curve(float x1, float y1, float x2, float y2);

void penUp() {
	pen_state = false;
	outgoingData += "5";
	send_count++;
}

void penDown() {
	pen_state = true;
	outgoingData += "6";
	send_count++;
}

void move_to(float x, float y) {                        //x,y are absolute co-ordinates
  // ----- apply scale factor
//  x2 = round(x * STEPS_PER_MM * SCALE_FACTOR);      //scale x and y
//  y2 = round(y * STEPS_PER_MM * SCALE_FACTOR);
	float x2 = x;
	float y2 = y;
	// ----- draw a line between these "scaled" co-ordinates
	if (type == 0) {
		line_traj(x_now, y_now, x2, y2);
	}
	else if (type == 1) {
		line_traj_curve(x_now, y_now, x2, y2);
	}
	//line_traj(x_now, y_now, x2, y2);
	//x_now = x2;
	//y_now = y2;
}

int getPt(int n1, int n2, float perc)
{
	int diff = n2 - n1;
	return n1 + (diff * perc);
}

void line_traj_curve(float x1, float y1, float x2, float y2) {
	x_pos = x1;
	y_pos = y1;
	//x_past = x1;
	//y_past = y1;
	float dx = x2 - x1;
	float dy = y2 - y1;

	float longest = std::fmax(abs(dy), abs(dx));      //longest axis
	float shortest = std::fmin(abs(dy), abs(dx));     //shortest axis
	float P = 2 * dx - dy;
	if (dx > dy) {
		for (int i = 0; i < longest; i++) {
			if (dx > 0) {
				dir_x = true;
				dir_y = true;
			}
			else {
				dir_x = false;
				dir_y = false;
			}
			//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
			x_pos++;
			if (P < 0) {
				P = P + 2 * dy;
			}
			else {
				P = P + 2 * dy - 2 * dx;
				if (dy > 0) {
					dir_x = true;
					dir_y = false;
				}
				else {
					dir_x = false;
					dir_y = true;
				}
				//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
				y_pos++;
			}
			/**
			if (pen_state) {
				std::cout << "A PEN DOWN : (" << x_pos << "," << y_pos << ")\n";
				*(point + count_vertex) = x_pos;
				*(point + count_vertex + 1) = y_pos;
				x_past = x_pos;
				y_past = y_pos;
				count_vertex += 2;
				count++;
			}
			else {
				std::cout << "A PEN UP :   (" << x_pos << "," << y_pos << ")\n";
				if (x_past != x_pos || y_past != y_pos) {
					std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "11";	//gerakan x-positif
						}
						else {
							outgoingData += "00";	//gereakan x-negatif
						}
						send_count += 2;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "10";	//gerakan y-positif
						}
						else {
							outgoingData += "01";	//gereakan y-negatif
						}
						send_count += 2;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				x_past = x_pos;
				y_past = y_pos;
			}
			**/
			if (pen_state) {
				std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
				if (x_past != x_pos || y_past != y_pos) {
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				std::cout << "A PEN DOWN : (" << x_pos << "," << y_pos << ")\n";
				*(point + count_vertex) = x_pos;
				*(point + count_vertex + 1) = y_pos;
				x_past = x_pos;
				y_past = y_pos;
				count_vertex += 2;
				count++;
			}
			else {
				std::cout << "A PEN UP :   (" << x_pos << "," << y_pos << ")\n";
				if (x_past != x_pos || y_past != y_pos) {
					std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				x_past = x_pos;
				y_past = y_pos;
			}
		}
	}
	else {
		for (int i = 0; i < longest; i++) {
			if (dy > 0) {
				dir_x = true;
				dir_y = false;
			}
			else {
				dir_x = false;
				dir_y = true;
			}
			//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
			y_pos++;
			if (P < 0) {
				P = P + 2 * dx;
			}
			else {
				P = P + 2 * dx - 2 * dy;
				if (dx > 0) {
					dir_x = true;
					dir_y = true;
				}
				else {
					dir_x = false;
					dir_y = false;
				}
				//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
				x_pos++;
			}
			if (pen_state) {
				std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
				if (x_past != x_pos || y_past != y_pos) {
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				std::cout << "B PEN DOWN : (" << x_pos << "," << y_pos << ")\n";
				*(point + count_vertex) = x_pos;
				*(point + count_vertex + 1) = y_pos;
				x_past = x_pos;
				y_past = y_pos;
				count_vertex += 2;
				count++;
			}
			else {
				std::cout << "B PEN UP :   (" << x_pos << "," << y_pos << ")\n";
				if (x_past != x_pos || y_past != y_pos) {
					std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				x_past = x_pos;
				y_past = y_pos;
			}
		}
	}
	x_now = x2;
	y_now = y2;
}

void line_traj(float x1, float y1, float x2, float y2) {
	x_pos = x1;
	y_pos = y1;
	//x_past = x1;
	//y_past = y1;
	float dx = x2 - x1;
	float dy = y2 - y1;

	float longest = std::fmax(abs(dy), abs(dx));      //longest axis
	float shortest = std::fmin(abs(dy), abs(dx));     //shortest axis
	if (std::fabs(dx) > std::fabs(dy)) {
		float P = 2 * abs(dx) - abs(dy);
		for (int i = 0; i < longest; i++) {
			if (dx > 0) {
				dir_x = true;
				dir_y = true;
				x_pos++;
			}
			else {
				dir_x = false;
				dir_y = false;
				x_pos--;
			}
			//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
			//x_pos++;
			if (P < 0) {
				P = P + 2 * std::fabs(dy);
			}
			else {
				P = P + 2 * std::fabs(dy) - 2 * std::fabs(dx);
				if (dy > 0) {
					dir_x = true;
					dir_y = false;
					y_pos++;
				}
				else {
					dir_x = false;
					dir_y = true;
					y_pos--;
				}
				//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
				//y_pos++;
			}
			/**
			if (pen_state) {
				std::cout << "A PEN DOWN : (" << x_pos << "," << y_pos << ")\n";
				*(point + count_vertex) = x_pos;
				*(point + count_vertex + 1) = y_pos;
				x_past = x_pos;
				y_past = y_pos;
				count_vertex += 2;
				count++;
			}
			else {
				std::cout << "A PEN UP :   (" << x_pos << "," << y_pos << ")\n";
				if (x_past != x_pos || y_past != y_pos) {
					std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "11";	//gerakan x-positif
						}
						else {
							outgoingData += "00";	//gereakan x-negatif
						}
						send_count += 2;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "10";	//gerakan y-positif
						}
						else {
							outgoingData += "01";	//gereakan y-negatif
						}
						send_count += 2;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				x_past = x_pos;
				y_past = y_pos;
			}
			**/
			if (pen_state) {
				std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
				if (x_past != x_pos || y_past != y_pos) {
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				std::cout << "A PEN DOWN : (" << x_pos << "," << y_pos << ")\n";
				*(point + count_vertex) = x_pos;
				*(point + count_vertex + 1) = y_pos;
				x_past = x_pos;
				y_past = y_pos;
				count_vertex += 2;
				count++;
			}
			else {
				std::cout << "A PEN UP :   (" << x_pos << "," << y_pos << ")\n";
				if (x_past != x_pos || y_past != y_pos) {
					std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				x_past = x_pos;
				y_past = y_pos;
			}
		}
	}
	else {
		float P = 2 * std::fabs(dy) - std::fabs(dx);
		for (int i = 0; i < longest; i++) {
			if (dy > 0) {
				dir_x = true;
				dir_y = false;
				y_pos++;
			}
			else {
				dir_x = false;
				dir_y = true;
				y_pos--;
			}
			//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
			//y_pos++;
			if (P < 0) {
				P = P + 2 * std::fabs(dx);
			}
			else {
				P = P + 2 * std::fabs(dx) - 2 * std::fabs(dy);
				if (dx > 0) {
					dir_x = true;
					dir_y = true;
					x_pos++;
				}
				else {
					dir_x = false;
					dir_y = false;
					x_pos--;
				}
				//step(dir_x, X_DIR, X_STP, dir_y, Y_DIR, Y_STP);
				x_pos++;
			}
			if (pen_state) {
				std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
				if (x_past != x_pos || y_past != y_pos) {
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				std::cout << "B PEN DOWN : (" << x_pos << "," << y_pos << ")\n";
				*(point + count_vertex) = x_pos;
				*(point + count_vertex + 1) = y_pos;
				x_past = x_pos;
				y_past = y_pos;
				count_vertex += 2;
				count++;
			}
			else {
				std::cout << "B PEN UP :   (" << x_pos << "," << y_pos << ")\n";
				if (x_past != x_pos || y_past != y_pos) {
					std::cout << "x_past : (" << x_past << "," << y_past << ")" << "x_pos (" << x_pos << ", " << y_pos << ") Count : " << count_check << "\n";
					if (x_past != x_pos) {
						if (x_pos > x_past) {
							outgoingData += "1";	//gerakan x-positif
						}
						else {
							outgoingData += "2";	//gereakan x-negatif
						}
						send_count++;
					}
					if (y_past != y_pos) {
						if (y_pos > y_past) {
							outgoingData += "3";	//gerakan y-positif
						}
						else {
							outgoingData += "4";	//gereakan y-negatif
						}
						send_count++;
					}
					count_check++;
					x_past = x_pos;
					y_past = y_pos;
				}
				x_past = x_pos;
				y_past = y_pos;
			}
		}
	}
	x_now = x2;
	y_now = y2;
}

void draw_arc_cw(float x, float y, float i, float j) {

	// ----- variables
	//x_now = LAST_X / (STEPS_PER_MM * SCALE_FACTOR); //current unscaled X co-ordinate
	//float y_now = LAST_Y / (STEPS_PER_MM * SCALE_FACTOR); //urrent unscaled Y co-ordinate
	float nextX = x;                    //next X co-ordinate
	float nextY = y;                    //next Y co-ordinate
	float newX;                         //interpolated X co-ordinate
	float newY;                         //interpolated Y co-ordinate
	float I = i;                        //horizontal distance thisX from circle center
	float J = j;                        //vertical distance thisY from circle center
	float circleX = x_now + I;          //circle X co-ordinate
	float circleY = y_now + J;          //circle Y co-ordinate
	float delta_x;                      //horizontal distance between thisX and nextX
	float delta_y;                      //vertical distance between thisY and nextY
	float chord;                       //line_length between lastXY and nextXY
	float radius;                       //circle radius
	float alpha;                       //interior angle of arc
	float beta;                        //fraction of alpha
	float arc;                         //subtended by alpha
	float current_angle;                //measured CCW from 3 o'clock
	float next_angle;                   //measured CCW from 3 o'clock

		// ----- calculate arc
	delta_x = x_now - nextX;
	delta_y = y_now - nextY;
	chord = sqrt(delta_x * delta_x + delta_y * delta_y);
	radius = sqrt(I * I + J * J);
	alpha = 2 * asin(chord / (2 * radius)); //see construction lines
	arc = alpha * radius;         //radians

	// ----- sub-divide alpha
	int segments = 1;
	if (arc > ARC_MAX) {
		segments = (int)(arc / ARC_MAX);
		beta = alpha / segments;
	}
	else {
		beta = alpha;
	}

	// ----- calculate current angle
	/*
		atan2() angles between 0 and PI are CCW +ve from 3 o'clock.
		atan2() angles between 2*PI and PI are CW -ve relative to 3 o'clock
	*/
	current_angle = atan2(-J, -I);

	if (current_angle <= 0) {
		current_angle += TWO_PI;        //angles now 360..0 degrees CW
	}
	// ----- plot intermediate CW co-ordinates
	next_angle = current_angle;                             //initialise angle
	for (int segment = 1; segment < segments; segment++) {
		next_angle -= beta;                                   //move CW around circle
		if (next_angle < 0) next_angle += TWO_PI;             //check if angle crosses zero
		newX = circleX + radius * cos(next_angle);            //standard circle formula
		newY = circleY + radius * sin(next_angle);
		move_to(newX, newY);
	}
	// ----- draw final line
	move_to(nextX, nextY);
}

void draw_arc_ccw(float x, float y, float i, float j) {
	// ----- variables
	float nextX = x;                    //next X co-ordinate
	float nextY = y;                    //next Y co-ordinate
	float newX;                         //interpolated X co-ordinate
	float newY;                         //interpolated Y co-ordinate
	float I = i;                        //horizontal distance thisX from circle center
	float J = j;                        //vertical distance thisY from circle center
	float circleX = x_now + I;          //circle X co-ordinate
	float circleY = y_now + J;          //circle Y co-ordinate
	float delta_x;                      //horizontal distance between thisX and nextX
	float delta_y;                      //vertical distance between thisY and nextY
	float chord;                       //line_length between lastXY and nextXY
	float radius;                       //circle radius
	float alpha;                       //interior angle of arc
	float beta;                        //fraction of alpha
	float arc;                         //subtended by alpha
	float current_angle;                //measured CCW from 3 o'clock
	float next_angle;                   //measured CCW from 3 o'clock

		// ----- calculate arc
	delta_x = x_now - nextX;
	delta_y = y_now - nextY;
	chord = sqrt(delta_x * delta_x + delta_y * delta_y);
	radius = sqrt(I * I + J * J);
	alpha = 2 * asin(chord / (2 * radius));     //see construction lines
	arc = alpha * radius;                       //radians

	// ----- sub-divide alpha
	int segments = 1;
	if (arc > ARC_MAX) {
		segments = (int)(arc / ARC_MAX);
		beta = alpha / segments;
	}
	else {
		beta = alpha;
	}

	// ----- calculate current angle
	/*
		tan2() angles between 0 and PI are CCW +ve from 3 o'clock.
		atan2() angles between 2*PI and PI are CW -ve relative to 3 o'clock
	*/
	current_angle = atan2(-J, -I);
	if (current_angle <= 0) current_angle += TWO_PI;        //angles now 360..0 degrees CW

	// ----- plot intermediate CCW co-ordinates
	next_angle = current_angle;                             //initialise angle
	for (int segment = 1; segment < segments; segment++) {
		next_angle += beta;                                   //move CCW around circle
		if (next_angle > TWO_PI) next_angle -= TWO_PI;        //check if angle crosses zero
		newX = circleX + radius * cos(next_angle);            //standard circle formula
		newY = circleY + radius * sin(next_angle);
		move_to(newX, newY);
	}

	// ----- draw final line
	move_to(nextX, nextY);
}

void process(std::string stringInput) {
	// ----- convert string to upper case
	std::string INPUT_STRING = stringInput;
	std::string SUB_STRING;
	float X, Y, I, J;

	for (auto & c : INPUT_STRING) c = toupper(c);
	//GOO - Linear Trajectory PEN UP
	if (INPUT_STRING.rfind("G00", 0) == 0) {
		START = INPUT_STRING.find('X');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			X = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('Y');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			Y = std::stod(SUB_STRING);
		}
		std::cout << "G00 - (" << X << "," << Y << ")\n";
		if (pen_state) {
			penUp();
		}
		type = 0;
		move_to(X, Y);
	}
	//G01 - Linear Trajectory Pen Down
	else if (INPUT_STRING.rfind("G01", 0) == 0) {
		START = INPUT_STRING.find('X');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			X = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('Y');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			Y = std::stod(SUB_STRING);
		}
		std::cout << "G01 - (" << X << "," << Y << ")\n";
		if (!pen_state) {
			penDown();
		}
		type = 0;
		move_to(X, Y);
	}
	//G02 - CW Arc PEN DOWN
	else if (INPUT_STRING.rfind("G02", 0) == 0) {
		START = INPUT_STRING.find('X');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			X = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('Y');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			Y = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('I');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			I = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('J');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			J = std::stod(SUB_STRING);
		}
		std::cout << "G02 - (" << X << "," << Y << "," << I << "," << J << ")\n";
		if (!pen_state) {
			penDown();
		}
		type = 1;
		draw_arc_cw(X, Y, I, J);

	}
	//G03 - CCW ARC PEN DOWN
	else if (INPUT_STRING.rfind("G03", 0) == 0) {
		START = INPUT_STRING.find('X');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			X = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('Y');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			Y = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('I');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			I = std::stod(SUB_STRING);
		}

		START = INPUT_STRING.find('J');
		if (!(START < 0)) {
			FINISH = START + 8;
			SUB_STRING = INPUT_STRING.substr(START + 1, FINISH - START + 1); //substring(start, lenght)
			J = std::stod(SUB_STRING);
		}
		std::cout << "G03 - (" << X << "," << Y << "," << I << "," << J << ")\n";
		if (!pen_state) {
			penDown();
		}
		type = 1;
		draw_arc_ccw(X, Y, I, J);
	}
}

int main(void) {
	GLFWwindow *window;
	point = pointVertex;

	printf("Program Menggambar Mekatron!\n");

	Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected\n");

	char incomingData[256] = "";		// don't forget to pre-allocate memory
	//printf("%s\n",incomingData);
	int dataLength = 20;
	int readResult = 0;
	/**
	if(SP->IsConnected())
	{
		// printf("Bytes read: (0 means no data available) %i\n",readResult);
		std::cin >> outgoingData;
		char *charArr = new char[outgoingData.size() + 1];
		copy(outgoingData.begin(), outgoingData.end(), charArr);
		charArr[outgoingData.size()] = '\n';
		SP->WriteData(charArr, dataLength);

		readResult = SP->ReadData(incomingData, dataLength);
		incomingData[readResult] = 0;

		printf("%s", incomingData);

		Sleep(500);
	}
	**/
	//x_now = 900;
	//y_now = 900;

	std::string strings[] = {
		"G00 X659.037110 Y906.906250",
		"G03 X650.562218 Y906.108982 Z-1.000000 I1.071146 J-56.828526 F400.000000",
		"G03 X639.714840 Y903.722660 Z-1.000000 I19.523120 J-114.592552",
		"G02 X627.298112 Y900.480163 Z-1.000000 I-188.404831 J696.076861",
		"G02 X618.158200 Y898.296880 Z-1.000000 I-95.103337 J377.909661",
		"G03 X602.868584 Y893.857934 Z-1.000000 I29.221377 J-129.202283",
		"G03 X589.433590 Y888.048830 Z-1.000000 I37.615870 J-105.436457",
		"G02 X584.452974 Y885.768548 Z-1.000000 I-28.281128 J55.192450",
		"G02 X579.044920 Y883.837890 Z-1.000000 I-25.525303 J62.960373",
		"G03 X573.665076 Y881.837275 Z-1.000000 I15.345851 J-49.500210",
		"G03 X564.214840 Y877.367190 Z-1.000000 I68.786826 J-157.647132",
		"G02 X557.857430 Y874.263827 Z-1.000000 I-88.880043 J174.012207",
		"G02 X555.609710 Y873.539637 Z-1.000000 I-4.021170 J8.630503",
		"G02 X553.508463 Y873.698061 Z-1.000000 I-0.755954 J3.987547",
		"G02 X550.986330 Y874.859380 Z-1.000000 I4.921112 J14.007017",
		"G03 X547.202942 Y876.107247 Z-1.000000 I-4.869122 J-8.403281",
		"G03 X545.343750 Y875.494140 Z-1.000000 I-0.262172 J-2.330457",
		"G02 X544.086843 Y874.993637 Z-1.000000 I-1.273335 J1.369231",
		"G02 X543.597660 Y875.478540 Z-1.000000 I-0.004261 J0.484903",
		"G03 X541.760030 Y878.214566 Z-1.000000 I-2.955634 J0.000000",
		"G03 X528.144530 Y881.792990 Z-1.000000 I-20.397014 J-49.916411",
		"G03 X521.402636 Y881.671248 Z-1.000000 I-2.951615 J-23.282932",
		"G03 X519.960940 Y880.453140 Z-1.000000 I0.296768 J-1.813457",
		"G03 X519.861213 Y879.073764 Z-1.000000 I2.566100 J-0.878819",
		"G03 X520.250000 Y878.914080 Z-1.000000 I0.246172 J0.046223",
		"G02 X521.207347 Y878.973868 Z-1.000000 I0.528173 J-0.762699",
		"G02 X522.888670 Y877.537130 Z-1.000000 I-2.281066 J-4.371526",
		"G02 X523.271402 Y875.473307 Z-1.000000 I-1.922374 J-1.423900",
		"G02 X521.326170 Y871.910180 Z-1.000000 I-8.618526 J2.392598",
		"G01 X518.193360 Y868.402360 Z-1.000000",
		"G01 X517.343750 Y874.042990 Z-1.000000",
		"G03 X516.070710 Y879.193856 Z-1.000000 I-28.351276 J-4.274287",
		"G03 X515.378910 Y879.685570 Z-1.000000 I-0.691800 J-0.240796",
		"G03 X514.306219 Y878.538458 Z-1.000000 I0.000000 J-1.075105",
		"G03 X516.666020 Y860.511740 Z-1.000000 I143.745779 J9.649332",
		"G01 X518.943360 Y848.869160 Z-1.000000",
		"G01 X515.818360 Y845.166040 Z-1.000000",
		"G02 X512.395960 Y841.830130 Z-1.000000 I-18.837572 J15.902453",
		"G02 X508.283200 Y839.128930 Z-1.000000 I-16.323190 J20.371582",
		"G03 X502.919648 Y835.096173 Z-1.000000 I9.976504 J-18.851844",
		"G03 X487.076485 Y817.716214 Z-1.000000 I292.150077 J-282.228264",
		"G03 X472.363269 Y799.617894 Z-1.000000 I320.085206 J-275.246380",
		"G03 X467.855470 Y792.115250 Z-1.000000 I30.528086 J-23.447655",
		"G03 X462.378201 Y777.491351 Z-1.000000 I103.506321 J-47.105180",
		"G03 X458.748050 Y761.558610 Z-1.000000 I119.378802 J-35.579453",
		"G03 X458.793429 Y757.160549 Z-1.000000 I12.934474 J-2.065808",
		"G03 X459.677730 Y756.421900 Z-1.000000 I0.884301 J0.160012",
		"G03 X460.877995 Y757.077577 Z-1.000000 I0.000015 J1.426397",
		"G03 X462.095700 Y760.187520 Z-1.000000 I-7.156861 J4.595655",
		"G02 X463.508386 Y766.229530 Z-1.000000 I56.530342 J-10.031240",
		"G02 X467.052210 Y777.546865 Z-1.000000 I288.795240 J-84.217624",
		"G02 X471.114826 Y788.859482 Z-1.000000 I263.793210 J-88.348285",
		"G02 X472.951170 Y792.599630 Z-1.000000 I21.635258 J-8.301638",
		"G02 X478.718793 Y800.766097 Z-1.000000 I62.408482 J-37.956475",
		"G02 X493.067438 Y817.633878 Z-1.000000 I399.923843 J-325.660035",
		"G02 X508.174795 Y833.908522 Z-1.000000 I379.486403 J-337.118802",
		"G02 X515.082030 Y840.025410 Z-1.000000 I43.665040 J-42.348591",
		"G03 X522.187569 Y845.457344 Z-1.000000 I-708.947139 J934.740640",
		"G03 X529.658200 Y851.257830 Z-1.000000 I-779.688313 J1011.896414",
		"G02 X543.949874 Y860.935793 Z-1.000000 I77.513123 J-99.074040",
		"G02 X564.376950 Y871.439470 Z-1.000000 I111.830024 J-192.367296",
		"G03 X569.875609 Y873.921140 Z-1.000000 I-1910.062694 J4239.480751",
		"G03 X583.933590 Y880.281270 Z-1.000000 I-12055.960918 J26666.291752",
		"G02 X596.136898 Y885.358589 Z-1.000000 I88.341276 J-195.123827",
		"G02 X611.085495 Y890.717193 Z-1.000000 I146.885976 J-386.229650",
		"G02 X626.745979 Y895.543385 Z-1.000000 I100.471387 J-298.197638",
		"G02 X630.939450 Y896.119160 Z-1.000000 I4.193471 J-14.982998",
		"G03 X633.915303 Y896.418300 Z-1.000000 I-0.000000 J14.951531",
		"G03 X644.437500 Y898.859400 Z-1.000000 I-38.964626 J191.852510",
		"G02 X662.081056 Y901.323756 Z-1.000000 I19.325165 J-73.966827",
		"G02 X674.101560 Y899.511740 Z-1.000000 I0.782887 J-35.583170",
		"G03 X682.168282 Y897.522011 Z-1.000000 I16.537539 J49.699338",
		"G03 X698.830080 Y895.459000 Z-1.000000 I34.600159 J211.130773",
		"G02 X706.748650 Y894.627113 Z-1.000000 I-16.104376 J-191.397976",
		"G02 X709.830080 Y894.072290 Z-1.000000 I-3.713848 J-29.460728",
		"G02 X724.174186 Y889.890750 Z-1.000000 I-32.413751 J-137.883560",
		"G02 X737.679690 Y884.334000 Z-1.000000 I-44.734912 J-127.917500",
		"G03 X741.798172 Y882.885998 Z-1.000000 I8.853333 J18.600104",
		"G03 X745.765620 Y882.423850 Z-1.000000 I3.967448 J16.798802",
		"G02 X753.279162 Y879.408501 Z-1.000000 I0.000000 J-10.868667",
		"G02 X756.250000 Y872.394550 Z-1.000000 I-6.653376 J-6.954254",
		"G02 X753.800894 Y865.260407 Z-1.000000 I-11.830112 J0.073749",
		"G02 X746.617190 Y859.857440 Z-1.000000 I-13.322671 J10.236469",
		"G03 X742.130204 Y857.411188 Z-1.000000 I7.482819 J-19.063410",
		"G03 X735.263213 Y851.935878 Z-1.000000 I52.533737 J-72.930296",
		"G03 X728.954071 Y845.675597 Z-1.000000 I49.373808 J-56.068495",
		"G03 X728.164060 Y843.607440 Z-1.000000 I2.312090 J-2.068157",
		"G03 X729.588536 Y837.703769 Z-1.000000 I12.945980 J-0.000000",
		"G03 X741.516103 Y816.439403 Z-1.000000 I288.113189 J147.630511",
		"G03 X754.752323 Y796.369637 Z-1.000000 I300.092754 J183.514726",
		"G03 X761.701170 Y788.765640 Z-1.000000 I34.179062 J24.257167",
		"G02 X765.556955 Y784.861189 Z-1.000000 I-21.125346 J-24.718122",
		"G02 X765.544920 Y784.152360 Z-1.000000 I-0.417003 J-0.347437",
		"G03 X765.504702 Y783.023240 Z-1.000000 I0.659706 J-0.588775",
		"G03 X767.465845 Y781.367949 Z-1.000000 I4.819669 J3.720812",
		"G03 X770.290347 Y780.243504 Z-1.000000 I6.511057 J12.245512",
		"G03 X773.828120 Y779.748070 Z-1.000000 I3.664949 J13.291595",
		"G02 X777.968289 Y778.499417 Z-1.000000 I-0.071368 J-7.724759",
		"G02 X783.898440 Y773.199240 Z-1.000000 I-14.247656 J-21.908708",
		"G02 X788.853779 Y766.549294 Z-1.000000 I-66.357891 J-54.619158",
		"G02 X789.265620 Y765.228540 Z-1.000000 I-2.008099 J-1.350758",
		"G03 X790.052826 Y763.199305 Z-1.000000 I3.079760 J0.027431",
		"G03 X794.399440 Y759.109121 Z-1.000000 I26.298256 J23.592343",
		"G03 X799.366411 Y755.769659 Z-1.000000 I23.702362 J29.890306",
		"G03 X802.097660 Y755.052750 Z-1.000000 I2.731249 J4.844242",
		"G03 X803.748562 Y756.564807 Z-1.000000 I0.000000 J1.657277",
		"G03 X802.548830 Y760.179710 Z-1.000000 I-4.779191 J0.420396",
		"G02 X801.967956 Y761.024323 Z-1.000000 I3.038947 J2.712059",
		"G02 X802.022610 Y761.238054 Z-1.000000 I0.147553 J0.076122",
		"G02 X802.317841 Y761.304136 Z-1.000000 I0.231899 J-0.343507",
		"G02 X803.744140 Y760.986350 Z-1.000000 I-1.697641 J-10.979102",
		"G02 X805.752397 Y759.570741 Z-1.000000 I-1.069306 J-3.649287",
		"G02 X809.115230 Y753.585960 Z-1.000000 I-24.022212 J-17.435193",
		"G03 X811.645370 Y748.752970 Z-1.000000 I29.219596 J12.218100",
		"G03 X813.067675 Y747.220624 Z-1.000000 I4.929715 J3.149447",
		"G03 X814.127867 Y747.286874 Z-1.000000 I0.491194 J0.655661",
		"G03 X814.947270 Y749.216820 Z-1.000000 I-1.863107 J1.929946",
		"G02 X815.176841 Y750.249063 Z-1.000000 I2.435476 J-0.000000",
		"G02 X815.554778 Y750.393244 Z-1.000000 I0.263512 J-0.123309",
		"G02 X816.138381 Y749.991527 Z-1.000000 I-0.706022 J-1.650469",
		"G02 X818.003910 Y747.908220 Z-1.000000 I-25.767366 J-24.950695",
		"G02 X820.867472 Y744.146201 Z-1.000000 I-44.794491 J-37.067379",
		"G02 X821.058590 Y743.539080 Z-1.000000 I-0.868755 J-0.607121",
		"G03 X821.517657 Y741.841560 Z-1.000000 I3.368048 J-0.000000",
		"G03 X825.114388 Y736.213921 Z-1.000000 I71.313028 J41.614324",
		"G03 X829.179244 Y730.889859 Z-1.000000 I63.997740 J44.647729",
		"G03 X830.224610 Y730.400410 Z-1.000000 I1.045366 J0.871622",
		"G02 X831.325852 Y729.783036 Z-1.000000 I0.000000 J-1.290859",
		"G02 X834.812045 Y723.354660 Z-1.000000 I-60.061040 J-36.731383",
		"G02 X837.646399 Y716.657416 Z-1.000000 I-73.600774 J-35.097119",
		"G02 X838.171880 Y713.734400 Z-1.000000 I-7.866976 J-2.923016",
		"G03 X838.503511 Y711.557609 Z-1.000000 I7.309926 J-0.000000",
		"G03 X840.960373 Y704.345430 Z-1.000000 I140.144707 J43.716368",
		"G03 X843.846544 Y697.169269 Z-1.000000 I129.036068 J47.728392",
		"G03 X844.587890 Y696.388690 Z-1.000000 I1.442805 J0.627956",
		"G02 X845.115577 Y695.690978 Z-1.000000 I-0.438143 J-0.879775",
		"G02 X845.505860 Y691.513690 Z-1.000000 I-22.160079 J-4.177288",
		"G03 X846.036132 Y687.258545 Z-1.000000 I17.337738 J-0.000000",
		"G03 X847.298830 Y684.437520 Z-1.000000 I8.707937 J2.204588",
		"G02 X848.719885 Y681.067471 Z-1.000000 I-7.836869 J-5.289222",
		"G02 X849.167613 Y676.565165 Z-1.000000 I-21.078108 J-4.369506",
		"G02 X848.500170 Y672.438487 Z-1.000000 I-13.610119 J0.083966",
		"G02 X847.218730 Y671.507830 Z-1.000000 I-1.281440 J0.416891",
		"G02 X844.522976 Y672.138970 Z-1.000000 I0.000000 J6.072683",
		"G02 X826.681393 Y681.397445 Z-1.000000 I238.454898 J481.335611",
		"G02 X809.549600 Y691.084816 Z-1.000000 I272.182321 J501.337553",
		"G02 X804.685530 Y694.599630 Z-1.000000 I16.238485 J27.595108",
		"G03 X803.208379 Y695.545505 Z-1.000000 I-3.866652 J-4.412107",
		"G03 X800.087870 Y696.650410 Z-1.000000 I-8.521583 J-19.107962",
		"G02 X796.870666 Y697.607331 Z-1.000000 I15.465577 J57.882503",
		"G02 X794.892560 Y698.355490 Z-1.000000 I7.417295 J22.600161",
		"G03 X793.859215 Y697.803131 Z-1.000000 I-0.296150 J-0.688729",
		"G03 X792.929670 Y687.746110 Z-1.000000 I54.792427 J-10.135792",
		"G02 X791.845255 Y671.198435 Z-1.000000 I-129.684317 J0.189206",
		"G02 X790.638650 Y670.138690 Z-1.000000 I-1.206605 J0.157036",
		"G02 X790.060330 Y670.484029 Z-1.000000 I0.000000 J0.656910",
		"G02 X785.222640 Y680.191430 Z-1.000000 I149.978165 J80.800856",
		"G03 X780.107722 Y689.950434 Z-1.000000 I-95.823893 J-44.003579",
		"G03 X773.906230 Y699.287130 Z-1.000000 I-94.078960 J-55.759950",
		"G02 X767.848631 Y708.189358 Z-1.000000 I107.966037 J79.978524",
		"G02 X753.043003 Y732.481224 Z-1.000000 I1202.047546 J749.292792",
		"G02 X738.669292 Y757.179792 Z-1.000000 I1194.965584 J711.960303",
		"G02 X734.357400 Y765.666040 Z-1.000000 I88.692768 J50.403676",
		"G03 X732.373858 Y768.111916 Z-1.000000 I-5.626765 J-2.535924",
		"G03 X730.812328 Y768.100749 Z-1.000000 I-0.773244 J-1.057345",
		"G03 X729.254243 Y766.315531 Z-1.000000 I3.460790 J-4.593006",
		"G03 X725.716780 Y759.154320 Z-1.000000 I68.035906 J-38.062394",
		"G02 X721.037183 Y750.793670 Z-1.000000 I-42.900291 J18.522068",
		"G02 X719.488260 Y750.300800 Z-1.000000 I-1.098799 J0.772846",
		"G03 X717.541580 Y749.934997 Z-1.000000 I-0.659327 J-1.853971",
		"G03 X712.938068 Y744.811125 Z-1.000000 I17.977103 J-20.781357",
		"G03 X709.169311 Y738.599677 Z-1.000000 I49.031240 J-33.998451",
		"G03 X704.634750 Y728.878930 Z-1.000000 I98.986320 J-52.093433",
		"G02 X700.041570 Y718.716615 Z-1.000000 I-143.356575 J58.675365",
		"G02 X698.988260 Y717.498070 Z-1.000000 I-2.890184 J1.433761",
		"G03 X698.054996 Y716.308325 Z-1.000000 I1.457026 J-2.103836",
		"G03 X697.607400 Y713.884790 Z-1.000000 I6.337392 J-2.423535",
		"G02 X696.976089 Y711.856371 Z-1.000000 I-3.574340 J-0.000000",
		"G02 X695.773420 Y711.224630 Z-1.000000 I-1.202669 J0.828914",
		"G02 X694.370488 Y711.655533 Z-1.000000 I0.000000 J2.499280",
		"G02 X693.939430 Y712.468770 Z-1.000000 I0.551600 J0.813237",
		"G03 X692.960224 Y715.392343 Z-1.000000 I-4.853998 J-0.000000",
		"G03 X673.716780 Y739.269550 Z-1.000000 I-382.965407 J-288.951634",
		"G02 X664.923447 Y750.289331 Z-1.000000 I145.165727 J124.854541",
		"G02 X663.079848 Y754.386820 Z-1.000000 I8.510966 J6.292866",
		"G02 X663.874130 Y758.166373 Z-1.000000 I4.674004 J0.990983",
		"G02 X668.632790 Y763.250020 Z-1.000000 I21.945128 J-15.773174",
		"G02 X672.705908 Y765.948355 Z-1.000000 I14.945796 J-18.137233",
		"G02 X674.145305 Y765.992325 Z-1.000000 I0.765161 J-1.466297",
		"G02 X675.047441 Y764.891172 Z-1.000000 I-0.647652 J-1.450720",
		"G02 X675.587870 Y760.144550 Z-1.000000 I-21.522215 J-4.854497",
		"G03 X677.722917 Y755.770701 Z-1.000000 I5.604190 J0.027600",
		"G03 X680.439430 Y756.320330 Z-1.000000 I1.125054 J1.427404",
		"G03 X681.093245 Y758.077374 Z-1.000000 I-5.573867 J3.074264",
		"G03 X682.871070 Y767.818380 Z-1.000000 I-173.535935 J36.704673",
		"G02 X684.612047 Y777.592156 Z-1.000000 I207.433710 J-31.907685",
		"G02 X685.509750 Y780.834000 Z-1.000000 I23.497717 J-4.761564",
		"G03 X685.701657 Y785.257036 Z-1.000000 I-7.131185 J2.525090",
		"G03 X684.027320 Y786.552750 Z-1.000000 I-1.674337 J-0.433943",
		"G03 X682.968973 Y785.934922 Z-1.000000 I-0.000000 J-1.215394",
		"G03 X680.993407 Y781.536745 Z-1.000000 I22.892253 J-12.925483",
		"G03 X679.700277 Y776.827154 Z-1.000000 I37.059412 J-12.707868",
		"G03 X679.273420 Y772.689470 Z-1.000000 I19.840611 J-4.137684",
		"G02 X679.182138 Y769.706025 Z-1.000000 I-48.800972 J-0.000000",
		"G02 X678.949453 Y769.247796 Z-1.000000 I-0.645385 J0.039530",
		"G02 X678.535354 Y769.245439 Z-1.000000 I-0.208487 J0.251458",
		"G02 X676.904280 Y770.724630 Z-1.000000 I12.438062 J15.354074",
		"G03 X674.477456 Y772.493459 Z-1.000000 I-6.605903 J-6.514053",
		"G03 X673.529280 Y772.427750 Z-1.000000 I-0.418837 J-0.830129",
		"G02 X672.545897 Y772.305643 Z-1.000000 I-0.608336 J0.878340",
		"G02 X670.109360 Y773.709000 Z-1.000000 I3.024630 J8.068286",
		"G02 X668.682488 Y775.075237 Z-1.000000 I8.108300 J9.896370",
		"G02 X668.328727 Y775.931704 Z-1.000000 I1.041000 J0.931275",
		"G02 X668.515970 Y776.903460 Z-1.000000 I2.045786 J0.109726",
		"G02 X669.804670 Y779.408220 Z-1.000000 I31.253156 J-14.495862",
		"G03 X672.725835 Y785.108784 Z-1.000000 I-65.508601 J37.167599",
		"G03 X675.990182 Y792.688448 Z-1.000000 I-154.006906 J70.819190",
		"G03 X678.903928 Y800.658006 Z-1.000000 I-123.780734 J49.772828",
		"G03 X679.273420 Y802.957050 Z-1.000000 I-6.967785 J2.299044",
		"G03 X678.663220 Y805.295893 Z-1.000000 I-4.787388 J0.000000",
		"G03 X677.939247 Y805.605975 Z-1.000000 I-0.563534 J-0.315527",
		"G03 X677.021713 Y804.987861 Z-1.000000 I0.409968 J-1.598616",
		"G03 X675.390600 Y801.960960 Z-1.000000 I9.777663 J-7.221843",
		"G02 X669.780326 Y788.117659 Z-1.000000 I-214.436352 J78.846122",
		"G02 X666.187480 Y781.226580 Z-1.000000 I-56.531499 J25.092039",
		"G03 X663.855864 Y773.290066 Z-1.000000 I13.889650 J-8.391300",
		"G03 X665.886700 Y770.498070 Z-1.000000 I2.825722 J-0.079223",
		"G02 X667.686708 Y769.099920 Z-1.000000 I-0.874142 J-2.983148",
		"G02 X667.897042 Y767.397823 Z-1.000000 I-1.841773 J-1.091639",
		"G02 X666.842258 Y765.592899 Z-1.000000 I-4.219607 J1.255243",
		"G02 X663.736310 Y763.269550 Z-1.000000 I-8.752821 J8.463375",
		"G03 X660.356363 Y760.728990 Z-1.000000 I6.068178 J-11.591680",
		"G03 X658.488260 Y757.791040 Z-1.000000 I6.403961 J-6.134870",
		"G02 X656.687548 Y754.719842 Z-1.000000 I-10.328060 J3.992074",
		"G02 X652.816390 Y750.943380 Z-1.000000 I-19.679840 J16.300973",
		"G03 X648.560622 Y747.417710 Z-1.000000 I68.804351 J-87.383769",
		"G03 X646.189430 Y745.169940 Z-1.000000 I25.564096 J-29.342363",
		"G02 X643.784429 Y742.966033 Z-1.000000 I-19.907593 J19.309877",
		"G02 X639.251930 Y739.525410 Z-1.000000 I-53.370663 J65.601995",
		"G03 X634.813004 Y735.975623 Z-1.000000 I27.867858 J-39.398388",
		"G03 X627.904280 Y729.300800 Z-1.000000 I92.679448 J-102.839969",
		"G02 X621.125407 Y722.461982 Z-1.000000 I-264.471574 J255.374265",
		"G02 X612.046860 Y713.716820 Z-1.000000 I-441.434931 J449.178473",
		"G03 X596.801074 Y697.264240 Z-1.000000 I115.415465 J-122.239799",
		"G03 X589.246070 Y685.552750 Z-1.000000 I50.399759 J-40.805138",
		"G01 X584.951150 Y676.998070 Z-1.000000",
		"G01 X573.824200 Y688.291040 Z-1.000000",
		"G02 X562.750573 Y699.636408 Z-1.000000 I1185.676214 J1168.354113",
		"G02 X557.939430 Y704.720720 Z-1.000000 I234.815472 J227.017832",
		"G03 X552.998174 Y709.434142 Z-1.000000 I-40.500326 J-37.511241",
		"G03 X551.843730 Y709.855490 Z-1.000000 I-1.154444 J-1.370846",
		"G03 X551.413617 Y709.008400 Z-1.000000 I0.000000 J-0.532741",
		"G03 X560.710920 Y697.720720 Z-1.000000 I102.060073 J74.590852",
		"G02 X564.972836 Y693.085060 Z-1.000000 I-667.413671 J-617.881278",
		"G02 X576.181620 Y680.777360 Z-1.000000 I-4904.304306 J-4477.672248",
		"G03 X587.582208 Y669.312562 Z-1.000000 I132.580504 J120.437206",
		"G03 X588.953100 Y669.044940 Z-1.000000 I0.903568 J0.983517",
		"G03 X589.908914 Y670.078607 Z-1.000000 I-0.546990 J1.464538",
		"G03 X590.666000 Y675.021500 Z-1.000000 I-18.940658 J5.430502",
		"G02 X594.163096 Y685.763703 Z-1.000000 I19.751065 J-0.489571",
		"G02 X615.537090 Y711.013690 Z-1.000000 I139.386261 J-96.318321",
		"G03 X630.960919 Y726.048405 Z-1.000000 I-3620.789647 J3729.928431",
		"G03 X637.300760 Y732.316430 Z-1.000000 I-622.782498 J636.258239",
		"G02 X643.731873 Y738.321806 Z-1.000000 I92.687357 J-92.812008",
		"G02 X644.519510 Y738.617210 Z-1.000000 I0.787637 J-0.902337",
		"G03 X645.338274 Y738.867398 Z-1.000000 I-0.000000 J1.464837",
		"G03 X649.210920 Y741.779320 Z-1.000000 I-26.246409 J38.936956",
		"G02 X653.071102 Y744.837712 Z-1.000000 I60.053285 J-71.831628",
		"G02 X657.513650 Y748.009790 Z-1.000000 I69.286890 J-92.340453",
		"G01 X662.035140 Y751.080100 Z-1.000000",
		"G01 X663.818340 Y747.216820 Z-1.000000",
		"G03 X665.976701 Y743.594574 Z-1.000000 I18.266548 J8.430186",
		"G03 X673.861310 Y733.794940 Z-1.000000 I143.206354 J107.149533",
		"G02 X681.807601 Y723.999242 Z-1.000000 I-153.206789 J-132.402548",
		"G02 X688.689430 Y714.306660 Z-1.000000 I-143.491678 J-109.169882",
		"G03 X695.473604 Y704.777015 Z-1.000000 I142.545898 J94.299064",
		"G03 X696.283180 Y704.376970 Z-1.000000 I0.809576 J0.619152",
		"G03 X697.745762 Y705.218821 Z-1.000000 I-0.000000 J1.691427",
		"G03 X706.259750 Y721.550800 Z-1.000000 I-174.346794 J101.273537",
		"G02 X718.353213 Y744.503973 Z-1.000000 I235.140928 J-109.227517",
		"G02 X722.283180 Y748.591820 Z-1.000000 I10.665348 J-6.320417",
		"G03 X725.049947 Y751.164659 Z-1.000000 I-5.904017 J9.123109",
		"G03 X728.113260 Y756.308610 Z-1.000000 I-21.785992 J16.458043",
		"G02 X730.203420 Y760.105713 Z-1.000000 I25.992206 J-11.833891",
		"G02 X731.458910 Y761.548899 Z-1.000000 I5.869435 J-3.838379",
		"G02 X732.413958 Y761.666785 Z-1.000000 I0.564571 J-0.646262",
		"G02 X733.052710 Y760.623070 Z-1.000000 I-0.533334 J-1.043715",
		"G03 X733.692621 Y757.987006 Z-1.000000 I5.749485 J0.000000",
		"G03 X745.557427 Y735.914595 Z-1.000000 I657.559396 J339.239440",
		"G03 X757.467249 Y715.253624 Z-1.000000 I815.846643 J456.523975",
		"G03 X765.867170 Y701.980490 Z-1.000000 I236.555515 J140.409952",
		"G02 X773.438318 Y690.350871 Z-1.000000 I-366.697306 J-247.007636",
		"G02 X780.146460 Y679.275410 Z-1.000000 I-325.901455 J-204.959935",
		"G03 X786.399905 Y668.881128 Z-1.000000 I378.144838 J220.422575",
		"G03 X788.658635 Y665.709953 Z-1.000000 I26.088125 J16.191768",
		"G03 X790.968056 Y664.208541 Z-1.000000 I3.482413 J2.829688",
		"G03 X792.113260 Y664.548850 Z-1.000000 I0.297139 J1.097141",
		"G03 X794.942066 Y670.346333 Z-1.000000 I-8.871972 J7.917844",
		"G03 X795.820290 Y685.013690 Z-1.000000 I-60.656131 J10.991822",
		"G02 X795.799844 Y692.987680 Z-1.000000 I69.760624 J4.165894",
		"G02 X796.304670 Y693.890640 Z-1.000000 I1.174610 J-0.064101",
		"G02 X797.263925 Y693.924632 Z-1.000000 I0.506130 J-0.730894",
		"G02 X804.583960 Y688.986350 Z-1.000000 I-38.374408 J-64.776934",
		"G03 X811.773721 Y683.840012 Z-1.000000 I86.784805 J113.648433",
		"G03 X819.224512 Y679.102080 Z-1.000000 I102.723099 J153.313151",
		"G03 X827.203425 Y674.684070 Z-1.000000 I64.015815 J106.198424",
		"G03 X829.037090 Y674.246110 Z-1.000000 I1.833665 J3.619641",
		"G02 X830.642208 Y673.907866 Z-1.000000 I0.000000 J-3.977615",
		"G02 X838.658180 Y670.001970 Z-1.000000 I-47.324097 J-107.300504",
		"G02 X845.526433 Y665.044969 Z-1.000000 I-14.990498 J-28.007038",
		"G02 X846.962870 Y662.126970 Z-1.000000 I-3.223860 J-3.399558",
		"G03 X848.197670 Y659.449095 Z-1.000000 I4.537440 J0.468640",
		"G03 X850.421860 Y658.496110 Z-1.000000 I2.224190 J2.119046",
		"G02 X853.077021 Y657.601433 Z-1.000000 I0.000000 J-4.387239",
		"G02 X858.367170 Y652.455100 Z-1.000000 I-18.411299 J-24.217970",
		"G03 X866.796296 Y642.188152 Z-1.000000 I526.587901 J423.733066",
		"G03 X881.667950 Y624.636740 Z-1.000000 I1548.119876 J1296.676000",
		"G03 X889.885504 Y615.321741 Z-1.000000 I330.907625 J283.639611",
		"G03 X893.521427 Y611.841279 Z-1.000000 I26.600768 J24.149543",
		"G03 X897.517821 Y609.253766 Z-1.000000 I12.831325 J15.437926",
		"G03 X903.033180 Y606.878930 Z-1.000000 I26.905367 J54.893580",
		"G01 X911.705060 Y603.638690 Z-1.000000",
		"G03 X910.722429 Y607.052034 Z-1.000000 I-6.170994 J0.071610",
		"G03 X907.193340 Y610.304710 Z-1.000000 I-8.030965 J-5.172604",
		"G02 X903.301050 Y612.970515 Z-1.000000 I10.147240 J18.990222",
		"G02 X902.195290 Y614.603540 Z-1.000000 I2.452272 J2.851372",
		"G03 X901.135765 Y616.307000 Z-1.000000 I-4.226622 J-1.447658",
		"G03 X897.648420 Y619.173850 Z-1.000000 I-16.048596 J-15.967624",
		"G02 X894.108185 Y621.965818 Z-1.000000 I15.730568 J23.586977",
		"G02 X877.994690 Y637.198353 Z-1.000000 I759.126044 J819.168406",
		"G02 X862.383748 Y652.539837 Z-1.000000 I802.653476 J832.364565",
		"G02 X857.923810 Y657.480490 Z-1.000000 I46.808174 J46.737164",
		"G02 X856.054913 Y661.272137 Z-1.000000 I7.309291 J5.959151",
		"G02 X855.771460 Y666.589860 Z-1.000000 I14.582151 J3.443694",
		"G03 X856.362079 Y672.203102 Z-1.000000 I-136.133845 J17.161548",
		"G03 X857.830060 Y690.962910 Z-1.000000 I-1519.039297 J128.304256",
		"G03 X858.271431 Y709.667981 Z-1.000000 I-193.135981 J13.915048",
		"G03 X857.136700 Y719.039080 Z-1.000000 I-49.346796 J-1.221074",
		"G03 X852.801795 Y736.276210 Z-1.000000 I-288.324374 J-63.346027",
		"G03 X850.515600 Y742.367210 Z-1.000000 I-37.937635 J-10.764957",
		"G02 X849.429468 Y744.804562 Z-1.000000 I53.889661 J25.474965",
		"G02 X845.185530 Y755.087910 Z-1.000000 I947.611999 J397.096930",
		"G03 X834.330618 Y778.417076 Z-1.000000 I-240.791645 J-97.848876",
		"G03 X820.929670 Y801.113300 Z-1.000000 I-241.233270 J-127.131386",
		"G02 X817.417900 Y808.470413 Z-1.000000 I25.053927 J16.475682",
		"G02 X816.339820 Y815.494160 Z-1.000000 I21.690441 J6.923891",
		"G02 X816.974308 Y822.732340 Z-1.000000 I43.763801 J-0.189369",
		"G02 X818.576150 Y828.486350 Z-1.000000 I29.197829 J-5.028328",
		"G03 X820.262818 Y834.247397 Z-1.000000 I-31.918247 J12.472165",
		"G03 X821.654280 Y843.894550 Z-1.000000 I-88.765898 J17.727123",
		"G02 X823.119493 Y853.027309 Z-1.000000 I66.145619 J-5.928149",
		"G02 X823.933570 Y853.683610 Z-1.000000 I0.826799 J-0.192521",
		"G03 X824.343868 Y854.044936 Z-1.000000 I-0.006278 J0.420746",
		"G03 X824.080060 Y854.710960 Z-1.000000 I-0.715120 J0.102004",
		"G02 X823.413136 Y856.000493 Z-1.000000 I1.115340 J1.394062",
		"G02 X822.857400 Y875.576190 Z-1.000000 I323.715559 J18.985725",
		"G01 X822.892600 Y895.435570 Z-1.000000",
		"G01 X819.503930 Y895.871110 Z-1.000000",
		"G03 X816.168280 Y895.798489 Z-1.000000 I-1.427288 J-11.084679",
		"G03 X810.335960 Y894.281270 Z-1.000000 I6.213122 J-35.852358",
		"G03 X783.717092 Y883.830754 Z-1.000000 I129.018149 J-367.752742",
		"G03 X766.923850 Y875.373070 Z-1.000000 I67.382849 J-154.693546",
		"G02 X763.852739 Y875.294601 Z-1.000000 I-1.607933 J2.793449",
		"G02 X762.388690 Y877.683610 Z-1.000000 I1.217147 J2.389009",
		"G03 X761.715834 Y879.235844 Z-1.000000 I-2.126877 J-0.000000",
		"G03 X757.193380 Y882.550800 Z-1.000000 I-15.983120 J-17.062716",
		"G02 X752.201834 Y885.736530 Z-1.000000 I26.055229 J46.327865",
		"G02 X749.339860 Y888.201190 Z-1.000000 I12.671818 J17.608569",
		"G03 X746.603671 Y890.140654 Z-1.000000 I-6.652594 J-6.485621",
		"G03 X745.392600 Y890.037130 Z-1.000000 I-0.515217 J-1.108350",
		"G02 X744.016301 Y889.799857 Z-1.000000 I-0.943467 J1.362328",
		"G02 X727.884790 Y894.773460 Z-1.000000 I63.346576 J234.107369",
		"G03 X711.485173 Y899.513089 Z-1.000000 I-53.266496 J-153.565690",
		"G03 X700.664080 Y901.113300 Z-1.000000 I-15.147064 J-65.041056",
		"G02 X689.730234 Y902.285565 Z-1.000000 I8.486197 J130.728399",
		"G02 X678.376970 Y904.453140 Z-1.000000 I21.497732 J143.416935",
		"G03 X667.533758 Y906.426823 Z-1.000000 I-25.767110 J-110.789275",
		"G03 X659.037130 Y906.906270 Z-1.000000 I-7.429493 J-56.136247",
		"G01 X659.037110 Y906.906250 Z-1.000000",
		"G00 X531.722690 Y875.181640",
		"G02 X532.814898 Y875.099818 Z-1.000000 I-0.252235 J-10.697627 F400.000000",
		"G02 X534.410190 Y874.861330 Z-1.000000 I-4.519799 J-35.688626",
		"G02 X546.157075 Y871.711246 Z-1.000000 I-11.353031 J-65.813781",
		"G02 X547.265650 Y870.121090 Z-1.000000 I-0.586184 J-1.590156",
		"G02 X546.808477 Y869.263339 Z-1.000000 I-1.033246 J0.000000",
		"G02 X539.568390 Y864.951170 Z-1.000000 I-43.331307 J64.518718",
		"G03 X532.183033 Y860.485830 Z-1.000000 I32.943720 J-62.826690",
		"G03 X527.488310 Y856.603520 Z-1.000000 I20.094004 J-29.078603",
		"G02 X523.017643 Y852.511461 Z-1.000000 I-58.988827 J59.958443",
		"G02 X522.423860 Y852.292970 Z-1.000000 I-0.593783 J0.697604",
		"G02 X521.031896 Y853.582467 Z-1.000000 I0.000000 J1.396035",
		"G02 X521.421943 Y860.059206 Z-1.000000 I23.764995 J1.818925",
		"G02 X523.412574 Y866.908970 Z-1.000000 I40.659221 J-8.101969",
		"G02 X526.326200 Y871.876950 Z-1.000000 I16.707155 J-6.460047",
		"G02 X529.247868 Y874.479457 Z-1.000000 I10.000223 J-8.285372",
		"G02 X531.722690 Y875.181640 Z-1.000000 I2.372382 J-3.649079",
		"G01 X531.722690 Y875.181640 Z-1.000000",
		"G00 X517.986360 Y766.654300",
		"G03 X517.080852 Y766.568377 Z-1.000000 I-0.401775 J-0.580206 F400.000000",
		"G03 X514.691440 Y763.294920 Z-1.000000 I9.983023 J-9.795750",
		"G02 X512.301001 Y759.512462 Z-1.000000 I-25.260139 J13.317328",
		"G02 X511.597690 Y759.160160 Z-1.000000 I-0.703311 J0.525869",
		"G03 X509.296742 Y757.385152 Z-1.000000 I-0.000000 J-2.378867",
		"G03 X509.507840 Y751.197270 Z-1.000000 I10.445131 J-2.741209",
		"G03 X510.899717 Y749.018056 Z-1.000000 I5.081513 J1.711486",
		"G03 X511.659082 Y748.861533 Z-1.000000 I0.505740 J0.533300",
		"G03 X512.207085 Y749.484200 Z-1.000000 I-0.300298 J0.816767",
		"G03 X512.201200 Y752.187500 Z-1.000000 I-5.851654 J1.338918",
		"G02 X512.291485 Y754.628381 Z-1.000000 I4.520626 J1.054899",
		"G02 X513.843780 Y757.437500 Z-1.000000 I7.647256 J-2.392358",
		"G03 X518.503167 Y764.796717 Z-1.000000 I-23.408146 J19.975168",
		"G03 X517.986360 Y766.654300 Z-1.000000 I-1.375131 J0.618102",
		"G01 X517.986360 Y766.654300 Z-1.000000",
		"G00 X514.337920 Y747.826170",
		"G03 X513.056589 Y746.988910 Z-1.000000 I-0.138714 J-1.186810 F400.000000",
		"G03 X513.121120 Y743.091800 Z-1.000000 I6.044080 J-1.849007",
		"G01 X513.117120 Y743.091800 Z-1.000000",
		"G03 X514.603238 Y741.022353 Z-1.000000 I4.176045 J1.430582",
		"G03 X516.044850 Y740.841800 Z-1.000000 I0.874647 J1.138057",
		"G03 X517.629473 Y743.247152 Z-1.000000 I-1.031727 J2.404336",
		"G03 X516.044850 Y746.882810 Z-1.000000 I-4.967449 J-0.001930",
		"G03 X515.025405 Y747.610698 Z-1.000000 I-3.314372 J-3.564115",
		"G03 X514.337820 Y747.826170 Z-1.000000 I-0.912141 J-1.705899",
		"G01 X514.337920 Y747.826170 Z-1.000000",
		"G00 X512.998080 Y739.177730",
		"G03 X512.049880 Y738.648691 Z-1.000000 I0.136811 J-1.359459 F400.000000",
		"G03 X510.052969 Y735.466463 Z-1.000000 I16.089117 J-12.313904",
		"G03 X508.573654 Y732.012046 Z-1.000000 I26.661363 J-13.461381",
		"G03 X507.619170 Y728.332030 Z-1.000000 I20.406125 J-7.256519",
		"G02 X506.350908 Y724.198592 Z-1.000000 I-16.221395 J2.715920",
		"G02 X504.490260 Y721.501950 Z-1.000000 I-8.581111 J3.930621",
		"G03 X502.802334 Y718.980205 Z-1.000000 I5.763483 J-5.683554",
		"G03 X502.869170 Y717.679690 Z-1.000000 I1.370515 J-0.581542",
		"G02 X502.757927 Y717.258250 Z-1.000000 I-0.276363 J-0.152453",
		"G02 X502.207060 Y717.273440 Z-1.000000 I-0.263793 J0.429730",
		"G03 X500.003168 Y717.332419 Z-1.000000 I-1.147065 J-1.656478",
		"G03 X485.285190 Y706.847660 Z-1.000000 I70.956897 J-115.178279",
		"G02 X478.754946 Y702.103148 Z-1.000000 I-44.462049 J54.330312",
		"G02 X477.171910 Y701.638670 Z-1.000000 I-1.583036 J2.465413",
		"G03 X475.666597 Y701.089643 Z-1.000000 I0.000000 J-2.338133",
		"G03 X469.830110 Y695.460940 Z-1.000000 I38.521509 J-45.783859",
		"G02 X463.936925 Y689.702543 Z-1.000000 I-49.901185 J45.174475",
		"G02 X461.160190 Y687.970700 Z-1.000000 I-6.932839 J8.023754",
		"G03 X459.032872 Y686.237986 Z-1.000000 I1.940238 J-4.554354",
		"G03 X458.800810 Y684.570310 Z-1.000000 I1.532770 J-1.063274",
		"G03 X459.768875 Y683.250606 Z-1.000000 I2.594954 J0.888609",
		"G03 X460.609410 Y683.238280 Z-1.000000 I0.428895 J0.582175",
		"G02 X461.589462 Y682.649089 Z-1.000000 I0.357117 J-0.515675",
		"G02 X459.871120 Y676.986330 Z-1.000000 I-16.675207 J1.967936",
		"G03 X458.679820 Y673.326123 Z-1.000000 I10.959525 J-5.591005",
		"G03 X458.077497 Y667.388487 Z-1.000000 I52.198148 J-8.294426",
		"G03 X458.418516 Y663.419281 Z-1.000000 I15.313869 J-0.683547",
		"G03 X459.259800 Y663.289060 Z-1.000000 I0.445865 J0.097841",
		"G02 X460.014916 Y664.397151 Z-1.000000 I7.748882 J-4.469194",
		"G02 X460.218495 Y664.435546 Z-1.000000 I0.124071 J-0.098942",
		"G02 X460.356889 Y664.222113 Z-1.000000 I-0.140495 J-0.242685",
		"G02 X460.441440 Y662.605470 Z-1.000000 I-15.413119 J-1.616643",
		"G02 X459.350610 Y658.487594 Z-1.000000 I-8.359272 J0.010962",
		"G02 X446.826200 Y638.636720 Z-1.000000 I-220.698618 J125.367825",
		"G02 X435.820849 Y625.421557 Z-1.000000 I-82.757487 J57.728841",
		"G02 X426.464870 Y618.042970 Z-1.000000 I-34.452365 J34.064334",
		"G03 X419.072588 Y612.299099 Z-1.000000 I20.938054 J-34.575803",
		"G03 X408.978512 Y601.371229 Z-1.000000 I107.716262 J-109.623409",
		"G03 X400.125462 Y589.314447 Z-1.000000 I89.675549 J-75.125636",
		"G03 X400.033230 Y586.492190 Z-1.000000 I2.325606 J-1.488637",
		"G03 X403.403435 Y583.602142 Z-1.000000 I5.548613 J3.060378",
		"G03 X414.277807 Y580.672508 Z-1.000000 I22.081749 J60.317342",
		"G03 X425.487085 Y579.697675 Z-1.000000 I11.089602 J62.582333",
		"G03 X429.484410 Y581.273440 Z-1.000000 I-0.011083 J5.886109",
		"G02 X431.185763 Y582.428962 Z-1.000000 I4.609933 J-4.957248",
		"G02 X431.695340 Y582.326170 Z-1.000000 I0.186213 J-0.391351",
		"G02 X432.222450 Y581.332392 Z-1.000000 I-1.758775 J-1.569552",
		"G02 X434.407010 Y571.917009 Z-1.000000 I-249.780962 J-62.915389",
		"G02 X436.194338 Y562.887833 Z-1.000000 I-326.242532 J-69.271293",
		"G02 X437.187530 Y556.460940 Z-1.000000 I-113.020682 J-20.756053",
		"G03 X438.585584 Y549.038353 Z-1.000000 I61.297147 J7.702442",
		"G03 X441.164090 Y540.710940 Z-1.000000 I79.865831 J20.166800",
		"G02 X444.128943 Y532.470612 Z-1.000000 I-523.938093 J-193.165394",
		"G02 X446.242220 Y526.240230 Z-1.000000 I-294.943811 J-103.515276",
		"G03 X452.902939 Y511.450973 Z-1.000000 I72.365598 J23.697152",
		"G03 X464.595730 Y494.906250 Z-1.000000 I102.521276 J60.051488",
		"G03 X472.208232 Y487.444227 Z-1.000000 I43.052894 J36.307075",
		"G03 X484.116615 Y478.861230 Z-1.000000 I82.348238 J101.700571",
		"G03 X496.923973 Y471.673228 Z-1.000000 I69.421542 J108.689255",
		"G03 X506.541050 Y468.115230 Z-1.000000 I22.519301 J46.092246",
		"G03 X511.918747 Y466.844475 Z-1.000000 I31.312212 J120.495599",
		"G03 X513.851280 Y466.682503 Z-1.000000 I1.604422 J7.532998",
		"G03 X515.508797 Y467.203586 Z-1.000000 I-0.142898 J3.351286",
		"G03 X517.209010 Y468.570310 Z-1.000000 I-5.308169 J8.344296",
		"G02 X530.570935 Y479.084909 Z-1.000000 I49.960737 J-49.742523",
		"G02 X537.894560 Y481.134770 Z-1.000000 I7.323625 J-12.057784",
		"G02 X540.506816 Y480.892028 Z-1.000000 I-0.000000 J-14.177223",
		"G02 X541.993915 Y480.243755 Z-1.000000 I-0.667622 J-3.561276",
		"G02 X543.137594 Y479.021515 Z-1.000000 I-2.754020 J-3.723205",
		"G02 X544.839870 Y475.998050 Z-1.000000 I-25.320358 J-16.246849",
		"G03 X547.511319 Y470.952466 Z-1.000000 I80.518697 J39.401651",
		"G03 X550.900420 Y465.384770 Z-1.000000 I101.059225 J57.700217",
		"G02 X555.559123 Y456.176388 Z-1.000000 I-40.759002 J-26.403440",
		"G02 X570.603550 Y413.339840 Z-1.000000 I-878.339903 J-332.537846",
		"G03 X574.558817 Y402.576732 Z-1.000000 I141.342479 J45.832753",
		"G03 X576.720730 Y398.490230 Z-1.000000 I22.333593 J9.200194",
		"G01 X579.285190 Y394.595703 Z-1.000000",
		"G01 X576.181670 Y390.957031 Z-1.000000",
		"G03 X570.552176 Y380.594421 Z-1.000000 I21.675898 J-18.485861",
		"G03 X568.070340 Y363.691406 Z-1.000000 I57.302787 J-17.047364",
		"G03 X568.864854 Y353.211430 Z-1.000000 I67.406573 J-0.159836",
		"G03 X569.820340 Y352.392578 Z-1.000000 I0.955486 J0.148032",
		"G02 X571.122849 Y351.696069 Z-1.000000 I0.000000 J-1.566136",
		"G02 X574.164090 Y345.886719 Z-1.000000 I-25.673340 J-17.140928",
		"G03 X577.138474 Y339.626061 Z-1.000000 I47.684824 J18.817763",
		"G03 X579.880890 Y335.695312 Z-1.000000 I21.363815 J11.983124",
		"G01 X583.031280 Y332.009766 Z-1.000000",
		"G01 X579.367220 Y326.792969 Z-1.000000",
		"G03 X567.446274 Y302.313250 Z-1.000000 I70.350846 J-49.401360",
		"G03 X563.169950 Y273.324219 Z-1.000000 I95.300036 J-28.868137",
		"G03 X567.136433 Y246.329367 Z-1.000000 I94.622528 J0.114508",
		"G03 X577.513700 Y225.361328 Z-1.000000 I66.634440 J19.926051",
		"G01 X582.765650 Y218.136719 Z-1.000000",
		"G01 X581.976590 Y206.855469 Z-1.000000",
		"G02 X581.311176 Y199.630380 Z-1.000000 I-163.972221 J11.458271",
		"G02 X580.666325 Y196.546292 Z-1.000000 I-16.998346 J1.944721",
		"G02 X579.383013 Y193.826312 Z-1.000000 I-10.119922 J3.111943",
		"G02 X576.988310 Y190.638670 Z-1.000000 I-27.317246 J18.028643",
		"G03 X573.816016 Y185.245252 Z-1.000000 I13.794670 J-11.743378",
		"G03 X571.494170 Y175.955080 Z-1.000000 I39.694813 J-14.855957",
		"G03 X570.881063 Y163.148199 Z-1.000000 I75.877131 J-10.050599",
		"G03 X572.474865 Y156.489191 Z-1.000000 I17.314565 J0.623920",
		"G03 X576.623606 Y151.294810 Z-1.000000 I12.682192 J5.875255",
		"G03 X585.267610 Y146.222660 Z-1.000000 I24.328201 J31.558667",
		"G03 X592.985276 Y143.661944 Z-1.000000 I19.031862 J44.449185",
		"G03 X598.087313 Y143.231805 Z-1.000000 I3.954319 J16.429993",
		"G03 X602.934563 Y144.485121 Z-1.000000 I-0.924004 J13.573750",
		"G03 X608.035190 Y147.773440 Z-1.000000 I-9.594730 J20.482763",
		"G02 X612.943745 Y150.449221 Z-1.000000 I8.790411 J-10.285354",
		"G02 X622.771510 Y152.185550 Z-1.000000 I12.356270 J-41.256365",
		"G02 X632.788109 Y151.653769 Z-1.000000 I2.639606 J-44.882480",
		"G02 X638.746120 Y149.529300 Z-1.000000 I-2.935805 J-17.650147",
		"G03 X644.687217 Y147.248276 Z-1.000000 I10.336345 J18.044250",
		"G03 X652.994170 Y146.447270 Z-1.000000 I7.565724 J34.986740",
		"G03 X668.308286 Y149.195552 Z-1.000000 I-1.031159 J49.787068",
		"G03 X675.037140 Y154.107420 Z-1.000000 I-4.685259 J13.483339",
		"G03 X677.307195 Y158.747723 Z-1.000000 I-11.028023 J8.270364",
		"G03 X677.738310 Y164.011720 Z-1.000000 I-13.866894 J3.785335",
		"G03 X674.272913 Y176.551040 Z-1.000000 I-39.025771 J-4.036746",
		"G03 X665.710970 Y189.812500 Z-1.000000 I-52.341529 J-24.398412",
		"G01 X660.015650 Y196.433590 Z-1.000000",
		"G01 X659.957050 Y206.960938 Z-1.000000",
		"G03 X658.941993 Y224.062353 Z-1.000000 I-159.102984 J-0.862748",
		"G03 X656.527360 Y237.142578 Z-1.000000 I-95.296523 J-10.828925",
		"G02 X656.335181 Y238.843113 Z-1.000000 I5.964195 J1.535147",
		"G02 X656.732410 Y239.119837 Z-1.000000 I0.300629 J-0.008076",
		"G02 X657.640695 Y238.632296 Z-1.000000 I-1.004233 J-2.960710",
		"G02 X661.679710 Y235.394531 Z-1.000000 I-90.592955 J-117.150170",
		"G03 X667.661170 Y230.426198 Z-1.000000 I0.000000 J-6.000000 ",
		"G03 X671.328140 Y227.380859 Z-1.000000 I0.000000 J-1.150000",
		"G02 X674.987680 Y224.328050 Z-1.000000 I-682.561782 J-821.937539",
		"G02 X679.349630 Y220.658203 Z-1.000000 I-935.426995 J-1116.267968",
		"G03 X694.235186 Y211.809505 Z-1.000000 I34.001704 J40.253906",
		"G03 X713.462910 Y207.378906 Z-1.000000 I24.807654 J63.722063",
		"G01 X723.818380 Y206.531250 Z-1.000000",
		"G01 X725.281270 Y196.718750 Z-1.000000",
		"G03 X726.767183 Y188.542016 Z-1.000000 I129.251535 J19.264788",
		"G03 X727.973928 Y184.710376 Z-1.000000 I21.024425 J4.515630",
		"G03 X729.976545 Y181.434168 Z-1.000000 I11.839384 J4.986786",
		"G03 X732.933610 Y178.550780 Z-1.000000 I15.942605 J13.391969",
		"G03 X736.789544 Y176.583620 Z-1.000000 I6.738451 J8.445701",
		"G03 X743.169940 Y175.716800 Z-1.000000 I6.380396 J23.048670",
		"G02 X747.387087 Y175.512672 Z-1.000000 I0.000000 J-43.663783",
		"G02 X749.416658 Y174.948655 Z-1.000000 I-0.586493 J-6.044082",
		"G02 X751.109521 Y173.718163 Z-1.000000 I-2.475244 J-5.185079",
		"G02 X753.677750 Y170.701170 Z-1.000000 I-26.678131 J-25.311486",
		"G02 X756.596124 Y165.376087 Z-1.000000 I-16.156795 J-12.316855",
		"G02 X757.568380 Y160.087890 Z-1.000000 I-14.962872 J-5.484459",
		"G03 X759.903351 Y148.243133 Z-1.000000 I33.289726 J0.409916",
		"G03 X764.834000 Y141.734380 Z-1.000000 I14.162699 J5.606869",
		"G03 X774.341379 Y139.013030 Z-1.000000 I8.237439 J10.810244",
		"G03 X787.781270 Y143.886720 Z-1.000000 I-2.654969 J28.289502",
		"G03 X797.031164 Y154.211739 Z-1.000000 I-16.648069 J24.220406",
		"G03 X801.410180 Y169.289060 Z-1.000000 I-33.215624 J17.821631",
		"G02 X803.183870 Y179.014079 Z-1.000000 I45.929863 J-3.352626",
		"G02 X805.796900 Y184.095700 Z-1.000000 I14.733896 J-4.363708",
		"G03 X809.229527 Y188.925356 Z-1.000000 I-53.057820 J41.344968",
		"G03 X814.794940 Y198.015625 Z-1.000000 I-182.930631 J118.245991",
		"G02 X820.754363 Y206.434504 Z-1.000000 I49.059938 J-28.409104",
		"G02 X823.351580 Y208.031250 Z-1.000000 I3.769777 J-3.221152",
		"G03 X834.671280 Y212.542326 Z-1.000000 I-10.288506 J42.274943",
		"G03 X845.908220 Y220.937500 Z-1.000000 I-28.638719 J50.050837",
		"G02 X853.248202 Y227.229971 Z-1.000000 I60.972719 J-63.695711",
		"G02 X854.810570 Y227.759766 Z-1.000000 I1.562368 J-2.038817",
		"G03 X856.494306 Y228.107997 Z-1.000000 I0.000000 J4.244646",
		"G03 X862.351580 Y231.017578 Z-1.000000 I-24.415916 J56.502071",
		"G02 X868.198198 Y234.114366 Z-1.000000 I60.058540 J-106.320763",
		"G02 X877.285180 Y238.417969 Z-1.000000 I126.087272 J-254.485731",
		"G03 X886.346417 Y242.776318 Z-1.000000 I-87.356721 J193.217806",
		"G03 X906.007830 Y253.080078 Z-1.000000 I-472.731554 J925.966766",
		"G02 X925.789261 Y263.109165 Z-1.000000 I217.433116 J-404.343314",
		"G02 X939.009790 Y268.822266 Z-1.000000 I83.426670 J-174.902175",
		"G03 X952.402699 Y274.176149 Z-1.000000 I-273.084645 J702.558323",
		"G03 X970.789080 Y281.880859 Z-1.000000 I-544.890064 J1326.106586",
		"G02 X1004.846081 Y294.041128 Z-1.000000 I116.874129 J-273.555351",
		"G02 X1020.332020 Y296.240234 Z-1.000000 I15.485939 J-53.425840",
		"G02 X1034.530906 Y294.674301 Z-1.000000 I-0.000000 J-65.156207",
		"G02 X1051.390620 Y289.304688 Z-1.000000 I-21.777199 J-97.530048",
		"G02 X1057.920957 Y285.305702 Z-1.000000 I-9.745452 J-23.245815",
		"G02 X1069.164120 Y274.275391 Z-1.000000 I-68.884904 J-81.459394",
		"G02 X1079.446851 Y262.074399 Z-1.000000 I-288.434396 J-253.519778",
		"G02 X1083.919920 Y256.009766 Z-1.000000 I-66.613911 J-53.814082",
		"G03 X1089.425420 Y249.691173 Z-1.000000 I27.840137 J18.699777",
		"G03 X1092.119495 Y248.647835 Z-1.000000 I2.735924 J3.064673",
		"G03 X1094.323541 Y249.852607 Z-1.000000 I0.026187 J2.570553",
		"G03 X1096.613320 Y255.880859 Z-1.000000 I-13.947789 J8.746949",
		"G03 X1096.885104 Y262.373883 Z-1.000000 I-25.901603 J4.336388",
		"G03 X1095.215070 Y275.064448 Z-1.000000 I-131.206648 J-10.811175",
		"G03 X1092.411292 Y287.455314 Z-1.000000 I-145.214527 J-26.346172",
		"G03 X1089.291020 Y296.240234 Z-1.000000 I-59.471596 J-16.176823",
		"G03 X1086.859008 Y301.008616 Z-1.000000 I-44.806958 J-19.848453",
		"G03 X1084.436577 Y304.227115 Z-1.000000 I-14.679379 J-8.527677",
		"G03 X1081.313506 Y306.979394 Z-1.000000 I-17.846675 J-17.102966",
		"G03 X1074.189500 Y312.000000 Z-1.000000 I-107.418869 J-144.857749",
		"G02 X1066.561979 Y317.485324 Z-1.000000 I77.466131 J115.764994",
		"G02 X1062.578100 Y320.958984 Z-1.000000 I26.971215 J34.954155",
		"G03 X1054.511821 Y327.330014 Z-1.000000 I-33.290346 J-33.856654",
		"G03 X1041.463175 Y334.407897 Z-1.000000 I-62.576883 J-99.798462",
		"G03 X1027.675269 Y339.843580 Z-1.000000 I-53.357481 J-115.139423",
		"G03 X1015.642600 Y342.708984 Z-1.000000 I-23.171996 J-70.608942",
		"G03 X990.278851 Y345.020513 Z-1.000000 I-32.058234 J-211.455466",
		"G03 X967.041020 Y344.232422 Z-1.000000 I-5.584548 J-178.324689",
		"G01 X951.232420 Y342.662109 Z-1.000000",
		"G01 X936.042970 Y347.564453 Z-1.000000",
		"G02 X915.454423 Y356.171373 Z-1.000000 I41.022148 J127.057077",
		"G02 X904.423830 Y364.009766 Z-1.000000 I22.667301 J43.579210",
		"G02 X899.471682 Y371.248027 Z-1.000000 I14.113553 J14.969133",
		"G02 X891.363280 Y396.220703 Z-1.000000 I170.836280 J69.271696",
		"G03 X885.074554 Y420.843944 Z-1.000000 I-1673.360123 J-414.258088",
		"G03 X882.169925 Y430.564090 Z-1.000000 I-148.873497 J-39.193161",
		"G03 X878.549323 Y439.361175 Z-1.000000 I-67.259318 J-22.538201",
		"G03 X874.291020 Y447.167970 Z-1.000000 I-95.439690 J-46.993877",
		"G02 X870.306455 Y454.489149 Z-1.000000 I83.579741 J50.233313",
		"G02 X869.951170 Y456.021480 Z-1.000000 I3.126796 J1.532331",
		"G02 X871.279898 Y458.805707 Z-1.000000 I3.581408 J-0.000000",
		"G02 X878.240492 Y462.918996 Z-1.000000 I17.415718 J-21.525164",
		"G02 X886.390800 Y465.555754 Z-1.000000 I21.782907 J-53.416876",
		"G02 X899.085940 Y467.902340 Z-1.000000 I27.353916 J-112.472046",
		"G02 X902.552432 Y468.165262 Z-1.000000 I4.294615 J-33.638912",
		"G02 X903.887487 Y467.829799 Z-1.000000 I0.062877 J-2.574084",
		"G02 X904.827484 Y466.849060 Z-1.000000 I-1.174265 J-2.066326",
		"G02 X906.128910 Y463.900390 Z-1.000000 I-24.432180 J-12.544932",
		"G03 X908.393130 Y460.164746 Z-1.000000 I10.950995 J4.083525",
		"G03 X910.172325 Y459.304607 Z-1.000000 I1.943040 J1.748975",
		"G03 X912.047159 Y459.934355 Z-1.000000 I0.164309 J2.616511",
		"G03 X914.773440 Y463.357420 Z-1.000000 I-7.662524 J8.899971",
		"G02 X917.251308 Y466.649072 Z-1.000000 I11.430101 J-6.025813",
		"G02 X918.419920 Y466.861330 Z-1.000000 I0.744284 J-0.774649",
		"G03 X919.715432 Y467.042483 Z-1.000000 I0.498355 J1.159012",
		"G03 X926.876950 Y473.755860 Z-1.000000 I-44.872526 J55.044398",
		"G02 X942.196275 Y488.791703 Z-1.000000 I170.191351 J-158.078094",
		"G02 X950.011720 Y494.453120 Z-1.000000 I31.311737 J-34.999853",
		"G02 X952.635483 Y495.485056 Z-1.000000 I4.743148 J-8.208242",
		"G02 X964.856509 Y497.993894 Z-1.000000 I60.662708 J-264.479738",
		"G02 X976.797299 Y499.929879 Z-1.000000 I57.586925 J-317.393002",
		"G02 X984.234380 Y500.662110 Z-1.000000 I12.064060 J-84.397159",
		"G01 X990.345700 Y500.994140 Z-1.000000",
		"G01 X990.681640 Y507.156250 Z-1.000000",
		"G02 X991.316406 Y513.255715 Z-1.000000 I61.954881 J-3.364828",
		"G02 X992.056640 Y516.058590 Z-1.000000 I13.659287 J-2.108207",
		"G03 X992.946669 Y518.843920 Z-1.000000 I-25.328538 J9.628392",
		"G03 X995.810550 Y530.439450 Z-1.000000 I-438.787487 J114.523793",
		"G02 X1000.904469 Y548.634435 Z-1.000000 I209.075790 J-48.722902",
		"G02 X1005.081431 Y558.001106 Z-1.000000 I47.630084 J-15.625431",
		"G02 X1011.257948 Y565.737725 Z-1.000000 I28.377629 J-16.321419",
		"G02 X1019.609400 Y571.820310 Z-1.000000 I30.323229 J-32.859497",
		"G03 X1035.409706 Y583.026566 Z-1.000000 I-46.195542 J81.875570",
		"G03 X1037.404300 Y587.429690 Z-1.000000 I-3.862714 J4.403124",
		"G03 X1036.135735 Y590.111992 Z-1.000000 I-3.470064 J-0.000000",
		"G03 X1025.507800 Y597.191410 Z-1.000000 I-40.719184 J-49.612170",
		"G03 X1017.795014 Y601.063607 Z-1.000000 I-89.172034 J-167.998768",
		"G03 X1013.558850 Y602.661446 Z-1.000000 I-11.850287 J-25.002935",
		"G03 X1009.145128 Y603.513126 Z-1.000000 I-7.024444 J-24.540666",
		"G03 X1001.367200 Y604.101560 Z-1.000000 I-14.805561 J-144.001542",
		"G03 X989.177593 Y603.906433 Z-1.000000 I-4.586013 J-94.351864",
		"G03 X973.845700 Y601.878910 Z-1.000000 I12.233774 J-151.493132",
		"G02 X958.529854 Y599.308572 Z-1.000000 I-87.425918 J474.027612",
		"G02 X945.121090 Y597.533200 Z-1.000000 I-54.618369 J360.989832",
		"G03 X922.423950 Y593.457111 Z-1.000000 I20.166736 J-177.526707",
		"G03 X908.054690 Y588.373050 Z-1.000000 I18.724802 J-75.770758",
		"G02 X903.455820 Y587.055696 Z-1.000000 I-6.410303 J13.692322",
		"G02 X891.529300 Y586.378910 Z-1.000000 I-11.292014 J93.566475",
		"G03 X879.487660 Y585.987047 Z-1.000000 I-1.031719 J-153.506907",
		"G03 X873.166020 Y585.058590 Z-1.000000 I3.097314 J-43.074372",
		"G02 X862.789749 Y583.758294 Z-1.000000 I-11.996191 J53.677661",
		"G02 X849.314450 Y584.355470 Z-1.000000 I-2.695998 J91.498636",
		"G02 X829.666477 Y589.048611 Z-1.000000 I10.237564 J86.334839",
		"G02 X821.478520 Y594.544920 Z-1.000000 I7.122678 J19.457823",
		"G02 X816.416474 Y600.731971 Z-1.000000 I55.091902 J50.238749",
		"G02 X810.846820 Y608.870724 Z-1.000000 I124.537901 J91.201096",
		"G02 X805.772438 Y617.635330 Z-1.000000 I98.422956 J62.834486",
		"G02 X805.169920 Y620.089840 Z-1.000000 I4.698277 J2.454510",
		"G03 X804.672150 Y623.050469 Z-1.000000 I-9.053473 J0.000000",
		"G03 X802.962890 Y626.468750 Z-1.000000 I-13.873884 J-4.800941",
		"G02 X801.099736 Y629.895524 Z-1.000000 I16.822301 J11.366255",
		"G02 X799.093750 Y635.576170 Z-1.000000 I44.507125 J18.911151",
		"G03 X795.290554 Y643.113609 Z-1.000000 I-21.479873 J-6.109969",
		"G03 X793.000000 Y643.468750 Z-1.000000 I-1.332289 J-1.028604",
		"G03 X792.146918 Y642.363600 Z-1.000000 I1.312068 J-1.894634",
		"G03 X791.724610 Y639.972660 Z-1.000000 I6.557122 J-2.390940",
		"G03 X792.181940 Y637.171024 Z-1.000000 I8.810171 J-0.000000",
		"G03 X799.064450 Y618.093750 Z-1.000000 I425.101788 J142.583869",
		"G02 X802.723375 Y607.742377 Z-1.000000 I-176.150754 J-68.086789",
		"G02 X803.462890 Y604.396480 Z-1.000000 I-17.534379 J-5.630146",
		"G03 X805.573641 Y593.038646 Z-1.000000 I96.554772 J12.068781",
		"G03 X806.265620 Y593.097660 Z-1.000000 I0.341286 J0.084655",
		"G02 X806.847909 Y594.318895 Z-1.000000 I1.856254 J-0.135631",
		"G02 X808.089840 Y594.810550 Z-1.000000 I1.241931 J-1.322748",
		"G02 X809.879267 Y594.277826 Z-1.000000 I-0.000000 J-3.271718",
		"G02 X812.648085 Y592.116513 Z-1.000000 I-11.436301 J-17.505025",
		"G02 X814.320549 Y590.025742 Z-1.000000 I-6.117585 J-6.607932",
		"G02 X813.958980 Y589.376950 Z-1.000000 I-0.373868 J-0.216792",
		"G03 X813.160550 Y588.873531 Z-1.000000 I0.026422 J-0.926777",
		"G03 X813.175780 Y587.851560 Z-1.000000 I0.967706 J-0.496677",
		"G03 X814.277629 Y587.053524 Z-1.000000 I1.398843 J0.771705",
		"G03 X816.812500 Y587.164060 Z-1.000000 I1.031994 J5.454533",
		"G02 X819.343459 Y587.388773 Z-1.000000 I1.863849 J-6.627134",
		"G02 X820.041020 Y586.849610 Z-1.000000 I-0.080296 J-0.824715",
		"G03 X821.374616 Y585.546895 Z-1.000000 I2.031711 J0.745911",
		"G03 X833.875695 Y581.835561 Z-1.000000 I52.806658 J154.961744",
		"G03 X846.271109 Y579.097230 Z-1.000000 I49.881271 J196.370203",
		"G03 X853.449220 Y578.363280 Z-1.000000 I7.937335 J42.159665",
		"G03 X860.546122 Y578.525772 Z-1.000000 I1.548067 J87.448765",
		"G03 X871.978243 Y579.456613 Z-1.000000 I-20.488884 J322.301555",
		"G03 X883.643746 Y580.864359 Z-1.000000 I-27.533291 J277.196588",
		"G03 X886.933590 Y581.689450 Z-1.000000 I-2.286863 J16.089557",
		"G02 X887.759102 Y581.942477 Z-1.000000 I2.803723 J-7.674131",
		"G02 X887.866351 Y581.903113 Z-1.000000 I0.024627 J-0.098686",
		"G02 X887.859560 Y581.806046 Z-1.000000 I-0.061895 J-0.044441",
		"G02 X887.117280 Y581.125993 Z-1.000000 I-5.459853 J5.214319",
		"G02 X885.410078 Y579.837379 Z-1.000000 I-23.547837 J29.421851",
		"G02 X878.890620 Y575.251950 Z-1.000000 I-442.327671 J621.963981",
		"G03 X875.407061 Y572.622811 Z-1.000000 I32.140004 J-46.207279",
		"G03 X874.293559 Y571.262908 Z-1.000000 I2.808076 J-3.435106",
		"G03 X873.902725 Y569.617649 Z-1.000000 I2.980044 J-1.576966",
		"G03 X874.179690 Y566.349610 Z-1.000000 I25.519466 J0.517012",
		"G02 X873.734358 Y553.194046 Z-1.000000 I-35.824202 J-5.372625",
		"G02 X866.041020 Y529.353520 Z-1.000000 I-128.141092 J28.189547",
		"G02 X860.034134 Y518.035553 Z-1.000000 I-73.432733 J31.720583",
		"G02 X853.630860 Y509.910160 Z-1.000000 I-43.979070 J28.072253",
		"G02 X840.085491 Y496.994206 Z-1.000000 I-158.751116 J152.926613",
		"G02 X837.970700 Y496.199220 Z-1.000000 I-2.114791 J2.415347",
		"G02 X837.013618 Y496.714518 Z-1.000000 I-0.000000 J1.146460",
		"G02 X832.280948 Y504.519756 Z-1.000000 I100.480782 J66.263507",
		"G02 X828.205351 Y512.350139 Z-1.000000 I141.636964 J78.695752",
		"G02 X825.794920 Y518.113280 Z-1.000000 I49.725182 J24.183182",
		"G02 X818.522004 Y541.182723 Z-1.000000 I333.344441 J117.771957",
		"G02 X817.037110 Y549.613280 Z-1.000000 I42.650278 J11.858141",
		"G03 X816.236998 Y555.721680 Z-1.000000 I-58.306916 J-4.530757",
		"G03 X814.375000 Y563.994140 Z-1.000000 I-108.435610 J-20.061339",
		"G03 X811.351074 Y574.262432 Z-1.000000 I-194.815164 J-51.792022",
		"G03 X810.166020 Y577.005860 Z-1.000000 I-15.018311 J-4.859665",
		"G02 X809.168338 Y579.117470 Z-1.000000 I16.745097 J9.203127",
		"G02 X807.572270 Y583.494140 Z-1.000000 I70.821932 J28.306442",
		"G03 X805.853512 Y587.170418 Z-1.000000 I-16.339422 J-5.399191",
		"G03 X805.207030 Y587.304690 Z-1.000000 I-0.387090 J-0.240278",
		"G03 X804.674852 Y586.468575 Z-1.000000 I0.727153 J-1.050244",
		"G03 X803.544920 Y577.533200 Z-1.000000 I104.161549 J-17.710990",
		"G02 X801.915206 Y568.602265 Z-1.000000 I-46.536394 J3.877785",
		"G02 X797.253910 Y555.480470 Z-1.000000 I-105.549501 J30.105854",
		"G03 X791.867918 Y542.546962 Z-1.000000 I631.656242 J-270.633287",
		"G03 X791.724610 Y541.810550 Z-1.000000 I1.820435 J-0.736412",
		"G02 X791.450814 Y540.783181 Z-1.000000 I-2.064404 J0.000000",
		"G02 X786.482875 Y532.473240 Z-1.000000 I-234.252724 J134.403537",
		"G02 X781.468095 Y524.671787 Z-1.000000 I-270.592362 J168.424508",
		"G02 X778.617190 Y520.751950 Z-1.000000 I-47.010377 J31.194082",
		"G02 X775.190756 Y517.239593 Z-1.000000 I-19.003503 J15.111132",
		"G02 X773.826170 Y516.744140 Z-1.000000 I-1.364586 J1.631459",
		"G03 X772.542183 Y516.203372 Z-1.000000 I0.000000 J-1.794720",
		"G03 X770.144530 Y512.976560 Z-1.000000 I10.721466 J-10.470669",
		"G02 X767.641495 Y509.059957 Z-1.000000 I-31.731840 J17.521155",
		"G02 X763.758703 Y504.118060 Z-1.000000 I-76.494888 J56.104893",
		"G02 X759.378321 Y499.365095 Z-1.000000 I-54.335941 J45.681573",
		"G02 X758.343750 Y498.939450 Z-1.000000 I-1.034571 J1.044491",
		"G03 X757.366079 Y498.622958 Z-1.000000 I-0.000000 J-1.668301",
		"G03 X753.609380 Y495.515620 Z-1.000000 I22.017302 J-30.442952",
		"G02 X749.797362 Y492.546446 Z-1.000000 I-17.610173 J18.677436",
		"G02 X748.238280 Y492.091800 Z-1.000000 I-1.559082 J2.445896",
		"G03 X746.921374 Y491.668345 Z-1.000000 I0.000000 J-2.259458",
		"G03 X746.498050 Y490.843750 Z-1.000000 I0.591456 J-0.824595",
		"G02 X745.813901 Y489.525568 Z-1.000000 I-1.611976 J-0.000000",
		"G02 X740.951433 Y486.635269 Z-1.000000 I-21.157672 J30.059103",
		"G02 X735.692522 Y484.457056 Z-1.000000 I-18.958972 J38.335625",
		"G02 X733.054690 Y484.195310 Z-1.000000 I-1.885041 J5.574445",
		"G03 X730.838335 Y483.402122 Z-1.000000 I-0.331193 J-2.567673",
		"G03 X726.224610 Y476.939450 Z-1.000000 I21.453371 J-20.193854",
		"G02 X721.736643 Y469.664061 Z-1.000000 I-55.742575 J29.363962",
		"G02 X716.736330 Y463.666020 Z-1.000000 I-42.785846 J30.585458",
		"G03 X711.690256 Y457.765571 Z-1.000000 I45.210909 J-43.772377",
		"G03 X710.533200 Y455.435550 Z-1.000000 I5.651421 J-4.258716",
		"G02 X709.314179 Y453.175563 Z-1.000000 I-5.694161 J1.612632",
		"G02 X704.316410 Y448.251950 Z-1.000000 I-33.389254 J28.893810",
		"G03 X699.115882 Y443.512564 Z-1.000000 I55.677800 J-66.318204",
		"G03 X698.830080 Y442.828120 Z-1.000000 I0.676658 J-0.684444",
		"G02 X697.558708 Y439.550332 Z-1.000000 I-4.861002 J-0.000000",
		"G02 X685.765540 Y428.014996 Z-1.000000 I-107.998138 J98.616013",
		"G02 X673.199782 Y417.866180 Z-1.000000 I-137.936054 J157.931991",
		"G02 X661.250000 Y410.005860 Z-1.000000 I-72.462089 J97.148104",
		"G03 X651.412367 Y403.963415 Z-1.000000 I102.870613 J-178.511926",
		"G03 X650.503910 Y402.984380 Z-1.000000 I1.512488 J-2.314454",
		"G02 X648.938074 Y401.317750 Z-1.000000 I-4.043772 J2.230329",
		"G02 X645.035715 Y399.307190 Z-1.000000 I-13.013318 J20.465574",
		"G02 X640.834954 Y398.028745 Z-1.000000 I-9.208341 J22.716368",
		"G02 X638.478520 Y398.109380 Z-1.000000 I-1.013427 J4.856037",
		"G03 X635.803829 Y397.853626 Z-1.000000 I-1.007127 J-3.581312",
		"G03 X629.607420 Y393.662109 Z-1.000000 I12.939291 J-25.804333",
		"G02 X622.328841 Y387.873985 Z-1.000000 I-58.346929 J65.900915",
		"G02 X617.614245 Y385.353680 Z-1.000000 I-12.144607 J17.048430",
		"G02 X612.467401 Y	384.232898 Z-1.000000 I-6.320330 J16.646122",
		"G02 X604.951170 Y384.300781 Z-1.000000 I-3.306317 J50.058558",
		"G02 X598.313157 Y385.168335 Z-1.000000 I6.116250 J72.626855",
		"G02 X594.946968 Y386.757913 Z-1.000000 I1.242366 J6.989925",
		"G02 X592.282594 Y389.839356 Z-1.000000 I8.766176 J10.272284",
		"G02 X585.558590 Y400.849610 Z-1.000000 I411.094207 J258.615111",
		"G02 X580.596186 Y410.548777 Z-1.000000 I75.566778 J44.781426",
		"G02 X576.310550 Y422.078120 Z-1.000000 I102.629284 J44.710091",
		"G03 X572.425306 Y433.798776 Z-1.000000 I-307.666748 J-95.483225",
		"G03 X570.375000 Y438.898440 Z-1.000000 I-60.265285 J-21.267498",
		"G02 X568.269056 Y443.979868 Z-1.000000 I77.348019 J35.033167",
		"G02 X564.988280 Y453.187500 Z-1.000000 I251.137866 J94.671374",
		"G03 X561.435389 Y462.259224 Z-1.000000 I-93.878294 J-31.535325",
		"G03 X557.503910 Y469.835940 Z-1.000000 I-69.359131 J-31.181368",
		"G02 X553.828703 Y477.136489 Z-1.000000 I51.426577 J30.464267",
		"G02 X554.009770 Y478.146480 Z-1.000000 I0.869208 J0.365398",
		"G02 X554.780383 Y478.229916 Z-1.000000 I0.433834 J-0.406479",
		"G02 X556.269530 Y476.593750 Z-1.000000 I-2.746702 J-3.995647",
		"G03 X564.012385 Y464.178210 Z-1.000000 I129.381545 J72.065644",
		"G03 X569.763670 Y457.164060 Z-1.000000 I46.759292 J32.475525",
		"G02 X571.894911 Y454.181646 Z-1.000000 I-9.070959 J-8.734833",
		"G02 X572.027340 Y452.755860 Z-1.000000 I-1.709457 J-0.877820",
		"G03 X572.159752 Y451.265023 Z-1.000000 I1.944757 J-0.578570",
		"G03 X578.052730 Y440.785160 Z-1.000000 I116.989796 J58.888261",
		"G02 X584.425180 Y430.318896 Z-1.000000 I-462.224798 J-288.601541",
		"G02 X587.943360 Y424.121090 Z-1.000000 I-158.206007 J-93.902967",
		"G03 X593.588440 Y415.712005 Z-1.000000 I47.284020 J25.642775",
		"G03 X597.521803 Y413.230408 Z-1.000000 I5.680240 J4.645237",
		"G03 X602.297085 Y413.439847 Z-1.000000 I2.029149 J8.278433",
		"G03 X612.978520 Y417.984380 Z-1.000000 I-25.216131 J74.092848",
		"G02 X623.959622 Y423.297286 Z-1.000000 I108.175194 J-209.579675",
		"G02 X625.416020 Y423.611330 Z-1.000000 I1.456398 J-3.220042",
		"G03 X628.241673 Y424.607988 Z-1.000000 I-0.000000 J4.503875",
		"G03 X641.871090 Y436.869140 Z-1.000000 I-105.744219 J131.250344",
		"G02 X649.298650 Y443.856442 Z-1.000000 I112.391241 J-112.031398",
		"G02 X655.927730 Y449.265620 Z-1.000000 I73.854875 J-83.744313",
		"G03 X662.521484 Y454.712893 Z-1.000000 I-58.012032 J76.936156",
		"G03 X671.783200 Y463.591800 Z-1.000000 I-146.155242 J161.726611",
		"G02 X681.239992 Y472.186472 Z-1.000000 I81.323196 J-79.980586",
		"G02 X689.095700 Y477.640620 Z-1.000000 I40.344830 J-49.724956",
		"G03 X697.054349 Y483.027176 Z-1.000000 I-38.819423 J65.928531",
		"G03 X702.928590 Y488.185379 Z-1.000000 I-38.114946 J49.329825",
		"G03 X707.715786 Y494.129477 Z-1.000000 I-24.649554 J24.751782",
		"G03 X707.988280 Y496.455080 Z-1.000000 I-2.259266 J1.443487",
		"G03 X706.898758 Y497.757794 Z-1.000000 I-2.268579 J-0.790355",
		"G03 X705.319168 Y497.950869 Z-1.000000 I-1.023729 J-1.817330",
		"G03 X703.522909 Y497.103548 Z-1.000000 I1.555060 J-5.624246",
		"G03 X700.050780 Y494.259770 Z-1.000000 I17.323974 J-24.693361",
		"G02 X691.160281 Y487.145208 Z-1.000000 I-45.793022 J48.111725",
		"G02 X680.089840 Y480.923830 Z-1.000000 I-45.815993 J68.565696",
		"G02 X669.838891 Y476.826801 Z-1.000000 I-42.538580 J91.560748",
		"G02 X658.613523 Y473.885626 Z-1.000000 I-26.807269 J79.421244",
		"G02 X646.800334 Y472.189073 Z-1.000000 I-25.026372 J132.283764",
		"G02 X624.269530 Y470.363280 Z-1.000000 I-63.593940 J644.835976",
		"G02 X608.582460 Y469.709571 Z-1.000000 I-22.857077 J359.953690",
		"G02 X599.980413 Y470.108433 Z-1.000000 I-1.293585 J65.059419",
		"G02 X591.610174 Y471.697395 Z-1.000000 I6.380740 J56.452675",
		"G02 X579.044920 Y475.474610 Z-1.000000 I58.024736 J215.812957",
		"G03 X566.132899 Y479.270901 Z-1.000000 I-60.025743 J-180.304308",
		"G03 X562.310550 Y479.755860 Z-1.000000 I-3.851316 J-15.049320",
		"G02 X558.718608 Y480.286221 Z-1.000000 I0.029433 J12.627989",
		"G02 X557.642580 Y481.154300 Z-1.000000 I0.523988 J1.750444",
		"G03 X552.873950 Y485.002502 Z-1.000000 I-7.095230 J-3.913589",
		"G03 X540.875000 Y486.179690 Z-1.000000 I-9.004309 J-30.039355",
		"G03 X534.209775 Y484.516005 Z-1.000000 I2.209994 J-23.037198",
		"G03 X523.130860 Y478.810550 Z-1.000000 I29.178680 J-70.268791",
		"G01 X512.386720 Y472.111330 Z-1.000000",
		"G01 X506.296880 Y474.072270 Z-1.000000",
		"G02 X500.394499 Y476.387117 Z-1.000000 I15.626138 J48.525763",
		"G02 X491.965006 Y480.637626 Z-1.000000 I64.189906 J137.783483",
		"G02 X483.732195 Y485.502125 Z-1.000000 I64.210055 J118.069823",
		"G02 X480.650390 Y488.019530 Z-1.000000 I9.291620 J14.519868",
		"G03 X478.056490 Y490.348286 Z-1.000000 I-18.136305 J-17.592247",
		"G03 X475.111330 Y492.341800 Z-1.000000 I-16.214045 J-20.781857",
		"G02 X472.299734 Y494.474296 Z-1.000000 I7.336058 J12.591973",
		"G02 X467.607420 Y499.623050 Z-1.000000 I39.475455 J40.688483",
		"G02 X463.062880 Y506.186657 Z-1.000000 I43.251171 J34.801530",
		"G02 X456.318353 Y518.372480 Z-1.000000 I213.209297 J125.965019",
		"G02 X450.190581 Y531.258191 Z-1.000000 I189.268828 J97.906255",
		"G02 X449.476560 Y534.667970 Z-1.000000 I7.884532 J3.430700",
		"G03 X448.997034 Y537.850162 Z-1.000000 I-10.877922 J-0.011974",
		"G03 X447.154300 Y542.230470 Z-1.000000 I-22.710107 J-6.976061",
		"G02 X445.320632 Y546.617133 Z-1.000000 I20.418022 J11.111508",
		"G02 X443.547893 Y553.695902 Z-1.000000 I72.631709 J21.950555",
		"G02 X442.509273 Y560.999839 Z-1.000000 I64.597178 J12.911532",
		"G02 X442.849610 Y563.767580 Z-1.000000 I6.740839 J0.575904",
		"G03 X443.000551 Y566.005745 Z-1.000000 I-4.087181 J1.399810",
		"G03 X441.757810 Y569.285160 Z-1.000000 I-10.352136 J-2.047786",
		"G02 X440.319083 Y572.686521 Z-1.000000 I12.395063 J7.247896",
		"G02 X438.790238 Y579.464086 Z-1.000000 I67.859574 J18.868599",
		"G02 X437.942119 Y586.371258 Z-1.000000 I68.119336 J11.869894",
		"G02 X438.181640 Y589.806640 Z-1.000000 I12.182749 J0.876640",
		"G02 X439.950978 Y595.603886 Z-1.000000 I34.840729 J-7.464876",
		"G02 X444.060170 Y604.816615 Z-1.000000 I122.676236 J-49.195007",
		"G02 X448.896543 Y613.716496 Z-1.000000 I113.450895 J-55.887451",
		"G02 X452.000000 Y617.857420 Z-1.000000 I21.137428 J-12.608236",
		"G03 X457.196483 Y625.925574 Z-1.000000 I-20.046812 J18.619153",
		"G03 X460.972660 Y639.322270 Z-1.000000 I-53.127305 J22.205743",
		"G02 X462.398905 Y646.054086 Z-1.000000 I61.631307 J-9.540607",
		"G02 X467.428338 Y663.777190 Z-1.000000 I709.536470 J-191.775866",
		"G02 X473.148854 Y682.043289 Z-1.000000 I634.449748 J-188.666062",
		"G02 X474.025390 Y683.216800 Z-1.000000 I2.187883 J-0.720093",
		"G03 X474.709283 Y684.226253 Z-1.000000 I-0.960652 J1.387223",
		"G03 X475.148440 Y688.099610 Z-1.000000 I-16.861897 J3.873357",
		"G02 X477.265045 Y693.697032 Z-1.000000 I8.459574 J0.000000",
		"G02 X490.333980 Y705.308590 Z-1.000000 I54.146839 J-47.782465",
		"G03 X497.598056 Y710.488219 Z-1.000000 I-176.748716 J255.561505",
		"G03 X499.552730 Y712.082030 Z-1.000000 I-14.863005 J20.223742",
		"G03 X508.571090 Y721.131739 Z-1.000000 I-81.137546 J89.874894",
		"G03 X508.703120 Y722.291020 Z-1.000000 I-0.750252 J0.672605",
		"G02 X508.879338 Y722.745952 Z-1.000000 I0.278639 J0.153664",
		"G02 X509.921880 Y722.705080 Z-1.000000 I0.468093 J-1.376886",
		"G02 X511.327692 Y721.565610 Z-1.000000 I-1.316042 J-3.060595",
		"G02 X511.818360 Y720.019530 Z-1.000000 I-2.190489 J-1.546080",
		"G03 X512.371923 Y716.729666 Z-1.000000 I10.052739 J0.000000",
		"G03 X515.058268 Y710.015016 Z-1.000000 I72.471898 J25.099306",
		"G03 X518.414205 Y703.673545 Z-1.000000 I55.353855 J25.234808",
		"G03 X519.437500 Y703.306640 Z-1.000000 I0.738145 J0.448246",
		"G03 X520.748147 Y704.393204 Z-1.000000 I-0.807160 J2.307374",
		"G03 X525.463628 Y713.282184 Z-1.000000 I-123.907152 J71.426265",
		"G03 X529.523561 Y722.378439 Z-1.000000 I-136.773507 J66.500302",
		"G03 X530.113280 Y725.263670 Z-1.000000 I-7.395714 J3.014511",
		"G03 X529.539259 Y727.037001 Z-1.000000 I-2.882518 J0.046510",
		"G03 X528.727110 Y727.094950 Z-1.000000 I-0.431154 J-0.322516",
		"G03 X527.485743 Y725.483928 Z-1.000000 I5.629078 J-5.621242",
		"G03 X524.183590 Y719.326170 Z-1.000000 I83.577344 J-48.783383",
		"G02 X519.959777 Y712.164867 Z-1.000000 I-48.658394 J23.872968",
		"G02 X519.160160 Y712.134770 Z-1.000000 I-0.411184 J0.287182",
		"G02 X518.437797 Y713.389713 Z-1.000000 I3.463145 J2.828810",
		"G02 X516.669920 Y718.757810 Z-1.000000 I63.711456 J23.957265",
		"G03 X514.872293 Y724.133468 Z-1.000000 I-59.167205 J-16.797195",
		"G03 X513.433590 Y727.132810 Z-1.000000 I-19.797120 J-7.651418",
		"G02 X512.532470 Y730.107409 Z-1.000000 I5.857517 J3.398257",
		"G02 X513.017580 Y733.753910 Z-1.000000 I9.480560 J0.594279",
		"G03 X513.809973 Y737.435262 Z-1.000000 I-15.519547 J5.266463",
		"G03 X513.515620 Y738.861330 Z-1.000000 I-2.448788 J0.237961",
		"G03 X513.266057 Y739.118615 Z-1.000000 I-0.596735 J-0.329147",
		"G03 X512.998050 Y739.177730 Z-1.000000 I-0.223971 J-0.378325",
		"G01 X512.998080 Y739.177730 Z-1.000000",
		"G00 X539.320340 Y723.476560",
		"G03 X539.100983 Y723.047046 Z-1.000000 I-0.002406 J-0.269542 F400.000000",
		"G03 X543.832060 Y717.357420 Z-1.000000 I51.098888 J37.678277",
		"G03 X548.650149 Y712.580859 Z-1.000000 I65.655152 J61.407701",
		"G03 X550.161792 Y711.444822 Z-1.000000 I6.483347 J7.053192",
		"G03 X550.432634 Y711.672317 Z-1.000000 I0.096899 J0.159609",
		"G03 X548.982450 Y714.230470 Z-1.000000 I-9.569684 J-3.734808",
		"G03 X541.421849 Y722.621673 Z-1.000000 I-60.340104 J-46.765635",
		"G03 X539.320340 Y723.476560 Z-1.000000 I-2.130248 J-2.226200",
		"G01 X539.320340 Y723.476560 Z-1.000000",
		"G00 X459.173860 Y754.792970",
		"G03 X458.107239 Y753.761581 Z-1.000000 I0.649535 J-1.738945 F400.000000",
		"G03 X455.533230 Y745.962890 Z-1.000000 I52.582720 J-21.679402",
		"G02 X453.229927 Y737.800181 Z-1.000000 I-144.401186 J36.339920",
		"G02 X451.986360 Y734.636720 Z-1.000000 I-22.849689 J7.156132",
		"G03 X451.052617 Y731.524269 Z-1.000000 I9.552915 J-4.562187",
		"G03 X451.259800 Y729.378910 Z-1.000000 I4.610724 J-0.637413",
		"G03 X452.164053 Y728.529980 Z-1.000000 I1.253700 J0.429345",
		"G03 X453.171100 Y728.928325 Z-1.000000 I0.237973 J0.870504",
		"G03 X454.162072 Y730.973949 Z-1.000000 I-9.751941 J5.987027",
		"G03 X456.509800 Y737.931640 Z-1.000000 I-131.555407 J48.265568",
		"G03 X460.222704 Y753.936068 Z-1.000000 I-110.234395 J34.006427",
		"G03 X459.173860 Y754.792970 Z-1.000000 I-0.774498 J0.122359",
		"G01 X459.173860 Y754.792970 Z-1.000000",
		"G00 X451.373080 Y725.798830",
		"G03 X450.786173 Y725.574911 Z-1.000000 I-0.197175 J-0.364313 F400.000000",
		"G03 X448.986360 Y718.648440 Z-1.000000 I36.828258 J-13.266735",
		"G03 X448.109508 Y710.129991 Z-1.000000 I70.333182 J-11.544147",
		"G03 X447.923137 Y697.348650 Z-1.000000 I229.036459 J-9.731709",
		"G03 X448.574150 Y684.174016 Z-1.000000 I182.869612 J2.432939",
		"G03 X449.476590 Y681.787110 Z-1.000000 I4.664594 J0.399537",
		"G03 X450.705349 Y680.255808 Z-1.000000 I20.379731 J15.094621",
		"G03 X451.416520 Y679.884110 Z-1.000000 I0.828772 J0.719498",
		"G03 X452.288712 Y680.000141 Z-1.000000 I0.198628 J1.843033",
		"G03 X455.166050 Y681.173830 Z-1.000000 I-36.585326 J93.803953",
		"G03 X457.081039 Y682.772524 Z-1.000000 I-1.693066 J3.974310",
		"G03 X456.884800 Y683.785160 Z-1.000000 I-0.616436 J0.405874",
		"G03 X455.154390 Y684.460272 Z-1.000000 I-2.283105 J-3.296724",
		"G03 X453.564480 Y684.246090 Z-1.000000 I-0.411251 J-2.955383",
		"G02 X452.392655 Y684.244696 Z-1.000000 I-0.587539 J1.366444",
		"G02 X452.002027 Y684.873458 Z-1.000000 I0.253360 J0.593126",
		"G02 X452.289354 Y686.084854 Z-1.000000 I3.518118 J-0.194677",
		"G02 X453.828150 Y689.007810 Z-1.000000 I20.456930 J-8.903060",
		"G03 X454.949175 Y690.962600 Z-1.000000 I-25.167923 J15.732040",
		"G03 X455.096420 Y691.534887 Z-1.000000 I-1.143593 J0.599323",
		"G03 X454.856192 Y691.883496 Z-1.000000 I-0.362053 J0.007583",
		"G03 X453.890650 Y692.050780 Z-1.000000 I-0.965542 J-2.702848",
		"G02 X452.890084 Y692.793838 Z-1.000000 I0.000000 J1.045186",
		"G02 X451.728194 Y698.981115 Z-1.000000 I28.052277 J8.470588",
		"G02 X451.499203 Y705.699365 Z-1.000000 I76.306394 J5.963927",
		"G02 X451.937530 Y717.050780 Z-1.000000 I197.652053 J-1.948034",
		"G03 X451.870910 Y725.037909 Z-1.000000 I-52.702195 J3.554256",
		"G03 X451.373080 Y725.798830 Z-1.000000 I-0.953132 J-0.080271",
		"G01 X451.373080 Y725.798830 Z-1.000000",
		"G00 X452.953150 Y632.746090",
		"G02 X453.048695 Y632.655028 Z-1.000000 I-0.065875 J-0.164773 F400.000000",
		"G02 X453.109410 Y632.392580 Z-1.000000 I-0.613616 J-0.280201",
		"G02 X452.849239 Y631.029891 Z-1.000000 I-3.243011 J-0.087009",
		"G02 X451.353550 Y628.285160 Z-1.000000 I-15.179272 J6.491765",
		"G02 X449.920019 Y626.867259 Z-1.000000 I-4.266554 J2.879968",
		"G02 X449.519560 Y627.076170 Z-1.000000 I-0.142190 J0.215711",
		"G02 X449.779616 Y628.439509 Z-1.000000 I3.246968 J0.087116",
		"G02 X451.275420 Y631.185550 Z-1.000000 I15.160034 J-6.477454",
		"G02 X452.565733 Y632.676246 Z-1.000000 I6.841577 J-4.618134",
		"G02 X452.953150 Y632.746090 Z-1.000000 I0.249503 J-0.274563",
		"G01 X452.953150 Y632.746090 Z-1.000000",
		"G00 X448.316440 Y631.439450",
		"G02 X448.418771 Y631.217118 Z-1.000000 I-0.049897 J-0.157681 F400.000000",
		"G02 X447.293000 Y628.888670 Z-1.000000 I-24.432599 J10.376444",
		"G03 X445.103192 Y624.478410 Z-1.000000 I56.562079 J-30.833310",
		"G03 X444.707842 Y623.163851 Z-1.000000 I4.810507 J-2.163475",
		"G03 X445.049377 Y622.843297 Z-1.000000 I0.278564 J-0.045425",
		"G03 X446.425810 Y623.572270 Z-1.000000 I-0.670687 J2.930341",
		"G02 X447.875575 Y624.183593 Z-1.000000 I1.562241 J-1.680152",
		"G02 X448.246364 Y623.821073 Z-1.000000 I0.017334 J-0.353153",
		"G02 X447.964321 Y622.669065 Z-1.000000 I-2.796403 J0.074106",
		"G02 X446.337910 Y620.246090 Z-1.000000 I-9.571560 J4.667517",
		"G02 X444.269262 Y618.424561 Z-1.000000 I-8.574018 J7.651803",
		"G02 X443.203140 Y618.093750 Z-1.000000 I-1.066122 J1.552522",
		"G03 X441.994721 Y617.339446 Z-1.000000 I0.000000 J-1.345114",
		"G03 X437.091879 Y606.292545 Z-1.000000 I146.624830 J-71.686569",
		"G03 X433.013798 Y595.065370 Z-1.000000 I171.942019 J-68.809260",
		"G03 X432.369160 Y591.021480 Z-1.000000 I12.361587 J-4.043890",
		"G02 X432.154518 Y588.566203 Z-1.000000 I-14.150218 J-0.000000",
		"G02 X431.614070 Y587.481423 Z-1.000000 I-2.102397 J0.370417",
		"G02 X430.566823 Y586.811889 Z-1.000000 I-1.824202 J1.699526",
		"G02 X427.785180 Y586.101560 Z-1.000000 I-6.692883 J20.407665",
		"G02 X417.050807 Y585.458571 Z-1.000000 I-7.961294 J42.985843",
		"G02 X408.146500 Y587.349610 Z-1.000000 I1.987364 J31.267186",
		"G02 X407.867197 Y588.225017 Z-1.000000 I0.202807 J0.546967",
		"G02 X412.423850 Y593.626950 Z-1.000000 I28.488766 J-19.408133",
		"G03 X419.060968 Y601.448903 Z-1.000000 I-35.752497 J37.063720",
		"G03 X419.371110 Y603.699220 Z-1.000000 I-2.077005 J1.432787",
		"G02 X419.653610 Y605.046220 Z-1.000000 I1.273449 J0.436049",
		"G02 X425.289080 Y610.136720 Z-1.000000 I30.165637 J-27.730419",
		"G03 X431.491614 Y615.273099 Z-1.000000 I-48.413133 J64.775404",
		"G03 X440.875020 Y624.408200 Z-1.000000 I-144.868622 J158.193133",
		"G02 X447.820106 Y631.323910 Z-1.000000 I183.183927 J-177.016899",
		"G02 X448.316430 Y631.439450 Z-1.000000 I0.345567 J-0.360653",
		"G01 X448.316440 Y631.439450 Z-1.000000",
		"G00 X472.798860 Y572.585940",
		"G03 X468.905466 Y570.998131 Z-1.000000 I0.635706 J-7.126097 F400.000000",
		"G03 X468.097852 Y569.222961 Z-1.000000 I1.395148 J-1.706019",
		"G03 X468.976705 Y567.337540 Z-1.000000 I2.639616 J0.082865",
		"G03 X473.027370 Y565.304690 Z-1.000000 I5.775528 J6.456231",
		"G02 X476.153142 Y564.491218 Z-1.000000 I-5.942715 J-29.247054",
		"G02 X479.039090 Y563.386720 Z-1.000000 I-8.272576 J-25.938059",
		"G03 X482.329213 Y562.208548 Z-1.000000 I9.138593 J20.337089",
		"G03 X483.645912 Y562.178140 Z-1.000000 I0.719859 J2.648204",
		"G03 X484.507184 Y562.860670 Z-1.000000 I-0.274225 J1.230715",
		"G03 X484.927760 Y564.699220 Z-1.000000 I-3.808327 J1.838550",
		"G03 X484.007282 Y567.358081 Z-1.000000 I-4.300385 J-0.000000",
		"G03 X480.823885 Y570.267125 Z-1.000000 I-10.389739 J-8.173265",
		"G03 X476.824061 Y572.151511 Z-1.000000 I-8.870713 J-13.641868",
		"G03 X472.798860 Y572.585940 Z-1.000000 I-3.103844 J-9.893693",
		"G01 X472.798860 Y572.585940 Z-1.000000",
		"G00 X517.058620 Y580.763670",
		"G03 X514.914475 Y579.661466 Z-1.000000 I1.248141 J-5.064673 F400.000000",
		"G03 X510.744170 Y575.636720 Z-1.000000 I31.633185 J-36.950164",
		"G03 X507.032835 Y571.340009 Z-1.000000 I67.175619 J-61.774944",
		"G03 X506.080665 Y569.691190 Z-1.000000 I4.710982 J-3.819870",
		"G03 X506.087859 Y568.086314 Z-1.000000 I2.068996 J-0.793179",
		"G03 X507.173860 Y566.322270 Z-1.000000 I5.480157 J2.157446",
		"G03 X508.448294 Y565.081637 Z-1.000000 I9.458801 J8.441611",
		"G03 X509.237132 Y564.884890 Z-1.000000 I0.594313 J0.703083",
		"G03 X510.108906 Y565.289842 Z-1.000000 I-0.461321 J2.133968",
		"G03 X512.966830 Y567.574220 Z-1.000000 I-63.804137 J82.753577",
		"G02 X516.772750 Y569.631590 Z-1.000000 I7.076602 J-8.542017",
		"G02 X522.697300 Y570.576170 Z-1.000000 I6.284877 J-20.367569",
		"G03 X528.020248 Y571.589405 Z-1.000000 I-0.268758 J15.900358",
		"G03 X528.973060 Y572.871366 Z-1.000000 I-0.518504 J1.380443",
		"G03 X528.338313 Y574.604361 Z-1.000000 I-2.268533 J0.151843",
		"G03 X523.242220 Y578.716800 Z-1.000000 I-19.210119 J-18.591253",
		"G03 X519.354144 Y580.629672 Z-1.000000 I-11.575868 J-18.621083",
		"G03 X517.058620 Y580.763670 Z-1.000000 I-1.364404 J-3.644299",
		"G01 X517.058620 Y580.763670 Z-1.000000",
		"G00 X593.808620 Y464.185550",
		"G02 X594.734111 Y463.886499 Z-1.000000 I-0.207918 J-2.225066 F400.000000",
		"G02 X595.953150 Y462.962890 Z-1.000000 I-3.337187 J-5.670924",
		"G03 X601.229919 Y460.240997 Z-1.000000 I7.221584 J7.524225",
		"G03 X605.363310 Y461.177730 Z-1.000000 I0.990373 J5.217706",
		"G02 X608.351327 Y462.527998 Z-1.000000 I4.679154 J-6.373296",
		"G02 X616.451200 Y463.605470 Z-1.000000 I10.501665 J-47.961990",
		"G01 X624.789090 Y464.013670 Z-1.000000",
		"G01 X628.015650 Y457.166020 Z-1.000000",
		"G02 X630.738651 Y449.682282 Z-1.000000 I-39.450266 J-18.591466",
		"G02 X632.001207 Y442.559861 Z-1.000000 I-42.652051 J-11.233831",
		"G02 X631.815053 Y435.446655 Z-1.000000 I-29.709890 J-2.781525",
		"G02 X630.328150 Y432.460940 Z-1.000000 I-5.303640 J0.778143",
		"G02 X627.892696 Y430.820432 Z-1.000000 I-5.272492 J5.199342",
		"G02 X626.394560 Y430.830080 Z-1.000000 I-0.737646 J1.778401",
		"G03 X624.774276 Y430.920565 Z-1.000000 I-0.933963 J-2.171979",
		"G03 X618.097690 Y428.312500 Z-1.000000 I13.373451 J-44.085707",
		"G02 X611.342928 Y425.470324 Z-1.000000 I-30.676928 J63.459473",
		"G02 X608.990260 Y425.027340 Z-1.000000 I-2.547437 J7.060366",
		"G03 X606.651234 Y424.593351 Z-1.000000 I0.196761 J-7.580674",
		"G03 X603.044950 Y422.917970 Z-1.000000 I7.087533 J-19.975002",
		"G02 X599.050013 Y421.165138 Z-1.000000 I-9.558551 J16.356298",
		"G02 X596.843562 Y421.094519 Z-1.000000 I-1.232783 J4.012613",
		"G02 X595.002594 Y422.192828 Z-1.000000 I0.831013 J3.484987",
		"G02 X592.951200 Y425.324220 Z-1.000000 I9.397172 J8.393781",
		"G03 X590.734561 Y429.679271 Z-1.000000 I-55.581898 J-25.548502",
		"G03 X588.156280 Y433.882810 Z-1.000000 I-54.571533 J-30.579497",
		"G02 X586.380952 Y438.246409 Z-1.000000 I9.701253 J6.489895",
		"G02 X585.998080 Y445.525390 Z-1.000000 I27.507946 J5.096469",
		"G02 X587.703827 Y452.544860 Z-1.000000 I22.558899 J-1.764877",
		"G02 X591.066440 Y457.556640 Z-1.000000 I14.553610 J-6.130678",
		"G03 X594.161623 Y460.755225 Z-1.000000 I-110.589743 J110.111533",
		"G03 X594.501205 Y461.338755 Z-1.000000 I-1.089501 J1.024602",
		"G03 X594.288392 Y461.590011 Z-1.000000 I-0.186697 J0.057622",
		"G03 X592.183620 Y461.173830 Z-1.000000 I2.346321 J-17.396530",
		"G02 X589.358310 Y460.798274 Z-1.000000 I-2.916602 J11.126433",
		"G02 X588.941920 Y461.086481 Z-1.000000 I-0.003491 J0.439851",
		"G02 X589.057181 Y461.600422 Z-1.000000 I0.456261 J0.167570",
		"G02 X591.369170 Y463.390620 Z-1.000000 I8.661104 J-8.797531",
		"G02 X592.836405 Y464.070863 Z-1.000000 I3.927897 J-6.549713",
		"G02 X593.808620 Y464.185550 Z-1.000000 I0.754326 J-2.216374",
		"G01 X593.808620 Y464.185550 Z-1.000000",
		"G00 X663.994170 Y464.683590",
		"G01 X657.882840 Y458.556640 Z-1.000000 F400.000000",
		"G02 X651.643869 Y452.720921 Z-1.000000 I-87.252578 J87.028882",
		"G02 X650.826200 Y452.400390 Z-1.000000 I-0.856550 J0.981849",
		"G03 X650.012139 Y452.077872 Z-1.000000 I0.038369 J-1.285484",
		"G03 X644.410190 Y446.720700 Z-1.000000 I66.772627 J-75.431101",
		"G02 X639.645415 Y442.816639 Z-1.000000 I-19.264041 J18.651457",
		"G02 X638.937530 Y443.203120 Z-1.000000 I-0.248440 J0.386481",
		"G02 X639.559265 Y445.030823 Z-1.000000 I2.997300 J0.000000",
		"G02 X642.910190 Y448.367190 Z-1.000000 I14.260156 J-10.971422",
		"G03 X646.673140 Y451.612575 Z-1.000000 I-25.010439 J32.803258",
		"G03 X650.287140 Y455.548830 Z-1.000000 I-33.236497 J34.142664",
		"G02 X654.064520 Y459.228783 Z-1.000000 I18.270572 J-14.975616",
		"G02 X658.841830 Y462.193360 Z-1.000000 I16.441662 J-21.163630",
		"G01 X663.994170 Y464.683590 Z-1.000000",
		"G00 X715.437530 Y443.394530",
		"G02 X716.730669 Y443.255890 Z-1.000000 I0.506076 J-1.379744 F400.000000",
		"G02 X720.064480 Y440.937500 Z-1.000000 I-25.603570 J-40.373754",
		"G03 X727.999675 Y437.503073 Z-1.000000 I10.934873 J14.380576",
		"G03 X748.464870 Y436.154300 Z-1.000000 I16.886704 J100.289679",
		"G03 X761.541970 Y437.445965 Z-1.000000 I-3.657101 J103.868875",
		"G03 X765.898470 Y438.998050 Z-1.000000 I-2.073505 J12.710160",
		"G02 X769.351259 Y440.617293 Z-1.000000 I11.309928 J-19.625806",
		"G02 X770.692165 Y440.602115 Z-1.000000 I0.650626 J-1.759239",
		"G02 X771.635558 Y439.687612 Z-1.000000 I-0.631334 J-1.595128",
		"G02 X772.734410 Y436.292970 Z-1.000000 I-17.280107 J-7.468774",
		"G03 X773.741942 Y432.562553 Z-1.000000 I41.302387 J9.153912",
		"G03 X777.173860 Y422.408200 Z-1.000000 I310.424675 J99.258669",
		"G02 X780.009611 Y406.082725 Z-1.000000 I-46.029761 J-16.404438",
		"G02 X777.207060 Y402.185550 Z-1.000000 I-4.101727 J-0.006628",
		"G03 X770.128586 Y398.034027 Z-1.000000 I6.288888 J-18.833017",
		"G03 X763.470730 Y389.677734 Z-1.000000 I22.650311 J-24.877042",
		"G03 X759.241602 Y381.757985 Z-1.000000 I171.400368 J-96.616451",
		"G03 X758.343780 Y379.527344 Z-1.000000 I12.640679 J-6.383817",
		"G02 X757.077465 Y377.759836 Z-1.000000 I-3.263847 J1.000978",
		"G02 X752.984410 Y375.660156 Z-1.000000 I-8.944134 J12.396157",
		"G02 X747.809008 Y374.594322 Z-1.000000 I-7.013069 J20.955470",
		"G02 X731.370109 Y373.647036 Z-1.000000 I-26.648226 J319.333453",
		"G02 X715.078821 Y373.519990 Z-1.000000 I-10.784531 J338.325700",
		"G02 X707.828150 Y374.220703 Z-1.000000 I0.740774 J45.528945",
		"G02 X691.930685 Y378.778163 Z-1.000000 I14.144904 J79.346402",
		"G02 X676.656280 Y386.744141 Z-1.000000 I32.564998 J81.068899",
		"G02 X662.116746 Y397.668379 Z-1.000000 I76.784202 J117.333126",
		"G02 X660.937530 Y400.234380 Z-1.000000 I2.202229 J2.566001",
		"G02 X661.326527 Y401.040550 Z-1.000000 I1.029866 J0.000000",
		"G02 X665.521510 Y403.878910 Z-1.000000 I20.539108 J-25.836801",
		"G03 X669.659163 Y406.474275 Z-1.000000 I-28.672996 J50.307750",
		"G03 X678.665002 Y412.832603 Z-1.000000 I-252.905720 J367.768866",
		"G03 X687.669467 Y419.546183 Z-1.000000 I-242.999195 J335.313456",
		"G03 X689.804720 Y421.476560 Z-1.000000 I-10.849090 J14.146659",
		"G02 X697.292244 Y428.341135 Z-1.000000 I50.144581 J-47.179331",
		"G02 X699.449250 Y429.089840 Z-1.000000 I2.157006 J-2.732796",
		"G03 X700.911520 Y429.466170 Z-1.000000 I-0.000000 J3.029064",
		"G03 X701.312530 Y430.117190 Z-1.000000 I-0.374758 J0.679856",
		"G02 X701.602328 Y431.052465 Z-1.000000 I1.882048 J-0.070623",
		"G02 X704.480500 Y435.132810 Z-1.000000 I41.495728 J-26.214769",
		"G02 X707.515970 Y437.563746 Z-1.000000 I6.701466 J-5.257372",
		"G02 X708.783230 Y437.185550 Z-1.000000 I0.424425 J-0.890103",
		"G03 X709.565456 Y436.209274 Z-1.000000 I4.134575 J2.511252",
		"G03 X710.011666 Y436.083732 Z-1.000000 I0.340399 J0.354125",
		"G03 X710.455781 Y436.359790 Z-1.000000 I-0.169356 J0.767726",
		"G03 X711.421910 Y437.767580 Z-1.000000 I-8.989516 J7.204673",
		"G02 X714.325539 Y442.504638 Z-1.000000 I106.068853 J-61.757572",
		"G02 X715.437530 Y443.394530 Z-1.000000 I1.883705 J-1.214133",
		"G01 X715.437530 Y443.394530 Z-1.000000",
		"G00 X735.351590 Y450.828120",
		"G03 X734.322601 Y450.773914 Z-1.000000 I-0.105501 J-7.790922 F400.000000",
		"G03 X733.382840 Y450.582030 Z-1.000000 I0.688823 J-5.770761",
		"G03 X733.358723 Y450.241583 Z-1.000000 I0.051009 J-0.174691",
		"G03 X734.886750 Y449.900390 Z-1.000000 I1.581003 J3.488274",
		"G03 X737.024671 Y450.190755 Z-1.000000 I0.100638 J7.274810",
		"G03 X737.048860 Y450.531250 Z-1.000000 I-0.050966 J0.174727",
		"G03 X736.378503 Y450.735556 Z-1.000000 I-1.117268 J-2.463993",
		"G03 X735.351590 Y450.828120 Z-1.000000 I-1.120826 J-6.691935",
		"G01 X735.351590 Y450.828120 Z-1.000000",
		"G00 X736.574250 Y448.089840",
		"G03 X735.545254 Y448.035632 Z-1.000000 I-0.105500 J-7.790931 F400.000000",
		"G03 X734.605500 Y447.843750 Z-1.000000 I0.688828 J-5.770751",
		"G03 X734.581380 Y447.503294 Z-1.000000 I0.051006 J-0.174696",
		"G03 X736.109410 Y447.162110 Z-1.000000 I1.581007 J3.488401",
		"G03 X738.247321 Y447.452476 Z-1.000000 I0.100637 J7.274735",
		"G03 X738.271510 Y447.792970 Z-1.000000 I-0.050966 J0.174727",
		"G03 X737.601153 Y447.997272 Z-1.000000 I-1.117342 J-2.464287",
		"G03 X736.574250 Y448.089840 Z-1.000000 I-1.120811 J-6.691468",
		"G01 X736.574250 Y448.089840 Z-1.000000",
		"G00 X739.355500 Y462.447270",
		"G03 X738.404863 Y462.133565 Z-1.000000 I1.323656 J-5.608381 F400.000000",
		"G03 X736.875030 Y461.365230 Z-1.000000 I5.660153 J-13.177135",
		"G02 X733.419402 Y459.707055 Z-1.000000 I-12.366133 J21.341112",
		"G02 X732.902370 Y459.826170 Z-1.000000 I-0.170719 J0.440650",
		"G03 X732.481370 Y459.828716 Z-1.000000 I-0.211696 J-0.196542",
		"G03 X730.246120 Y457.033200 Z-1.000000 I15.114095 J-14.376384",
		"G03 X728.812932 Y454.755693 Z-1.000000 I41.574000 J-27.751352",
		"G03 X728.666302 Y454.196995 Z-1.000000 I0.905183 J-0.536154",
		"G03 X728.938075 Y453.874573 Z-1.000000 I0.335678 J0.007195",
		"G03 X730.316420 Y453.742190 Z-1.000000 I1.378345 J7.109350",
		"G03 X732.235745 Y454.291219 Z-1.000000 I0.000000 J3.629353",
		"G03 X736.256262 Y457.208591 Z-1.000000 I-18.649240 J29.930158",
		"G03 X739.810019 Y460.670998 Z-1.000000 I-21.519812 J25.642536",
		"G03 X740.386730 Y462.193360 Z-1.000000 I-1.720957 J1.522362",
		"G03 X740.177847 Y462.453323 Z-1.000000 I-0.266208 J0.000000",
		"G03 X739.355480 Y462.447270 Z-1.000000 I-0.397924 J-1.804525",
		"G01 X739.355500 Y462.447270 Z-1.000000",
		"G00 X755.857450 Y481.712890",
		"G02 X757.604945 Y480.942328 Z-1.000000 I-0.771961 J-4.117448 F400.000000",
		"G02 X758.984410 Y479.365230 Z-1.000000 I-3.067567 J-4.075005",
		"G02 X761.197418 Y474.842498 Z-1.000000 I-38.721977 J-21.749757",
		"G02 X764.824006 Y465.734094 Z-1.000000 I-232.633012 J-97.900970",
		"G02 X768.153047 Y456.228484 Z-1.000000 I-208.365196 J-78.309115",
		"G02 X768.490260 Y454.072270 Z-1.000000 I-6.549988 J-2.128835",
		"G02 X767.738221 Y452.096384 Z-1.000000 I-3.001241 J0.011238",
		"G02 X763.664090 Y448.607420 Z-1.000000 I-14.506078 J12.815828",
		"G02 X758.523974 Y446.643284 Z-1.000000 I-7.312683 J11.429316",
		"G02 X745.035190 Y445.585940 Z-1.000000 I-13.131497 J80.953111",
		"G02 X725.963290 Y447.406556 Z-1.000000 I0.459674 J105.619652",
		"G02 X721.263700 Y450.060550 Z-1.000000 I1.548659 J8.230230",
		"G02 X721.098772 Y451.373893 Z-1.000000 I0.869758 J0.776250",
		"G02 X725.311632 Y458.357175 Z-1.000000 I56.566812 J-29.363110",
		"G02 X730.219808 Y464.815274 Z-1.000000 I77.466462 J-53.780590",
		"G02 X734.753940 Y469.492190 Z-1.000000 I32.574528 J-27.043731",
		"G02 X750.391501 Y480.772826 Z-1.000000 I64.630513 J-73.113829",
		"G02 X755.857450 Y481.712890 Z-1.000000 I3.998366 J-6.887521",
		"G01 X755.857450 Y481.712890 Z-1.000000",
		"G00 X823.402370 Y485.074220",
		"G03 X822.755932 Y485.028975 Z-1.000000 I-0.129079 J-2.796404 F400.000000",
		"G03 X822.052760 Y484.808590 Z-1.000000 I0.579198 J-3.079999",
		"G03 X821.960818 Y484.377056 Z-1.000000 I0.111815 J-0.249385",
		"G03 X822.791050 Y483.982420 Z-1.000000 I0.884943 J0.791099",
		"G03 X824.302332 Y484.269296 Z-1.000000 I0.153182 J3.317232",
		"G03 X824.394560 Y484.701170 Z-1.000000 I-0.111898 J0.249681",
		"G03 X824.024737 Y484.950819 Z-1.000000 I-0.677031 J-0.604189",
		"G03 X823.402370 Y485.074220 Z-1.000000 I-0.720437 J-2.002341",
		"G01 X823.402370 Y485.074220 Z-1.000000",
		"G00 X829.293000 Y482.503910",
		"G03 X829.008485 Y482.118340 Z-1.000000 I0.000000 J-0.297758 F400.000000",
		"G03 X830.363310 Y479.037110 Z-1.000000 I14.027030 J4.329249",
		"G03 X832.450892 Y476.164316 Z-1.000000 I15.469289 J9.046225",
		"G03 X832.921910 Y476.164060 Z-1.000000 I0.235623 J0.209869",
		"G03 X833.073522 Y476.959834 Z-1.000000 I-0.587499 J0.524261",
		"G03 X831.825576 Y479.638650 Z-1.000000 I-16.547964 J-6.078902",
		"G03 X830.141324 Y482.099242 Z-1.000000 I-16.118291 J-9.226097",
		"G03 X829.293000 Y482.503910 Z-1.000000 I-0.848324 J-0.686856",
		"G01 X829.293000 Y482.503910 Z-1.000000",
		"G00 X828.156280 Y477.746090",
		"G03 X827.628068 Y477.746877 Z-1.000000 I-0.264458 J-0.235998 F400.000000",
		"G03 X826.638700 Y476.056640 Z-1.000000 I3.632082 J-3.260692",
		"G03 X826.230305 Y474.629870 Z-1.000000 I14.405740 J-4.895298",
		"G03 X826.279100 Y474.438393 Z-1.000000 I0.208511 J-0.048820",
		"G03 X826.431335 Y474.427650 Z-1.000000 I0.081642 J0.072922",
		"G03 X827.322300 Y475.291020 Z-1.000000 I-5.818462 J6.895836",
		"G03 X828.311946 Y476.980644 Z-1.000000 I-3.619403 J3.254596",
		"G03 X828.156280 Y477.746090 Z-1.000000 I-0.734102 J0.249259",
		"G01 X828.156280 Y477.746090 Z-1.000000",
		"G00 X830.050810 Y467.574220",
		"G03 X829.943816 Y467.096663 Z-1.000000 I1.829885 J-0.660742 F400.000000",
		"G03 X829.853550 Y465.384770 Z-1.000000 I20.510077 J-1.939791",
		"G03 X830.115409 Y462.978798 Z-1.000000 I10.155863 J-0.111903",
		"G03 X830.416050 Y462.960940 Z-1.000000 I0.152958 J0.035465",
		"G03 X830.724452 Y464.664831 Z-1.000000 I-4.895004 J1.765844",
		"G03 X830.462950 Y467.070310 Z-1.000000 I-10.089394 J0.120127",
		"G03 X830.194680 Y467.600528 Z-1.000000 I-1.217884 J-0.283227",
		"G03 X830.050840 Y467.574220 Z-1.000000 I-0.064259 J-0.055044",
		"G01 X830.050810 Y467.574220 Z-1.000000",
		"G00 X847.949250 Y440.732420",
		"G03 X847.027436 Y439.727616 Z-1.000000 I1.756701 J-2.536849 F400.000000",
		"G03 X846.726590 Y438.595700 Z-1.000000 I1.978958 J-1.131916",
		"G03 X847.094117 Y437.674159 Z-1.000000 I1.339106 J-0.000000",
		"G03 X847.949250 Y437.306640 Z-1.000000 I0.855133 J0.811092",
		"G03 X848.723242 Y437.755550 Z-1.000000 I0.000000 J0.891698",
		"G03 X849.171910 Y439.443360 Z-1.000000 I-2.950290 J1.687810",
		"G03 X848.688251 Y440.656011 Z-1.000000 I-1.762072 J-0.000015",
		"G03 X847.949250 Y440.732420 Z-1.000000 I-0.414061 J-0.392766",
		"G01 X847.949250 Y440.732420 Z-1.000000",
		"G00 X809.986360 Y386.632812",
		"G03 X809.385358 Y386.223579 Z-1.000000 I0.000000 J-0.645934 F400.000000",
		"G03 X809.447300 Y385.263672 Z-1.000000 I1.051442 J-0.414104",
		"G03 X810.312385 Y384.077932 Z-1.000000 I5.103996 J2.815302",
		"G03 X810.742220 Y383.892578 Z-1.000000 I0.429835 J0.405715",
		"G03 X811.063053 Y384.111249 Z-1.000000 I0.000000 J0.344698",
		"G03 X811.281280 Y385.263672 Z-1.000000 I-2.933774 J1.152423",
		"G03 X810.891253 Y386.243285 Z-1.000000 I-1.425237 J0.000000",
		"G03 X809.986360 Y386.632812 Z-1.000000 I-0.904893 J-0.856295",
		"G01 X809.986360 Y386.632812 Z-1.000000",
		"G00 X815.630890 Y345.544922",
		"G02 X815.951517 Y345.326593 Z-1.000000 I0.000000 J-0.344592 F400.000000",
		"G02 X816.169950 Y344.175781 Z-1.000000 I-2.922300 J-1.150812",
		"G02 X815.779921 Y343.196166 Z-1.000000 I-1.425237 J-0.000000",
		"G02 X814.875030 Y342.806641 Z-1.000000 I-0.904891 J0.856296",
		"G02 X814.274004 Y343.215916 Z-1.000000 I0.000000 J0.645945",
		"G02 X814.335970 Y344.175781 Z-1.000000 I1.051489 J0.414052",
		"G02 X815.201027 Y345.359883 Z-1.000000 I5.073736 J-2.798629",
		"G02 X815.630890 Y345.544922 Z-1.000000 I0.429863 J-0.406785",
		"G01 X815.630890 Y345.544922 Z-1.000000",
		"G00 X845.832030 Y321.740234",
		"G03 X842.528232 Y321.026955 Z-1.000000 I-0.275422 J-6.732269 F400.000000",
		"G03 X841.835357 Y319.978787 Z-1.000000 I0.567729 J-1.128379",
		"G03 X842.326853 Y318.666384 Z-1.000000 I1.707795 J-0.108664",
		"G03 X845.503910 Y316.578125 Z-1.000000 I6.508900 J6.441677",
		"G02 X848.661876 Y314.797676 Z-1.000000 I-4.693419 J-12.015526",
		"G02 X849.171880 Y313.763672 Z-1.000000 I-0.793190 J-1.034004",
		"G02 X848.670626 Y313.140802 Z-1.000000 I-0.637623 J-0.000000",
		"G02 X847.033200 Y313.294922 Z-1.000000 I-0.577635 J2.638359",
		"G03 X845.793247 Y313.737635 Z-1.000000 I-4.288536 J-10.053579",
		"G03 X845.624999 Y313.681582 Z-1.000000 I-0.043530 J-0.149874",
		"G03 X845.621213 Y313.507163 Z-1.000000 I0.119429 J-0.089843",
		"G03 X846.421880 Y312.482422 Z-1.000000 I8.693765 J5.967582",
		"G02 X847.398289 Y310.548463 Z-1.000000 I-3.185116 J-2.821550",
		"G02 X847.949220 Y305.324219 Z-1.000000 I-24.494184 J-5.224244",
		"G03 X848.287499 Y300.848369 Z-1.000000 I29.779645 J0.000000",
		"G03 X848.544640 Y300.836544 Z-1.000000 I0.129749 J0.019725",
		"G03 X848.894340 Y302.903860 Z-1.000000 I-13.542729 J3.354073",
		"G03 X849.443360 Y311.357422 Z-1.000000 I-153.360149 J14.204654",
		"G01 X849.824220 Y321.578125 Z-1.000000",
		"G01 X845.832030 Y321.740234 Z-1.000000",
		"G00 X845.836030 Y320.740234",
		"G02 X845.988461 Y320.751064 Z-1.000000 I0.081787 J-0.072999 F400.000000",
		"G02 X846.879000 Y319.888672 Z-1.000000 I-5.785795 J-6.865629",
		"G02 X847.868924 Y318.197713 Z-1.000000 I-3.619212 J-3.254006",
		"G02 X847.712980 Y317.431641 Z-1.000000 I-0.735068 J-0.249276",
		"G02 X847.184767 Y317.430854 Z-1.000000 I-0.264459 J0.235997",
		"G02 X846.195400 Y319.121094 Z-1.000000 I3.632139 J3.260718",
		"G02 X845.787121 Y320.548288 Z-1.000000 I14.364198 J4.881181",
		"G02 X845.836030 Y320.740234 Z-1.000000 I0.209170 J0.048906",
		"G01 X845.836030 Y320.740234 Z-1.000000",
		"G00 X880.164090 Y342.941406",
		"G03 X880.057096 Y342.463856 Z-1.000000 I1.829807 J-0.660727 F400.000000",
		"G03 X879.966830 Y340.751953 Z-1.000000 I20.510309 J-1.939803",
		"G03 X880.230024 Y338.345855 Z-1.000000 I10.111244 J-0.111411",
		"G03 X880.531280 Y338.328125 Z-1.000000 I0.153251 J0.035709",
		"G03 X880.838859 Y340.032011 Z-1.000000 I-4.881110 J1.760824",
		"G03 X880.576180 Y342.437500 Z-1.000000 I-10.127755 J0.111137",
		"G03 X880.307912 Y342.967714 Z-1.000000 I-1.217882 J-0.283228",
		"G03 X880.164070 Y342.941406 Z-1.000000 I-0.064259 J-0.055045",
		"G01 X880.164090 Y342.941406 Z-1.000000",
		"G00 X899.109410 Y357.867188",
		"G02 X899.488996 Y357.730061 Z-1.000000 I0.004141 J-0.582470 F400.000000",
		"G02 X903.255890 Y354.371094 Z-1.000000 I-58.575953 J-69.481342",
		"G03 X912.047339 Y348.210940 Z-1.000000 I25.519163 J27.066187",
		"G03 X931.064480 Y340.371094 Z-1.000000 I61.413268 J121.985477",
		"G01 X943.900420 Y336.179688 Z-1.000000",
		"G01 X971.400420 Y336.615234 Z-1.000000",
		"G02 X994.460293 Y336.483133 Z-1.000000 I8.467344 J-534.677998",
		"G02 X1007.665866 Y335.500969 Z-1.000000 I-3.850072 J-141.033811",
		"G02 X1020.396331 Y333.045369 Z-1.000000 I-11.306449 J-92.842391",
		"G02 X1031.841830 Y329.253906 Z-1.000000 I-25.852058 J-97.212334",
		"G02 X1040.225765 Y325.680942 Z-1.000000 I-73.455938 J-183.986750",
		"G02 X1043.448192 Y323.865091 Z-1.000000 I-8.763717 J-19.319377",
		"G02 X1045.850396 Y321.463508 Z-1.000000 I-5.221561 J-7.625115",
		"G02 X1047.220730 Y318.724609 Z-1.000000 I-9.263366 J-6.346929",
		"G03 X1048.753207 Y316.315581 Z-1.000000 I5.700054 J1.934078",
		"G03 X1051.169930 Y314.933594 Z-1.000000 I3.968354 J4.135492",
		"G02 X1055.964416 Y312.844808 Z-1.000000 I-5.194177 J-18.469325",
		"G02 X1067.134530 Y305.489380 Z-1.000000 I-100.249106 J-164.399804",
		"G02 X1077.829783 Y297.299148 Z-1.000000 I-106.896285 J-150.669341",
		"G02 X1080.414130 Y294.185547 Z-1.000000 I-6.889816 J-8.348006",
		"G02 X1082.339078 Y289.936345 Z-1.000000 I-25.744450 J-14.223214",
		"G02 X1085.203130 Y280.898438 Z-1.000000 I-114.112544 J-41.134247",
		"G02 X1087.453736 Y270.481471 Z-1.000000 I-93.456424 J-25.643050",
		"G02 X1086.734930 Y269.488420 Z-1.000000 I-0.857008 J-0.136342",
		"G02 X1085.211658 Y269.985620 Z-1.000000 I-0.278755 J1.728000",
		"G02 X1076.494130 Y279.494141 Z-1.000000 I121.254091 J119.917663",
		"G03 X1064.455961 Y291.460587 Z-1.000000 I-76.923752 J-65.346426",
		"G03 X1053.220217 Y298.692742 Z-1.000000 I-34.093958 J-40.623844",
		"G03 X1040.496545 Y303.032263 Z-1.000000 I-24.835244 J-51.995114",
		"G03 X1021.515630 Y305.753906 Z-1.000000 I-29.080896 J-135.264167",
		"G02 X1006.327584 Y307.300023 Z-1.000000 I19.634585 J268.249281",
		"G02 X1002.474630 Y308.240234 Z-1.000000 I2.349027 J17.990970",
		"G03 X989.761296 Y310.841670 Z-1.000000 I-15.140408 J-41.625820",
		"G03 X969.568390 Y310.044922 Z-1.000000 I-5.882589 J-107.195224",
		"G03 X926.661460 Y303.649895 Z-1.000000 I202.770824 J-1507.612674",
		"G03 X918.556670 Y301.697266 Z-1.000000 I9.080790 J-55.488308",
		"G02 X913.918155 Y300.790252 Z-1.000000 I-6.160487 J19.190732",
		"G02 X904.500030 Y300.632812 Z-1.000000 I-6.044711 J79.820805",
		"G02 X898.014767 Y301.024187 Z-1.000000 I7.554426 J179.108056",
		"G02 X896.301810 Y301.434984 Z-1.000000 I0.441235 J5.616655",
		"G02 X895.417503 Y302.472138 Z-1.000000 I0.572619 J1.383800",
		"G02 X895.257840 Y304.457031 Z-1.000000 I6.342398 J1.509046",
		"G02 X895.546513 Y307.877842 Z-1.000000 I154.348375 J-11.302463",
		"G02 X897.025420 Y322.947266 Z-1.000000 I2958.936065 J-282.781448",
		"G03 X898.262674 Y338.027988 Z-1.000000 I-406.600386 J40.949467",
		"G03 X898.609410 Y347.939453 Z-1.000000 I-174.430340 J11.063947",
		"G02 X898.885854 Y357.653355 Z-1.000000 I222.318018 J-1.465967",
		"G02 X899.109410 Y357.867188 Z-1.000000 I0.224875 J-0.011323",
		"G01 X899.109410 Y357.867188 Z-1.000000",
		"G00 X950.550870 Y297.609375",
		"G02 X951.454617 Y297.219654 Z-1.000000 I0.000000 J-1.242737 F400.000000",
		"G02 X951.843840 Y296.240234 Z-1.000000 I-1.037668 J-0.979420",
		"G02 X951.625407 Y295.089423 Z-1.000000 I-3.140733 J0.000000",
		"G02 X951.304780 Y294.871094 Z-1.000000 I-0.320627 J0.126264",
		"G02 X950.875587 Y295.056192 Z-1.000000 I-0.000000 J0.590142",
		"G02 X950.011810 Y296.240234 Z-1.000000 I4.234447 J3.996182",
		"G02 X949.949844 Y297.200100 Z-1.000000 I0.989525 J0.545813",
		"G02 X950.550870 Y297.609375 Z-1.000000 I0.601026 J-0.236670",
		"G01 X950.550870 Y297.609375 Z-1.000000",
		"G00 X765.443360 Y237.855469",
		"G02 X768.719059 Y236.856094 Z-1.000000 I-2.620429 J-14.457245 F400.000000",
		"G02 X775.833980 Y233.404297 Z-1.000000 I-40.939159 J-93.443332",
		"G02 X781.780250 Y229.004474 Z-1.000000 I-11.929563 J-22.340603",
		"G02 X786.998050 Y222.281250 Z-1.000000 I-23.671304 J-23.757308",
		"G02 X789.577110 Y217.415446 Z-1.000000 I-52.282214 J-30.827953",
		"G02 X791.049028 Y212.941224 Z-1.000000 I-17.186354 J-8.133145",
		"G02 X791.742021 Y207.839036 Z-1.000000 I-43.102163 J-8.452417",
		"G02 X792.771480 Y192.835940 Z-1.000000 I-956.298180 J-73.154610",
		"G02 X793.653143 Y171.283871 Z-1.000000 I-544.087714 J-33.051878",
		"G02 X793.096705 Y163.419054 Z-1.000000 I-43.026749 J-0.907940",
		"G02 X790.465686 Y156.916594 Z-1.000000 I-15.711382 J2.573613",
		"G02 X786.144530 Y152.373050 Z-1.000000 I-16.822968 J11.672954",
		"G02 X774.483765 Y147.073539 Z-1.000000 I-17.137388 J22.229583",
		"G02 X767.937500 Y148.751950 Z-1.000000 I-1.524076 J7.661006",
		"G02 X766.411248 Y151.197683 Z-1.000000 I2.995406 J3.568367",
		"G02 X765.054690 Y160.367190 Z-1.000000 I46.693643 J11.593064",
		"G03 X764.570224 Y166.566358 Z-1.000000 I-112.462761 J-5.670464",
		"G03 X763.944808 Y169.324892 Z-1.000000 I-12.012503 J-1.273318",
		"G03 X762.648441 Y171.747592 Z-1.000000 I-8.607843 J-3.047799",
		"G03 X759.812500 Y175.173830 Z-1.000000 I-36.309300 J-27.166879",
		"G02 X756.511270 Y179.296818 Z-1.000000 I29.868998 J27.298891",
		"G02 X755.429165 Y181.672910 Z-1.000000 I5.896238 J4.119678",
		"G02 X755.409537 Y184.184185 Z-1.000000 I5.188027 J1.296263",
		"G02 X756.677730 Y187.716800 Z-1.000000 I15.864908 J-3.701484",
		"G02 X757.766650 Y189.584186 Z-1.000000 I14.297590 J-7.086110",
		"G02 X758.349925 Y190.000985 Z-1.000000 I0.854424 J-0.579171",
		"G02 X759.036839 Y189.907486 Z-1.000000 I0.233219 J-0.856647",
		"G02 X760.607420 Y188.787110 Z-1.000000 I-5.927209 J-9.969992",
		"G03 X763.231534 Y187.380613 Z-1.000000 I4.596753 J5.425036",
		"G03 X767.773440 Y186.716800 Z-1.000000 I4.696680 J16.265284",
		"G03 X772.022332 Y187.818613 Z-1.000000 I0.077022 J8.446331",
		"G03 X777.068360 Y191.998050 Z-1.000000 I-9.284943 J16.346013",
		"G03 X781.756904 Y200.188158 Z-1.000000 I-19.663613 J16.693780",
		"G03 X781.802730 Y205.554688 Z-1.000000 I-7.877320 J2.750727",
		"G03 X780.046287 Y208.127865 Z-1.000000 I-5.351159 J-1.766626",
		"G03 X778.568808 Y208.339883 Z-1.000000 I-0.911940 J-1.100960",
		"G03 X777.054071 Y207.092854 Z-1.000000 I1.394327 J-3.237127",
		"G03 X774.628910 Y202.464844 Z-1.000000 I18.424138 J-12.604001",
		"G02 X771.599449 Y197.098956 Z-1.000000 I-19.436582 J7.435334",
		"G02 X769.173830 Y195.470700 Z-1.000000 I-3.694050 J2.882185",
		"G02 X766.051450 Y195.110924 Z-1.000000 I-2.679961 J9.529577",
		"G02 X763.970908 Y195.940981 Z-1.000000 I0.152293 J3.404199",
		"G02 X762.386675 Y197.907871 Z-1.000000 I4.667302 J5.380737",
		"G02 X759.443360 Y203.910156 Z-1.000000 I66.817579 J36.487840",
		"G02 X757.099049 Y212.076871 Z-1.000000 I29.478598 J12.881866",
		"G02 X756.634770 Y222.867188 Z-1.000000 I51.408493 J7.617117",
		"G02 X758.254053 Y233.404975 Z-1.000000 I58.312880 J-3.567307",
		"G02 X759.972660 Y236.279297 Z-1.000000 I5.588166 J-1.390309",
		"G02 X762.360350 Y237.655468 Z-1.000000 I4.360153 J-4.805535",
		"G02 X765.443360 Y237.855469 Z-1.000000 I1.943027 J-6.089451",
		"G01 X765.443360 Y237.855469 Z-1.000000",
		"G00 X743.935550 Y228.585938",
		"G02 X746.927184 Y226.856588 Z-1.000000 I-0.433224 J-4.201758 F400.000000",
		"G02 X748.943360 Y220.619141 Z-1.000000 I-8.640311 J-6.237447",
		"G03 X749.310161 Y215.351902 Z-1.000000 I38.001966 J0.000000",
		"G03 X750.917970 Y206.505859 Z-1.000000 I109.311269 J15.298686",
		"G02 X752.128065 Y199.021135 Z-1.000000 I-61.444964 J-13.774314",
		"G02 X752.003497 Y194.644301 Z-1.000000 I-16.998639 J-1.706398",
		"G02 X750.639482 Y190.610624 Z-1.000000 I-12.492847 J1.977077",
		"G02 X747.751950 Y186.496090 Z-1.000000 I-18.990908 J10.257113",
		"G02 X741.910126 Y182.948455 Z-1.000000 I-8.728937 J7.790149",
		"G02 X737.832595 Y183.803849 Z-1.000000 I-1.167049 J4.583038",
		"G02 X734.760413 Y187.852952 Z-1.000000 I6.110776 J7.826469",
		"G02 X731.710940 Y199.019531 Z-1.000000 I43.395463 J17.850516",
		"G02 X730.754666 Y209.304518 Z-1.000000 I101.809201 J14.652934",
		"G02 X731.062500 Y214.287109 Z-1.000000 I23.980257 J1.019258",
		"G02 X737.552612 Y226.427768 Z-1.000000 I21.258716 J-3.559352",
		"G02 X743.935550 Y228.585938 Z-1.000000 I5.551893 J-5.902032",
		"G01 X743.935550 Y228.585938 Z-1.000000",
		"G00 X675.951170 Y234.419922",
		"G02 X676.442952 Y234.409231 Z-1.000000 I0.213971 J-1.473636 F400.000000",
		"G02 X677.744140 Y234.066406 Z-1.000000 I-1.795844 J-9.456842",
		"G03 X685.970298 Y231.944492 Z-1.000000 I18.209592 J53.587876",
		"G03 X702.439948 Y229.440708 Z-1.000000 I55.548569 J309.973422",
		"G03 X719.230539 Y227.831955 Z-1.000000 I36.225916 J289.664432",
		"G03 X724.986330 Y228.158203 Z-1.000000 I1.561059 J23.395271",
		"G01 X729.302730 Y228.941406 Z-1.000000",
		"G01 X726.900390 Y223.919922 Z-1.000000",
		"G03 X724.766660 Y218.837266 Z-1.000000 I51.061638 J-24.425196",
		"G03 X724.498050 Y217.312500 Z-1.000000 I4.193373 J-1.524766",
		"G02 X724.046063 Y216.024818 Z-1.000000 I-2.060253 J0.000000",
		"G02 X722.564450 Y214.896484 Z-1.000000 I-2.999260 J2.401402",
		"G02 X713.797821 Y214.088616 Z-1.000000 I-5.626387 J13.085341",
		"G02 X692.107420 Y221.781250 Z-1.000000 I21.454227 J94.918834",
		"G02 X685.299087 Y225.673522 Z-1.000000 I28.991033 J58.611582",
		"G02 X679.876733 Y229.589060 Z-1.000000 I39.759948 J60.773056",
		"G02 X675.663617 Y233.655685 Z-1.000000 I18.957443 J23.856092",
		"G02 X675.951170 Y234.419922 Z-1.000000 I0.354549 J0.302813",
		"G01 X675.951170 Y234.419922 Z-1.000000",
		"G00 X600.453120 Y219.029297",
		"G02 X605.748759 Y217.921737 Z-1.000000 I0.703688 J-9.849366 F400.000000",
		"G02 X617.126950 Y210.328125 Z-1.000000 I-30.397875 J-57.869155",
		"G03 X629.510783 Y200.259488 Z-1.000000 I477.694510 J574.886214",
		"G03 X633.977592 Y197.137119 Z-1.000000 I27.420510 J34.471077",
		"G03 X638.471809 Y195.323509 Z-1.000000 I7.706293 J12.621324",
		"G03 X642.410160 Y194.890620 Z-1.000000 I3.938351 J17.698784",
		"G02 X648.116308 Y194.136199 Z-1.000000 I0.000000 J-21.956751",
		"G02 X651.314450 Y192.494140 Z-1.000000 I-2.225859 J-8.270623",
		"G03 X654.306883 Y190.185642 Z-1.000000 I41.708581 J50.971776",
		"G03 X656.673830 Y188.601560 Z-1.000000 I21.692746 J29.853058",
		"G02 X659.301756 Y186.295448 Z-1.000000 I-5.440502 J-8.850094",
		"G02 X663.976235 Y179.834173 Z-1.000000 I-56.182240 J-45.567185",
		"G02 X667.925917 Y172.991895 Z-1.000000 I-69.802278 J-44.854235",
		"G02 X670.023440 Y167.826170 Z-1.000000 I-27.512878 J-14.180211",
		"G02 X671.048363 Y163.984754 Z-1.000000 I-54.636543 J-16.634935",
		"G02 X671.148985 Y162.307073 Z-1.000000 I-4.955951 J-1.139101",
		"G02 X670.638806 Y160.767818 Z-1.000000 I-3.797439 J0.404469",
		"G02 X669.017580 Y158.369140 Z-1.000000 I-20.654354 J12.212710",
		"G02 X667.156942 Y156.409640 Z-1.000000 I-10.241444 J7.861605",
		"G02 X665.113577 Y155.263367 Z-1.000000 I-3.920369 J4.594111",
		"G02 X662.651243 Y154.743575 Z-1.000000 I-3.628680 J11.097476",
		"G02 X656.560550 Y154.296880 Z-1.000000 I-11.360835 J113.158501",
		"G02 X646.955982 Y154.574399 Z-1.000000 I-2.961633 J63.841266",
		"G02 X637.103520 Y156.330080 Z-1.000000 I7.208406 J68.974522",
		"G03 X627.321066 Y158.795467 Z-1.000000 I-737.798663 J-2906.884124",
		"G03 X619.685550 Y160.689450 Z-1.000000 I-464.293182 J-1855.441045",
		"G02 X613.949620 Y162.638447 Z-1.000000 I8.375118 J34.063083",
		"G02 X612.337432 Y164.207079 Z-1.000000 I1.292255 J2.940929",
		"G02 X612.176691 Y166.431573 Z-1.000000 I2.793586 J1.319919",
		"G02 X614.402340 Y171.871090 Z-1.000000 I30.833114 J-9.440684",
		"G03 X616.441432 Y177.013578 Z-1.000000 I-24.069753 J12.519617",
		"G03 X617.939407 Y183.958184 Z-1.000000 I-54.358913 J15.359268",
		"G03 X618.502534 Y191.061884 Z-1.000000 I-50.076300 J7.543835",
		"G03 X617.904300 Y195.117190 Z-1.000000 I-13.262742 J0.115274",
		"G03 X613.619681 Y200.291200 Z-1.000000 I-8.435511 J-2.624421",
		"G03 X609.640620 Y199.515625 Z-1.000000 I-1.511585 J-2.839869",
		"G03 X609.035048 Y198.157568 Z-1.000000 I2.084020 J-1.743331",
		"G03 X608.144530 Y189.072270 Z-1.000000 I101.402221 J-14.525480",
		"G02 X607.316059 Y181.185374 Z-1.000000 I-76.877531 J4.088566",
		"G02 X606.385725 Y178.695617 Z-1.000000 I-6.562132 J1.033340",
		"G02 X604.596606 Y177.663199 Z-1.000000 I-1.866072 J1.167363",
		"G02 X601.419920 Y178.322270 Z-1.000000 I-0.239035 J6.833127",
		"G02 X598.422666 Y180.499421 Z-1.000000 I4.347273 J9.136545",
		"G02 X594.176702 Y185.721252 Z-1.000000 I32.259825 J30.568184",
		"G02 X590.746491 Y191.499882 Z-1.000000 I38.799502 J26.938902",
		"G02 X589.349610 Y195.707030 Z-1.000000 I13.815311 J6.922513",
		"G02 X589.361158 Y200.232825 Z-1.000000 I12.500097 J2.231016",
		"G02 X591.326032 Y207.723705 Z-1.000000 I50.961877 J-9.364277",
		"G02 X594.417546 Y214.831907 Z-1.000000 I46.485556 J-15.991209",
		"G02 X596.925780 Y217.730469 Z-1.000000 I7.610709 J-4.051314",
		"G02 X598.584067 Y218.624312 Z-1.000000 I4.025229 J-5.482564",
		"G02 X600.453120 Y219.029297 Z-1.000000 I2.350359 J-6.331734",
		"G01 X600.453120 Y219.029297 Z-1.000000",
		"G00 X582.449220 Y182.560550",
		"G02 X583.137757 Y182.231963 Z-1.000000 I-0.003719 J-0.893483 F400.000000",
		"G02 X585.712890 Y178.560550 Z-1.000000 I-23.894587 J-19.498497",
		"G03 X588.598348 Y175.043882 Z-1.000000 I13.215488 J7.901322",
		"G03 X595.171880 Y170.003910 Z-1.000000 I34.212254 J37.815494",
		"G02 X600.336176 Y166.035492 Z-1.000000 I-21.447430 J-33.255073",
		"G02 X601.933331 Y163.556692 Z-1.000000 I-3.844759 J-4.231222",
		"G02 X601.972308 Y160.686263 Z-1.000000 I-4.653930 J-1.498674",
		"G02 X599.837890 Y155.513670 Z-1.000000 I-25.151868 J7.351994",
		"G02 X596.777975 Y152.134058 Z-1.000000 I-8.512076 J4.631834",
		"G02 X593.298830 Y151.062500 Z-1.000000 I-3.479145 J5.112281",
		"G02 X589.421789 Y151.773208 Z-1.000000 I-0.000000 J10.930343",
		"G02 X585.031250 Y154.197270 Z-1.000000 I6.290943 J16.582529",
		"G02 X582.647231 Y156.318842 Z-1.000000 I11.895358 J15.767112",
		"G02 X581.360723 Y158.349465 Z-1.000000 I4.417986 J4.221879",
		"G02 X580.719331 Y160.843743 Z-1.000000 I10.470720 J4.022097",
		"G02 X579.982420 Y167.341800 Z-1.000000 I136.625194 J18.784779",
		"G02 X580.466487 Y180.881311 Z-1.000000 I54.134870 J4.842968",
		"G02 X582.449220 Y182.560550 Z-1.000000 I1.973380 J-0.319876",
		"G01 X582.449220 Y182.560550 Z-1.000000",
		"G00 X590.251950 Y325.419922",
		"G02 X591.460981 Y325.376680 Z-1.000000 I0.507428 J-2.736128 F400.000000",
		"G02 X593.933590 Y324.556641 Z-1.000000 I-5.027934 J-19.298164",
		"G02 X597.543500 Y322.450232 Z-1.000000 I-5.530410 J-13.624365",
		"G02 X602.281250 Y317.931641 Z-1.000000 I-22.412644 J-28.242759",
		"G01 X606.664060 Y312.914062 Z-1.000000",
		"G01 X616.039060 Y314.093750 Z-1.000000",
		"G03 X625.264765 Y315.706020 Z-1.000000 I-12.166626 J96.821394",
		"G03 X627.898440 Y316.759766 Z-1.000000 I-1.956724 J8.708634",
		"G02 X629.362455 Y317.328058 Z-1.000000 I2.388123 J-3.982281",
		"G02 X630.022967 Y316.983472 Z-1.000000 I0.117720 J-0.579688",
		"G02 X630.280349 Y315.863502 Z-1.000000 I-2.899612 J-1.255923",
		"G02 X630.384770 Y311.011719 Z-1.000000 I-112.663982 J-4.851783",
		"G03 X630.774379 Y303.613171 Z-1.000000 I71.212649 J0.040537",
		"G03 X632.278445 Y291.798081 Z-1.000000 I269.655057 J28.323932",
		"G03 X634.447911 Y279.638007 Z-1.000000 I218.432497 J32.696753",
		"G03 X635.353520 Y277.818359 Z-1.000000 I3.910824 J0.811175",
		"G02 X636.344012 Y275.997210 Z-1.000000 I-3.558298 J-3.115229",
		"G02 X636.081334 Y275.128164 Z-1.000000 I-0.808219 J-0.229929",
		"G02 X635.000116 Y274.631528 Z-1.000000 I-1.417399 J1.660528",
		"G02 X631.296880 Y274.355469 Z-1.000000 I-3.569149 J22.902079",
		"G02 X626.255764 Y275.016318 Z-1.000000 I0.118528 J20.462012",
		"G02 X620.775390 Y277.095703 Z-1.000000 I6.872542 J26.374753",
		"G03 X611.775726 Y279.549721 Z-1.000000 I-10.425754 J-20.505231",
		"G03 X609.183590 Y277.816406 Z-1.000000 I-0.159415 J-2.566498",
		"G03 X610.959236 Y272.423841 Z-1.000000 I4.449041 J-1.523657",
		"G03 X630.068360 Y263.384766 Z-1.000000 I37.066441 J53.642157",
		"G02 X636.775816 Y259.126856 Z-1.000000 I-3.869935 J-13.508340",
		"G02 X644.960940 Y246.355469 Z-1.000000 I-38.555612 J-33.718716",
		"G02 X648.182449 Y237.429860 Z-1.000000 I-57.820693 J-25.913324",
		"G02 X651.767615 Y222.704352 Z-1.000000 I-223.932971 J-62.319335",
		"G02 X654.336190 Y207.407299 Z-1.000000 I-190.536187 J-39.857686",
		"G02 X653.933590 Y203.462891 Z-1.000000 I-8.692226 J-1.105548",
		"G02 X652.831034 Y202.455174 Z-1.000000 I-1.472785 J0.504374",
		"G02 X646.888670 Y201.738281 Z-1.000000 I-5.942364 J24.269838",
		"G02 X640.819719 Y202.870264 Z-1.000000 I-0.000000 J16.834857",
		"G02 X633.945310 Y206.740234 Z-1.000000 I9.913704 J25.650830",
		"G02 X627.669672 Y211.976245 Z-1.000000 I64.533822 J83.726070",
		"G02 X622.552730 Y217.019531 Z-1.000000 I54.520278 J60.434013",
		"G03 X613.646322 Y223.815056 Z-1.000000 I-24.182138 J-22.459539",
		"G03 X600.539060 Y228.513672 Z-1.000000 I-23.593024 J-45.183795",
		"G03 X596.213709 Y228.429684 Z-1.000000 I-1.980543 J-9.421684",
		"G03 X592.292970 Y226.443359 Z-1.000000 I2.421142 J-9.641681",
		"G02 X588.694965 Y223.794590 Z-1.000000 I-24.916081 J30.077132",
		"G02 X588.347660 Y223.873047 Z-1.000000 I-0.135518 J0.208037",
		"G02 X580.123051 Y240.624900 Z-1.000000 I86.007956 J52.622008",
		"G02 X574.087890 Y261.335938 Z-1.000000 I125.870212 J47.913204",
		"G02 X572.366140 Y272.989225 Z-1.000000 I101.811735 J20.996321",
		"G02 X572.482737 Y281.518116 Z-1.000000 I41.046439 J3.704099",
		"G02 X574.301235 Y289.900977 Z-1.000000 I44.307260 J-5.222920",
		"G02 X578.912110 Y302.257812 Z-1.000000 I132.998636 J-42.588917",
		"G03 X583.568846 Y313.779490 Z-1.000000 I-281.610263 J120.520788",
		"G03 X584.585940 Y317.162109 Z-1.000000 I-23.030364 J8.769048",
		"G02 X585.741828 Y320.434179 Z-1.000000 I14.235440 J-3.188596",
		"G02 X587.664060 Y323.392578 Z-1.000000 I13.502266 J-6.669466",
		"G02 X589.185199 Y324.917887 Z-1.000000 I9.173925 J-7.627699",
		"G02 X590.251950 Y325.419922 Z-1.000000 I1.490455 J-1.782637",
		"G01 X590.251950 Y325.419922 Z-1.000000",
		"G00 X639.324220 Y330.000000",
		"G02 X640.069867 Y330.085654 Z-1.000000 I0.420894 J-0.375648 F400.000000",
		"G02 X645.189450 Y325.869141 Z-1.000000 I-25.285126 J-35.916852",
		"G03 X649.333789 Y322.744842 Z-1.000000 I15.187276 J15.834860",
		"G03 X661.626960 Y316.140331 Z-1.000000 I102.048695 J175.203160",
		"G03 X674.047560 Y310.407447 Z-1.000000 I115.782605 J234.527802",
		"G03 X685.384770 Y306.025391 Z-1.000000 I64.159845 J149.136895",
		"G03 X694.784046 Y303.743689 Z-1.000000 I16.899252 J49.114425",
		"G03 X710.279515 Y302.162183 Z-1.000000 I26.044264 J178.477145",
		"G03 X725.932934 Y301.967986 Z-1.000000 I9.930882 J169.511424",
		"G03 X733.664060 Y303.197266 Z-1.000000 I-1.066729 J31.634567",
		"G02 X738.555434 Y304.605724 Z-1.000000 I548.402824 J-1895.326547",
		"G02 X743.111330 Y305.904297 Z-1.000000 I464.396625 J-1620.641812",
		"G03 X747.767363 Y307.611194 Z-1.000000 I-9.012697 J31.788366",
		"G03 X756.760255 Y312.002747 Z-1.000000 I-74.542082 J164.048551",
		"G03 X765.453110 Y316.845233 Z-1.000000 I-88.030774 J168.249638",
		"G03 X770.455080 Y320.210938 Z-1.000000 I-26.564847 J44.879272",
		"G02 X775.181864 Y322.951175 Z-1.000000 I13.290788 J-17.479141",
		"G02 X778.529300 Y323.630859 Z-1.000000 I3.347436 J-7.903204",
		"G03 X781.995484 Y324.030942 Z-1.000000 I0.000000 J15.214954",
		"G03 X784.396480 Y325.003906 Z-1.000000 I-1.909303 J8.160581",
		"G02 X786.455095 Y325.476539 Z-1.000000 I1.755764 J-2.927857",
		"G02 X787.386096 Y324.889963 Z-1.000000 I-0.107054 J-1.202033",
		"G02 X787.795741 Y323.488608 Z-1.000000 I-2.530336 J-1.500221",
		"G02 X787.575192 Y320.751777 Z-1.000000 I-12.059001 J-0.405524",
		"G02 X785.602721 Y315.119125 Z-1.000000 I-20.629786 J4.062555",
		"G02 X781.519530 Y310.394531 Z-1.000000 I-11.975583 J6.223069",
		"G03 X777.475588 Y306.584291 Z-1.000000 I12.435758 J-17.249617",
		"G03 X770.970454 Y297.725193 Z-1.000000 I83.940734 J-68.454595",
		"G03 X765.384042 Y288.349214 Z-1.000000 I100.691529 J-66.346443",
		"G03 X762.265620 Y281.173828 Z-1.000000 I42.042876 J-22.537153",
		"G03 X760.060703 Y270.448922 Z-1.000000 I41.532559 J-14.127720",
		"G03 X760.558590 Y259.246094 Z-1.000000 I45.912151 J-3.572005",
		"G02 X760.067188 Y253.214484 Z-1.000000 I-12.167605 J-2.044514",
		"G02 X756.471300 Y248.054021 Z-1.000000 I-10.786944 J3.683439",
		"G02 X750.432472 Y244.423183 Z-1.000000 I-15.777556 J19.403986",
		"G02 X735.882810 Y239.181641 Z-1.000000 I-55.569852 J131.438289",
		"G02 X714.790140 Y234.193627 Z-1.000000 I-52.519616 J174.997201",
		"G02 X712.269530 Y235.058594 Z-1.000000 I-0.475067 J2.720755",
		"G03 X711.525686 Y235.467795 Z-1.000000 I-0.967079 J-0.877276",
		"G03 X696.169920 Y237.851562 Z-1.000000 I-74.605368 J-429.941955",
		"G02 X677.034974 Y241.870599 Z-1.000000 I18.756817 J136.863658",
		"G02 X665.604965 Y246.706868 Z-1.000000 I13.938640 J48.867439",
		"G02 X656.271386 Y254.366920 Z-1.000000 I18.400601 J31.937056",
		"G02 X649.691410 Y264.630859 Z-1.000000 I30.909370 J27.056393",
		"G03 X646.758513 Y270.838983 Z-1.000000 I-154.997226 J-69.428318",
		"G03 X644.845700 Y274.375000 Z-1.000000 I-52.472842 J-26.099869",
		"G02 X642.918839 Y279.037803 Z-1.000000 I15.782827 J9.251642",
		"G02 X640.560697 Y290.479373 Z-1.000000 I112.499752 J29.150324",
		"G02 X639.207917 Y301.945826 Z-1.000000 I160.569928 J24.756622",
		"G02 X638.669920 Y314.429688 Z-1.000000 I160.421465 J13.166951",
		"G02 X638.945962 Y329.067367 Z-1.000000 I316.859746 J1.345994",
		"G02 X639.324220 Y330.000000 Z-1.000000 I1.493480 J-0.062703",
		"G01 X639.324220 Y330.000000 Z-1.000000",
		"G00 X638.843780 Y385.205078",
		"G02 X638.862011 Y384.977560 Z-1.000000 I-0.156663 J-0.127043 F400.000000",
		"G02 X637.976590 Y383.609375 Z-1.000000 I-13.359080 J7.674734",
		"G03 X636.370551 Y380.247230 Z-1.000000 I8.349250 J-6.052954",
		"G03 X633.220390 Y366.820009 Z-1.000000 I198.522187 J-53.658444",
		"G03 X630.956799 Y353.526864 Z-1.000000 I234.178600 J-46.715831",
		"G03 X630.382840 Y345.544922 Z-1.000000 I57.762946 J-8.165179",
		"G02 X630.014080 Y334.428533 Z-1.000000 I-185.246628 J0.580816",
		"G02 X629.272225 Y329.849706 Z-1.000000 I-23.792849 J1.505371",
		"G02 X627.343123 Y326.151078 Z-1.000000 I-9.131016 J2.410087",
		"G02 X624.738310 Y324.160156 Z-1.000000 I-5.866629 J4.976117",
		"G02 X611.977201 Y322.918895 Z-1.000000 I-8.129996 J17.364964",
		"G02 X603.142610 Y329.535156 Z-1.000000 I3.432213 J13.789459",
		"G02 X600.740399 Y338.147887 Z-1.000000 I15.175488 J8.874026",
		"G02 X602.794950 Y352.929688 Z-1.000000 I48.965469 J0.727879",
		"G02 X606.186047 Y362.596995 Z-1.000000 I106.040349 J-31.768407",
		"G02 X608.807980 Y367.450486 Z-1.000000 I22.957787 J-9.267211",
		"G02 X612.504384 Y371.291646 Z-1.000000 I13.805837 J-9.586424",
		"G02 X617.677760 Y374.365234 Z-1.000000 I16.595744 J-22.042852",
		"G03 X623.891204 Y377.668525 Z-1.000000 I-23.584448 J51.857367",
		"G03 X625.576200 Y379.212891 Z-1.000000 I-3.141604 J5.119075",
		"G02 X627.249589 Y380.729926 Z-1.000000 I4.640582 J-3.437419",
		"G02 X630.818390 Y382.351562 Z-1.000000 I8.922496 J-14.898286",
		"G03 X634.628281 Y383.675508 Z-1.000000 I-29.534440 J91.134423",
		"G03 X637.103550 Y384.685547 Z-1.000000 I-14.574314 J39.254832",
		"G02 X638.624951 Y385.269832 Z-1.000000 I6.304013 J-14.141915",
		"G02 X638.843780 Y385.205078 Z-1.000000 I0.062264 J-0.191717",
		"G01 X638.843780 Y385.205078 Z-1.000000",
		"G00 X653.011750 Y395.732422",
		"G02 X654.667227 Y395.611082 Z-1.000000 I0.710989 J-1.653506 F400.000000",
		"G02 X659.685580 Y391.619141 Z-1.000000 I-14.106647 J-22.884074",
		"G03 X680.903776 Y376.305254 Z-1.000000 I56.212915 J55.529545",
		"G03 X708.839870 Y367.337891 Z-1.000000 I43.998777 J89.071279",
		"G03 X716.281504 Y366.603367 Z-1.000000 I9.450427 J57.680717",
		"G03 X731.909955 Y366.390259 Z-1.000000 I12.951182 J376.617421",
		"G03 X747.764512 Y366.862634 Z-1.000000 I-2.484444 J349.690158",
		"G03 X752.539090 Y367.648438 Z-1.000000 I-1.148990 J21.879483",
		"G02 X753.450607 Y367.055488 Z-1.000000 I0.195325 J-0.696827",
		"G02 X753.968780 Y357.814453 Z-1.000000 I-52.097921 J-7.556337",
		"G03 X754.555949 Y347.644673 Z-1.000000 I56.848007 J-1.819626",
		"G03 X756.626980 Y339.910156 Z-1.000000 I34.726998 J5.154132",
		"G03 X760.182877 Y332.732595 Z-1.000000 I44.369704 J17.511969",
		"G03 X763.023470 Y329.333984 Z-1.000000 I12.501801 J7.562725",
		"G02 X765.957161 Y326.314029 Z-1.000000 I-17.990585 J-20.411613",
		"G02 X765.937530 Y325.828125 Z-1.000000 I-0.279207 J-0.232069",
		"G02 X762.429964 Y323.200027 Z-1.000000 I-10.245566 J10.019432",
		"G02 X750.586067 Y317.408009 Z-1.000000 I-89.414750 J167.835510",
		"G02 X738.530542 Y312.473929 Z-1.000000 I-83.873426 J187.734632",
		"G02 X732.218780 Y310.763672 Z-1.000000 I-12.405310 J33.280213",
		"G02 X711.971441 Y308.998071 Z-1.000000 I-20.431480 J117.323859",
		"G02 X696.035190 Y310.707032 Z-1.000000 I-0.114561 J74.089861",
		"G02 X682.899727 Y314.247934 Z-1.000000 I30.206859 J138.190862",
		"G02 X668.933695 Y319.309028 Z-1.000000 I62.793100 J195.076958",
		"G02 X655.288791 Y325.550035 Z-1.000000 I62.377710 J154.414911",
		"G02 X648.947300 Y329.728517 Z-1.000000 I15.996710 J31.178815",
		"G02 X642.176118 Y335.738800 Z-1.000000 I83.068153 J100.403895",
		"G02 X639.898900 Y339.000596 Z-1.000000 I6.880867 J7.229683",
		"G02 X639.065944 Y342.897639 Z-1.000000 I9.281557 J4.021383",
		"G02 X639.263700 Y352.076173 Z-1.000000 I135.684844 J1.668008",
		"G02 X640.459205 Y363.940129 Z-1.000000 I132.220395 J-7.331346",
		"G02 X642.019560 Y370.882813 Z-1.000000 I46.604830 J-6.827663",
		"G02 X645.062495 Y379.841593 Z-1.000000 I147.056061 J-44.952822",
		"G02 X648.105305 Y387.300345 Z-1.000000 I129.466724 J-48.466127",
		"G02 X651.810125 Y394.656750 Z-1.000000 I75.273275 J-33.297884",
		"G02 X653.011750 Y395.732423 Z-1.000000 I2.202007 J-1.250845",
		"G01 X653.011750 Y395.732422 Z-1.000000",
		"G00 X583.697300 Y384.957031",
		"G02 X584.560975 Y384.865837 Z-1.000000 I0.363608 J-0.691781 F400.000000",
		"G02 X586.689480 Y382.888672 Z-1.000000 I-17.147324 J-20.594150",
		"G03 X589.604353 Y380.254738 Z-1.000000 I20.800256 J20.088979",
		"G03 X593.712920 Y377.361328 Z-1.000000 I29.188796 J37.083605",
		"G02 X596.500189 Y375.215109 Z-1.000000 I-9.762683 J-15.561687",
		"G02 X597.473347 Y373.303587 Z-1.000000 I-2.149591 J-2.297839",
		"G02 X597.349202 Y370.864564 Z-1.000000 I-7.002288 J-0.866257",
		"G02 X595.447300 Y363.349609 Z-1.000000 I-163.719038 J37.436244",
		"G03 X593.813016 Y355.738553 Z-1.000000 I63.121970 J-17.534854",
		"G03 X592.617220 Y344.886719 Z-1.000000 I127.551664 J-19.547100",
		"G01 X591.878940 Y333.957031 Z-1.000000",
		"G01 X586.843780 Y339.064453 Z-1.000000",
		"G02 X581.550465 Y347.928166 Z-1.000000 I15.466635 J15.248920",
		"G02 X578.742220 Y365.404297 Z-1.000000 I61.026087 J18.769997",
		"G02 X579.312370 Y374.829975 Z-1.000000 I58.567690 J1.187380",
		"G02 X580.785190 Y379.785156 Z-1.000000 I17.376124 J-2.468203",
		"G02 X582.949821 Y384.202514 Z-1.000000 I117.874487 J-55.022820",
		"G02 X583.697300 Y384.957031 Z-1.000000 I1.567205 J-0.805074",
		"G01 X583.697300 Y384.957031 Z-1.000000",
		"G00 X671.978550 Y647.539060",
		"G01 X671.261750 Y633.843750 Z-1.000000 F400.000000",
		"G03 X671.171111 Y623.275399 Z-1.000000 I120.956954 J-6.321945",
		"G03 X671.825717 Y615.934445 Z-1.000000 I68.396560 J2.399383",
		"G03 X673.590360 Y609.011974 Z-1.000000 I33.169975 J4.769375",
		"G03 X675.021510 Y607.693360 Z-1.000000 I2.013802 J0.749715",
		"G03 X675.998333 Y607.970675 Z-1.000000 I0.262866 J0.933129",
		"G03 X681.105500 Y614.316410 Z-1.000000 I-45.963535 J42.220354",
		"G02 X686.417500 Y620.871850 Z-1.000000 I50.350219 J-35.369831",
		"G02 X691.599640 Y625.589840 Z-1.000000 I31.075079 J-28.927235",
		"G02 X697.403042 Y629.320618 Z-1.000000 I24.903658 J-32.359698",
		"G02 X703.769120 Y632.150929 Z-1.000000 I23.836759 J-45.040235",
		"G02 X710.498996 Y633.939829 Z-1.000000 I13.262060 J-36.338712",
		"G02 X713.980500 Y633.757810 Z-1.000000 I1.337390 J-7.806192",
		"G02 X717.820970 Y630.939849 Z-1.000000 I-1.835781 J-6.527884",
		"G02 X719.306042 Y625.962009 Z-1.000000 I-7.629685 J-4.986661",
		"G02 X718.294272 Y620.229961 Z-1.000000 I-16.651665 J-0.016114",
		"G02 X714.002000 Y611.867190 Z-1.000000 I-35.746491 J13.064319",
		"G03 X711.070407 Y606.600993 Z-1.000000 I27.692525 J-18.864991",
		"G03 X710.475867 Y603.680548 Z-1.000000 I6.323857 J-2.808141",
		"G03 X711.332786 Y601.003377 Z-1.000000 I4.856754 J0.078839",
		"G03 X714.226610 Y598.021480 Z-1.000000 I10.287063 J7.088101",
		"G03 X721.508528 Y596.106657 Z-1.000000 I6.064920 J8.260720",
		"G03 X730.503960 Y599.968750 Z-1.000000 I-2.056846 J17.197672",
		"G03 X734.324900 Y604.908259 Z-1.000000 I-9.116681 J10.999768",
		"G03 X739.314500 Y618.777340 Z-1.000000 I-72.080672 J33.764134",
		"G02 X741.197860 Y624.778627 Z-1.000000 I59.191351 J-15.279623",
		"G02 X741.523490 Y624.726560 Z-1.000000 I0.157619 J-0.058531",
		"G03 X742.276100 Y623.384319 Z-1.000000 I1.688661 J0.064733",
		"G03 X743.916070 Y623.134770 Z-1.000000 I1.087767 J1.635018",
		"G02 X745.957929 Y623.166426 Z-1.000000 I1.078220 J-3.679533",
		"G02 X747.044970 Y622.322270 Z-1.000000 I-0.437677 J-1.685591",
		"G03 X748.165154 Y621.339386 Z-1.000000 I1.920647 J1.059169",
		"G03 X750.849660 Y620.832030 Z-1.000000 I2.684506 J6.848416",
		"G02 X753.273990 Y620.274361 Z-1.000000 I-0.000000 J-5.548426",
		"G02 X753.832080 Y619.382810 Z-1.000000 I-0.433083 J-0.891551",
		"G03 X754.279813 Y618.785956 Z-1.000000 I0.621687 J0.000000",
		"G03 X755.308640 Y618.953120 Z-1.000000 I0.339004 J1.163159",
		"G02 X756.879568 Y619.724716 Z-1.000000 I3.353537 J-4.842659",
		"G02 X759.585990 Y620.310550 Z-1.000000 I4.451044 J-14.018379",
		"G02 X761.152288 Y620.217650 Z-1.000000 I0.525171 J-4.395962",
		"G02 X761.977892 Y619.561637 Z-1.000000 I-0.294837 J-1.218584",
		"G02 X762.341633 Y618.320916 Z-1.000000 I-3.224439 J-1.618985",
		"G02 X762.634820 Y614.234380 Z-1.000000 I-60.988818 J-6.429398",
		"G02 X761.996517 Y599.477497 Z-1.000000 I-90.721742 J-3.468117",
		"G02 X759.824270 Y591.806640 Z-1.000000 I-26.243552 J3.288699",
		"G03 X757.904555 Y586.299841 Z-1.000000 I28.947043 J-13.179187",
		"G03 X757.986380 Y584.406250 Z-1.000000 I3.250997 J-0.808083",
		"G02 X758.065781 Y582.898016 Z-1.000000 I-2.609610 J-0.893590",
		"G02 X757.546930 Y582.484380 Z-1.000000 I-0.518851 J0.118597",
		"G03 X756.547219 Y582.189609 Z-1.000000 I-0.000000 J-1.842637",
		"G03 X749.265670 Y577.136720 Z-1.000000 I72.478315 J-112.219111",
		"G02 X741.916951 Y572.050214 Z-1.000000 I-77.634919 J104.311125",
		"G02 X736.039110 Y568.638670 Z-1.000000 I-40.664603 J63.292811",
		"G03 X730.239806 Y565.207577 Z-1.000000 I29.100644 J-55.803106",
		"G03 X728.775440 Y563.798830 Z-1.000000 I3.177835 J-4.768764",
		"G02 X726.842388 Y562.555925 Z-1.000000 I-2.555149 J1.849282",
		"G02 X717.487180 Y561.151143 Z-1.000000 I-18.859627 J93.743419",
		"G02 X708.287665 Y560.748326 Z-1.000000 I-8.205367 J82.143471",
		"G02 X707.384820 Y561.662110 Z-1.000000 I0.011005 J0.913784",
		"G02 X707.841983 Y562.744306 Z-1.000000 I1.509468 J0.000000",
		"G02 X709.330130 Y563.552730 Z-1.000000 I2.183548 J-2.245571",
		"G03 X709.597298 Y564.327599 Z-1.000000 I-0.107117 J0.470426",
		"G03 X705.859430 Y567.820310 Z-1.000000 I-14.589544 J-11.867128",
		"G02 X700.717831 Y571.950456 Z-1.000000 I30.881533 J43.709718",
		"G02 X695.800830 Y576.974610 Z-1.000000 I40.966069 J45.010510",
		"G02 X692.799843 Y580.606155 Z-1.000000 I67.783440 J59.069687",
		"G02 X692.118975 Y581.892610 Z-1.000000 I3.242549 J2.539554",
		"G02 X692.221739 Y583.124315 Z-1.000000 I1.539488 J0.491697",
		"G02 X693.324270 Y584.728520 Z-1.000000 I6.857766 J-3.532202",
		"G03 X694.493689 Y586.909043 Z-1.000000 I-3.941710 J3.517790",
		"G03 X694.095750 Y587.675780 Z-1.000000 I-0.623378 J0.163099",
		"G02 X693.381987 Y588.448030 Z-1.000000 I0.408387 J1.093436",
		"G02 X693.521530 Y589.681640 Z-1.000000 I1.564320 J0.447746",
		"G03 X693.327758 Y590.248775 Z-1.000000 I-0.354508 J0.195546",
		"G03 X692.207080 Y590.250000 Z-1.000000 I-0.561759 J-1.298667",
		"G02 X690.054410 Y589.424810 Z-1.000000 I-11.374590 J26.452430",
		"G02 X689.490280 Y589.332030 Z-1.000000 I-0.564130 J1.668645",
		"G03 X689.297447 Y589.098050 Z-1.000000 I0.000000 J-0.196451",
		"G03 X689.824270 Y587.619140 Z-1.000000 I5.156286 J1.003503",
		"G02 X690.144589 Y586.819929 Z-1.000000 I-3.425456 J-1.836697",
		"G02 X690.064782 Y586.725567 Z-1.000000 I-0.072082 J-0.019965",
		"G02 X689.832489 Y586.817496 Z-1.000000 I0.047774 J0.460174",
		"G02 X688.732470 Y587.732420 Z-1.000000 I9.941096 J13.070984",
		"G03 X686.860210 Y588.653410 Z-1.000000 I-2.536156 J-2.792162",
		"G03 X684.469035 Y588.682972 Z-1.000000 I-1.284596 J-7.184712",
		"G03 X682.389253 Y587.892323 Z-1.000000 I0.805029 J-5.248336",
		"G03 X681.716850 Y586.656250 Z-1.000000 I0.799930 J-1.236073",
		"G03 X682.103504 Y585.141839 Z-1.000000 I3.159081 J-0.000000",
		"G03 X686.761427 Y577.121772 Z-1.000000 I155.242765 J84.799805",
		"G03 X691.676393 Y569.541570 Z-1.000000 I168.239604 J103.702194",
		"G03 X694.144580 Y566.494140 Z-1.000000 I19.625838 J13.372203",
		"G03 X697.321250 Y564.177243 Z-1.000000 I8.056076 J7.709394",
		"G03 X701.179740 Y563.070310 Z-1.000000 I5.086430 J10.451688",
		"G02 X705.061932 Y561.949248 Z-1.000000 I-1.223910 J-11.520816",
		"G02 X705.685007 Y561.054291 Z-1.000000 I-0.495607 J-1.009418",
		"G02 X705.230583 Y560.123914 Z-1.000000 I-0.975314 J-0.099793",
		"G02 X701.625050 Y558.718750 Z-1.000000 I-5.485407 J8.746761",
		"G02 X696.407331 Y558.107168 Z-1.000000 I-7.206619 J38.920083",
		"G02 X687.149120 Y557.870449 Z-1.000000 I-9.418837 J187.210103",
		"G02 X677.564136 Y558.162616 Z-1.000000 I-0.131166 J153.067985",
		"G02 X676.462940 Y558.697250 Z-1.000000 I0.099386 J1.606101",
		"G03 X675.539849 Y559.325979 Z-1.000000 I-1.722612 J-1.537117",
		"G03 X668.043020 Y561.804670 Z-1.000000 I-39.851371 J-107.954440",
		"G02 X663.429967 Y563.398389 Z-1.000000 I14.407444 J49.175795",
		"G02 X660.901010 Y564.807622 Z-1.000000 I4.205874 J10.521503",
		"G02 X658.800071 Y566.810197 Z-1.000000 I6.921494 J9.364827",
		"G02 X655.783250 Y570.867170 Z-1.000000 I43.522335 J35.513968",
		"G02 X652.964341 Y575.401575 Z-1.000000 I57.869298 J39.119092",
		"G02 X651.754497 Y578.504381 Z-1.000000 I9.931745 J5.659852",
		"G02 X651.307442 Y581.973882 Z-1.000000 I17.521357 J4.021232",
		"G02 X651.166070 Y590.962870 Z-1.000000 I300.139826 J9.215977",
		"G02 X651.662935 Y603.971901 Z-1.000000 I168.735455 J0.069344",
		"G02 X652.246140 Y604.988260 Z-1.000000 I1.359427 J-0.104557",
		"G03 X653.024480 Y605.947202 Z-1.000000 I-1.257732 J1.816204",
		"G03 X654.152390 Y609.519510 Z-1.000000 I-18.653423 J7.853793",
		"G02 X655.444057 Y613.700339 Z-1.000000 I24.753036 J-5.357505",
		"G02 X661.114367 Y627.002917 Z-1.000000 I398.942232 J-162.191924",
		"G02 X667.209009 Y640.073794 Z-1.000000 I397.135474 J-177.218582",
		"G02 X669.605520 Y644.148420 Z-1.000000 I27.007032 J-13.142243",
		"G01 X671.978550 Y647.539060 Z-1.000000",
		"G00 X716.671910 Y695.880860",
		"G02 X723.968476 Y694.952261 Z-1.000000 I1.324467 J-18.723932 F400.000000",
		"G02 X734.480500 Y689.960940 Z-1.000000 I-15.526351 J-46.264572",
		"G02 X741.751256 Y684.032599 Z-1.000000 I-20.696280 J-32.805492",
		"G02 X745.908230 Y678.070310 Z-1.000000 I-16.792946 J-16.138519",
		"G02 X748.858473 Y671.376248 Z-1.000000 I-88.808106 J-43.137143",
		"G02 X748.759800 Y670.888670 Z-1.000000 I-0.423282 J-0.168112",
		"G02 X748.286683 Y670.814395 Z-1.000000 I-0.286662 J0.282012",
		"G02 X739.166050 Y675.910160 Z-1.000000 I88.515575 J169.139380",
		"G03 X728.718583 Y681.749151 Z-1.000000 I-112.406609 J-188.858438",
		"G03 X723.651452 Y683.736388 Z-1.000000 I-11.641576 J-22.230325",
		"G03 X718.431857 Y684.342829 Z-1.000000 I-4.624015 J-17.032975",
		"G03 X710.689480 Y683.640620 Z-1.000000 I2.314714 J-68.555236",
		"G03 X700.790499 Y681.727194 Z-1.000000 I16.882570 J-113.903383",
		"G03 X696.541050 Y680.255860 Z-1.000000 I5.313103 J-22.217314",
		"G02 X692.918055 Y678.651780 Z-1.000000 I-54.886506 J119.073831",
		"G02 X692.177700 Y678.503125 Z-1.000000 I-0.730287 J1.719147",
		"G02 X691.893026 Y678.828246 Z-1.000000 I0.001555 J0.288552",
		"G02 X692.230500 Y680.279260 Z-1.000000 I7.296191 J-0.932183",
		"G02 X693.593538 Y682.363160 Z-1.000000 I4.774268 J-1.635036",
		"G02 X700.187665 Y687.842569 Z-1.000000 I59.236057 J-64.579393",
		"G02 X707.143241 Y692.627925 Z-1.000000 I60.719468 J-80.808836",
		"G02 X711.501980 Y694.759730 Z-1.000000 I12.200793 J-19.424133",
		"G02 X714.094846 Y695.510533 Z-1.000000 I6.886264 J-18.928834",
		"G02 X716.671910 Y695.880820 Z-1.000000 I3.852304 J-17.657801",
		"G01 X716.671910 Y695.880860 Z-1.000000",
		"G00 X721.457060 Y700.623050",
		"G03 X719.467979 Y700.519674 Z-1.000000 I0.696941 J-32.597873 F400.000000",
		"G03 X716.552760 Y700.246090 Z-1.000000 I10.866143 J-131.454551",
		"G03 X708.188729 Y698.765332 Z-1.000000 I6.294078 J-59.914379",
		"G03 X704.330110 Y697.181640 Z-1.000000 I3.543660 J-14.126577",
		"G03 X691.990503 Y687.645267 Z-1.000000 I31.839414 J-53.950278",
		"G03 X674.019560 Y667.031250 Z-1.000000 I135.848509 J-136.570769",
		"G03 X660.373224 Y646.401641 Z-1.000000 I160.565227 J-121.041010",
		"G03 X650.548860 Y626.310550 Z-1.000000 I142.444773 J-82.101779",
		"G03 X646.879712 Y616.839584 Z-1.000000 I274.555163 J-111.811650",
		"G03 X645.409610 Y611.744920 Z-1.000000 I34.807137 J-12.803289",
		"G03 X644.675745 Y606.486901 Z-1.000000 I36.754935 J-7.810132",
		"G03 X644.146510 Y596.179690 Z-1.000000 I308.847236 J-21.025291",
		"G01 X643.599640 Y580.429690 Z-1.000000",
		"G01 X648.531280 Y571.945310 Z-1.000000",
		"G03 X654.018631 Y564.018714 Z-1.000000 I52.823179 J30.705280",
		"G03 X657.240260 Y561.273440 Z-1.000000 I9.129837 J7.451074",
		"G03 X669.266919 Y555.960659 Z-1.000000 I30.246087 J52.199828",
		"G03 X685.324250 Y552.376950 Z-1.000000 I28.524137 J90.041111",
		"G03 X697.004440 Y551.857339 Z-1.000000 I8.787426 J65.992445",
		"G03 X712.552485 Y553.292405 Z-1.000000 I-6.986589 J160.639579",
		"G03 X728.047730 Y556.449704 Z-1.000000 I-18.358698 J129.702020",
		"G03 X733.054720 Y559.132810 Z-1.000000 I-3.212538 J12.008341",
		"G02 X736.301330 Y561.557842 Z-1.000000 I16.577430 J-18.807927",
		"G02 X748.568390 Y568.873050 Z-1.000000 I165.686270 J-263.900476",
		"G01 X761.027370 Y575.919920 Z-1.000000",
		"G01 X766.251980 Y587.078120 Z-1.000000",
		"G01 X771.478550 Y598.234380 Z-1.000000",
		"G01 X771.513750 Y616.039060 Z-1.000000",
		"G03 X770.559714 Y633.637506 Z-1.000000 I-157.056844 J0.310813",
		"G03 X768.859460 Y640.005860 Z-1.000000 I-21.832793 J-2.417877",
		"G02 X766.294632 Y646.218243 Z-1.000000 I154.385594 J67.374867",
		"G02 X763.789140 Y653.017580 Z-1.000000 I182.116262 J70.969437",
		"G03 X760.877132 Y660.165601 Z-1.000000 I-69.684295 J-24.221282",
		"G03 X754.660542 Y672.732931 Z-1.000000 I-314.516563 J-147.758388",
		"G03 X747.803224 Y685.222194 Z-1.000000 I-279.999763 J-145.608715",
		"G03 X745.027420 Y689.095700 Z-1.000000 I-21.272255 J-12.312625",
		"G03 X741.877814 Y691.867140 Z-1.000000 I-12.148893 J-10.631219",
		"G03 X733.664140 Y696.724610 Z-1.000000 I-46.485883 J-69.231537",
		"G03 X726.242626 Y699.883579 Z-1.000000 I-24.901233 J-48.204287",
		"G03 X721.457110 Y700.623050 Z-1.000000 I-4.487447 J-13.186132",
		"G01 X721.457060 Y700.623050 Z-1.000000",
		"G00 X801.269560 Y809.814450",
		"G03 X799.652964 Y809.702152 Z-1.000000 I0.097472 J-13.095237 F400.000000",
		"G03 X799.369882 Y809.534829 Z-1.000000 I0.054929 J-0.416054",
		"G03 X799.382192 Y809.276179 Z-1.000000 I0.165443 J-0.121744",
		"G03 X800.279330 Y808.466800 Z-1.000000 I4.206652 J3.760875",
		"G03 X801.859849 Y807.370436 Z-1.000000 I27.838532 J38.444605",
		"G03 X802.319900 Y807.231595 Z-1.000000 I0.460051 J0.692774",
		"G03 X802.693375 Y807.427724 Z-1.000000 I-0.000000 J0.453656",
		"G03 X803.335970 Y808.466800 Z-1.000000 I-10.099677 J6.964174",
		"G03 X803.136303 Y809.267172 Z-1.000000 I-0.530624 J0.292718",
		"G03 X801.269560 Y809.814450 Z-1.000000 I-1.841885 J-2.825266",
		"G01 X801.269560 Y809.814450 Z-1.000000",
		"G00 X814.408230 Y852.292970",
		"G03 X813.978467 Y852.107989 Z-1.000000 I-0.000000 J-0.591722 F400.000000",
		"G03 X813.113310 Y850.923830 Z-1.000000 I4.207785 J-3.982368",
		"G03 X813.051343 Y849.963966 Z-1.000000 I0.989519 J-0.545814",
		"G03 X813.652370 Y849.554690 Z-1.000000 I0.601027 J0.236670",
		"G03 X814.557268 Y849.944216 Z-1.000000 I-0.000000 J1.245837",
		"G03 X814.947300 Y850.923830 Z-1.000000 I-1.035197 J0.979614",
		"G03 X814.728861 Y852.074641 Z-1.000000 I-3.140650 J-0.000000",
		"G03 X814.408230 Y852.292970 Z-1.000000 I-0.320631 J-0.126269",
		"G01 X814.408230 Y852.292970 Z-1.000000",
		"G00 X814.525420 Y887.902340",
		"G02 X816.489304 Y886.999413 Z-1.000000 I-0.000000 J-2.587206 F400.000000",
		"G02 X817.392610 Y884.558590 Z-1.000000 I-2.846021 J-2.440823",
		"G02 X816.951345 Y881.446536 Z-1.000000 I-11.194627 J0.000000",
		"G02 X816.183620 Y880.378910 Z-1.000000 I-1.884712 J0.545441",
		"G02 X815.718483 Y880.524930 Z-1.000000 I-0.175794 J0.253861",
		"G02 X815.703150 Y881.667970 Z-1.000000 I1.590955 J0.592965",
		"G03 X815.960186 Y883.532615 Z-1.000000 I-4.642510 J1.589994",
		"G03 X815.691335 Y883.727544 Z-1.000000 I-0.213780 J-0.011984",
		"G03 X815.164241 Y883.382079 Z-1.000000 I0.257609 J-0.967890",
		"G03 X813.773470 Y881.154300 Z-1.000000 I9.164567 J-7.269317",
		"G03 X813.332864 Y875.815873 Z-1.000000 I6.802237 J-3.248818",
		"G03 X815.626980 Y874.730470 Z-1.000000 I1.625736 J0.469036",
		"G02 X816.421641 Y874.298002 Z-1.000000 I0.227303 J-0.528659",
		"G02 X816.716830 Y868.304690 Z-1.000000 I-24.972417 J-4.233892",
		"G03 X816.348781 Y861.191453 Z-1.000000 I199.448609 J-13.885882",
		"G03 X816.255890 Y854.689450 Z-1.000000 I165.836067 J-5.620897",
		"G02 X815.715513 Y848.685424 Z-1.000000 I-31.760941 J-0.167768",
		"G02 X815.007840 Y848.013670 Z-1.000000 I-0.817779 J0.152873",
		"G03 X813.725084 Y847.839762 Z-1.000000 I42.915549 J-321.365478",
		"G03 X812.501980 Y847.669920 Z-1.000000 I68.394544 J-497.027014",
		"G03 X811.596857 Y847.224287 Z-1.000000 I0.223714 J-1.596395",
		"G03 X811.281280 Y846.472660 Z-1.000000 I0.737306 J-0.751627",
		"G03 X811.332729 Y841.691030 Z-1.000000 I222.225784 J-0.000000",
		"G03 X811.448501 Y840.641516 Z-1.000000 I5.982121 J0.128747",
		"G03 X811.779205 Y840.502841 Z-1.000000 I0.212309 J0.042642",
		"G03 X812.357450 Y841.119140 Z-1.000000 I-0.976744 J1.495853",
		"G02 X812.959634 Y841.774094 Z-1.000000 I1.708067 J-0.966137",
		"G02 X813.298553 Y841.738984 Z-1.000000 I0.148697 J-0.217972",
		"G02 X813.567451 Y841.276711 Z-1.000000 I-0.814875 J-0.783347",
		"G02 X814.001980 Y839.408200 Z-1.000000 I-15.459179 J-4.579871",
		"G02 X814.139256 Y836.253758 Z-1.000000 I-12.433973 J-2.121312",
		"G02 X813.709010 Y835.857420 Z-1.000000 I-0.430246 J0.035358",
		"G03 X813.133269 Y835.470374 Z-1.000000 I0.000000 J-0.621738",
		"G03 X812.107450 Y831.927730 Z-1.000000 I16.259911 J-6.628111",
		"G02 X810.739061 Y828.594431 Z-1.000000 I-8.393864 J1.498332",
		"G02 X808.898470 Y827.261720 Z-1.000000 I-2.794178 J1.921640",
		"G03 X807.114314 Y825.985600 Z-1.000000 I0.935753 J-3.193569",
		"G03 X806.390650 Y823.703120 Z-1.000000 I3.237707 J-2.282480",
		"G03 X807.114305 Y821.977497 Z-1.000000 I2.419310 J0.000012",
		"G03 X808.390650 Y821.738280 Z-1.000000 I0.816286 J0.830718",
		"G02 X809.541585 Y821.643652 Z-1.000000 I0.486360 J-1.131109",
		"G02 X809.855500 Y820.798830 Z-1.000000 I-0.404644 J-0.631089",
		"G02 X808.842716 Y819.462210 Z-1.000000 I-2.285828 J0.680004",
		"G02 X804.453150 Y817.388670 Z-1.000000 I-10.665343 J16.894928",
		"G02 X793.603282 Y817.086574 Z-1.000000 I-5.928031 J17.917815",
		"G02 X778.797520 Y823.656699 Z-1.000000 I13.786298 J51.034831",
		"G02 X766.141047 Y834.281096 Z-1.000000 I35.316378 J54.921974",
		"G02 X757.382840 Y847.656250 Z-1.000000 I35.576660 J32.851077",
		"G03 X755.310072 Y852.170337 Z-1.000000 I-137.526216 J-60.416061",
		"G03 X754.814538 Y852.817529 Z-1.000000 I-1.792801 J-0.859386",
		"G03 X754.453915 Y852.706271 Z-1.000000 I-0.145384 J-0.168841",
		"G03 X754.052760 Y850.523440 Z-1.000000 I13.471393 J-3.604013",
		"G03 X755.823297 Y842.166668 Z-1.000000 I13.890882 J-1.422907",
		"G03 X765.042868 Y830.271097 Z-1.000000 I46.909137 J26.836005",
		"G03 X777.129540 Y820.765300 Z-1.000000 I51.001074 J52.411108",
		"G03 X791.384800 Y814.195310 Z-1.000000 I32.403434 J51.557270",
		"G03 X800.254044 Y811.904592 Z-1.000000 I27.382820 J87.705882",
		"G03 X803.074250 Y811.791020 Z-1.000000 I1.756181 J8.536993",
		"G02 X804.695213 Y811.732122 Z-1.000000 I0.626483 J-5.093375",
		"G02 X805.501178 Y811.176469 Z-1.000000 I-0.238748 J-1.208643",
		"G02 X805.893145 Y810.103166 Z-1.000000 I-2.340719 J-1.463050",
		"G02 X806.218780 Y806.761720 Z-1.000000 I-37.337817 J-5.325279",
		"G02 X805.823086 Y802.597927 Z-1.000000 I-14.223027 J-0.749053",
		"G02 X805.125030 Y802.333980 Z-1.000000 I-0.440017 J0.108665",
		"G03 X804.410612 Y802.196022 Z-1.000000 I-0.289733 J-0.418403",
		"G03 X804.072300 Y800.740230 Z-1.000000 I1.792670 J-1.183805",
		"G02 X804.084659 Y797.154156 Z-1.000000 I-14.443764 J-1.842837",
		"G02 X803.256588 Y793.158307 Z-1.000000 I-24.049078 J2.900035",
		"G02 X801.699659 Y789.409221 Z-1.000000 I-18.576114 J5.516501",
		"G02 X800.261750 Y787.910160 Z-1.000000 I-3.469633 J1.888938",
		"G02 X797.656227 Y787.238432 Z-1.000000 I-2.437113 J4.064103",
		"G02 X791.113310 Y788.234380 Z-1.000000 I1.020350 J28.693175",
		"G02 X781.625590 Y791.053691 Z-1.000000 I58.538373 J214.370919",
		"G02 X774.053715 Y793.672660 Z-1.000000 I56.038639 J174.272309",
		"G02 X766.488381 Y796.840477 Z-1.000000 I34.215053 J92.329518",
		"G02 X766.056670 Y797.519530 Z-1.000000 I0.318198 J0.679053",
		"G03 X765.616192 Y798.555759 Z-1.000000 I-1.439109 J-0.000000",
		"G03 X762.695340 Y800.759770 Z-1.000000 I-10.561507 J-10.959148",
		"G02 X755.692960 Y806.657341 Z-1.000000 I13.272677 J22.864949",
		"G02 X746.634615 Y819.284330 Z-1.000000 I80.035402 J66.978401",
		"G02 X739.579030 Y833.141595 Z-1.000000 I86.482246 J52.758272",
		"G02 X737.943390 Y841.333980 Z-1.000000 I19.698665 J8.192385",
		"G02 X741.690998 Y854.023714 Z-1.000000 I23.358088 J-0.000000",
		"G02 X748.580110 Y857.771480 Z-1.000000 I6.889112 J-4.457871",
		"G03 X750.651477 Y858.274355 Z-1.000000 I0.000000 J4.517467",
		"G03 X755.527370 Y861.355470 Z-1.000000 I-15.180612 J29.422091",
		"G02 X759.960411 Y863.606285 Z-1.000000 I8.368374 J-10.990836",
		"G02 X761.293000 Y863.068360 Z-1.000000 I0.328574 J-1.105590",
		"G03 X762.058508 Y862.314736 Z-1.000000 I1.699872 J0.961077",
		"G03 X762.410515 Y862.403170 Z-1.000000 I0.128068 J0.235024",
		"G03 X762.608525 Y862.997675 Z-1.000000 I-1.027054 J0.672306",
		"G03 X762.533230 Y864.962890 Z-1.000000 I-9.664466 J0.613766",
		"G02 X762.761460 Y865.665073 Z-1.000000 I0.833139 J0.117387",
		"G02 X763.523470 Y865.988280 Z-1.000000 I0.762010 J-0.736672",
		"G03 X764.532052 Y866.279303 Z-1.000000 I0.000000 J1.893202",
		"G03 X766.273470 Y867.718750 Z-1.000000 I-4.681268 J7.436412",
		"G02 X767.986901 Y869.336887 Z-1.000000 I13.722222 J-12.814100",
		"G02 X771.250030 Y871.859380 Z-1.000000 I36.727743 J-44.139622",
		"G01 X774.611360 Y874.269530 Z-1.000000",
		"G01 X774.638660 Y869.787110 Z-1.000000",
		"G03 X776.876670 Y859.393332 Z-1.000000 I25.976956 J0.155577",
		"G03 X783.298820 Y850.300780 Z-1.000000 I26.016805 J11.561614",
		"G03 X790.543439 Y844.916645 Z-1.000000 I25.957631 J27.361212",
		"G03 X799.664113 Y840.542534 Z-1.000000 I34.884560 J61.043459",
		"G03 X808.426550 Y838.317734 Z-1.000000 I13.125496 J33.327196",
		"G03 X810.058580 Y839.761720 Z-1.000000 I0.177211 J1.443986",
		"G03 X809.576708 Y840.619173 Z-1.000000 I-1.003820 J-0.000000",
		"G03 X801.808580 Y844.783200 Z-1.000000 I-41.877870 J-68.796664",
		"G02 X790.101081 Y851.231675 Z-1.000000 I42.827773 J91.607755",
		"G02 X784.193323 Y856.186031 Z-1.000000 I16.254036 J25.381409",
		"G02 X780.313878 Y862.462401 Z-1.000000 I14.284717 J13.166563",
		"G02 X778.966790 Y869.566410 Z-1.000000 I19.397051 J7.357864",
		"G01 X778.890590 Y875.916020 Z-1.000000",
		"G01 X784.390590 Y877.779300 Z-1.000000",
		"G02 X789.946361 Y879.312867 Z-1.000000 I16.137295 J-47.631385",
		"G02 X793.150360 Y879.664060 Z-1.000000 I3.303849 J-15.350680",
		"G03 X796.005959 Y880.221983 Z-1.000000 I-0.049869 J7.842055",
		"G03 X796.839810 Y881.134770 Z-1.000000 I-0.525418 J1.317246",
		"G02 X797.776305 Y882.255988 Z-1.000000 I1.769627 J-0.526365",
		"G02 X804.363110 Y885.172375 Z-1.000000 I31.663475 J-62.617028",
		"G02 X811.108731 Y887.423214 Z-1.000000 I29.283397 J-76.526946",
		"G02 X814.525360 Y887.902340 Z-1.000000 I3.416629 J-11.942370",
		"G01 X814.525420 Y887.902340 Z-1.000000",
		"G00 X911.705080 Y603.638670",
		"G02 X911.668967 Y603.390943 Z-1.000000 I-0.948409 J0.011760 F400.000000",
		"G03 X911.623080 Y603.138670 Z-1.000000 I1.270856 J-0.361473",
		"G01 X912.072300 Y603.501950 Z-1.000000",
		"G01 X911.705080 Y603.638670 Z-1.000000",
		"G00 X911.623080 Y603.138670",
		"G01 X906.941440 Y599.337890 Z-1.000000 F400.000000",
		"G03 X902.579986 Y595.113700 Z-1.000000 I21.870911 J-26.945285",
		"G03 X902.585970 Y594.308590 Z-1.000000 I0.460836 J-0.399152",
		"G03 X904.674556 Y593.797850 Z-1.000000 I1.415697 J1.263425",
		"G03 X908.648470 Y596.609380 Z-1.000000 I-3.445213 J9.083781",
		"G03 X910.929488 Y600.321882 Z-1.000000 I-11.068869 J9.357881",
		"G03 X911.623080 Y603.138670 Z-1.000000 I-8.456512 J3.576076",
		"G01 X911.623080 Y603.138670 Z-1.000000",
		"G00 X898.296910 Y593.802730",
		"G03 X895.036767 Y593.826821 Z-1.000000 I-1.676236 J-6.235289 F400.000000",
		"G03 X892.414090 Y591.802730 Z-1.000000 I1.020728 J-4.033776",
		"G03 X892.669257 Y591.159075 Z-1.000000 I0.392845 J-0.216669",
		"G03 X895.089870 Y590.939450 Z-1.000000 I1.698092 J5.266356",
		"G03 X898.069747 Y591.871438 Z-1.000000 I-1.189946 J9.034481",
		"G03 X898.933620 Y592.917970 Z-1.000000 I-0.787190 J1.529610",
		"G03 X898.826430 Y593.457173 Z-1.000000 I-0.570441 J0.166856",
		"G03 X898.296910 Y593.802730 Z-1.000000 I-0.793973 J-0.638171",
		"G01 X898.296910 Y593.802730 Z-1.000000",
		"G00 X907.234410 Y561.535160",
		"G02 X913.246394 Y559.997126 Z-1.000000 I-3.657930 J-26.817449 F400.000000",
		"G02 X925.535526 Y554.650919 Z-1.000000 I-53.228754 J-139.152412",
		"G02 X937.192535 Y548.402209 Z-1.000000 I-79.180699 J-161.709563",
		"G02 X947.130890 Y541.857420 Z-1.000000 I-56.082030 J-95.979527",
		"G02 X953.360737 Y536.846841 Z-1.000000 I-57.293937 J-77.613978",
		"G02 X963.640547 Y527.529120 Z-1.000000 I-260.326951 J-297.536137",
		"G02 X973.646577 Y517.777268 Z-1.000000 I-256.534494 J-273.230296",
		"G02 X977.908230 Y512.960940 Z-1.000000 I-41.413020 J-40.937257",
		"G02 X978.923157 Y511.063189 Z-1.000000 I-4.766530 J-3.769435",
		"G02 X978.308379 Y509.665593 Z-1.000000 I-1.092181 J-0.353582",
		"G02 X975.643470 Y508.765068 Z-1.000000 I-5.687791 J12.438421",
		"G02 X962.234410 Y505.792970 Z-1.000000 I-320.584791 J1414.631169",
		"G03 X952.343289 Y503.336147 Z-1.000000 I35.891171 J-165.636285",
		"G03 X947.048836 Y501.111489 Z-1.000000 I6.258645 J-22.307366",
		"G03 X942.258944 Y497.680437 Z-1.000000 I14.781324 J-25.694322",
		"G03 X932.214870 Y488.523440 Z-1.000000 I247.975710 J-282.085194",
		"G03 X921.988789 Y478.335055 Z-1.000000 I189.054041 J-199.979679",
		"G03 X919.720730 Y475.404300 Z-1.000000 I13.125776 J-12.500786",
		"G01 X917.748080 Y472.232420 Z-1.000000",
		"G01 X917.669980 Y477.025390 Z-1.000000",
		"G03 X916.615802 Y486.063687 Z-1.000000 I-45.702419 J-0.749856",
		"G03 X912.795170 Y499.817569 Z-1.000000 I-126.501155 J-27.732618",
		"G03 X907.622520 Y513.064642 Z-1.000000 I-145.932518 J-49.349557",
		"G03 X901.314510 Y525.390620 Z-1.000000 I-115.971580 J-51.573153",
		"G02 X895.288210 Y537.588256 Z-1.000000 I85.606081 J49.881570",
		"G02 X895.125060 Y540.443360 Z-1.000000 I4.044568 J1.663334",
		"G02 X897.029386 Y546.072814 Z-1.000000 I57.004572 J-16.146624",
		"G02 X900.105447 Y553.284175 Z-1.000000 I135.943516 J-53.726014",
		"G02 X903.761563 Y560.533712 Z-1.000000 I99.509321 J-45.638241",
		"G02 X904.750060 Y561.429690 Z-1.000000 I1.801033 J-0.993734",
		"G02 X905.584496 Y561.617252 Z-1.000000 I0.918437 J-2.136065",
		"G02 X907.234440 Y561.535160 Z-1.000000 I0.348026 J-9.627039",
		"G01 X907.234410 Y561.535160 Z-1.000000",
		"G00 X883.056670 Y516.689450",
		"G02 X883.509451 Y516.727952 Z-1.000000 I0.242659 J-0.172070 F400.000000",
		"G02 X886.488310 Y513.265620 Z-1.000000 I-21.248560 J-21.294070",
		"G02 X894.635501 Y499.825808 Z-1.000000 I-70.772035 J-52.091201",
		"G02 X898.550810 Y488.634770 Z-1.000000 I-44.892973 J-21.986730",
		"G03 X900.411677 Y480.461894 Z-1.000000 I662.881919 J146.632099",
		"G03 X901.162140 Y477.482420 Z-1.000000 I89.572069 J20.976944",
		"G02 X901.398358 Y475.703929 Z-1.000000 I-6.610556 J-1.782941",
		"G02 X901.000500 Y474.974591 Z-1.000000 I-0.866394 J-0.000563",
		"G02 X900.044726 Y474.652680 Z-1.000000 I-1.117570 J1.738302",
		"G02 X896.548860 Y474.515620 Z-1.000000 I-3.495866 J44.514418",
		"G03 X887.965157 Y473.481597 Z-1.000000 I0.000000 J-36.144835",
		"G03 X875.451200 Y469.359380 Z-1.000000 I19.964198 J-81.661638",
		"G03 X870.960499 Y466.802773 Z-1.000000 I8.278453 J-19.763451",
		"G03 X867.966830 Y463.804690 Z-1.000000 I8.577752 J-11.558795",
		"G01 X865.373080 Y460.298830 Z-1.000000",
		"G01 X861.855500 Y465.507810 Z-1.000000",
		"G03 X858.198625 Y470.610091 Z-1.000000 I-93.118886 J-62.877976",
		"G03 X854.304720 Y475.445310 Z-1.000000 I-87.704179 J-66.644519",
		"G02 X851.110180 Y480.160304 Z-1.000000 I19.616782 J16.730610",
		"G02 X851.250030 Y481.242190 Z-1.000000 I0.838382 J0.441608",
		"G02 X852.345353 Y482.161126 Z-1.000000 I4.039542 J-3.702666",
		"G02 X858.339870 Y485.759770 Z-1.000000 I70.713951 J-111.001193",
		"G03 X868.968091 Y494.358923 Z-1.000000 I-20.322516 J35.985456",
		"G03 X879.123080 Y509.246090 Z-1.000000 I-53.807775 J47.611017",
		"G02 X882.652111 Y516.031970 Z-1.000000 I510.882953 J-261.376649",
		"G02 X883.056670 Y516.689450 Z-1.000000 I5.223282 J-2.760772",
		"G01 X883.056670 Y516.689450 Z-1.000000",
		"G00 X807.789090 Y563.994140",
		"G01 X809.359410 Y559.853520 Z-1.000000 F400.000000",
		"G02 X810.575945 Y555.627527 Z-1.000000 I-25.057082 J-9.501269",
		"G02 X811.834010 Y547.527340 Z-1.000000 I-91.222074 J-18.315775",
		"G03 X813.346468 Y538.797783 Z-1.000000 I71.601904 J7.909740",
		"G03 X817.013212 Y525.193293 Z-1.000000 I227.134122 J53.921843",
		"G03 X821.500218 Y511.827443 Z-1.000000 I221.765237 J67.011989",
		"G03 X825.007840 Y503.929690 Z-1.000000 I63.718032 J23.571242",
		"G03 X829.571575 Y496.910575 Z-1.000000 I39.149370 J20.461202",
		"G03 X832.058620 Y495.058590 Z-1.000000 I4.734081 J3.761505",
		"G02 X839.376089 Y489.632246 Z-1.000000 I-6.651522 J-16.616636",
		"G02 X849.291050 Y473.544920 Z-1.000000 I-60.073001 J-48.123332",
		"G03 X854.005376 Y464.897306 Z-1.000000 I64.164193 J29.370861",
		"G03 X859.603550 Y457.419920 Z-1.000000 I53.439611 J34.174886",
		"G02 X865.425772 Y450.065251 Z-1.000000 I-75.724251 J-65.927936",
		"G02 X869.666050 Y443.412110 Z-1.000000 I-57.789169 J-41.508851",
		"G02 X877.487123 Y426.935744 Z-1.000000 I-115.716534 J-65.023276",
		"G02 X882.291050 Y411.285160 Z-1.000000 I-99.537066 J-39.115352",
		"G02 X887.364273 Y387.235865 Z-1.000000 I-902.275019 J-202.895565",
		"G02 X890.119170 Y371.566406 Z-1.000000 I-379.276081 J-74.758631",
		"G02 X891.474418 Y356.734162 Z-1.000000 I-118.114988 J-18.270412",
		"G02 X891.376140 Y335.479680 Z-1.000000 I-318.165474 J-9.156308",
		"G02 X889.756058 Y314.086103 Z-1.000000 I-285.430141 J10.856774",
		"G02 X887.550810 Y304.035156 Z-1.000000 I-49.798851 J5.658821",
		"G03 X885.816910 Y298.250882 Z-1.000000 I103.964249 J-34.316447",
		"G03 X884.425810 Y292.462891 Z-1.000000 I103.425781 J-27.918769",
		"G02 X882.832369 Y287.539364 Z-1.000000 I-24.174214 J5.104084",
		"G02 X882.156280 Y287.349609 Z-1.000000 I-0.416596 J0.185001",
		"G03 X881.270461 Y287.426221 Z-1.000000 I-0.509899 J-0.736260",
		"G03 X880.236360 Y286.462891 Z-1.000000 I0.952073 J-2.058717",
		"G03 X880.291301 Y285.900879 Z-1.000000 I0.436888 J-0.240982",
		"G03 X880.761750 Y285.837891 Z-1.000000 I0.269803 J0.226772",
		"G02 X881.943704 Y285.083928 Z-1.000000 I0.432790 J-0.624964",
		"G02 X878.953150 Y275.333984 Z-1.000000 I-39.644906 J6.826483",
		"G02 X873.869378 Y266.709477 Z-1.000000 I-42.347232 J19.151265",
		"G02 X857.172831 Y245.474083 Z-1.000000 I-420.382766 J313.348676",
		"G02 X839.219653 Y224.967736 Z-1.000000 I-388.076287 J321.646213",
		"G02 X833.312530 Y220.267578 Z-1.000000 I-18.239343 J16.860979",
		"G02 X829.814585 Y218.639612 Z-1.000000 I-11.420825 J19.967537",
		"G02 X826.243660 Y217.577051 Z-1.000000 I-10.286646 J28.038452",
		"G02 X822.816983 Y217.188519 Z-1.000000 I-3.411540 J14.783112",
		"G02 X822.287140 Y217.722656 Z-1.000000 I0.000529 J0.530385",
		"G02 X822.549177 Y220.015769 Z-1.000000 I10.726424 J-0.064196",
		"G02 X823.704632 Y224.430423 Z-1.000000 I64.684724 J-14.571510",
		"G02 X825.224646 Y228.813718 Z-1.000000 I51.238278 J-15.312915",
		"G02 X825.699250 Y229.140625 Z-1.000000 I0.481192 J-0.190627",
		"G03 X826.173488 Y229.548224 Z-1.000000 I-0.005555 J0.486148",
		"G03 X826.875030 Y236.330078 Z-1.000000 I-56.862610 J9.309308",
		"G02 X827.624997 Y243.194952 Z-1.000000 I53.138043 J-2.331767",
		"G02 X828.203150 Y243.892578 Z-1.000000 I0.900777 J-0.158130",
		"G03 X828.874894 Y244.888382 Z-1.000000 I-0.366330 J0.971590",
		"G03 X828.306455 Y250.708228 Z-1.000000 I-39.513284 J-0.921677",
		"G03 X826.938698 Y256.489234 Z-1.000000 I-48.074930 J-8.321981",
		"G03 X825.613310 Y258.853516 Z-1.000000 I-6.006884 J-1.813747",
		"G03 X823.177851 Y260.342811 Z-1.000000 I-3.683089 J-3.286979",
		"G03 X821.559755 Y260.137455 Z-1.000000 I-0.553270 J-2.118072",
		"G03 X820.489653 Y258.862275 Z-1.000000 I1.135303 J-2.039313",
		"G03 X820.322300 Y255.908203 Z-1.000000 I5.112569 J-1.771412",
		"G02 X820.517812 Y252.586521 Z-1.000000 I-10.091420 J-2.260570",
		"G02 X819.158230 Y244.623047 Z-1.000000 I-60.639998 J6.255106",
		"G03 X816.849060 Y234.522501 Z-1.000000 I472.429309 J-113.320213",
		"G03 X814.449250 Y222.966797 Z-1.000000 I618.092584 J-134.388328",
		"G02 X812.501062 Y213.390267 Z-1.000000 I-910.127014 J180.164041",
		"G02 X811.730500 Y209.955078 Z-1.000000 I-117.221002 J24.490309",
		"G02 X810.430112 Y206.222197 Z-1.000000 I-19.277499 J4.622577",
		"G02 X808.486985 Y202.713188 Z-1.000000 I-25.767066 J11.976097",
		"G02 X806.064808 Y199.811673 Z-1.000000 I-13.320098 J8.657810",
		"G02 X805.089870 Y199.738281 Z-1.000000 I-0.532772 J0.565107",
		"G03 X804.683430 Y199.573401 Z-1.000000 I-0.149235 J-0.215517",
		"G03 X804.779330 Y198.160156 Z-1.000000 I2.681478 J-0.527916",
		"G02 X804.718417 Y196.955115 Z-1.000000 I-1.536903 J-0.526372",
		"G02 X804.154330 Y196.800780 Z-1.000000 I-0.346783 J0.159453",
		"G03 X803.251292 Y196.785213 Z-1.000000 I-0.440683 J-0.636388",
		"G03 X802.724640 Y195.736330 Z-1.000000 I0.781155 J-1.048883",
		"G02 X802.348964 Y193.878855 Z-1.000000 I-4.779839 J-0.000000",
		"G02 X801.501980 Y192.835940 Z-1.000000 I-2.216805 J0.934947",
		"G02 X800.689738 Y193.243652 Z-1.000000 I-0.294772 J0.425686",
		"G02 X801.388700 Y196.101560 Z-1.000000 I5.422110 J0.188335",
		"G03 X801.801775 Y198.051061 Z-1.000000 I-2.882772 J1.629336",
		"G03 X800.759800 Y202.564453 Z-1.000000 I-17.775633 J-1.726761",
		"G02 X799.463508 Y207.202428 Z-1.000000 I26.774254 J9.983421",
		"G02 X799.056670 Y211.513672 Z-1.000000 I22.639640 J4.311244",
		"G03 X798.393307 Y215.612644 Z-1.000000 I-12.995623 J0.000000",
		"G03 X795.417745 Y222.979288 Z-1.000000 I-59.881587 J-19.903321",
		"G03 X791.588137 Y229.906074 Z-1.000000 I-61.391495 J-29.419450",
		"G03 X788.277370 Y234.007812 Z-1.000000 I-19.273721 J-12.169984",
		"G03 X784.805567 Y236.941899 Z-1.000000 I-21.483278 J-21.899344",
		"G03 X781.640650 Y238.820312 Z-1.000000 I-11.777497 J-16.238296",
		"G02 X778.714173 Y240.694052 Z-1.000000 I5.949570 J12.514473",
		"G02 X778.279330 Y241.623047 Z-1.000000 I0.774928 J0.928995",
		"G03 X777.796446 Y242.343939 Z-1.000000 I-0.779547 J-0.000000",
		"G03 X775.357450 Y242.826172 Z-1.000000 I-2.438996 J-5.926750",
		"G02 X772.660174 Y243.401974 Z-1.000000 I-0.000000 J6.605431",
		"G02 X769.601590 Y245.400391 Z-1.000000 I4.737651 J10.590789",
		"G02 X767.734392 Y248.433789 Z-1.000000 I4.297841 J4.736895",
		"G02 X766.289090 Y257.390625 Z-1.000000 I39.973166 J11.045216",
		"G02 X766.886500 Y269.880412 Z-1.000000 I63.698097 J3.212381",
		"G02 X769.804330 Y280.721123 Z-1.000000 I47.597296 J-6.998013",
		"G02 X775.089789 Y290.693117 Z-1.000000 I48.839262 J-19.499564",
		"G02 X784.087920 Y301.968750 Z-1.000000 I76.048509 J-51.459756",
		"G02 X791.757431 Y308.423721 Z-1.000000 I30.824917 J-28.841033",
		"G02 X797.144560 Y310.535156 Z-1.000000 I7.718938 J-11.766049",
		"G03 X802.942395 Y312.031959 Z-1.000000 I-5.764514 J34.306018",
		"G03 X807.507840 Y314.187500 Z-1.000000 I-8.216649 J23.315487",
		"G02 X812.048502 Y316.421221 Z-1.000000 I15.190238 J-25.146458",
		"G02 X816.453150 Y317.666016 Z-1.000000 I8.698422 J-22.363741",
		"G03 X819.998324 Y318.977155 Z-1.000000 I-1.949653 J10.720097",
		"G03 X820.248080 Y319.882812 Z-1.000000 I-0.339190 J0.580806",
		"G03 X819.181936 Y320.537644 Z-1.000000 I-1.110284 J-0.612352",
		"G03 X809.681297 Y320.477645 Z-1.000000 I-4.021415 J-115.448787",
		"G03 X800.416941 Y319.732471 Z-1.000000 I6.718793 J-141.493303",
		"G03 X795.695340 Y318.716797 Z-1.000000 I2.762029 J-24.322518",
		"G02 X793.691767 Y318.856911 Z-1.000000 I-0.826651 J2.574424",
		"G02 X792.945340 Y320.046875 Z-1.000000 I0.575315 J1.189964",
		"G02 X793.305842 Y321.944686 Z-1.000000 I5.175626 J-0.000000",
		"G02 X793.863310 Y322.376953 Z-1.000000 I0.642767 J-0.253337",
		"G03 X795.206635 Y322.602139 Z-1.000000 I-1.993714 J16.012621",
		"G03 X805.169950 Y324.767578 Z-1.000000 I-184.770227 J874.142640",
		"G03 X813.586638 Y327.686097 Z-1.000000 I-8.529534 J38.193901",
		"G03 X817.919447 Y331.099483 Z-1.000000 I-5.486784 J11.421330",
		"G03 X820.391048 Y336.030296 Z-1.000000 I-9.816558 J8.005469",
		"G03 X821.554720 Y344.859375 Z-1.000000 I-37.928492 J9.490199",
		"G02 X822.336459 Y351.515564 Z-1.000000 I33.557497 J-0.567174",
		"G02 X823.859410 Y355.318359 Z-1.000000 I12.251087 J-2.699985",
		"G03 X825.454114 Y359.244653 Z-1.000000 I-11.437642 J6.932512",
		"G03 X826.369437 Y365.616408 Z-1.000000 I-37.366267 J8.619411",
		"G03 X826.243031 Y372.050799 Z-1.000000 I-41.072851 J2.411542",
		"G03 X825.130890 Y376.974609 Z-1.000000 I-20.274533 J-1.991904",
		"G01 X823.425810 Y381.697266 Z-1.000000",
		"G01 X835.027370 Y394.591797 Z-1.000000",
		"G03 X843.417126 Y404.589307 Z-1.000000 I-140.702074 J126.594045",
		"G03 X846.770267 Y409.843770 Z-1.000000 I-25.693474 J20.093456",
		"G03 X848.521325 Y415.461333 Z-1.000000 I-15.473683 J7.905013",
		"G03 X848.476590 Y420.917970 Z-1.000000 I-19.351291 J2.569857",
		"G02 X847.834200 Y425.673318 Z-1.000000 I167.928987 J25.106250",
		"G02 X847.552760 Y428.572270 Z-1.000000 I61.885967 J7.471228",
		"G03 X846.830193 Y430.642446 Z-1.000000 I-4.214051 J-0.309669",
		"G03 X845.970730 Y430.892580 Z-1.000000 I-0.581481 J-0.396346",
		"G02 X844.822456 Y431.211286 Z-1.000000 I-0.375787 J0.873992",
		"G02 X842.119370 Y435.710281 Z-1.000000 I25.943047 J18.648634",
		"G02 X840.113746 Y440.692590 Z-1.000000 I31.673262 J15.644876",
		"G02 X840.392610 Y442.185550 Z-1.000000 I1.534671 J0.485869",
		"G03 X840.309325 Y442.399753 Z-1.000000 I-0.103536 J0.083036",
		"G03 X839.759800 Y442.283150 Z-1.000000 I-0.120932 J-0.783276",
		"G02 X838.452659 Y442.331648 Z-1.000000 I-0.618756 J0.962577",
		"G02 X837.040242 Y444.058230 Z-1.000000 I2.826503 J3.753197",
		"G02 X836.280111 Y446.540825 Z-1.000000 I7.897423 J3.775736",
		"G02 X836.457060 Y449.191360 Z-1.000000 I6.159263 J0.919982",
		"G03 X836.790905 Y450.556120 Z-1.000000 I-16.190705 J4.683751",
		"G03 X836.757300 Y450.723578 Z-1.000000 I-0.216922 J0.043569",
		"G03 X836.653861 Y450.729293 Z-1.000000 I-0.053890 J-0.036419",
		"G03 X836.007840 Y449.855420 Z-1.000000 I6.082814 J-5.172518",
		"G02 X834.580628 Y449.678298 Z-1.000000 I-0.786982 J0.502690",
		"G02 X831.880527 Y453.380959 Z-1.000000 I8.526417 J9.053572",
		"G02 X829.824860 Y458.747554 Z-1.000000 I32.880108 J15.671691",
		"G02 X828.378940 Y465.861280 Z-1.000000 I40.384005 J11.912174",
		"G03 X827.504788 Y471.556747 Z-1.000000 I-75.207598 J-8.628205",
		"G03 X827.283230 Y471.546830 Z-1.000000 I-0.110052 J-0.021206",
		"G02 X826.815618 Y471.407668 Z-1.000000 I-0.262428 J0.026593",
		"G02 X825.347690 Y473.943310 Z-1.000000 I7.394659 J5.973623",
		"G03 X823.464935 Y477.051075 Z-1.000000 I-9.746512 J-3.780471",
		"G03 X821.699170 Y478.251599 Z-1.000000 I-3.191949 J-2.795979",
		"G03 X819.752928 Y478.248626 Z-1.000000 I-0.968976 J-2.715298",
		"G03 X817.668000 Y476.683540 Z-1.000000 I1.504399 J-4.175344",
		"G03 X817.087949 Y474.059101 Z-1.000000 I2.718821 J-1.977232",
		"G03 X820.372415 Y461.632290 Z-1.000000 I97.592459 J19.146701",
		"G03 X824.870799 Y449.771865 Z-1.000000 I152.483478 J51.050168",
		"G03 X830.779330 Y437.521430 Z-1.000000 I132.000372 J56.115258",
		"G02 X836.525572 Y426.167645 Z-1.000000 I-197.593216 J-107.134509",
		"G02 X838.448962 Y420.937120 Z-1.000000 I-28.736604 J-13.536048",
		"G02 X838.864572 Y415.807554 Z-1.000000 I-13.596858 J-3.683269",
		"G02 X837.742220 Y411.816360 Z-1.000000 I-12.098309 J1.248727",
		"G02 X834.590553 Y406.714213 Z-1.000000 I-25.116613 J11.990390",
		"G02 X826.922275 Y397.541250 Z-1.000000 I-134.823209 J104.915838",
		"G02 X818.392256 Y388.802330 Z-1.000000 I-108.208105 J97.088904",
		"G02 X816.375030 Y388.001903 Z-1.000000 I-2.017226 J2.141681",
		"G03 X813.347308 Y384.358314 Z-1.000000 I0.000000 J-3.079771",
		"G03 X818.173860 Y378.341747 Z-1.000000 I8.026468 J1.494676",
		"G02 X820.053018 Y376.547654 Z-1.000000 I-1.447195 J-3.396987",
		"G02 X821.720730 Y370.980419 Z-1.000000 I-16.637905 J-8.017431",
		"G02 X822.029396 Y367.040772 Z-1.000000 I-35.723816 J-4.780825",
		"G02 X821.666492 Y364.647492 Z-1.000000 I-6.995997 J-0.163321",
		"G02 X820.565313 Y362.393490 Z-1.000000 I-9.162599 J3.080345",
		"G02 X817.250030 Y357.681591 Z-1.000000 I-84.093570 J55.645817",
		"G02 X813.582092 Y352.946148 Z-1.000000 I-138.447814 J103.449430",
		"G02 X812.608997 Y352.076449 Z-1.000000 I-3.096825 J2.485754",
		"G02 X811.901483 Y352.243381 Z-1.000000 I-0.272039 J0.429816",
		"G02 X811.253940 Y353.886669 Z-1.000000 I4.528377 J2.733649",
		"G03 X809.936734 Y356.418607 Z-1.000000 I-5.196626 J-1.094875",
		"G03 X808.169950 Y357.185497 Z-1.000000 I-1.766784 J-1.651741",
		"G03 X806.683501 Y356.224595 Z-1.000000 I0.000000 J-1.630168",
		"G03 X805.396510 Y351.023388 Z-1.000000 I13.608358 J-6.127095",
		"G02 X804.105557 Y345.297520 Z-1.000000 I-18.421500 J1.144845",
		"G02 X801.730500 Y341.884716 Z-1.000000 I-8.590366 J3.445421",
		"G03 X799.778384 Y339.957259 Z-1.000000 I21.553753 J-23.781770",
		"G03 X799.379415 Y339.266372 Z-1.000000 I1.307654 J-1.215777",
		"G03 X799.529204 Y338.673792 Z-1.000000 I0.562559 J-0.173021",
		"G03 X800.533230 Y337.994091 Z-1.000000 I2.347250 J2.385858",
		"G03 X802.666132 Y337.607788 Z-1.000000 I1.877529 J4.285066",
		"G03 X808.189480 Y338.412059 Z-1.000000 I-1.695259 J31.010209",
		"G02 X813.885629 Y338.812244 Z-1.000000 I4.053881 J-16.963105",
		"G02 X814.658227 Y337.967901 Z-1.000000 I-0.080314 J-0.849136",
		"G02 X814.031187 Y336.518541 Z-1.000000 I-1.963069 J-0.011032",
		"G02 X808.224680 Y332.199169 Z-1.000000 I-22.644616 J24.378515",
		"G02 X798.893247 Y328.831882 Z-1.000000 I-13.697802 J23.346096",
		"G02 X795.130930 Y330.058544 Z-1.000000 I-0.694937 J4.251613",
		"G03 X793.441672 Y331.064379 Z-1.000000 I-2.771307 J-2.732859",
		"G03 X792.945380 Y330.691356 Z-1.000000 I-0.107960 J-0.373023",
		"G02 X792.297176 Y329.515260 Z-1.000000 I-1.391051 J-0.000000",
		"G02 X791.228757 Y329.566417 Z-1.000000 I-0.497704 J0.787980",
		"G02 X789.757951 Y331.040057 Z-1.000000 I5.020203 J6.481358",
		"G02 X786.587960 Y335.615184 Z-1.000000 I43.070372 J33.228152",
		"G02 X784.244729 Y339.828114 Z-1.000000 I43.467544 J26.934764",
		"G02 X783.087480 Y343.204254 Z-1.000000 I12.311710 J6.106525",
		"G02 X782.630212 Y346.951568 Z-1.000000 I23.565796 J4.777184",
		"G02 X782.361400 Y356.460888 Z-1.000000 I325.882286 J13.970591",
		"G03 X781.294909 Y370.297770 Z-1.000000 I-109.768442 J-1.500968",
		"G03 X780.336470 Y371.400246 Z-1.000000 I-1.329003 J-0.187522",
		"G03 X779.239476 Y370.836137 Z-1.000000 I-0.248569 J-0.865310",
		"G03 X775.793040 Y358.347606 Z-1.000000 I85.014065 J-30.180985",
		"G03 X775.319145 Y352.330255 Z-1.000000 I24.970124 J-4.993851",
		"G03 X776.433660 Y342.433544 Z-1.000000 I69.224682 J2.784602",
		"G02 X778.271066 Y332.533858 Z-1.000000 I-8924.443608 J-1661.519053",
		"G02 X778.277410 Y332.470653 Z-1.000000 I-0.377057 J-0.069767",
		"G01 X778.246338 Y332.439135 Z-1.000000",
		"G02 X775.291090 Y332.470653 Z-1.000000 I-0.000000 J138.564055",
		"G02 X772.648915 Y333.320228 Z-1.000000 I0.103395 J4.854925",
		"G02 X768.239040 Y337.034097 Z-1.000000 I16.486866 J24.051717",
		"G02 X764.586621 Y341.519140 Z-1.000000 I23.472145 J22.844372",
		"G02 X763.072340 Y345.025341 Z-1.000000 I8.491379 J5.747411",
		"G03 X761.832779 Y347.647962 Z-1.000000 I-6.451716 J-1.445108",
		"G03 X760.531320 Y348.283153 Z-1.000000 I-1.301459 J-1.015699",
		"G02 X758.705591 Y350.179648 Z-1.000000 I0.000000 J1.827049",
		"G02 X760.529347 Y363.832258 Z-1.000000 I72.720113 J-2.766055",
		"G02 X764.648039 Y377.809470 Z-1.000000 I120.400025 J-27.883064",
		"G02 X769.832100 Y388.445262 Z-1.000000 I51.648305 J-18.592944",
		"G02 X774.515036 Y393.694721 Z-1.000000 I17.921259 J-11.273686",
		"G02 X779.318430 Y396.044872 Z-1.000000 I7.645526 J-9.542610",
		"G02 X784.710136 Y396.355684 Z-1.000000 I3.564655 J-14.915868",
		"G02 X789.207100 Y394.857372 Z-1.000000 I-1.405984 J-11.717505",
		"G03 X799.827464 Y389.107756 Z-1.000000 I109.367675 J189.334287",
		"G03 X802.507882 Y388.324492 Z-1.000000 I3.441338 J6.798664",
		"G03 X803.670505 Y389.320732 Z-1.000000 I0.105696 J1.053168",
		"G03 X803.105540 Y393.462841 Z-1.000000 I-11.088049 J0.597224",
		"G02 X802.515991 Y395.944403 Z-1.000000 I13.202018 J4.447240",
		"G02 X802.674630 Y397.651933 Z-1.000000 I3.657657 J0.521317",
		"G02 X803.515455 Y399.269014 Z-1.000000 I5.676337 J-1.924353",
		"G02 X806.248120 Y402.732370 Z-1.000000 I49.605328 J-36.329966",
		"G03 X810.142028 Y408.336191 Z-1.000000 I-26.961007 J22.889085",
		"G03 X812.419990 Y413.746040 Z-1.000000 I-22.888153 J12.822195",
		"G03 X813.086325 Y419.425116 Z-1.000000 I-16.244424 J4.784613",
		"G03 X811.814520 Y425.173780 Z-1.000000 I-17.910096 J-0.947321",
		"G03 X809.308437 Y430.770210 Z-1.000000 I-74.047581 J-29.799207",
		"G03 X805.878980 Y437.171830 Z-1.000000 I-100.571122 J-49.758238",
		"G02 X802.428537 Y443.565327 Z-1.000000 I107.369983 J62.073266",
		"G02 X799.619210 Y449.632760 Z-1.000000 I93.469340 J46.962023",
		"G03 X794.190620 Y458.275028 Z-1.000000 I-29.629682 J-12.585609",
		"G03 X791.777410 Y458.572220 Z-1.000000 I-1.373246 J-1.204536",
		"G03 X790.971790 Y457.293632 Z-1.000000 I1.097203 J-1.584431",
		"G03 X791.220582 Y453.574423 Z-1.000000 I8.206164 J-1.318983",
		"G03 X792.799989 Y449.242822 Z-1.000000 I38.955322 J11.750308",
		"G03 X798.347730 Y436.787060 Z-1.000000 I493.577324 J212.373801",
		"G01 X806.191480 Y419.757760 Z-1.000000",
		"G01 X799.257880 Y411.412060 Z-1.000000",
		"G02 X791.914202 Y403.940219 Z-1.000000 I-48.364187 J40.189826",
		"G02 X789.564520 Y403.068310 Z-1.000000 I-2.349682 J2.730092",
		"G02 X788.000760 Y403.300753 Z-1.000000 I0.000000 J5.376327",
		"G02 X787.363605 Y403.889213 Z-1.000000 I0.289652 J0.952790",
		"G02 X787.202551 Y404.913555 Z-1.000000 I2.225038 J0.874668",
		"G02 X787.541090 Y408.214790 Z-1.000000 I41.886427 J-2.627445",
		"G03 X787.485371 Y413.305452 Z-1.000000 I-16.567356 J2.364300",
		"G03 X784.658270 Y425.677680 Z-1.000000 I-102.689842 J-16.955902",
		"G02 X781.228238 Y438.035744 Z-1.000000 I412.862477 J121.246736",
		"G02 X780.183660 Y442.785110 Z-1.000000 I59.806025 J15.643324",
		"G03 X776.041404 Y460.016928 Z-1.000000 I-148.576507 J-26.601664",
		"G03 X768.884840 Y480.013620 Z-1.000000 I-207.320991 J-62.918600",
		"G02 X765.349550 Y490.604781 Z-1.000000 I73.156535 J30.304993",
		"G02 X765.296950 Y493.970650 Z-1.000000 I6.972538 J1.792309",
		"G02 X767.644398 Y499.323868 Z-1.000000 I14.876276 J-3.332118",
		"G02 X775.038650 Y508.720020 Z-1.000000 I81.175647 J-56.273237",
		"G02 X783.599562 Y517.073969 Z-1.000000 I63.564644 J-56.575934",
		"G02 X786.613350 Y517.919870 Z-1.000000 I2.431099 J-2.869823",
		"G03 X787.446504 Y517.860649 Z-1.000000 I0.766043 J4.886860",
		"G03 X788.880930 Y517.951070 Z-1.000000 I-0.197508 J14.556208",
		"G03 X790.203210 Y518.796904 Z-1.000000 I-0.199756 J1.768744",
		"G03 X794.092697 Y525.990746 Z-1.000000 I-70.755272 J42.903559",
		"G03 X797.269987 Y533.534195 Z-1.000000 I-77.641564 J37.143375",
		"G03 X797.291090 Y535.531150 Z-1.000000 I-2.807527 J1.028258",
		"G02 X797.279514 Y537.263871 Z-1.000000 I2.580226 J0.883637",
		"G02 X802.240300 Y550.638570 Z-1.000000 I175.671827 J-57.550779",
		"G01 X807.789090 Y563.994140 Z-1.000000",
		"G00 X992.179720 Y594.734380",
		"G02 X994.750578 Y594.479039 Z-1.000000 I-0.175587 J-14.837682 F400.000000",
		"G02 X994.802642 Y594.140771 Z-1.000000 I-0.033583 J-0.178310",
		"G02 X993.511196 Y593.607414 Z-1.000000 I-3.305347 J6.173206",
		"G02 X986.068390 Y591.462890 Z-1.000000 I-99.575824 J331.601091",
		"G02 X967.809490 Y587.309407 Z-1.000000 I-54.565357 J197.661578",
		"G02 X940.021510 Y583.177730 Z-1.000000 I-81.847639 J454.962767",
		"G03 X933.820640 Y582.004323 Z-1.000000 I5.290170 J-44.926845",
		"G03 X929.250030 Y580.349610 Z-1.000000 I6.829470 J-26.003983",
		"G03 X925.976690 Y577.601482 Z-1.000000 I3.517567 J-7.513361",
		"G03 X925.183620 Y574.554690 Z-1.000000 I3.685028 J-2.585813",
		"G03 X926.493022 Y571.562786 Z-1.000000 I5.325332 J0.548142",
		"G03 X929.843780 Y569.267580 Z-1.000000 I6.212864 J5.476646",
		"G02 X937.828409 Y565.264208 Z-1.000000 I-13.820467 J-37.528845",
		"G02 X948.972281 Y557.127439 Z-1.000000 I-69.666440 J-107.112590",
		"G02 X959.178702 Y547.745061 Z-1.000000 I-74.783661 J-91.594427",
		"G02 X963.140650 Y542.082030 Z-1.000000 I-15.754500 J-15.239539",
		"G02 X963.559392 Y540.808511 Z-1.000000 I-4.497466 J-2.184402",
		"G02 X963.367959 Y540.645921 Z-1.000000 I-0.142361 J-0.026377",
		"G02 X962.796883 Y540.968142 Z-1.000000 I0.664897 J1.845576",
		"G02 X960.296910 Y543.107420 Z-1.000000 I39.593485 J48.799568",
		"G03 X942.015966 Y556.520998 Z-1.000000 I-75.333691 J-83.505866",
		"G03 X918.992220 Y567.421880 Z-1.000000 I-72.011803 J-122.331428",
		"G03 X909.019131 Y570.954126 Z-1.000000 I-152.731137 J-415.382113",
		"G03 X905.701460 Y571.831994 Z-1.000000 I-7.940420 J-23.300654",
		"G03 X902.789082 Y571.730655 Z-1.000000 I-1.233468 J-6.451447",
		"G03 X900.978550 Y570.865230 Z-1.000000 I1.376516 J-5.206368",
		"G03 X895.816843 Y564.709972 Z-1.000000 I10.013446 J-13.639018",
		"G03 X887.517610 Y543.568360 Z-1.000000 I121.264709 J-59.802756",
		"G01 X884.283230 Y532.728520 Z-1.000000",
		"G01 X887.998080 Y529.187500 Z-1.000000",
		"G02 X891.382946 Y525.386214 Z-1.000000 I-21.522825 J-22.572735",
		"G02 X895.193390 Y519.640620 Z-1.000000 I-45.732296 J-34.465719",
		"G03 X898.687153 Y513.643488 Z-1.000000 I1386.424456 J803.676259",
		"G03 X900.689480 Y510.253910 Z-1.000000 I435.139699 J254.764059",
		"G02 X902.334558 Y506.712555 Z-1.000000 I-16.374010 J-9.759052",
		"G02 X906.568390 Y493.216800 Z-1.000000 I-236.457815 J-81.592542",
		"G02 X909.914866 Y479.527075 Z-1.000000 I-188.630546 J-53.364940",
		"G02 X910.005890 Y476.582030 Z-1.000000 I-8.386078 J-1.733121",
		"G02 X909.627314 Y476.193047 Z-1.000000 I-0.456144 J0.065226",
		"G02 X909.159112 Y476.434960 Z-1.000000 I-0.073679 J0.431438",
		"G02 X908.626560 Y477.790559 Z-1.000000 I8.938004 J4.293738",
		"G02 X907.490260 Y481.818360 Z-1.000000 I77.304022 J23.982748",
		"G03 X901.209253 Y503.421489 Z-1.000000 I-322.325066 J-81.999835",
		"G03 X898.683620 Y508.683590 Z-1.000000 I-21.275440 J-6.974340",
		"G02 X895.707007 Y513.351086 Z-1.000000 I220.274841 J143.759281",
		"G02 X891.421910 Y520.337890 Z-1.000000 I494.869749 J308.317441",
		"G03 X885.788031 Y527.747126 Z-1.000000 I-37.311980 J-22.524926",
		"G03 X883.010454 Y528.575740 Z-1.000000 I-2.106092 J-1.990148",
		"G03 X880.338039 Y526.714243 Z-1.000000 I1.031450 J-4.329821",
		"G03 X874.341830 Y516.269530 Z-1.000000 I75.205088 J-50.118055",
		"G02 X863.198620 Y498.842441 Z-1.000000 I-81.388612 J39.765284",
		"G02 X851.490260 Y488.203120 Z-1.000000 I-41.082963 J33.448909",
		"G01 X847.085970 Y485.267580 Z-1.000000",
		"G01 X844.816440 Y488.078120 Z-1.000000",
		"G02 X843.699210 Y490.729636 Z-1.000000 I4.041136 J3.263886",
		"G02 X844.332060 Y492.154300 Z-1.000000 I1.515109 J0.179865",
		"G02 X846.182185 Y493.217428 Z-1.000000 I6.500690 J-9.171509",
		"G02 X847.162140 Y493.439450 Z-1.000000 I1.027390 J-2.261008",
		"G03 X848.050964 Y493.811829 Z-1.000000 I-0.026146 J1.309356",
		"G03 X857.326200 Y503.365230 Z-1.000000 I-174.526353 J178.723975",
		"G03 X865.488333 Y513.896667 Z-1.000000 I-57.983203 J53.367111",
		"G03 X872.449250 Y526.716800 Z-1.000000 I-79.324406 J51.370430",
		"G03 X876.659712 Y537.535697 Z-1.000000 I-108.132201 J48.311279",
		"G03 X879.854350 Y548.597471 Z-1.000000 I-133.359712 J44.506451",
		"G03 X881.993683 Y560.036286 Z-1.000000 I-102.245741 J25.041867",
		"G03 X881.890650 Y565.265620 Z-1.000000 I-17.435682 J2.272149",
		"G02 X882.640537 Y567.778770 Z-1.000000 I2.917888 J0.497798",
		"G02 X888.181670 Y572.033200 Z-1.000000 I15.383504 J-14.300358",
		"G02 X896.522791 Y575.747428 Z-1.000000 I31.128925 J-58.683852",
		"G02 X922.509800 Y584.570310 Z-1.000000 I206.009311 J-564.099366",
		"G02 X933.034224 Y587.166856 Z-1.000000 I25.987081 J-82.704600",
		"G02 X948.789090 Y589.333980 Z-1.000000 I33.280290 J-183.593721",
		"G03 X964.641336 Y591.107763 Z-1.000000 I-43.136989 J457.236387",
		"G03 X975.679720 Y592.816410 Z-1.000000 I-28.957649 J223.585306",
		"G02 X986.708946 Y594.449995 Z-1.000000 I31.596255 J-175.274667",
		"G02 X992.179720 Y594.734380 Z-1.000000 I4.960992 J-42.672202",
		"G01 X992.179720 Y594.734380 Z-1.000000",
		"G00 X1007.578130 Y584.773440",
		"G02 X1020.552840 Y583.749772 Z-1.000000 I-3.177759 J-123.014440 F400.000000",
		"G02 X1021.502030 Y582.650390 Z-1.000000 I-0.143435 J-1.083290",
		"G02 X1021.143042 Y582.011942 Z-1.000000 I-0.776764 J0.016610",
		"G02 X1014.203130 Y578.000000 Z-1.000000 I-49.480885 J77.584364",
		"G03 X1005.957002 Y572.238831 Z-1.000000 I18.041314 J-34.605098",
		"G03 X999.376210 Y564.197689 Z-1.000000 I24.074354 J-26.415616",
		"G03 X994.817013 Y554.502755 Z-1.000000 I45.842410 J-27.477610",
		"G03 X989.597690 Y536.951170 Z-1.000000 I172.262704 J-60.777665",
		"G02 X986.453231 Y525.270810 Z-1.000000 I-244.169014 J59.469070",
		"G02 X986.068390 Y524.990230 Z-1.000000 I-0.375524 J0.110853",
		"G02 X985.592711 Y525.246321 Z-1.000000 I0.016052 J0.599641",
		"G02 X979.134800 Y534.914060 Z-1.000000 I227.955004 J159.261426",
		"G03 X963.048787 Y556.397931 Z-1.000000 I-151.596318 J-96.743368",
		"G03 X951.373080 Y566.734380 Z-1.000000 I-45.749279 J-39.914388",
		"G02 X943.582555 Y572.366562 Z-1.000000 I89.613933 J132.159502",
		"G02 X943.291050 Y572.957030 Z-1.000000 I0.455965 J0.592293",
		"G02 X943.647893 Y573.392483 Z-1.000000 I0.441481 J0.002156",
		"G02 X959.484410 Y576.121090 Z-1.000000 I69.140519 J-353.962599",
		"G03 X975.602252 Y578.907576 Z-1.000000 I-52.921692 J354.122833",
		"G03 X985.876980 Y581.306640 Z-1.000000 I-28.782528 J146.471849",
		"G02 X996.131735 Y583.767304 Z-1.000000 I47.208971 J-174.143640",
		"G02 X1001.767630 Y584.632810 Z-1.000000 I10.840074 J-51.804743",
		"G02 X1004.260010 Y584.781702 Z-1.000000 I3.162802 J-32.008652",
		"G02 X1007.578130 Y584.773440 Z-1.000000 I1.482056 J-71.088505",
		"G01 X1007.578130 Y584.773440 Z-1.000000",
		"G00 X0.00000 Y0.00000",
		};
	int size = sizeof(strings) / sizeof(strings[0]);
	//for (int i = 0; i < size; i++) {
	//	process(strings[i]);
	//}
	
	//process(string1);
	//process(string2);
	//process(string3);
	//process(string4);

	//x_now = 500;
	//y_now = 100;
	//draw_arc_ccw(100, 500, -400, 0);
	//arc_cw_traj(200, 200, 100, -100);
	//line_traj(x1, y1, x2, y2);
	//drawCurve(x1, y1, x2, y2,2);

	if (!glfwInit()) {
		return -1;
	}
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Open GL Project",NULL,NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// = SCREEN_WIDTH/2;
	//*(point + 1) = SCREEN_HEIGHT/2;
	std::string incomingDataString = "";
	std::string outgoingPart = "";

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		if (!draw) {
			for (int i = 0; i < size; i++) {
				count = 0;
				count_vertex = 0;
				point = pointVertex;
				process(strings[i]);
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(2, GL_FLOAT, 0, pointVertex);
				glDrawArrays(GL_POINTS, 0, count);
				glDisableClientState(GL_VERTEX_ARRAY);
				glfwSwapBuffers(window);
				glfwPollEvents();
				if (SP->IsConnected()) //communication
				{
					//std::cin >> outgoingData;
					//int sizeString = sizeof(outgoingData) / sizeof(outgoingData[0]);
					//sizeString = sizeString / 2;
					if (outgoingData != "") {
						std::cout << "Outgoing Data : " << outgoingData << std::endl;
						for (int j = 0; j < send_count; j = j++) {
							outgoingPart = outgoingData[j];
							//outgoingPart += "\n";
							char *charArr = new char[outgoingPart.size() + 1];
							copy(outgoingPart.begin(), outgoingPart.end(), charArr);
							charArr[outgoingPart.size()] = '\n';
							do {
								Sleep(100);
								incomingDataString = "";
								std::cout << "Outgoing Part : " << outgoingPart;
								SP->WriteData(charArr, dataLength);
								readResult = SP->ReadData(incomingData, dataLength);
								incomingData[readResult] = 0;
								incomingDataString = incomingData;
								std::cout << "   Incoming : " << incomingDataString << std::endl;
								//Sleep(50);
								//incomingDataString = "";
							} while (incomingDataString.length() == 0);
							//readResult = SP->ReadData(incomingData, dataLength);
							//incomingData[readResult] = 0;
							//printf("%s", incomingData);
							//Sleep(100);
						}
						send_count = 0;
					}
					else {
						std::cout << "Outgoing Data Empty! " << std::endl;
					}
				}
				outgoingData = "";
				//Sleep(10);
			}
			draw = true;
			count = 0;
			count_vertex = 0;
			point = pointVertex;
			for (int i = 0; i < size; i++) {
				process(strings[i]);
			}
			std::cout << "Count_Vertex : " << count_vertex << "\n";
			std::cout << "Count : " << count << "\n";
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, pointVertex);
			glDrawArrays(GL_POINTS, 0, count);
			glDisableClientState(GL_VERTEX_ARRAY);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		/**
		glClear(GL_COLOR_BUFFER_BIT);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, pointVertex);
		glDrawArrays(GL_POINTS, 0, count);
		glDisableClientState(GL_VERTEX_ARRAY);
		glfwSwapBuffers(window);
		glfwPollEvents();
		**/
	}
	glfwTerminate();
}

