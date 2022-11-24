Flappy Bird Clone

This project was created for fun and to try and recreate the popular game Flappy Bird. 
In Config.h you will see some configuration variables you can mess with to test different things. One variable regenerates the binary game data, and another chooses to load the binary game data or the regular game data.

Some features:
. Frame Rate independence (fixed time step with interpolation rendering)
. Screen Size independence (game scales with screen)
. OpenGL rendering with multiple version support 3.0+
. Binary game data export/import
. Protected save file (hashing)
. Error log output
. Configuration file support
. Audio

-Dependencies-
GLM: GLM math library is used. Its already included in the Src/ThirdParty directory
STBI: this image loading library is used. Its already included in the Src/ThirdParty directory
GLEW (version 2.1.0): is used to load OpenGL functions. Is available on cross-platform
GLFW (version 3.3): is used for window handling, and event handling. Is available on cross-platform
irrKlang (version 1.6.0): Audio library. Is available on cross-platform
*also need irrKlang.dll in running directory

This game runs on x64 bit. All dependencies support x64.

Currently only runs on Windows platform. If you build libraries for other operating systems should work. The internal code has no OS dependencies.

OpenGL Versions supported are 3.0 and up. 


<img width="314" alt="git_1" src="https://user-images.githubusercontent.com/33635742/203861828-ff6b0892-b55a-4c2a-9f4f-f14ea26f4154.png">
