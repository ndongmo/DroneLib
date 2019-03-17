#include "Utils.h"

void Utils::writeUInt32(UINT32 value, UINT8* byte, int index)
{
	byte[index++] = (UINT8)(value & 0xff);
	byte[index++] = (UINT8)((value & 0xff00) >> 8);
	byte[index++] = (UINT8)((value & 0xff0000) >> 16);
	byte[index] = (UINT8)((value & 0xff000000) >> 24);
}

void Utils::writeUInt16(UINT16 value, UINT8* byte, int index)
{
	byte[index++] = (UINT8)(value & 0xff);
	byte[index] = (UINT8)((value & 0xff00) >> 8);
}

UINT32 Utils::readUInt32(UINT8 *byte, int index){
	return (UINT32)((byte[index] << 0) | (byte[index + 1] << 8) |
		(byte[index + 2] << 16) | (byte[index + 3] << 24));
}

UINT32 Utils::readUInt32(char *byte, int index){
	return (UINT32)((UINT8)(byte[index] << 0) | ((UINT8)byte[index + 1] << 8) |
		((UINT8)byte[index + 2] << 16) | ((UINT8)byte[index + 3] << 24));
}

UINT16 Utils::readUInt16(UINT8 *byte, int index){
	return (UINT16)((byte[index] << 0) | (byte[index + 1] << 8));
}

UINT16 Utils::readUInt16(char *byte, int index){
	return (UINT16)((UINT8)(byte[index] << 0) | (UINT8)(byte[index + 1] << 8));
}

std::string Utils::readString(char *byte, int index)
{
	std::string str;
	while (*byte != '\0') {
		str += *byte++;
	}
	return str;
}