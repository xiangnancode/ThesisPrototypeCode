#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

#include <unistd.h> 
#include <fcntl.h>
#include <termios.h>
#include <wiringPi.h>

#define ARRAYSIZE 100
#define CLOCKWISE 1
#define COUNTER_CLOCKWISE 2

using namespace std;
using namespace cv;

void delayMS(float x);
void rotate(int* , int , int *, int);
void motor();

int moveX = 0, moveY = 0;
bool stop = false;

int main(int argc, char** argv) {
  VideoCapture cap(0);
  if(!cap.isOpened()) return -1;
  //namedWindow("src", WINDOW_AUTOSIZE);
  namedWindow("dst", WINDOW_AUTOSIZE);
  Mat src, gray, dst;
	
  double capWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  double capHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  double maxpl = 0;
  double minval = 0;
  Point maxloc;

  cout << capWidth << endl;
  cout << capHeight << endl;
  //int inp = 0;
  //cin >> inp;
  thread t1(motor);//creat a thread for motors
  //delayMS(inp);
  //cout << "time" << endl;

  while(1) {

    cap >> src;

    //medianBlur(src, src, 3);

    cvtColor(src, gray, CV_BGR2GRAY);
    minval = max(200.00, maxpl-20);
    threshold(gray, dst, minval, 255, 0);
    minMaxLoc(dst, NULL, &maxpl, NULL, &maxloc);
    cout << maxloc.x << "," << maxloc.y << endl;

    if (maxloc.x == 0 || (maxloc.x >= 280 && maxloc.x <= 360) ) moveX = 0;
    else if (maxloc.x < 280) moveX = 1;
    else if (maxloc.x > 360) moveX = 2;

    /*if (maxloc.y == 0 || maxloc.y == 240) moveY = 0;
    else if (maxloc.y < 240) moveY = 1;
    else if (maxloc.y > 240) moveY = 2;*/

    //imshow("src", src);
    imshow("dst", dst);
    //imshow("gray", gray);
   
    //++m;
    //dy = (m / 200) % 2 + 1;
    //dx = (m / 10) % 2 + 1;
    //cout << "dx is " << dx << endl;
    
		
    if(waitKey(1) >= 0) break;
     
  }
  stop = true;
  t1.join();
  cout << "main stops" << endl;
  return 0;
}

void motor() {
  // motor initialization
  int motorY[4] = {0, 1, 2, 3};
  int motorX[4] = {4, 5, 6, 7};

  if (-1 == wiringPiSetup()) {
    printf("Setup wiringPi failed!");
    return;
  }
  for (int i = 0; i < 8; ++i) {
    pinMode(i, OUTPUT);
  }

  delayMS(50);    // wait for a stable status

  //int m = 0;
  int k = 250, j = 400;
  for (int i = 0; i < 500; ++i) {
    rotate(motorY, (i < 250), motorX, 1);
  }
  //cout << "done" << endl;
  while (!stop) {
    int dx = moveX;
    int dy = moveY;
    if (dx == 1 && j <= 800) ++j;
    else if (dx == 2 && j >=0) --j;
    else dx = 0;

 
    if (dy == 1 && k <= 500) ++k;
    else if (dy ==2 && k >=0) --k;
    else dy = 0;
    rotate(motorY, dy, motorX, dx);
  }
  cout << "t1 stops" << endl;
}

/* Suspend execution for x milliseconds intervals.
*  @param ms Milliseconds to sleep.
*/
void delayMS(float x) {
  usleep(x * 1000);
}

/* Rotate the motor.
*  @param pins     A pointer which points to the pins number array.
*  @param direction  CLOCKWISE for clockwise rotation, COUNTER_CLOCKWISE for counter clockwise rotation.
*/
void rotate(int* motorY, int dy, int *motorX, int dx) {
  int order[8][4] = { {1, 1, 0, 0},
		      {0, 1, 0, 0},
		      {0, 1, 1, 0},
		      {0, 0, 1, 0},
		      {0, 0, 1, 1},
		      {0, 0, 0, 1},
		      {1, 0, 0, 1},
		      {1, 0, 0, 0} };
  for (int i = 0; i < 8; i++) {
    if (CLOCKWISE == dy) {
      for (int j = 0; j < 4; j++) {
        if (order[i][j]) {
          digitalWrite(motorY[3 - j], 1); // output a high level 
        } else {
          digitalWrite(motorY[3 - j], 0); // output a low level 
        }
      }
    } else if (COUNTER_CLOCKWISE == dy) {
      for (int j = 0; j < 4; j++) {
        if (order[i][j]) {
          digitalWrite(motorY[j], 1); // output a high level 
        } else {
          digitalWrite(motorY[j], 0); // output a low level 
	}
      }
    } else {
      digitalWrite(motorY[0], 1);
      digitalWrite(motorY[1], 1);
      digitalWrite(motorY[2], 0);
      digitalWrite(motorY[3], 0);
    }
    if (CLOCKWISE == dx) {
      for (int j = 0; j < 4; j++) {
        if (order[i][j]) {
          digitalWrite(motorX[3 - j], 1); // output a high level 
        } else {
          digitalWrite(motorX[3 - j], 0); // output a low level 
        }
      }
    } else if (COUNTER_CLOCKWISE == dx) {
      for (int j = 0; j < 4; j++) {
        if (order[i][j]) {
          digitalWrite(motorX[j], 1); // output a high level 
        } else {
          digitalWrite(motorX[j], 0); // output a low level 
	}
      }
    } else {
      digitalWrite(motorX[0], 1);
      digitalWrite(motorX[1], 1);
      digitalWrite(motorX[2], 0);
      digitalWrite(motorX[3], 0);
    }
    delayMS(0.7);
  }
}
