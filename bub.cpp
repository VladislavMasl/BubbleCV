#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;


void getArrayOfDistances(vector<vector<Point>> contours, Point center, double radius, vector<double>* distances_arr, vector<int>* N) { /*change 2 input arrays: distance
	and point number array given by link in memory*/

	for (int i = 0; i < contours.size(); ++i) {
		for (int j = 0; j < contours[i].size(); ++j) {
			Point rad = contours[i][j];
			(*distances_arr)[j] = (sqrt(pow((center.x - rad.x), 2) + pow((center.y - rad.y), 2))) - radius;
			(*N)[j] = j + 1;
		}
	}
}


double getMaxDistance(vector<vector<Point>> contours, Point center, double radius) { /*searching max distance between point on contour
	and poind on circle*/
	double distance_tmp = 0;
	double distance = 0;
	double current_max = 0;
	for (int i = 0; i < contours.size(); ++i) {
		for (int j = 0; j < contours[i].size(); ++j) {
			Point rad = contours[i][j];
			distance = (sqrt(pow((center.x - rad.x), 2) + pow((center.y - rad.y), 2))) - radius;
			if (distance > current_max) {
				distance_tmp = distance;
				distance = current_max;
				current_max = distance_tmp;
			}
		}
		return current_max;
	}
}


double getMinDistance(vector<vector<Point>> contours, Point center, double radius) { /*search the min dist between each point on contour and center
	point this function is checking that the minimum radius is correctly found*/
	int i = 0;
	int j = 0;
	size_t k = 0;
	double distance_tmp = 0;
	vector <double> distance_arr(contours[i].size());
	double distance = 0;
	double current_min = 9999999;
	double d_sum = 0;
	double d_mid = 0;
	for (i; i < contours.size(); ++i) {
		for (j; j < contours[i].size(); j++) {
			Point rad = contours[i][j];
			distance = (sqrt(pow((center.x - rad.x), 2) + pow((center.y - rad.y), 2))) - radius;
			//cout << "D_TMP" << distance_tmp << endl;
			//distance = distance_tmp - radius;
			distance_arr[i] = distance;
			//cout << "dD" << distance << endl;
			if (distance < current_min) {
				distance_tmp = distance;
				distance = current_min;
				current_min = distance_tmp;
			}
		}
		return current_min;
	}
}


Point getCenter(vector<vector<Point>> contours) { /*find center of contour trouhg the moments and terurn it as point {x,y}*/
	vector<Moments> mu(contours.size());
	Point center;
	for (int i = 0; i < contours.size(); ++i) {
		for (int j = 0; j < contours[i].size(); ++j) {
			mu[j] = moments(contours[i]);
			center.x = (mu[j].m10 / mu[j].m00);
			center.y = (mu[j].m01 / mu[j].m00);
			return center;
		}

	}
}


double getMinRadius(vector<vector<Point>> contours, Point center) {/*finding minimun distance between center and each
	point on the contour*/
	double min_rad = 999999999;
	double radius = 0;
	for (int i = 0; i < contours.size(); ++i) {
		for (size_t j = 0; j < contours[i].size(); ++j) {
			Point rad = (contours[i][j]);
			radius = sqrt(pow((center.x - rad.x), 2) + pow((center.y - rad.y), 2));
			if (radius < min_rad) {
				double tmp = min_rad;
				min_rad = radius;
				radius = tmp;
			}
		}
	}
	return min_rad;
}


//contours definition

