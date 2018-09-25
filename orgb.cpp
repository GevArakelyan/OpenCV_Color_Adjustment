#include <opencv2/core/mat.hpp>
#include "orgb.hpp"

namespace orgb {

	/**
   * \brief Converts LCC point to orgb space point.
   * \param C1 C1` axes from L`C`C` space
   * \param C2 C2` axes from L`C`C` space
   */
	std::pair<double, double> lcc_to_orgb(double C1, double C2)
	{
		double Q = atan2(C2, C1);
		double alpha = Q;
		if (std::abs(Q) < (CV_PI / 3))
		{
			//EXPAND
			Q *= 3./2.;
		}
		else if (std::abs(Q) >= (CV_PI / 3))
		{
			//COMPRESS
			Q = CV_PI / 2. + (3. / 4.)*(std::abs(Q) - CV_PI / 3);
			if (alpha < 0) Q = -Q;
		}
		const double delta = Q - alpha;

		const cv::Mat point1 = (cv::Mat_<double>(2, 1) << C1, C2);
		const cv::Mat rotation = (cv::Mat_<double>(2, 2) << std::cos(delta), -std::sin(delta), std::sin(delta), std::cos(delta));
		cv::Mat point2 = rotation * point1;
		return std::make_pair(point2.at<double>(0, 0),
			point2.at<double>(1, 0));
	}

	/**
 * \brief Converts orgb point to LCC space point.
 * \param Crg value of Crg axes in orgb space
 * \param Cyb value of Cyb axes in orgb space
 * \return
 */
	std::pair<double, double> lcc_from_orgb(double Crg, double Cyb)
	{
		double Q = atan2(Crg, Cyb);
		double Qo = Q;
		if (std::abs(Q) < (CV_PI / 2))
		{
			//COMPRESS
			Qo *= 2. / 3.;
		}
		else if (std::abs(Q) >= (CV_PI / 2))
		{
			//EXPAND
			Qo = CV_PI / 3. + (4. / 3.)*(std::abs(Qo) - CV_PI / 2);
			if (Q < 0) Qo = -Qo;
		}
		const double delta = Qo - Q;

		const cv::Mat point1 = (cv::Mat_<double>(2, 1) << Cyb, Crg);
		const cv::Mat rotation = (cv::Mat_<double>(2, 2) << std::cos(delta), -std::sin(delta), std::sin(delta), std::cos(delta));
		cv::Mat point2 = rotation * point1;
		Cyb = point2.at<double>(0, 0);
		Crg = point2.at<double>(1, 0);
		return std::make_pair(Cyb, Crg);
	}

	double clamp(double val, double min_val, double max_val)
	{
		assert(min_val < max_val);
		return max_val < val
			? max_val
			: val < min_val
			? min_val
			: val;
	}


	cv::Mat orgb_from_linear_bgr(const cv::Mat& linear_rgb_mat)
	{
		const cv::Mat	srgb = orgb::srgb_from_linear_rgb(linear_rgb_mat);

		const cv::Mat transformer = 
			(cv::Mat_<double>(3, 3) << 0.299, 0.587, 0.114,
									   0.5, 0.5, -1.0,
									   0.866, -0.866, 0.0);

		//get the lcc matrix
		const cv::Mat lcc = transformer * srgb;
		//finally let's create orgb matrix
		cv::Mat orgb = orgb::orgb_from_lcc(lcc);
		return orgb;
	}

	cv::Mat linear_bgr_from_orgb(const cv::Mat& orgb, int rows, int cols)
	{
		cv::Mat lcc2 = orgb::lcc_from_orgb(orgb);

		const cv::Mat inverse_trans_mat = (cv::Mat_<double>(3, 3) << 1., .114, .7436,
			1., .114, -.4111,
			1., -.886, .1163);

		cv::Mat modified_srgb = inverse_trans_mat * lcc2;

		cv::Mat bgr_mat = orgb::linear_rgb_from_srgb(modified_srgb, rows, cols);
		return bgr_mat;
	}

