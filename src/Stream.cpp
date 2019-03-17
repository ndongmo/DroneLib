#include "Stream.h"
#include "Network.h"
#include "Logger.h"
#include "Utils.h"

int Stream::begin(Command* cmd)
{
	m_cmd = cmd;
	m_videoFrame.frameACK = new UINT8[16];
	memset(m_videoFrame.frameACK, 0, 16);

	m_audioFrame.frameACK = new UINT8[16];
	memset(m_audioFrame.frameACK, 0, 16);

	return initStream();
}

int Stream::end()
{
	m_videoProcess.join();

	avcodec_close(m_videoDecoder.codecCtx);
	av_free(m_videoDecoder.codecCtx);
	av_frame_free(&m_videoDecoder.decodedFrame);
	av_frame_free(&m_videoDecoder.decodedFrameBGR);

	delete[] m_videoFrame.frame;
	delete[] m_videoFrame.frameACK;
	for (int i = 0; i < m_videoFrame.fragmentPerFrame; i++) {
		delete[] m_videoFrame.fragments[i].data;
	}
	delete[] m_videoFrame.fragments;

	delete[] m_audioFrame.frame;
	delete[] m_audioFrame.frameACK;
	for (int i = 0; i < m_audioFrame.fragmentPerFrame; i++) {
		delete[] m_audioFrame.fragments[i].data;
	}
	delete[] m_audioFrame.fragments;

	return 1;
}

void Stream::start()
{
	m_videoProcess = std::thread([this]{runVideo(); });
	SDL_PauseAudio(0);
}

void Stream::runVideo()
{
	while (true)
	{
		int finished = 0;
		int len = 0;

		StreamData *videoData = m_videoPool.takeData();
		if (videoData == NULL) continue;

		m_videoDecoder.avPacket.data = videoData->frame;
		m_videoDecoder.avPacket.size = videoData->size;

		while (m_videoDecoder.avPacket.size > 0) {
			//decoding
			len = avcodec_decode_video2(m_videoDecoder.codecCtx, m_videoDecoder.decodedFrame,
				&finished, &(m_videoDecoder.avPacket));
			if (len > 0) {
				if (finished){
					m_imageMtx.lock();
					sws_scale(m_videoDecoder.convertCtx,
						(const unsigned char* const*)m_videoDecoder.decodedFrame->data,
						m_videoDecoder.decodedFrame->linesize, 0, m_videoDecoder.codecCtx->height,
						m_videoDecoder.decodedFrameBGR->data, m_videoDecoder.decodedFrameBGR->linesize);
					m_newImage = true;
					m_imageMtx.unlock();
				}

				if (m_videoDecoder.avPacket.data) {
					m_videoDecoder.avPacket.size -= len;
					m_videoDecoder.avPacket.data += len;
				}
			}
			else {
				char buf[200];
				av_strerror(len, buf, 200);
				m_videoDecoder.avPacket.size = 0;
				logE << "Video Decode error :" << buf << std::endl;
			}
		}
	}
}

int Stream::updateAudio(UINT8* buffer, int size)
{
	int rSize = 0;

	m_soundMtx.lock();
	StreamData *audioData = m_audioPool.takeData();
	if (audioData != NULL && audioData->size - 10 > 0)
	{// -10 to remove some noise sound at the beginning frame (don't know why)
		rSize = audioData->size - 10;
		memcpy(buffer, audioData->frame + 10, rSize);
	}
	m_soundMtx.unlock();
	return rSize;
}

int Stream::initStream()
{
	avcodec_register_all();
	av_log_set_level(AV_LOG_QUIET);

	if (initAudio() < 0) return -1;
	if (initVideo() < 0) return -1;
	return 1;
}

int Stream::initAudio()
{
	SDL_AudioSpec audio;

	/* Set the audio format : this one was good for me */
	audio.freq = 16000;
	audio.format = AUDIO_S16SYS; // AUDIO_S16SYS
	audio.channels = 1;
	audio.silence = 0;
	audio.samples = 1024;
	audio.callback = Stream::forwardCallback;
	audio.userdata = this;

	if (SDL_OpenAudio(&audio, NULL) < 0) {
		logE << "Couldn't open audio : " << SDL_GetError() << std::endl;
		return -1;
	}
	return 1;
}

