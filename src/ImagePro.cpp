#include "ImagePro.h"
#include "Logger.h"

int ImagePro::begin()
{
	if (!m_faceCas.load("haarcascade_frontalface_default.xml")) {
		logE << "Loading haarcascade_frontalface_default" << std::endl;
		return -1;
	}
	return 1;
}

void ImagePro::start()
{
	m_process = std::thread([this]{run(); });
}
int ImagePro::end()
{
	m_process.join();
	return 1;
}
void ImagePro::run()
{
	while (true)
	{
		if (!m_newFace) 
		{
			m_faceCas.detectMultiScale(m_gray, m_faces, 1.4, 1, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30)); // detect faces
			m_newFace = true;
		}
	}
}

void ImagePro::process(cv::Mat& img)
{
	if (m_newFace) {
		cv::cvtColor(img, m_gray, cv::COLOR_BGR2GRAY);
		cv::equalizeHist(m_gray, m_gray);

		m_faces2.clear();
		for (size_t i = 0; i < m_faces.size(); i++)
			m_faces2.push_back(m_faces[i]);
		m_faces.clear();

		m_newFace = false;
	}
	if (0 < m_faces2.size())
	{
		cv::Point center((int)(m_faces2[0].x + m_faces2[0].width*0.5), (int)(m_faces2[0].y + m_faces2[0].height*0.5));
		cv::ellipse(img, center, cv::Size((int)(m_faces2[0].width*0.5),
			(int)(m_faces2[0].height*0.5)), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);
	}
}