void getContours(Mat* imgdil, Mat* img) {

	vector<vector<Point>> contours;  //vector<vector<Point>> - a matrix in each cell of which a contour point is stored {x,y}
	vector<Vec4i> hierarchy;  //4-dimensional array for contour hierarchy levels (4 hierarchy levels in total)

	int area_pass = 5000; //parameter for passing the contour over the area in the further algorithm


	findContours(*imgdil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //define the contours on the dilate image

	for (int i = 0; i < (contours.size()); ++i) { 
		int area = contourArea(contours[i]);
		string area_size, radius_size, radius_result, area_result, distance_size, distance_result, distance_M_size, distance_M_result; /*strings 
		for concatenation*/
		string sA{ "Area = " }, sR{ "Radius = " }, sD{ "Min distance = " }, sDm{ "Max distance = " };
		if (area >= area_pass) {
			area_size = to_string(area);
			area_result.append(sA).append(area_size);
			drawContours(*img, contours, i, Scalar(0, 255, 0), 1);
			
			vector<Rect> boundRect(contours.size());
			boundRect[i] = boundingRect(contours[i]);
			Point center_tm;
			center_tm = getCenter(contours);
			
			double radius = getMinRadius(contours, center_tm);
			
			circle(*img, center_tm, radius, Scalar(0, 0, 255), 1); //drawing circle according to calculated radius and center on each frame
			
			double min_dist = getMinDistance(contours, center_tm, radius);
			double max_dist = getMaxDistance(contours, center_tm, radius);
			
			vector<double> distances_arr(contours[i].size() * 3);
			vector<int> N(contours[i].size() * 3);
			
			getArrayOfDistances(contours, center_tm, radius, &distances_arr, &N);
			
			for (int k = 0; k < contours[i].size(); ++k) {
				cout << " N " << N[k] << endl;
				cout << " D " << distances_arr[k] << endl;
			}
			radius_size = to_string(radius);
			radius_result.append(sR).append(radius_size);
			distance_size = to_string(min_dist);
			distance_result.append(sD).append(distance_size);
			distance_M_size = to_string(max_dist);
			distance_M_result.append(sDm).append(distance_M_size);

			putText(*img, area_result, { boundRect[i].x + 10, boundRect[i].y - 10 }, FONT_HERSHEY_COMPLEX_SMALL, 0.75, Scalar(255, 0, 0), 1);
			putText(*img, radius_result, { boundRect[i].x + 10, boundRect[i].y - 30 }, FONT_HERSHEY_COMPLEX_SMALL, 0.75, Scalar(0, 0, 255), 1);
			putText(*img, distance_result, { boundRect[i].x + 10, boundRect[i].y - 50 }, FONT_HERSHEY_COMPLEX_SMALL, 0.75, Scalar(0, 255, 0), 1);
			putText(*img, distance_M_result, { boundRect[i].x + 10, boundRect[i].y - 70 }, FONT_HERSHEY_COMPLEX_SMALL, 0.75, Scalar(225, 255, 0), 1);
		}
	}
}

int main(int argv, char *argc[]) { //give arguments from console in our function: link on video and waitkey

	setlocale(LC_ALL, "Russian");
	
	string path = argc[1];
	int waitkey = atoi(argc[2]);
	  
	VideoCapture cap(path);
	Mat img, imgblur, imggray, imgcanny, imgdil, imgtr, imgcr;  //define arrays for images
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); /*создаемм фильтр-матрицу из единиц, для уменьшения шума.
	падение шума= 1/(n*n) где n - измерение матрицы чем больше измерений тем сильнее dilate - фильтрация*/

	while (true) {

		cap.read(img); //read video frame-by-frame
		if (cap.read(img) == false) {
			break;
		}
		Rect roi(250, 100, 350, 300);
		imgcr = img(roi); //croping each frame to ractangle with sides 250x100x350x300

		cvtColor(imgcr, imggray, COLOR_BGR2GRAY);  //image preprocessing
		threshold(imggray, imgtr, 85, 255, THRESH_BINARY_INV);
		GaussianBlur(imgtr, imgblur, Size(3, 3), 1, 1);
		Canny(imgblur, imgcanny, 45, 55);
		dilate(imgblur, imgdil, kernel);

		getContours(&imgdil, &imgcr); //passing images to the procedure via links

		imshow("image", imgcr);
		waitKey(waitkey); //ssampling waitkey from console
	}
	//system("pause");
	return 0;
}
