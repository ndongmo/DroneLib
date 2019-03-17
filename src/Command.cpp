#include "Command.h"
#include "Logger.h"
#include "Utils.h"

int Command::begin(DroneData* ev)
{
	m_event = ev;

	int err = m_sendSocket.open(IP, SEND_PORT);
	if (err == -1){
		logE << "Open send socket !" << std::endl;
		return err;
	}

	m_seqBuf = new int[256];
	m_buffer = new UINT8[256];
	memset(m_seqBuf, 0, 256); // init seq buffer

	return err;
}

void Command::start()
{
	sendAllStates();
	enableVideoStream(true);
	enableAudioStream(true);
}

int Command::end()
{
	delete[] m_seqBuf;
	delete[] m_buffer;
	return 1;
}

UINT8 Command::getNextSeqBuf(int id)
{
	m_seqBuf[id]++;
	if (m_seqBuf[id] > 255) m_seqBuf[id] = 0;
	return m_seqBuf[id];
}
int Command::getLength(const char* format)
{
	int length = 0;
	while (*format != '\0') {
		if (*format == 'b') length += 1;
		else length += *format - '0';

		format++;
	}
	return length;
}

void Command::sendCmd(int type, int id, const char* format, ...)
{
	int bufSize = getLength(format) + 7;
	m_sendMtx.lock();

	m_buffer[0] = (UINT8)type;								// data type (1 byte)
	m_buffer[1] = (UINT8)id;									// target buffer (1 byte)
	m_buffer[2] = getNextSeqBuf(id);							// Sequence number (1 byte)
	m_buffer[3] = (UINT8)(bufSize & 0xff);					// Total size of the frame  (1st byte)
	m_buffer[4] = (UINT8)((bufSize & 0xff00) >> 8);			// Total size of the frame  (2nd byte)
	m_buffer[5] = (UINT8)((bufSize & 0xff0000) >> 16);		// Total size of the frame  (3th byte)
	m_buffer[6] = (UINT8)((bufSize & 0xff000000) >> 24);		// Total size of the frame  (4th byte)

	va_list args;
	int count = 7;
	UINT8 v1; UINT16 v2; int v3; UINT8 b;

	va_start(args, format); {
		while (*format != '\0' && count < bufSize) {
			switch (*format++) {
			case 'b':
				b = (UINT8)va_arg(args, bool);
				m_buffer[count++] = b;
				break;
			case '1':
				v1 = (UINT8)va_arg(args, int);
				m_buffer[count++] = v1;
				break;
			case '2':
				v2 = (UINT16)va_arg(args, int);
				Utils::writeUInt16(v2, m_buffer, count);
				count += 2;
				break;
			case '4':
				v3 = va_arg(args, int);
				Utils::writeUInt32(v3, m_buffer, count);
				count += 4;
				break;
			}
		}
	} va_end(args);

	m_sendSocket.send((char*)m_buffer, bufSize);
	m_sendMtx.unlock();
}

void Command::sendCmd(int type, int id, int length, UINT8* data)
{
	int bufSize = length + 7;
	m_sendMtx.lock();

	m_buffer[0] = (UINT8)type;
	m_buffer[1] = (UINT8)id;
	m_buffer[2] = getNextSeqBuf(id);
	m_buffer[3] = (UINT8)(bufSize & 0xff);
	m_buffer[4] = (UINT8)((bufSize & 0xff00) >> 8);
	m_buffer[5] = (UINT8)((bufSize & 0xff0000) >> 16);
	m_buffer[6] = (UINT8)((bufSize & 0xff000000) >> 24);

	memcpy(&m_buffer[7], data, length);

	m_sendSocket.send((char*)m_buffer, bufSize);
	m_sendMtx.unlock();
}

void Command::sendAllStates()
{
	sendCmd(
		ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID,
		"112", ARCOMMANDS_ID_FEATURE_COMMON,
		ARCOMMANDS_ID_COMMON_CLASS_COMMON,
		ARCOMMANDS_ID_COMMON_COMMON_CMD_ALLSTATES);
}

void Command::sendAck(UINT8 seq, UINT8 id)
{
	id = (id + (int)(ARNETWORKAL_MANAGER_DEFAULT_ID_MAX / 2));
	sendCmd(ARNETWORKAL_FRAME_TYPE_ACK, id, "1", seq);
}
void Command::sendPong(UINT8* data, int size)
{
	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, ARNETWORK_MANAGER_INTERNAL_BUFFER_ID_PONG,
		size, data);
}
void Command::enableAudioStream(bool enable)
{
	if (m_event->m_audioState) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "1121",
		ARCOMMANDS_ID_FEATURE_COMMON, ARCOMMANDS_ID_COMMON_CLASS_AUDIO,
		ARCOMMANDS_ID_COMMON_AUDIO_CMD_CONTROLLERREADYFORSTREAMING, enable);
}

void Command::enableVideoStream(bool enable)
{
	if (
		(m_event->m_videoError == MediaError::MECAMERA_KO) ||
		(m_event->m_videoError == MediaError::MEMEMORY_FULL) ||
		(m_event->m_videoEvError == VideoEvError::VVBUSY)
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112b",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_MEDIASTREAMING,
		ARCOMMANDS_ID_JUMPINGSUMO_MEDIASTREAMING_CMD_VIDEOENABLE, enable);
}