int Stream::initVideo()
{
	// JumpingSumo use AV_CODEC_ID_MJPEG video format
	m_videoDecoder.codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);

	if (m_videoDecoder.codec == NULL){
		logE << "failed to find video decoder" << std::endl;
		return -1;
	}

	m_videoDecoder.codecCtx = avcodec_alloc_context3(m_videoDecoder.codec);
	if (m_videoDecoder.codecCtx == NULL){
		logE << "failed allocate video context" << std::endl;
		return -1;
	}

	m_videoDecoder.codecCtx->pix_fmt = AV_PIX_FMT_YUV422P;
	m_videoDecoder.codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	m_videoDecoder.codecCtx->codec_id = AV_CODEC_ID_MJPEG;

	if (avcodec_open2(m_videoDecoder.codecCtx, m_videoDecoder.codec, NULL) < 0){
		logE << "failed to open video avcodec" << std::endl;
		return -1;
	}

	m_videoDecoder.decodedFrame = av_frame_alloc();
	if (m_videoDecoder.decodedFrame == NULL){
		logE << "failed to allocate decoded video frame" << std::endl;
		return -1;
	}

	m_videoDecoder.decodedFrameBGR = av_frame_alloc();
	if (m_videoDecoder.decodedFrameBGR == NULL){
		logE << "failed to allocate decoded video frame RGB" << std::endl;
		return -1;
	}

	AVPixelFormat pf = AVPixelFormat::AV_PIX_FMT_BGR24;
	m_videoDecoder.buffer = (UINT8*)av_malloc(avpicture_get_size(pf, SCREEN_WIDTH, SCREEN_HEIGHT));
	avpicture_fill((AVPicture*)m_videoDecoder.decodedFrameBGR, m_videoDecoder.buffer, pf, SCREEN_WIDTH, SCREEN_HEIGHT);
	m_videoDecoder.convertCtx = sws_getContext(SCREEN_WIDTH, SCREEN_HEIGHT, m_videoDecoder.codecCtx->pix_fmt, SCREEN_WIDTH, 
		SCREEN_HEIGHT, pf, SWS_SPLINE, NULL, NULL, NULL);

	av_init_packet(&m_videoDecoder.avPacket);
	logI << "initialize video finished" << std::endl;
	return 1;
}

void Stream::newVideoFrame(NetworkFrame *networkFrame)
{
	StreamFrame streamFrame;
	parseStreamFrame(networkFrame, &streamFrame);
	appendNewFrame(&streamFrame, m_videoFrame, BD_NET_CD_VIDEO_ACK_ID, m_videoPool, m_videoMtx);
}

void Stream::newAudioFrame(NetworkFrame *networkFrame)
{
	StreamFrame streamFrame;
	parseStreamFrame(networkFrame, &streamFrame);
	appendNewFrame(&streamFrame, m_audioFrame, BD_NET_CD_SOUND_ACK_ID, m_audioPool, m_soundMtx);
}

void Stream::parseStreamFrame(NetworkFrame *networkFrame, StreamFrame *streamFrame)
{
	streamFrame->frameNumber = Utils::readUInt16(networkFrame->data, 0);
	streamFrame->frameFlags = networkFrame->data[2];
	streamFrame->fragmentNumber = networkFrame->data[3];
	streamFrame->fragmentPerFrame = networkFrame->data[4];
	streamFrame->frameSize = networkFrame->dataSize - 5;
	streamFrame->frame = new UINT8[streamFrame->frameSize];
	memcpy(streamFrame->frame, &networkFrame->data[5], streamFrame->frameSize);
}