	/**
	 * \brief convert image pixels from orgb space to lcc space.
	 * \param orgb matrix representing image in orgb space
	 * \return matrix representing image in lcc space
	 */
	cv::Mat lcc_from_orgb(const cv::Mat& orgb)
	{
		cv::Mat lcc; orgb.copyTo(lcc);
		for (int col = 0; col < orgb.cols; ++col)
		{
			auto& cyb = lcc.at<double>(1, col);
			auto& crg = lcc.at<double>(2, col);
			const auto crg_cyb_pair = orgb::lcc_from_orgb(crg, cyb);
			cyb = crg_cyb_pair.first;
			crg = crg_cyb_pair.second;
		}
		return lcc;
	}

	/**
	 * \brief convert image pixels from lcc space to orgb space.
	 * \param lcc matrix representing image in lcc space
	 * \return matrix representing image in orgb space
	 */
	cv::Mat orgb_from_lcc(const cv::Mat& lcc)
	{
		cv::Mat orgb; lcc.copyTo(orgb);
		for (int col = 0; col < orgb.cols; ++col)
		{
			auto& c1 = orgb.at<double>(1, col);
			auto& c2 = orgb.at<double>(2, col);
			const auto cc_pair = orgb::lcc_to_orgb(c1, c2);
			c1 = cc_pair.first;
			c2 = cc_pair.second;
		}
		return orgb;
	}

	cv::Mat srgb_from_linear_rgb(cv::Mat original)
	{
		cv::Mat srgb = cv::Mat::zeros(3, original.rows * original.cols, CV_64FC1);
		for (int row = 0; row < original.rows; ++row)
		{
			for (int col = 0; col < original.cols; ++col)
			{
				const auto pixel = original.at<cv::Vec3b>(row, col);
				srgb.at<double>(0, row*original.cols + col) = cv::pow(pixel[2] / 255.0, 1 / 2.2);
				srgb.at<double>(1, row*original.cols + col) = cv::pow(pixel[1] / 255.0, 1 / 2.2);
				srgb.at<double>(2, row*original.cols + col) = cv::pow(pixel[0] / 255.0, 1 / 2.2);
			}
		}
		return srgb;
	}

	cv::Mat linear_rgb_from_srgb(const cv::Mat& srgb, int rows, int cols)
	{
		cv::Mat rgb = cv::Mat::zeros(rows, cols, CV_8UC3);
		for (int row = 0; row < rows; ++row)
		{
			for (int col = 0; col < cols; ++col)
			{
				auto& pixel = rgb.at<cv::Vec3b>(row, col);
				const double red = cv::pow(srgb.at<double>(0, row*cols + col), 2.2) * 255.0;
				const double green = cv::pow(srgb.at<double>(1, row*cols + col), 2.2) * 255.0;
				const double blue = cv::pow(srgb.at<double>(2, row*cols + col), 2.2) * 255.0;
				pixel[0] = static_cast<uint8_t>(clamp(blue, 0., 255.));
				pixel[1] = static_cast<uint8_t>(clamp(green, 0., 255.));
				pixel[2] = static_cast<uint8_t>(clamp(red, 0., 255.));
			}
		}
		return rgb;
	}

	void scale_Crg_by_alpha(cv::Mat& orgb_matrix, double alpha)
	{
		for (int col = 0; col < orgb_matrix.cols; ++col)
		{
			auto& Crg = orgb_matrix.at<double>(2, col);
			Crg *= alpha;
		}
	}
	void scale_Cyb_by_alpha(cv::Mat& orgb_matrix, double alpha)
	{
		for (int col = 0; col < orgb_matrix.cols; ++col)
		{
			auto& Cyb = orgb_matrix.at<double>(1, col);
			Cyb *= alpha;
		}
	}
}
