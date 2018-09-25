#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "orgb.hpp"

using namespace  cv;


int main(int argc, char* argv[])
{
	const std::string input_image_name = "image.png";
	const std::string result1_image_name = "second.png";

	const Mat original = imread(input_image_name, CV_LOAD_IMAGE_COLOR);


	if (!original.data)
	{
		std::cout << "No Image Data \n";
		return -1;
	}
	//creating rgb non linear space | first deviding to 255.0 to get within range of [0,1]
	// then getting gamma correction for non-linear space. 
	Mat	srgb = orgb::srgb_from_linear_rgb(original);

	const Mat transformer = (Mat_<double>(3, 3) << 0.299, 0.587, 0.114,
		0.5, 0.5, -1.0,
		0.866, -0.866, 0.0);
	const Mat inverse_trans_mat = (Mat_<double>(3, 3) << 1., .114, .7436,
		1., .114, -.4111,
		1., -.886, .1163);



	//get the lcc matrix
	Mat lcc = transformer * srgb;
	//finally let's create orgb matrix
	Mat orgb_matrix = orgb::orgb_from_lcc(lcc);

	const double alpha = 2;

	
	orgb::scale_Crg_by_alpha(orgb_matrix, alpha);
	orgb::scale_Cyb_by_alpha(orgb_matrix, alpha);


	Mat lcc_from_orgb_matrix = orgb::lcc_from_orgb(orgb_matrix);


	Mat modified_srgb = inverse_trans_mat * lcc_from_orgb_matrix;

	Mat result1_image = orgb::linear_rgb_from_srgb(modified_srgb, original.rows, original.cols);

	//same as above 5 lines.
	//Mat orgb = orgb::orgb_from_linear_bgr(original);
	//Mat result1_image = orgb::linear_bgr_from_orgb(orgb,original.rows,original.cols);





	namedWindow(input_image_name, CV_WINDOW_AUTOSIZE);
	namedWindow(result1_image_name, CV_WINDOW_AUTOSIZE);

	imshow(input_image_name, original);
	imshow(result1_image_name, result1_image);

	waitKey(0);
	










	return 0;
}
