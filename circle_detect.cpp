//Ref for circle detection: https://github.com/sol-prog/OpenCV-red-circle-detection/blob/master/circle_detect.cpp
//REf for video capture: https://github.com/opencv/opencv/blob/master/samples/cpp/videocapture_basic.cpp
//More Ref for detection in video: https://pycad.co/face-and-circle-detection-using-opencv-with-c/

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;


void check_program_arguments(int argc) {
	if(argc != 2) {
		std::cout << "Error! Program usage:" << std::endl;
		std::cout << "./circle_detect image_circles_path" << std::endl;
		std::exit(-1);
	}	
}


int main(int argc, char **argv) {

	cv::Mat bgr_image; //stores each frame
	cv::Mat hsv_image;
	cv::Mat orig_image;
	unsigned int x,y, radius, w=2;


	std::vector<cv::Vec3f> circles;

	cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);

	VideoCapture vid; //integer (index) or (camera name as )string can be passed here

	int deviceID = 0;             // 0 = open default camera, 1 still used default cam
    int apiID = cv::CAP_V4L2;      // 0 = autodetect default API
    // open selected camera using selected API
    vid.open(deviceID, apiID); 		//a string name too can be passed here instead of interger deviceID
	if(!vid.isOpened()){
		std::cout << "cannot open camera";
		return -1;
	}

	std::cout << "Get Mathable teach by bringing in a marker\nPress any key to terminate\n";
	
	while(1){

		// Load input image		
		vid >> bgr_image;

		resize(bgr_image, bgr_image, Size(640, 480)); //because my image was vertical; values should be interchanged otherwise		

		orig_image = bgr_image.clone();

		cv::medianBlur(bgr_image, bgr_image, 3);

		// Convert input image to HSV
		
		cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

		// Threshold the HSV image, keep only the red pixels
		cv::Mat lower_red_hue_range;
		cv::Mat upper_red_hue_range;
		cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(20, 255, 255), lower_red_hue_range);
		cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), upper_red_hue_range);

		// Combine the above two images
		cv::Mat red_hue_image;
		cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

		cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);

		// Use the Hough transform to detect circles in the combined threshold image
		
		cv::HoughCircles(red_hue_image, circles, HOUGH_GRADIENT, 1, red_hue_image.rows/16, 100, 40, 20, 0); //modified here to replace  CV_HOUGH_GRADIENT with  HOUGH_GRADIENT
		// parameter 5: roughly marker size in pixels //parameter reference-:
		// https://docs.opencv.org/4.x/d3/de5/tutorial_js_houghcircles.html
		// Loop over all detected circles and outline them on the original image
		if(circles.size()){
			for(size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
				x = std::round(circles[current_circle][0]);
				y = std::round(circles[current_circle][1]);
				
				//radius = std::round(circles[current_circle][2]);
				//cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 0), 2);
				cv::rectangle(orig_image, cv::Point(x-w/2, y-w/2), cv::Point(x+w/2, y+w/2), cv::Scalar(0, 255, 0), 2);
			}
		}

		// Show images
		//cv::namedWindow("Threshold lower image", cv::WINDOW_AUTOSIZE);
		//cv::imshow("Threshold lower image", lower_red_hue_range);
		//cv::namedWindow("Threshold upper image", cv::WINDOW_AUTOSIZE);
		//cv::imshow("Threshold upper image", upper_red_hue_range);
		//cv::namedWindow("Combined threshold images", cv::WINDOW_AUTOSIZE);
		//cv::imshow("Combined threshold images", red_hue_image);
		//cv::namedWindow("Detected red circles on the input image", cv::WINDOW_AUTOSIZE);
		//cv::imshow("Detected red circles on the input image", orig_image);
		cv::imshow("Display Window", orig_image);

		if(cv::waitKey(5) >=0)
			break;

	}	
	return 0;
}