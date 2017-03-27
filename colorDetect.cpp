#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <unistd.h> 
#include <fcntl.h>
#include <termios.h>

#define ARRAYSIZE 100

using namespace std;
using namespace cv;

int init_UARTPi(void);
Point centroid(const Mat &mask);


int init_UARTPi(void) {
  int uart0_filestream = -1;
  uart0_filestream = open("/dev/video0", O_RDWR | O_NOCTTY | O_NDELAY);

  if (uart0_filestream == -1) {
  	printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
  }

  struct termios options;
  tcgetattr(uart0_filestream, &options);
  options.c_cflag = B9600|CS8|CLOCAL|CREAD;     //<Set baud rate
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart0_filestream,TCIFLUSH);
  tcsetattr(uart0_filestream,TCSANOW,&options);  

  return uart0_filestream;
}

Point centroid(const Mat &mask) {
  Moments m = moments(mask, true);
  Point center(m.m10/m.m00, m.m01/m.m00);
  return center;
}

int main(int argc, char** argv) {
	VideoCapture cap(0);
	if(!cap.isOpened()) return -1;
	namedWindow("src", WINDOW_AUTOSIZE);
	namedWindow("green", WINDOW_AUTOSIZE);
	//namedWindow("gray", WINDOW_AUTOSIZE);
	namedWindow("highlight", WINDOW_AUTOSIZE);
	//namedWindow("greenOnly", WINDOW_AUTOSIZE);
	Mat src, greenOnly, mask, highlight, gray;
	
	double capWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double capHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	cout << capWidth << endl;
	cout << capHeight << endl;
	double maxpl = 0;
	double minval = 0;
	Point maxloc;
	
	while(1) {

		cap >> src;
		cvtColor(src, greenOnly, CV_BGR2HSV);
		cvtColor(src, gray, CV_BGR2GRAY);
		//minMaxLoc(gray, NULL, &maxpl, NULL, &maxloc);
	
		minval = max(200.00, maxpl-10);
		threshold(gray, highlight, minval, 255, 0);

		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		dilate(greenOnly, greenOnly, element);
		erode(greenOnly, greenOnly, element);

		//Scalar(35, 128, 150), Scalar(180, 150, 255)
		//hue:60-90 for green 140-170 for purple
		inRange(greenOnly, Scalar(30, 100, 100), Scalar(90,255, 255), mask);
		//inRange(greenOnly, Scalar(90, 100, 100), Scalar(160, 255, 255), mask);

		//bitwise_and(frame, greenOnly, greenOnly, mask = mask);

		Point centerofgreen;
		centerofgreen = centroid(mask);
		maxloc = centroid(highlight);
		cout << "highlight(" << maxloc.x << "," << maxloc.y << ")  green(";
		cout << cout << centerofgreen.x << "," << centerofgreen.y << ")" << endl;

		imshow("src", src);
		imshow("green", mask);
		//imshow("gray", gray);
		imshow("highlight", highlight);
		//imshow("greenOnly", greenOnly);
		if(waitKey(33) >= 0) break;
	}

	return 0;
}