void Command::setVideoAutoRecord(bool enable)
{
	if (
		(m_event->m_videoError == MediaError::MECAMERA_KO) ||
		(m_event->m_videoError == MediaError::MEMEMORY_FULL) ||
		(m_event->m_videoEvError == VideoEvError::VVBUSY) 
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112b",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_VIDEOSETTINGS,
		ARCOMMANDS_ID_JUMPINGSUMO_VIDEOSETTINGS_CMD_AUTORECORD, enable);
}

void Command::recordVideo(bool start)
{
	if (
		(m_event->m_videoError == MediaError::MECAMERA_KO) ||
		(m_event->m_videoError == MediaError::MEMEMORY_FULL) ||
		(m_event->m_videoEvError == VideoEvError::VVBUSY)
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "1124",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_MEDIARECORD,
		ARCOMMANDS_ID_JUMPINGSUMO_MEDIARECORD_CMD_VIDEOV2, (int)start);
}

void Command::takePicture()
{
	if (
		(m_event->m_picError == MediaError::MECAMERA_KO) ||
		(m_event->m_picError == MediaError::MEMEMORY_FULL) ||
		(m_event->m_picEvError == PicEvError::PVBUSY) ||
		(m_event->m_picEvError == PicEvError::PVMEMORY_FULL)
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_MEDIARECORD,
		ARCOMMANDS_ID_JUMPINGSUMO_MEDIARECORD_CMD_PICTUREV2);
}

void Command::move(int x, int y)
{
	if (x > 100) x = 100;
	else if (x < -100) x = -100;
	if (y > 100) y = 100;
	else if (y < -100) y = -100;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112111",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_PILOTING,
		ARCOMMANDS_ID_JUMPINGSUMO_PILOTING_CMD_PCMD, 1, x, y);
}
void Command::changePosture(Posture p)
{
	if (p == m_event->m_posture) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "1124",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_PILOTING,
		ARCOMMANDS_ID_JUMPINGSUMO_PILOTING_CMD_POSTURE, (int)p);
}

void Command::stopJump()
{
	if (m_event->m_isJumping) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_ANIMATIONS,
		ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONS_CMD_JUMPSTOP);
}

void Command::cancelJump()
{
	if (m_event->m_isJumping) return;
	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_ANIMATIONS,
		ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONS_CMD_JUMPCANCEL);
}

void Command::loadJump()
{
	if (m_event->m_isJumping ||
		m_event->m_motorState == MotorState::MSBLOCKED ||
		m_event->m_motorState == MotorState::MSOVERHEATED ||
		m_event->m_animState == AnimState::ANBUSY ||
		m_event->m_animState == AnimState::ANLOADED ||
		m_event->m_animState == AnimState::ANLOW_BAT_LOADED
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "112",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_ANIMATIONS,
		ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONS_CMD_JUMPLOAD);
}

void Command::jump(JumpType type)
{
	if (m_event->m_isJumping || 
		m_event->m_motorState == MotorState::MSBLOCKED ||
		m_event->m_motorState == MotorState::MSOVERHEATED
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "1124",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_ANIMATIONS,
		ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONS_CMD_JUMP, (int)type);
}

void Command::startAnim(Animation anim)
{
	if (m_event->m_isJumping ||
		m_event->m_motorState == MotorState::MSBLOCKED ||
		m_event->m_motorState == MotorState::MSOVERHEATED ||
		m_event->m_animState == AnimState::ANBUSY ||
		m_event->m_animState == anim
		) return;

	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "1124",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_ANIMATIONS,
		ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONS_CMD_SIMPLEANIMATION, (int)anim);
}

void Command::addCapOffset(float offset)
{
	sendCmd(ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID, "1124",
		ARCOMMANDS_ID_FEATURE_JUMPINGSUMO, ARCOMMANDS_ID_JUMPINGSUMO_CLASS_PILOTING,
		ARCOMMANDS_ID_JUMPINGSUMO_PILOTING_CMD_ADDCAPOFFSET, offset);
}

void Command::setWifiScanMode(NetworkBand band)
{
	sendCmd(
		ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID,
		"1124", ARCOMMANDS_ID_FEATURE_JUMPINGSUMO,
		ARCOMMANDS_ID_JUMPINGSUMO_CLASS_NETWORK,
		ARCOMMANDS_ID_JUMPINGSUMO_NETWORK_CMD_WIFISCAN, (int)band);
}
void Command::incrVolume(int vol)
{
	m_event->m_volume += vol;
	if (m_event->m_volume < 0) m_event->m_volume = 0;
	else if (m_event->m_volume > 100) m_event->m_volume = 100;

	sendCmd(
		ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID,
		"1121", ARCOMMANDS_ID_FEATURE_JUMPINGSUMO,
		ARCOMMANDS_ID_JUMPINGSUMO_CLASS_AUDIOSETTINGS,
		ARCOMMANDS_ID_JUMPINGSUMO_AUDIOSETTINGS_CMD_MASTERVOLUME, m_event->m_volume);
}

void Command::setAudioTheme(AudioTheme theme)
{
	if (theme != m_event->m_audioTheme) {
		sendCmd(
			ARNETWORKAL_FRAME_TYPE_DATA, BD_NET_CD_NONACK_ID,
			"1124", ARCOMMANDS_ID_FEATURE_JUMPINGSUMO,
			ARCOMMANDS_ID_JUMPINGSUMO_CLASS_AUDIOSETTINGS,
			ARCOMMANDS_ID_JUMPINGSUMO_AUDIOSETTINGS_CMD_THEME, (int)theme);
	}
}