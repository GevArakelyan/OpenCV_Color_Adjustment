#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "orgb.hpp"
#include <thread>


using namespace std::chrono;

int main(int argc, char* argv[])
{
	std::string input_image_name = "roses.png";
	std::string more_red_image_name = "more_red.png";


	const cv::Mat original_image = cv::imread(input_image_name, CV_LOAD_IMAGE_COLOR);


	if (!original_image.data)
	{
		std::cout << "No Image Data \n";
		std::this_thread::sleep_for(3s);
		return -1;
	}
	
	cv::Mat image_in_orgb_space = orgb::orgb_from_linear_bgr(original_image);
	const double alpha = 1.4;

	orgb::scale_Crg_Colors(image_in_orgb_space, alpha);
	cv::Mat more_red_image = orgb::linear_bgr_from_orgb(image_in_orgb_space,original_image.rows,original_image.cols);


	cv::namedWindow(input_image_name, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(more_red_image_name, CV_WINDOW_AUTOSIZE);

	imshow(input_image_name, original_image);
	cv::imshow(more_red_image_name, more_red_image);

	cv::imwrite(more_red_image_name, more_red_image);

	cv::waitKey(0);

	return 0;
}
