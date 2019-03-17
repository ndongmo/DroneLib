#include "Drone.h"
#include "Logger.h"

int Drone::begin()
{
	int err;
	
	err = m_cmd.begin(&m_data);
	if (err < 0)return err;
	err = m_vstream.begin(&m_cmd);
	if (err < 0)return err;
	err = m_network.begin(&m_cmd, &m_data, &m_vstream);
	if (err < 0)return err;
	err = m_imgPro.begin();
	if (err < 0)return err;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logE << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		return -1;
	}
	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &m_window, &m_renderer) < 0)
	{
		logE << "Error creating window or renderer : " << SDL_GetError() << std::endl;
		return -1;
	}
	m_texture = SDL_CreateTexture(
		m_renderer, 
		SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	m_evHandler.init(0, 0);
	m_evHandler.setPlayWith(NS2::PlayWith::GAMEPAD);
	initEvents();

	return err;
}

int Drone::end()
{
	int err;
	err = m_vstream.end();
	err = m_network.end();
	err = m_cmd.end();
	err = m_imgPro.end();

	m_evHandler.destroy();
	SDL_DestroyTexture(m_texture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	return err;
}

void Drone::start()
{
	m_network.start();
	m_cmd.start();
	m_vstream.start();
	m_imgPro.start();

	run();
}

void Drone::run()
{
	m_running = true;

	while (m_running)
	{
		m_prevTicks = SDL_GetTicks();

		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (m_evHandler.getPlayWith() == NS2::PlayWith::KEYBOARD)
			{
				switch (ev.type) {
				case SDL_QUIT:
					m_running = false;
					break;
				case SDL_MOUSEMOTION:
					m_evHandler.setMouseCoords((float)ev.motion.x, (float)ev.motion.y);
					break;
				case SDL_KEYDOWN:
					m_evHandler.pressKey(ev.key.keysym.sym);
					break;
				case SDL_KEYUP:
					m_evHandler.releaseKey(ev.key.keysym.sym);
					break;
				case SDL_MOUSEBUTTONDOWN:
					m_evHandler.pressKey(ev.button.button);
					break;
				case SDL_MOUSEBUTTONUP:
					m_evHandler.releaseKey(ev.button.button);
					break;
				}
			}
			else
			{
				switch (ev.type) {
				case SDL_QUIT:
					m_running = false;
					break;
				case SDL_JOYBUTTONDOWN:
					m_evHandler.pressKey(ev.jbutton.button + MARGIN);
					break;
				case SDL_JOYBUTTONUP:
					m_evHandler.releaseKey(ev.jbutton.button + MARGIN);
					break;
				case SDL_JOYAXISMOTION:
					m_evHandler.updateJoystickAxis(ev.jaxis.axis, ev.jaxis.value);
					break;
				case SDL_JOYHATMOTION:
					m_evHandler.updateJoystickHats(ev.jhat.value + MARGIN);
					break;
				}
			}
		}

		onEvent();
		updateImage(m_vstream.getImage());

		float frameTicks = (float)(SDL_GetTicks() - m_prevTicks);

		if (1000.0f / FPS > frameTicks) {
			SDL_Delay((unsigned int)(1000.0f / FPS - frameTicks));
		}
	}
	end();
}

