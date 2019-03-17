/*!
* \file Command.h
* \brief Command manager class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "UDPSocket.h"
#include "Config.h"
#include "DroneData.h"

#include <queue>
#include <thread>
#include <mutex>

class Command
{
public:
	int begin(DroneData* ev);
	void start();
	int end();

	/*!
	* \brief Starts/Stops video recording.
	*/
	void recordVideo(bool start);
	/*!
	* \brief Enables video recording.
	*/
	void enableVideoStream(bool enable);
	/*!
	* \brief Enables audio recording.
	*/
	void enableAudioStream(bool enable);
	/*!
	* \brief Enables the video auto record setting.
	*/
	void setVideoAutoRecord(bool enable);
	/*!
	* \brief Sends Stop jump command.
	*/
	void stopJump();
	/*!
	* \brief Sends Cancel jump command.
	*/
	void cancelJump();
	/*!
	* \brief Sends Load jump command.
	*/
	void loadJump();
	/*!
	* \brief Sends a command for requesting drone jump.
	*/
	void jump(JumpType jt);
	/*!
	* \brief Sends Start anim command.
	*/
	void startAnim(Animation anim);
	/*!
	* \brief Sends a command with the given moves to the drone.
	*/
	void move(int x, int y);
	/*!
	* \brief Sends a command for requesting drone posture.
	*/
	void changePosture(Posture p);
	/*!
	* \brief Add the specified offset to the current cap.
	*/
	void addCapOffset(float offset);
	/*!
	* \brief Sends the take picture command to the drone.
	*/
	void takePicture();
	/*!
	* \brief Sends Wifi scan command to the drone.
	*/
	void setWifiScanMode(NetworkBand band);
	/*!
	* \brief Sends a command to increase/decrease drone volume.
	*/
	void incrVolume(int vol);
	/*!
	* \brief Sends a command to set the audio theme of the drone.
	*/
	void setAudioTheme(AudioTheme theme);

	/*!
	* \brief Sends the given formated command (UDP packet).
	* \param type : data type
	* \param id : target buffer id
	* \param length : the sum of all char in the format string.
	*	that means for and int value the length will be sizeof(int) = 4.
	* \param format : string value where every char represent an argument type at the same position.
	*	example : "1214" -> first argument is UINT8, 2) UINT16, 3) UINT8, 4)UINT32 or int.
	* \param ... : arguments
	*/
	void sendCmd(int type, int id, const char* format, ...);
	/*!
	* \brief Sends the given formated command (UDP packet).
	* \param type : data type
	* \param id : target buffer id
	* \param length : total data length (without type and id) in UINT8 unit.
	*	that means for and int value the length will be sizeof(int) = 4.
	* \param data : data to send
	*/
	void sendCmd(int type, int id, int length, UINT8* data);
	/*!
	* \brief Request all state command.
	*/
	void sendAllStates();
	/*!
	* \brief Builds and sends a pong command from the given ping network frame.
	*/
	void sendPong(UINT8* data, int size);
	/*!
	* \brief Builds and sends Ack packet with the given network frame sequence number and target buffer id.
	* \param networkFrame : network frame
	*/
	void sendAck(UINT8 seq, UINT8 id);

private:
	/*!
	* \brief Returns the next sequence number at the buffer id.
	*/
	UINT8 getNextSeqBuf(int id);
	/*!
	* \brief Returns the length of integers in the given string.
	*/
	int getLength(const char* format);

	int* m_seqBuf;
	UINT8* m_buffer;
	std::mutex m_sendMtx;
	UDPSocket m_sendSocket;			// Sender UDP socket
	DroneData* m_event;		// event handler
};