void Stream::appendNewFrame(StreamFrame *streamFrame, ArStreamFrame& avFrame, int id, Pool& pool, std::mutex& mxt)
{
	if (streamFrame->frameNumber != avFrame.frameNumber) {

		if (avFrame.fragmentCounter > 0) {
			bool sendFlag = false;

			if (avFrame.frameFlags == 1 || streamFrame->frameNumber > avFrame.frameNumber) {
				avFrame.waitFlag = false;
				sendFlag = true;
				//logI << "frame number: " << streamFrame->frameNumber << std::endl;
			}
			else {
				avFrame.waitFlag = true;
				//logI << "number1: " << streamFrame->frameNumber << " number2: " << avFrame.frameNumber << std::endl;
			}

			if (sendFlag) {
				bool skip = false;
				for (int i = 0; i < avFrame.fragmentPerFrame; i++) {
					if (avFrame.fragments[i].size < 0) {
						skip = true;
						break;
					}
					int tempSize = avFrame.frameSize;
					UINT8 *temp = new UINT8[tempSize];
					memcpy(temp, avFrame.frame, tempSize);

					delete[] avFrame.frame;
					avFrame.frameSize = tempSize + avFrame.fragments[i].size;
					avFrame.frame = new UINT8[avFrame.frameSize];

					memcpy(avFrame.frame, temp, tempSize);
					memcpy(&(avFrame.frame[tempSize]), avFrame.fragments[i].data, avFrame.fragments[i].size);

					delete[] temp;
				}

				if (!skip) {
					mxt.lock();
					pool.addData(avFrame.frame, avFrame.frameSize);
					mxt.unlock();
				}
			}
		}

		for (int i = 0; i < avFrame.fragmentPerFrame; i++) {
			delete[] avFrame.fragments[i].data;
		}
		delete[] avFrame.fragments;

		avFrame.frameNumber = streamFrame->frameNumber;
		avFrame.frameSize = 0;
		avFrame.frameFlags = streamFrame->frameFlags;
		avFrame.fragmentCounter = 0;
		avFrame.fragments = new Fragment[streamFrame->fragmentPerFrame];
		avFrame.fragmentPerFrame = streamFrame->fragmentPerFrame;

		delete[] avFrame.frameACK;
		avFrame.frameACK = new UINT8[16];
		memset(avFrame.frameACK, 0, 16);
	}

	delete[] avFrame.fragments[streamFrame->fragmentNumber].data;
	avFrame.fragments[streamFrame->fragmentNumber].data = new UINT8[streamFrame->frameSize];
	memcpy(avFrame.fragments[streamFrame->fragmentNumber].data, streamFrame->frame, streamFrame->frameSize);
	avFrame.fragments[streamFrame->fragmentNumber].size = streamFrame->frameSize;
	avFrame.fragmentCounter++;

	int bufferPosition = (int)floor(streamFrame->fragmentNumber / 8 | 0);
	avFrame.frameACK[bufferPosition] |= (UINT8)(1 << streamFrame->fragmentNumber % 8);

	delete[] streamFrame->frame;

	AckPacket ack;
	ack.frameNumber = avFrame.frameNumber;
	ack.packetsACK = new UINT8[16];

	memcpy(ack.packetsACK, &avFrame.frameACK[8], 8);
	memcpy(&ack.packetsACK[8], avFrame.frameACK, 8);

	UINT8* buf = new UINT8[18];
	memset(buf, 0, 18);

	buf[0] = (UINT8)(ack.frameNumber & 0xff);
	buf[1] = (UINT8)((ack.frameNumber & 0xff00) >> 8);
	memcpy(&buf[2], ack.packetsACK, 16);
	m_cmd->sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, id, 18, buf);
}

cv::Mat Stream::getImage()
{
	cv::Mat mat;

	m_imageMtx.lock();
	if (m_newImage) {
		IplImage *img = cvCreateImage(cvSize(SCREEN_WIDTH, SCREEN_HEIGHT), IPL_DEPTH_8U, 3);
		memcpy(img->imageData, m_videoDecoder.decodedFrameBGR->data[0], SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uchar) * 3);

		mat = cv::cvarrToMat(img, true);
		cvReleaseImage(&img);
		m_newImage = false;
	}
	m_imageMtx.unlock();
	return mat;
}

void Stream::audioCallback(UINT8 *stream, int len)
{
	int len1;

	static UINT8 audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	static unsigned int audio_buf_size = 0;
	static unsigned int audio_buf_index = 0;

	while (len > 0) {
		if (audio_buf_index >= audio_buf_size) {
			audio_buf_index = 0;
			audio_buf_size = updateAudio(audio_buf, sizeof(audio_buf));
			if (audio_buf_size <= 0) { /* If error, output silence */
				audio_buf_size = 1024;
				memset(audio_buf, 0, audio_buf_size);
			}
		}
		len1 = audio_buf_size - audio_buf_index;
		if (len1 > len)
			len1 = len;
		memcpy(stream, audio_buf + audio_buf_index, len1);
		len -= len1;
		stream += len1;
		audio_buf_index += len1;
	}
}