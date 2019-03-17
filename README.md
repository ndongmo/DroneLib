# DroneLib
DroneLib is a C++ (OpenCV + FFMPEG + SDL) Jumping sumo drone library for deskstop. This library doesn't use the ARDrone SDK and only support command for AR.DRONE 2.0. It can be adapt theoretically to other kind of drone (I tested only on Jumping sumo) by adding specific commands. The src provided here has been built on visual studio 12 (X86). This library provides a lightweight implementation of common protocols of ARDrone 2.0, so it can be a good start for a drone-based project!
Please, refer to this video on my [youtube channel](https://youtu.be/GuC5tC1gA5Y).

## Features

- Keyboard/Gamepad (interchangeable) for drone control
- Video streaming (640x480) decoded with ffmpeg decoder
- Bodies (Upper body) detection using Opencv CascadeClassifier
- Audio streaming (freq: 16000, format: AUDIO_S16SYS, channels: 1, samples: 1024); I used SDL_AudioSpec for decoding raw audio data
- Almost all Jumping sumo command are implemented.

 ## Architecture
  
  Here is the class diagram describing the library architecture.
  
  <img src="https://github.com/ndongmo/DroneLib/blob/master/class_diag.jpg" align="left" width="300" height="400" alt="Library architecture">
  
  ## How to use
  
  
