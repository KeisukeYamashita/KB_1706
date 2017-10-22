#include "opencv2\opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


int g_MeanBlockSize = 0;
int g_MeanOffset = 0;
int g_GaussianBlockSize = 0;
int g_GaussianOffset = 0;

void thinningIte(Mat& img, int pattern){
	Mat del_marker = Mat::ones(img.size(), CV_8UC1);
	int x, y;

	for (y = 1; y < img.rows - 1; ++y) {
		for (x = 1; x < img.cols - 1; ++x) {

			int v9, v2, v3;
			int v8, v1, v4;
			int v7, v6, v5;

			v1 = img.data[y   * img.step + x   * img.elemSize()];
			v2 = img.data[(y - 1) * img.step + x   * img.elemSize()];
			v3 = img.data[(y - 1) * img.step + (x + 1) * img.elemSize()];
			v4 = img.data[y   * img.step + (x + 1) * img.elemSize()];
			v5 = img.data[(y + 1) * img.step + (x + 1) * img.elemSize()];
			v6 = img.data[(y + 1) * img.step + x   * img.elemSize()];
			v7 = img.data[(y + 1) * img.step + (x - 1) * img.elemSize()];
			v8 = img.data[y   * img.step + (x - 1) * img.elemSize()];
			v9 = img.data[(y - 1) * img.step + (x - 1) * img.elemSize()];

			int S = (v2 == 0 && v3 == 1) + (v3 == 0 && v4 == 1) +
				(v4 == 0 && v5 == 1) + (v5 == 0 && v6 == 1) +
				(v6 == 0 && v7 == 1) + (v7 == 0 && v8 == 1) +
				(v8 == 0 && v9 == 1) + (v9 == 0 && v2 == 1);

			int N = v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9;

			int m1 = 0, m2 = 0;

			if (pattern == 0) m1 = (v2 * v4 * v6);
			if (pattern == 1) m1 = (v2 * v4 * v8);

			if (pattern == 0) m2 = (v4 * v6 * v8);
			if (pattern == 1) m2 = (v2 * v6 * v8);

			if (S == 1 && (N >= 2 && N <= 6) && m1 == 0 && m2 == 0)
				del_marker.data[y * del_marker.step + x * del_marker.elemSize()] = 0;
		}
	}
	img &= del_marker;
}

void thinning(const Mat& src, Mat& dst) {
	dst = src.clone();
	dst /= 255;         // 0��0 , 1�ȏ��1�ɕϊ������

	Mat prev = Mat::zeros(dst.size(), CV_8UC1);
	Mat diff;

	do {
		thinningIte(dst, 0);
		thinningIte(dst, 1);
		absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (countNonZero(diff) > 0);

	dst *= 255;
}

int main(int argc, char *argv[])
{
	cv::Mat src_img = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/fingerprint.bmp", 1);
	if (src_img.empty()) return -1;

	/// �摜��\������E�B���h�E
	// �E�B���h�E�̖��O�C�v���p�e�B
	// CV_WINDOW_AUTOSIZE : �E�B���h�E�T�C�Y���摜�T�C�Y�ɍ��킹��
	// CV_WINDOW_FREERATIO : �E�B���h�E�̃A�X�y�N�g����Œ肵�Ȃ�
	cv::namedWindow("image1", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::cvtColor(src_img, src_img, CV_BGR2GRAY);
	// �E�B���h�E���ŃE�B���h�E���w�肵�āC�����ɉ摜��`��
	cv::imshow("image1", src_img);
	medianBlur(src_img, src_img, 3);          // ���f�B�A���t�B���^
	// �f�t�H���g�̃v���p�e�B�ŕ\��
	cv::adaptiveThreshold(src_img,                              // 8�r�b�g�C�V���O���`�����l���̓��͉摜�D
		src_img,
		255,                              // 2�l���摜���쐬����ۂ̂O�ȊO�̐F�B�����ł�[ 0, 255 ]�ō쐬����.
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,   // �ߖT�̕��ϒl���v�Z����ۂɎg�p����A���S���Y��.ADAPTIVE_THRESH_MEAN_C �܂��� ADAPTIVE_THRESH_GAUSSIAN_C.
		cv::THRESH_BINARY,                // 臒l�̎�ށD THRESH_BINARY �܂��� THRESH_BINARY_INV �̂ǂ��炩�D
		41,                                // �s�N�Z����臒l�����߂邽�߂ɗ��p�����ߖT�̈�̃T�C�Y�D3, 5, 7, �ȂǁD
		5                                 // ���ς܂��͉��d���ς���������萔.���̒l�ɂ�� maxValue �͈̔͂��ς��.
	);
	cv::imshow("image2", src_img);
	/*
	// �摜�̓K���I臒l�����iMEAN�j
	cv::namedWindow("threshold_mean");
	cv::createTrackbar("blockSize", "threshold_mean", &g_MeanBlockSize, 255, onTrackbarMean, &src_img);
	cv::createTrackbar("offset", "threshold_mean", &g_MeanBlockSize, 255, onTrackbarMean, &src_img);
	cv::setTrackbarPos("blockSize", "threshold_mean", 41);
	cv::setTrackbarPos("offset", "threshold_mean", 5);

	// �摜�̓K���I臒l�����iGAUSSIAN�j
	cv::namedWindow("threshold_gaussian");
	cv::createTrackbar("blockSize", "threshold_gaussian", &g_GaussianBlockSize, 255, onTrackbarGaussian, &src_img);
	cv::createTrackbar("offset", "threshold_gaussian", &g_GaussianOffset, 255, onTrackbarGaussian, &src_img);
	cv::setTrackbarPos("blockSize", "threshold_gaussian", 41);
	cv::setTrackbarPos("offset", "threshold_gaussian", 5);
	*/
	thinning(src_img, src_img);
	cv::imshow("image3", src_img);
	/*
	// (1)�T���摜�S�̂ɑ΂��āC�e���v���[�g�̃}�b�`���O�l�i�w�肵����@�Ɉˑ��j���v�Z
	int dst_size = cvSize(src_img->width - tmp_img->width + 1, src_img->height - tmp_img->height + 1);
	dst_img = cvCreateImage(dst_size, IPL_DEPTH_32F, 1);
	cvMatchTemplate(src_img, tmp_img, dst_img, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(dst_img, &min_val, &max_val, &min_loc, &max_loc, NULL);

	// (2)�e���v���[�g�ɑΉ�����ʒu�ɋ�`��`��
	cvRectangle(src_img, max_loc,
		cvPoint(max_loc.x + tmp_img->width, max_loc.y + tmp_img->height), CV_RGB(255, 0, 0), 3);
	cvNamedWindow("Image", 1);
	cvShowImage("Image", src_img);
	*/
	// �L�[���͂��i�����Ɂj�҂�
	cv::waitKey(0);
	cvDestroyWindow("Image1");
	cvDestroyWindow("Image2");
	cvDestroyWindow("Image3");
	//cvReleaseImage(&src_img);
	return 0;
}