void Drone::onEvent()
{
	if (m_evHandler.isEventDown(MyEvent::QUIT)) {
		m_running = false;
		m_evHandler.releaseEvent(MyEvent::QUIT);
	}
	else if (m_evHandler.isEventDown(MyEvent::LONG_JUMP)) {
		m_cmd.jump(JumpType::JLONG);
		m_evHandler.releaseEvent(MyEvent::LONG_JUMP);
	}
	else if (m_evHandler.isEventDown(MyEvent::HIGH_JUMP)) {
		m_cmd.jump(JumpType::JHIGH);
		m_evHandler.releaseEvent(MyEvent::HIGH_JUMP);
	}
	else if (m_evHandler.isEventDown(MyEvent::CHANGE_POSTURE)) {
		if ((int)m_data.m_posture > 3 || m_posture > 3)m_posture = -1;
		m_posture++;
		m_cmd.changePosture((Posture)m_posture);
		m_evHandler.releaseEvent(MyEvent::CHANGE_POSTURE);
	}
	else if (m_evHandler.isEventDown(MyEvent::CHANGE_ANIM)) {
		if ((int)m_anim > 9)m_anim = 0; // avoid Stop(0)
		m_anim++;
		m_evHandler.releaseEvent(MyEvent::CHANGE_ANIM);
	}
	else if(m_evHandler.isEventDown(MyEvent::START_ANIM)) {
		m_cmd.startAnim((Animation)m_anim);
		m_evHandler.releaseEvent(MyEvent::START_ANIM);
	}
	else if (m_evHandler.isEventDown(MyEvent::STOP_ANIM)) {
		m_cmd.startAnim((Animation)(m_anim = 0));
		m_evHandler.releaseEvent(MyEvent::STOP_ANIM);
	}
	else if (m_evHandler.isEventDown(MyEvent::START_RECORD)) {
		m_cmd.recordVideo(true);
		m_evHandler.releaseEvent(MyEvent::START_RECORD);
	}
	else if (m_evHandler.isEventDown(MyEvent::STOP_RECORD)) {
		m_cmd.recordVideo(false);
		m_evHandler.releaseEvent(MyEvent::STOP_RECORD);
	}
	else if (m_evHandler.isEventDown(MyEvent::START_VSTREAMING)) {
		m_cmd.enableVideoStream(true);
		m_evHandler.releaseEvent(MyEvent::START_VSTREAMING);
	}
	else if (m_evHandler.isEventDown(MyEvent::STOP_VSTREAMING)) {
		m_cmd.enableVideoStream(false);
		m_evHandler.releaseEvent(MyEvent::STOP_VSTREAMING);
	}
	else if (m_evHandler.isEventDown(MyEvent::TAKE_PICTURE)) {
		m_cmd.takePicture();
		m_evHandler.releaseEvent(MyEvent::TAKE_PICTURE);
	}
	else if (m_evHandler.isEventDown(MyEvent::CHANGE_AUDIOTHEME)) {
		if ((int)m_data.m_audioTheme > 3 || m_atheme > 3)m_atheme = -1;
		m_atheme++;
		m_cmd.setAudioTheme((AudioTheme)m_atheme);
		m_evHandler.releaseEvent(MyEvent::CHANGE_AUDIOTHEME);
	}
	else if (m_evHandler.isEventDown(MyEvent::VOL_UP)) {
		m_cmd.incrVolume(5);
		m_evHandler.releaseEvent(MyEvent::VOL_UP);
	}
	else if (m_evHandler.isEventDown(MyEvent::VOL_DOWN)) {
		m_cmd.incrVolume(-5);
		m_evHandler.releaseEvent(MyEvent::VOL_DOWN);
	}

	int x = 0, y = 0;

	if (m_evHandler.isEventDown(MyEvent::GO_UP)) {
		x = SPEED;
	}
	else if (m_evHandler.isEventDown(MyEvent::GO_DOWN)) {
		x = -SPEED;
	}
	if (m_evHandler.isEventDown(MyEvent::GO_LEFT)) {
		y = SPEED;
	}
	else if (m_evHandler.isEventDown(MyEvent::GO_RIGHT)) {
		y = -SPEED;
	}

	m_cmd.move(x, y); // always send command event if x=0 and y=0
}

void Drone::initEvents()
{
	m_evHandler.addEvent(MyEvent::QUIT, SDLK_ESCAPE, NS2::Joystick::START);
	m_evHandler.addEvent(MyEvent::STOP_RECORD, SDLK_y, NS2::Joystick::Y);
	m_evHandler.addEvent(MyEvent::START_RECORD, SDLK_x, NS2::Joystick::X);
	m_evHandler.addEvent(MyEvent::VOL_UP, SDLK_UP, NS2::Joystick::R3);
	m_evHandler.addEvent(MyEvent::VOL_DOWN, SDLK_DOWN, NS2::Joystick::L3);
	m_evHandler.addEvent(MyEvent::STOP_ANIM, SDLK_c, NS2::Joystick::RT);
	m_evHandler.addEvent(MyEvent::CHANGE_AUDIOTHEME, SDLK_v, NS2::Joystick::LT);
	m_evHandler.addEvent(MyEvent::TAKE_PICTURE, SDLK_SPACE, NS2::Joystick::LB);
	m_evHandler.addEvent(MyEvent::CHANGE_POSTURE, SDLK_b, NS2::Joystick::RB);
	m_evHandler.addEvent(MyEvent::LONG_JUMP, SDLK_n, NS2::Joystick::DPAD_LEFT);
	m_evHandler.addEvent(MyEvent::HIGH_JUMP, SDLK_m, NS2::Joystick::DPAD_RIGHT);
	m_evHandler.addEvent(MyEvent::START_ANIM, SDLK_LEFT, NS2::Joystick::DPAD_DOWN);
	m_evHandler.addEvent(MyEvent::CHANGE_ANIM, SDLK_RIGHT, NS2::Joystick::DPAD_UP);
	m_evHandler.addEvent(MyEvent::GO_UP, SDLK_w, NS2::Joystick::AXE1_UP);
	m_evHandler.addEvent(MyEvent::GO_DOWN, SDLK_s, NS2::Joystick::AXE1_DOWN);
	m_evHandler.addEvent(MyEvent::GO_LEFT, SDLK_a, NS2::Joystick::AXE1_LEFT);
	m_evHandler.addEvent(MyEvent::GO_RIGHT, SDLK_d, NS2::Joystick::AXE1_RIGHT);
}

void Drone::updateImage(cv::Mat& mat)
{
	if (mat.empty()) return;

	m_imgPro.process(mat); // image processing

	IplImage * img = &(IplImage)(mat);

	unsigned char * texture_data = NULL;
	int texture_pitch = 0;

	SDL_LockTexture(m_texture, 0, (void **)&texture_data, &texture_pitch);
	memcpy(texture_data, (void *)img->imageData, img->width * img->height * img->nChannels);
	SDL_UnlockTexture(m_texture);

	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
	SDL_RenderPresent(m_renderer);
}
