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


	void scale_Crg_by_alpha(cv::Mat& orgb_matrix, double alpha);
	void scale_Cyb_by_alpha(cv::Mat& orgb_matrix, double alpha);
}
