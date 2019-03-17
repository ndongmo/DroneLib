/*!
* \file ImagePro.h
* \brief Image processing class.
* \author Ndongmo Silatsa Fabrice
* \date 16-03-2019
* \version 2.0
*/

#pragma once

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <thread>
#include <mutex>

class ImagePro
{
public:
	int begin();
	void start();
	int end();
	void process(cv::Mat& img);

private:
	void run();

	cv::CascadeClassifier m_faceCas;

	std::thread m_process;
	std::vector<cv::Rect> m_faces;
	std::vector<cv::Rect> m_faces2;
	cv::Mat m_gray;
	bool m_newFace = false;
};

