#include "DroneData.h"
#include "Logger.h"
#include "Utils.h"

void DroneData::newNavEvent(UINT8* data)
{
	if (data[0] == ARCOMMANDS_ID_FEATURE_COMMON)
	{
		UINT8 _class = data[1];
		UINT16 _cmd = Utils::readUInt16(data, 2);

		if (_class == ARCOMMANDS_ID_COMMON_CLASS_COMMONSTATE) {
			if (_cmd == ARCOMMANDS_ID_COMMON_COMMONSTATE_CMD_BATTERYSTATECHANGED) {
				logI << "------------------------- BATTERY VALUE CHANGED ---------------------------" << std::endl;
				logI << "battery :" << (m_battery = (int)data[4]) << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_COMMON_CLASS_AUDIOSTATE) {
			if (_cmd == ARCOMMANDS_ID_COMMON_AUDIOSTATE_CMD_AUDIOSTREAMINGRUNNING) {
				logI << "------------------------- AUDIO STATE CHANGED ---------------------------" << std::endl;
				logI << "enabled :" << (m_audioState = (int)data[4]) << std::endl;
			}
		}
	}
}

void DroneData::newEvent(UINT8* data)
{
	if (data[0] == ARCOMMANDS_ID_FEATURE_JUMPINGSUMO) {
		UINT8 _class = data[1];
		UINT16 _cmd = Utils::readUInt16(data, 2);

		if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_PILOTINGSTATE) {
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_PILOTINGSTATE_CMD_SPEEDCHANGED) {
				logI << "------------------------- SPEED CHANGED ---------------------------" << std::endl;
				logI << "speed :" << (m_speed = (int)data[4]) << std::endl;
				logI << "realSpeed :" << (m_realSpeed = (int)Utils::readUInt16(data, 5)) << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_PILOTINGSTATE_CMD_POSTURECHANGED) {
				logI << "------------------------- POSTURE CHANGED ------------------------" << std::endl;
				m_posture = (Posture)Utils::readUInt32(data, 4);
				if (m_posture == Posture::PSTANDING) logI << "standing" << std::endl;
				else if (m_posture == Posture::PJUMPER) logI << "jumper" << std::endl;
				else if (m_posture == Posture::PKICKER) logI << "kicker" << std::endl;
				else if (m_posture == Posture::PSTUCK) logI << "stuck" << std::endl;
				else if (m_posture == Posture::PUNKNOWN) logI << "unknown" << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_PILOTINGSTATE_CMD_ALERTSTATECHANGED) {
				logI << "---------------------- ALERT STATE CHANGED ------------------------" << std::endl;
				m_batteryState = (BatteryState)Utils::readUInt32(data, 4);
				if (m_batteryState == BatteryState::BNO_ALERT) logI << "No alert" << std::endl;
				else if (m_batteryState == BatteryState::BCRITICAL_ALERT) logI << "Critical battery alert" << std::endl;
				else if (m_batteryState == BatteryState::BLOW_ALERT) logI << "Low battery alert" << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_ANIMATIONSSTATE){
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONSSTATE_CMD_JUMPLOADCHANGED) {
				logI << "------------------------- JUMP LOAD CHANGED ---------------------------" << std::endl;
				m_animState = (AnimState)Utils::readUInt32(data, 4);
				if (m_animState == AnimState::ANUNKNOWN) logI << "Unknown state" << std::endl;
				else if (m_animState == AnimState::ANUNLOADED) logI << "Unloaded state" << std::endl;
				else if (m_animState == AnimState::ANLOADED) logI << "Loaded state" << std::endl;
				else if (m_animState == AnimState::ANBUSY) logI << "Busy : Unknown state" << std::endl;
				else if (m_animState == AnimState::ANLOW_BAT_UNLOADED) logI << "Unloaded state and low battery" << std::endl;
				else if (m_animState == AnimState::ANLOW_BAT_UNLOADED) logI << "Loaded state and low battery" << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONSSTATE_CMD_JUMPTYPECHANGED) {
				logI << "------------------------- JUMP TYPE CHANGED ------------------------" << std::endl;
				m_jumpState = (JumpState)Utils::readUInt32(data, 4);
				if (m_jumpState == JumpState::JSNONE) logI << "None" << std::endl;
				else if (m_jumpState == JumpState::JSLONG_JUMP) logI << "Long jump type" << std::endl;
				else if (m_jumpState == JumpState::JSHIGH_JUMP) logI << "High jump type" << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_ANIMATIONSSTATE_CMD_JUMPMOTORPROBLEMCHANGED) {
				logI << "---------------------- JUMP MOTOR PROBLEM CHANGED ------------------------" << std::endl;
				m_motorState = (MotorState)Utils::readUInt32(data, 4);
				if (m_motorState == MotorState::MSNONE) logI << "None" << std::endl;
				else if (m_motorState == MotorState::MSBLOCKED) logI << "Motor blocked" << std::endl;
				else if (m_motorState == MotorState::MSOVERHEATED) logI << "Motor over heated" << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_MEDIARECORDSTATE){
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_MEDIARECORDSTATE_CMD_PICTURESTATECHANGEDV2) {
				logI << "------------------------- PICTURE STATE CHANGED ---------------------------" << std::endl;
				m_picState = (PictureState)Utils::readUInt32(data, 4);
				if (m_picState == PictureState::PSREADY) logI << "The picture recording is ready" << std::endl;
				else if (m_picState == PictureState::PSBUSY) logI << "The picture recording is busy" << std::endl;
				else if (m_picState == PictureState::PSNOT_AVAILABLE) logI << "The picture recording is not available" << std::endl;

				m_picError = (MediaError)Utils::readUInt32(data, 8);
				if (m_picError == MediaError::MENO_ERROR) logI << "No Error" << std::endl;
				else if (m_picError == MediaError::MEUNKNOWN) logI << "Unknown generic error" << std::endl;
				else if (m_picError == MediaError::MECAMERA_KO) logI << "Picture camera is out of order" << std::endl;
				else if (m_picError == MediaError::MEMEMORY_FULL) logI << "Memory full ; cannot save one additional picture" << std::endl;
				else if (m_picError == MediaError::MELOW_BATTERY) logI << "Battery is too low to start/keep recording." << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_MEDIARECORDSTATE_CMD_VIDEOSTATECHANGEDV2) {
				logI << "------------------------- VIDEO STATE CHANGED ---------------------------" << std::endl;
				m_videoState = (VideoState)Utils::readUInt32(data, 4);
				if (m_videoState == VideoState::VSSTOPED) logI << "Video is stopped" << std::endl;
				else if (m_videoState == VideoState::VSSTARTED) logI << "Video is started" << std::endl;
				else if (m_videoState == VideoState::VSNOT_AVAILABLE) logI << "The video recording is not available" << std::endl;

				m_videoError = (MediaError)Utils::readUInt32(data, 8);
				if (m_videoError == MediaError::MENO_ERROR) logI << "No Error" << std::endl;
				else if (m_videoError == MediaError::MEUNKNOWN) logI << "Unknown generic error" << std::endl;
				else if (m_videoError == MediaError::MECAMERA_KO) logI << "Video camera is out of order" << std::endl;
				else if (m_videoError == MediaError::MEMEMORY_FULL) logI << "Memory full ; cannot save one additional video" << std::endl;
				else if (m_videoError == MediaError::MELOW_BATTERY) logI << "Battery is too low to start/keep recording." << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_MEDIARECORDEVENT) {
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_MEDIARECORDEVENT_CMD_PICTUREEVENTCHANGED) {
				logI << "------------------------- PICTURE EVENT CHANGED ---------------------------" << std::endl;
				m_picEvent = (PictureEvent)Utils::readUInt32(data, 4);
				if (m_picEvent == PictureEvent::PETAKEN) logI << "Picture taken and saved" << std::endl;
				else if (m_picEvent == PictureEvent::PEFAILED) logI << "Picture failed" << std::endl;

				m_picEvError = (PicEvError)Utils::readUInt32(data, 8);
				if (m_picEvError == PicEvError::PVRNO_ERROR) logI << "No Error" << std::endl;
				else if (m_picEvError == PicEvError::PVUNKNOWN) logI << "Unknown generic error" << std::endl;
				else if (m_picEvError == PicEvError::PVBUSY) logI << "Picture recording is busy" << std::endl;
				else if (m_picEvError == PicEvError::PVNOT_AVAILABLE) logI << "Picture recording not available" << std::endl;
				else if (m_picEvError == PicEvError::PVMEMORY_FULL) logI << "Memory full" << std::endl;
				else if (m_picEvError == PicEvError::PVLOW_BATTERY) logI << "Battery is too low to record" << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_MEDIARECORDEVENT_CMD_VIDEOEVENTCHANGED) {
				logI << "------------------------- VIDEO EVENT CHANGED ---------------------------" << std::endl;
				m_videoEvent = (VideoEvent)Utils::readUInt32(data, 4);
				if (m_videoEvent == VideoEvent::VESTART) logI << "Video start" << std::endl;
				else if (m_videoEvent == VideoEvent::VESTOP) logI << "Video stop and saved" << std::endl;
				else if (m_videoEvent == VideoEvent::VEFAILED) logI << "Video failed" << std::endl;

				m_videoEvError = (VideoEvError)Utils::readUInt32(data, 8);
				if (m_videoEvError == VideoEvError::VVRNO_ERROR) logI << "No Error" << std::endl;
				else if (m_videoEvError == VideoEvError::VVUNKNOWN) logI << "Unknown generic error" << std::endl;
				else if (m_videoEvError == VideoEvError::VVBUSY) logI << "Video recording is busy" << std::endl;
				else if (m_videoEvError == VideoEvError::VVNOT_AVAILABLE) logI << "Video recording not available" << std::endl;
				else if (m_videoEvError == VideoEvError::VVMEMORY_FULL) logI << "Memory full" << std::endl;
				else if (m_videoEvError == VideoEvError::VVLOW_BATTERY) logI << "Battery is too low to record" << std::endl;
				else if (m_videoEvError == VideoEvError::VVAUTOSTOPPED) logI << "Video was auto stopped" << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_NETWORKSETTINGS){
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_NETWORKSETTINGS_CMD_WIFISELECTION) {
				logI << "------------------------- TYPE OF WIFI SELECTION ---------------------------" << std::endl;
				m_wifiSelec = (WifiSelection)Utils::readUInt32(data, 4);
				if (m_wifiSelec == WifiSelection::WSAUTO) logI << "Auto selection" << std::endl;
				else if (m_wifiSelec == WifiSelection::WSMANUAL) logI << "Manual selection" << std::endl;

				m_wifiType = (WifiType)Utils::readUInt32(data, 8);
				if (m_wifiType == WifiType::WTBAND_24) logI << "Band : 2.4 GHz band" << std::endl;
				else if (m_wifiType == WifiType::WTBAND_5) logI << "Band : 5 GHz band" << std::endl;
				else if (m_wifiType == WifiType::WTBOTH) logI << "Band : Both 2.4 and 5 GHz bands" << std::endl;

				logI << "Channel : " << (int)data[12] << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_NETWORKSETTINGSSTATE){
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_NETWORKSETTINGSSTATE_CMD_WIFISELECTIONCHANGED) {
				logI << "------------------------- WIFI SELECTION CHANGED ---------------------------" << std::endl;
				int value = Utils::readUInt32(data, 4);
				if (value == 0) { logI << "Auto selection" << std::endl; m_wifiSelec = WifiSelection::WSAUTO; }
				else if (value == 1) { logI << "Auto selection 2.4ghz" << std::endl; m_wifiSelec = WifiSelection::WSAUTO; }
				else if (value == 2) { logI << "Auto selection 5 ghz" << std::endl; m_wifiSelec = WifiSelection::WSAUTO; }
				else if (value == 3) { logI << "Manual selection" << std::endl; m_wifiSelec = WifiSelection::WSMANUAL; }

				m_wifiType = (WifiType)Utils::readUInt32(data, 8);
				if (m_wifiType == WifiType::WTBAND_24) logI << "Band : 2.4 GHz band" << std::endl;
				else if (m_wifiType == WifiType::WTBAND_5) logI << "Band : 5 GHz band" << std::endl;
				else if (m_wifiType == WifiType::WTBOTH) logI << "Band : Both 2.4 and 5 GHz bands" << std::endl;

				logI << "Channel : " << (int)data[12] << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_NETWORKSTATE){
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_NETWORKSTATE_CMD_WIFISCANLISTCHANGED) {
				logI << "------------------------- WIFI SCAN LIST CHANGED ---------------------------" << std::endl;
				int index = 4;
				std::string str = Utils::readString((char*)data, index);
				logI << "SSID of the AP : " << str << std::endl;
				index += str.length();
				UINT16 rssi = Utils::readUInt16(data, index);
				logI << "RSSI of the AP in dbm : " << rssi << std::endl;
				index += 2;
				int value = Utils::readUInt32(data, index);
				if (value == 0) logI << "Band : 2.4 GHz band" << std::endl;
				else if (value == 1) logI << "Band : 5 GHz band" << std::endl;
				index += 4;

				logI << "Channel : " << (int)data[index] << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_NETWORKSTATE_CMD_WIFIAUTHCHANNELLISTCHANGED) {
				logI << "------------------------- WIFI AUTH CHANNEL LIST CHANGED ---------------------------" << std::endl;
				int value = Utils::readUInt32(data, 4);
				if (value == 0) logI << "Band : 2.4 GHz band" << std::endl;
				else if (value == 1) logI << "Band : 5 GHz band" << std::endl;

				logI << "The authorized channel : " << (int)data[8] << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_NETWORKSTATE_CMD_LINKQUALITYCHANGED) {
				logI << "------------------------- LINK QUALITY CHANGED ---------------------------" << std::endl;
				logI << "Quality : " << (m_wifiQuality = (int)data[4]) << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_AUDIOSETTINGSSTATE) {
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_AUDIOSETTINGSSTATE_CMD_MASTERVOLUMECHANGED) {
				logI << "------------------------- AUDIO VOLUME CHANGED ---------------------------" << std::endl;
				logI << "Volume : " << (m_volume = (int)data[4]) << std::endl;
			}
			else if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_AUDIOSETTINGSSTATE_CMD_THEMECHANGED) {
				logI << "------------------------- AUDIO THEME CHANGED ---------------------------" << std::endl;
				m_audioTheme = (AudioTheme)Utils::readUInt32(data, 4);
				if (m_audioTheme == AudioTheme::AUDEFAULT) logI << "Theme : Default audio theme" << std::endl;
				else if (m_audioTheme == AudioTheme::AUROBOT) logI << "Theme : Robot audio theme" << std::endl;
				else if (m_audioTheme == AudioTheme::AUINSECT) logI << "Theme : Insect audio theme" << std::endl;
				else if (m_audioTheme == AudioTheme::AUMONSTER) logI << "Theme : Monster audio theme" << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_MEDIASTREAMINGSTATE) {
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_MEDIASTREAMINGSTATE_CMD_VIDEOENABLECHANGED) {
				logI << "------------------------- VIDEO STATE CHANGED ---------------------------" << std::endl;
				m_streamState = (StreamState)Utils::readUInt32(data, 4);
				if (m_streamState == StreamState::SSENABLED) logI << "Video streaming is enabled" << std::endl;
				else if (m_streamState == StreamState::SSDISABLED) logI << "Video streaming is disabled" << std::endl;
				else if (m_streamState == StreamState::SSFAILED) logI << "Video streaming failed to start" << std::endl;
			}
		}
		else if (_class == ARCOMMANDS_ID_JUMPINGSUMO_CLASS_VIDEOSETTINGSSTATE) {
			if (_cmd == ARCOMMANDS_ID_JUMPINGSUMO_VIDEOSETTINGSSTATE_CMD_AUTORECORDCHANGED) {
				logI << "------------------------- VIDEO AUTO-RECORD CHANGED ---------------------------" << std::endl;
				m_videoAuto = (VideoAuto)data[4];
				if (m_videoAuto == VideoAuto::VADISABLED)
					logI << "video automatic recording status : DISABLED" << std::endl;
				else
					logI << "video automatic recording status : ENABLED" << std::endl;
			}
		}
	}
}
