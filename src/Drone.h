/*!
* \file Drone.h
* \brief Drone main class. It is think like a controller class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "Command.h"
#include "Network.h"
#include "DroneData.h"
#include "Stream.h"
#include "ImagePro.h"
#include "EventHandler.h"

#include <SDL/SDL.h>

#define FPS 60
#define SPEED 20

enum MyEvent {
	LONG_JUMP,
	HIGH_JUMP,
	CHANGE_POSTURE,
	CHANGE_AUDIOTHEME,
	CHANGE_ANIM,
	START_ANIM, 
	STOP_ANIM,
	VOL_UP,
	VOL_DOWN,
	GO_UP,
	GO_DOWN,
	GO_LEFT,
	GO_RIGHT,
	START_RECORD,
	STOP_RECORD,
	START_VSTREAMING,
	STOP_VSTREAMING,
	TAKE_PICTURE,
	QUIT,
};

class Drone
{
public:
	int begin();
	int end();
	void start();

private:
	void run();
	void initEvents();
	void onEvent();
	void updateImage(cv::Mat& mat);

	bool m_running;
	unsigned int m_anim = 1;
	unsigned int m_atheme = 0;
	unsigned int m_posture = 0;
	unsigned int m_prevTicks;

	SDL_Window* m_window = nullptr;
	SDL_Texture* m_texture = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	Command m_cmd;
	Network m_network;
	DroneData m_data;
	Stream m_vstream;
	ImagePro m_imgPro;
	NS2::EventHandler m_evHandler;
};

