# DroneLib
DroneLib is a C++ (OpenCV + FFMPEG + SDL) Jumping sumo drone library for desktop. This library doesn't use the ARDrone SDK and only support command for AR.DRONE 2.0. It can be adapted theoretically with other kind of drone (I tested only on Jumping sumo) by adding specific commands. The source provided here has been built in visual studio 2013 (X86). This library provides a lightweight implementation of common protocols of ARDrone 2.0, so it can be a good start for a drone-based project!
Please, refer to this video on my [youtube channel](https://youtu.be/GuC5tC1gA5Y).

## Features

- Keyboard/Gamepad (interchangeable) for drone control
- Video streaming (640x480) decoded with ffmpeg decoder
- Bodies (Upper body) detection using Opencv CascadeClassifier
- Audio streaming (freq: 16000, format: AUDIO_S16SYS, channels: 1, samples: 1024); I used SDL_AudioSpec for decoding raw audio data
- Almost all Jumping sumo commands are implemented.

 ## Architecture
  
  Here is the class diagram describing the library architecture.
  
  <img src="https://github.com/ndongmo/DroneLib/blob/master/class_diag.jpg" align="left" width="100%" height="400" alt="Library architecture">
  
  
  ## How to use
  
  First, you can try the executable in the setup directory. the DLLs inside are those of X86 system. The default parameters are :
  - drone address : 192.168.2.1
  - discovery port : 44444
  - receiving port : 43210
  - sending port : 54321
  
  In case of error, you can consult the generated log file.
  The source code has been compiled as I said earlier with Visual studio 2013 with window socket, ffmpeg, OpenCV and SDL libraries. So,   you should link these libraries before compiling. For any further questions, please feel free to ask!
  
  <table style="width:100%">
  <tr>
    <td>
      <img src="https://github.com/ndongmo/DroneLib/blob/master/IMG_1.jpg" width="10%" height="300" />
    </td>
    <td>
      <img src="https://github.com/ndongmo/DroneLib/blob/master/20.png" width="30%" height="300" />
    </td>
    <td>
     <img src="https://github.com/ndongmo/DroneLib/blob/master/48.png" width="30%" height="300" />
    </td>
    <td>
      <img src="https://github.com/ndongmo/DroneLib/blob/master/837.png" width="30%" height="300" />
    </td>
  </tr>
 <tr>
  <td> 
   <img src="https://github.com/ndongmo/DroneLib/blob/master/IMG_2.jpg" width="10%" height="300" />
  </td>
  <td>
      <img src="https://github.com/ndongmo/DroneLib/blob/master/956.png" width="30%" height="300" />
    </td>
  <td>
      <img src="https://github.com/ndongmo/DroneLib/blob/master/1065.png" width="30%" height="300" />
    </td>
  <td>
      <img src="https://github.com/ndongmo/DroneLib/blob/master/1166.png" width="30%" height="300" />
    </td>
 </tr>
 </table>
  
  ## Authors

* F. Ndongmo Silatsa

## Licence

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/ndongmo/DroneLib/blob/master/LICENSE.md) file for details

## Acknowledgments

* [Dranger](http://dranger.com/ffmpeg/tutorial03.html) ffmpeg and SDL Tutorial.
* [puku0x](https://github.com/puku0x/cvdrone) for its implementation of CV Drone.
* Internet ...

 

