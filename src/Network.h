/*!
* \file Network.h
* \brief Network manager class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma

#include "UDPSocket.h"
#include "TCPClient.h"
#include "Command.h"
#include "DroneData.h"
#include "Stream.h"
#include "Config.h"

#include <thread>

typedef struct NetworkFrame {
	UINT8 type;		// data type (1 byte)
	UINT8 id;		// target buffer (1 byte)
	UINT8 seq;		// Sequence number (1 byte)
	UINT32 size;	// Total size of the frame
	int dataSize;	// Total size of the data
	UINT8 *data;	// Data 

} NetworkFrame;	// Network frame data structure

class Network
{
public:
	int begin(Command* cmd, DroneData* ddata, Stream* stream);
	int end();
	void start();

	/*!
	* \brief Starts a handshake connection with the drone.
	* \return -1 when the communication failed, otherwise 1.
	*/
	int discovery();

private:
	void run();
	/*!
	* \brief Builds the given network frame from the given raw data buffer.
	* \param buf : raw data buffer
	* \param networkFrame : network frame to build
	*/
	void parseRawFrame(char* buf, NetworkFrame* networkFrame);

	int m_maxFragmentSize = 65000;		// max fragment size
	int m_maxFragmentNumber = 1;		// max fragment number
	std::thread m_process;
	UDPSocket m_recpSocket;				// Receiver UDP socket
	Command* m_cmd;
	DroneData* m_ddata;
	Stream* m_stream;
};

