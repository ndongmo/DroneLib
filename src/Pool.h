/*!
* \file Pool.h
* \brief Pool data manager class.
* \author Ndongmo Silatsa Fabrice
* \date 12-03-2019
* \version 2.0
*/

#pragma once

#include <WS2tcpip.h>
#include <mutex>

#define POOL_SIZE 256

typedef struct {
	UINT8 *frame;	// stream frame
	int size;		// frame size
} StreamData;		// Stream data structure

class Pool
{
public:
	~Pool();

	/*!
	* \brief Add the given data in the data pool.
	* \param data : data to add
	* \param size : size
	*/
	void addData(UINT8 *data, int size);
	/*!
	* \brief Retrieves the current video data (relying on the readindex cursor).
	* \return current video data in the pool
	*/
	StreamData* takeData();

private:
	int m_readIndex = 0;				// Pool cursor for reading data
	int m_writeIndex = 0;				// Pool cursor for adding data
	StreamData m_pool[POOL_SIZE];		// pool of stream data
};

