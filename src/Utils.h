/*!
* \file Utils.h
* \brief Implements useful functions.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma

#include <WS2tcpip.h>
#include <string>

class Utils
{
public:
	/*!
	* \brief Writes the given UINT32 on the given char* buffer starting to the index.
	*/
	static void writeUInt32(UINT32 value, UINT8* byte, int index);
	/*!
	* \brief Writes the given UINT16 on the given char* buffer starting to the index.
	*/
	static void writeUInt16(UINT16 value, UINT8* byte, int index);
	/*!
	* \brief Gets the string from the given char array.
	* \return string.
	*/
	static std::string readString(char *byte, int index);
	/*!
	* \brief Build an unsigned int32 by reading
	*	consecutive byte (UINT8) starting to the given index.
	* \return the builded UINT32.
	*/
	static UINT32 readUInt32(UINT8 *byte, int index);
	static UINT32 readUInt32(char *byte, int index);
	/*!
	* \brief Build an unsigned int16 by reading
	*	consecutive byte (UINT8) starting to the given index.
	* \return the builded UINT16.
	*/
	static UINT16 readUInt16(UINT8 *byte, int index);
	static UINT16 readUInt16(char *byte, int index);
};

