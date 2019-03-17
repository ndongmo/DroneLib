/*!
* \file DroneData.h
* \brief Handles drone events and keeps drone data.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "Config.h"

#include <WS2tcpip.h>

class DroneData
{
public:
	void newEvent(UINT8* data);
	void newNavEvent(UINT8* data);
	
	int m_volume = 0;
	int m_speed = 0;
	int m_realSpeed = 0;
	int m_wifiQuality = 0;
	int m_battery = 100;
	bool m_isJumping = false;
	bool m_isAnimated = false;
	bool m_audioState = false;
	Posture m_posture;
	NetworkBand m_NetworkBand;
	AudioTheme m_audioTheme;
	BatteryState m_batteryState;
	AnimState m_animState;
	JumpState m_jumpState;
	MotorState m_motorState;
	PictureState m_picState;
	MediaError m_picError;
	VideoState m_videoState;
	MediaError m_videoError;
	PictureEvent m_picEvent;
	VideoEvent m_videoEvent;
	PicEvError m_picEvError;
	VideoEvError m_videoEvError;
	WifiSelection m_wifiSelec;
	WifiType m_wifiType;
	StreamState m_streamState;
	VideoAuto m_videoAuto;
};

