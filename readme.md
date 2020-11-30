# VoIP Project
  
Voice over IP project to better understand C++ and learn the fundementals of voice over ip applications.  
  
The application captures, encodes and sends audio data as RTP packets and is able to receive, decode and play incoming RTP packets coming from another instance of this application.  
  
Implemented in this project:
- A simple socket implementation using "Winsock" Windows sockets
- Interaction with the soundcard through RtAudio
- Encoding and decoding the raw audio data through the Opus library
- Creating and sending RTP packets with the encoded audio data
  
Example:
Send RTP packets to 127.0.0.1. Use audio device 3 as input and default audio output device as output. Use dual channel audio for input and output:
```
voip_project.exe 127.0.0.1 -i 3 -o -1 --inCh 2 --outCh 2
```
To set or test custom paramters start the application with " -h" to see available arguments.  
Call the application with "-l" to see your connected audio devices and their capabilities.  
  
I only implemented windows socket connection, so the applicaiton won't run on Linux at the moment.  
The application only works with Opus compatible frame sizes and sample rates (are set by default)  
This is by no means a complete voice over ip client and just meant as a learning experience.  