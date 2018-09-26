#pragma once
#include <utility>
#include <opencv2/core/mat.hpp>

namespace orgb {

	/**
	 * \brief convert bgr matrix to orgb matrix.
	 * \param linear_bgr_mat matrix containing bgr colors in range [0,255]
	 * \return image matrix in orgb space.
	 */
	cv::Mat orgb_from_linear_bgr(const cv::Mat& linear_rgb_mat);

	/**
	 * \brief 
	 * \param orgb image matrix in orgb space.
	 * \param rows number of rows in output matrix.
	 * \param cols number in columns in output matrix.
	 * \return image matrix in rgb space in blue,green,red order.
	 */
	cv::Mat linear_bgr_from_orgb(const cv::Mat& orgb, int rows, int cols);


	cv::Mat lcc_from_orgb(const cv::Mat& orgb);
	
	cv::Mat orgb_from_lcc(const cv::Mat& lcc);
	
	cv::Mat srgb_from_linear_rgb(cv::Mat original);

	cv::Mat linear_rgb_from_srgb(const cv::Mat& srgb, int rows, int cols);


	/**
	 * \brief scale both axes colors.(this does not change Luminance).
	 * \param image matrix in orgb space to scale.
	 * \param red_green_scale scale factor for red_green axes.
	 * \param yellow_blue_scale scale factor for yellow_blue axes.
	 */
	void scale_Crg_Cyb_Colors(cv::Mat& image, double red_green_scale, double yellow_blue_scale);

	void scale_Crg_Colors(cv::Mat& image, double scale);
	void scale_Cyb_Colors(cv::Mat& image, double scale);

} //orgb namespace
