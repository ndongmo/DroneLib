#include "Network.h"
#include "Logger.h"
#include "JsonParser.h"
#include "Utils.h"

int Network::begin(Command* cmd, DroneData* ddata, Stream* stream)
{
	m_cmd = cmd;
	m_stream = stream;
	m_ddata = ddata;

	int err = discovery();
	if (err == -1) {
		logE << "discovery error" << std::endl;
		return err;
	}

	err = m_recpSocket.open(IP, RECP_PORT);
	if (err == -1){
		logE << "socket error" << std::endl;
	}
	return err;
}

int Network::end()
{
	m_process.join();

	return 1;
}

void Network::start()
{
	m_process = std::thread([this]{run(); });
}

void Network::run()
{
	int len;
	char *buf = new char[m_maxFragmentSize];

	while (true) {

		memset(buf, 0, m_maxFragmentSize);					// buffer initialization
		len = m_recpSocket.receive(buf, m_maxFragmentSize); //receive data
		if (len == -1){
			logE << "Network receive socket error" << std::endl;
			return;
		}

		NetworkFrame networkFrame;
		parseRawFrame(buf, &networkFrame);

		if (networkFrame.type == ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK) {
			//logI << "ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK" << std::endl;
			m_cmd->sendAck(networkFrame.seq, networkFrame.id);
		}
		if (networkFrame.type == ARNETWORKAL_FRAME_TYPE_DATA_LOW_LATENCY && networkFrame.id == BD_NET_DC_VIDEO_DATA_ID) {
			//logI << "new video frame" << std::endl;
			m_stream->newVideoFrame(&networkFrame);
		}
		else if (networkFrame.type == ARNETWORKAL_FRAME_TYPE_DATA_LOW_LATENCY && networkFrame.id == BD_NET_DC_SOUND_DATA_ID) {
			//logI << "new audio frame" << std::endl;
			m_stream->newAudioFrame(&networkFrame);
		}
		else if (networkFrame.id == BD_NET_DC_EVENT_ID){
			m_ddata->newEvent(networkFrame.data);
		}
		else if (networkFrame.id == BD_NET_DC_NAVDATA_ID){
			m_ddata->newNavEvent(networkFrame.data);
		}
		else if (networkFrame.id == ARNETWORK_MANAGER_INTERNAL_BUFFER_ID_PING){
			//logI << "ping" << std::endl; 
			m_cmd->sendPong(networkFrame.data, networkFrame.size);
		}
		else 
			logI << "type = " << (int)networkFrame.type << " | id = " << (int)networkFrame.id << std::endl;

		delete[] networkFrame.data;

	}
	delete[] buf;
}

int Network::discovery()
{
	TCPClient tcpClient;
	JsonParser parser;
	parser.add("controller_type", "computer")
		  .add("controller_name", "ndongmo")
		  .add("d2c_port", "43210");
	std::string message = parser.getJson();

	char buf[1024];

	//socket open
	int err = tcpClient.open(IP, DISCOVERY_PORT);
	if ( err < 0) return err;
	

	//send message
	if (tcpClient.send(message.c_str(), message.length()) == -1){
		return -1;
	}

	//receive
	if (tcpClient.receive(buf, 1024) == -1){
		return -1;
	}

	err = parser.decode(buf);
	if (err < 0){
		logE << "Json parser error" << std::endl;
		return err;
	}

	try {
		m_maxFragmentSize = std::stoi(parser.get("arstream_fragment_size"));
		m_maxFragmentNumber = std::stoi(parser.get("arstream_fragment_maximum_number"));
	}
	catch (std::exception) {
		logE << "Json parser error" << std::endl;
		return err;
	}

	return 1;
}

void Network::parseRawFrame(char* buf, NetworkFrame *networkFrame)
{
	networkFrame->type = (UINT8)buf[0];
	networkFrame->id = (UINT8)buf[1];
	networkFrame->seq = (UINT8)buf[2];
	networkFrame->size = Utils::readUInt32(buf, 3);
	networkFrame->dataSize = networkFrame->size - 7;

	if (networkFrame->size > 7){
		networkFrame->data = new UINT8[networkFrame->dataSize];
		memcpy(networkFrame->data, &buf[7], networkFrame->dataSize);
	}
}