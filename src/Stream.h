/*!
* \file Stream.h
* \brief Video and audio stream manager class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "Config.h"
#include "Pool.h"
#include "Command.h"

#include <WS2tcpip.h>
#include <thread>
#include <mutex>

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>

#include <SDL/SDL.h>

extern "C" {
#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h> 
#include <libswscale/swscale.h> 
#include <libswresample/swresample.h>
}

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 65000

typedef struct {
	AVCodec *codec;
	AVCodecContext *codecCtx;
	AVFrame *decodedFrame;
	AVFrame *decodedFrameBGR;
	AVPacket avPacket;
	SwsContext *convertCtx;
	UINT8 *outputData;
	UINT8 *buffer;
	int outputDataSize;

} VideoDecoder; // FFMPEG video decoder

typedef struct {
	UINT16 frameNumber;			// Identifier of the frame
	UINT8 frameFlags;			// A bitfield indicating flags for the frame (1 byte).
	UINT8 fragmentNumber;		// The index of the current fragment in the frame.
	UINT8 fragmentPerFrame;		// The total number of fragments in the frame.
	UINT8 *frame;				// Current frame
	int frameSize;				// Frame size

} StreamFrame;	// Simple stream frame data structure

typedef struct Fragment {
	Fragment() {
		size = 0;
		data = new UINT8[1];
	}
	UINT8 *data;	// Fragment data
	int size;		// Fragment size

} Fragment;	// Fragment data structure

typedef struct{
	UINT16 frameNumber;
	UINT8 *packetsACK;
}AckPacket;

typedef struct ArStreamFrame {
	ArStreamFrame(){
		frameSize = 0;
		frame = new UINT8[0];
		fragments = new Fragment[1];
		fragments[0].data = new UINT8[1];
		waitFlag = false;
		fragmentPerFrame = 1;
		fragmentCounter = 0;
	}
	UINT16 frameNumber;		// Identifier of the frame
	UINT8 *frameACK;
	AckPacket ackPacket;
	UINT8 *frame;			// Current frame
	UINT8 frameFlags;		// A bitfield indicating flags for the frame (1 byte).
	Fragment *fragments;	// Fragments
	int frameSize;			// Frame size
	int fragmentPerFrame;	// The total number of fragments in the frame.
	int fragmentCounter;	// The index of the current fragment in the frame.
	bool waitFlag;

}ArStreamFrame;	// Binary streams structure to transport live audio and video data

struct NetworkFrame;

class Stream
{
public:
	int begin(Command* cmd);
	int end();
	void start();
	
	/*!
	* \brief Manages the new comming video frame.
	*/
	void newVideoFrame(NetworkFrame *networkFrame);
	/*!
	* \brief Manages the new comming audio frame.
	*/
	void newAudioFrame(NetworkFrame *networkFrame);
	/*!
	* \brief Gets the last captured image.
	* \return an image.
	*/
	cv::Mat getImage();
	/*!
	* \brief SDL Audio callback.
	*/
	void audioCallback(UINT8 *stream, int len);
	/*!
	* \brief forward callback to simulate SDL audio callback.
	*/
	static void forwardCallback(void *userdata, UINT8 *stream, int len)
	{
		static_cast<Stream*>(userdata)->audioCallback(stream, len);
	}

private:
	/*!
	* \brief Main video decoder process.
	*/
	void runVideo();
	/*!
	* \brief get the next audio frame from the pool.
	*/
	int updateAudio(UINT8* buffer, int size);
	/*!
	* \brief Initializes stream management and all ffmpeg stuff.
	* \return -1 when an error occurred, otherwise 1
	*/
	int initStream();
	/*!
	* \brief Initializes video stream decoder.
	* \return -1 when an error occurred, otherwise 1
	*/
	int initVideo();
	/*!
	* \brief Initializes audio stream decoder.
	* \return -1 when an error occurred, otherwise 1
	*/
	int initAudio();
	/*!
	* \brief Parses and retrieves the stream frame from the given network frame.
	* \param networkFrame : received network frame
	* \param streamFrame : builded stream frame
	*/
	void parseStreamFrame(NetworkFrame *networkFrame, StreamFrame *streamFrame);
	/*!
	* \brief Builds the given network frame from the given raw data buffer.
	* \param networkFrame : network frame to build
	*/
	void appendNewFrame(StreamFrame *streamFrame, ArStreamFrame& frame, int id, Pool& pool, std::mutex& mxt);

	bool m_newImage = false;
	int m_readIndex = 0;			// Pool cursor for reading data
	int m_writeIndex = 0;			// Pool cursor for adding data
	std::mutex m_videoMtx;
	std::mutex m_imageMtx;			// image mutex
	std::mutex m_soundMtx;			// sound mutex
	std::thread m_videoProcess;
	VideoDecoder m_videoDecoder;		// video decoder
	Pool m_videoPool;					// pool of video data
	Pool m_audioPool;					// pool of video data
	ArStreamFrame m_videoFrame;
	ArStreamFrame m_audioFrame;
	Command* m_cmd;
};

