## VolanteDesktop

This repository is the software in C++ that connects SimHub and VJoy to some Boards likes ESP, Arduino or STM with different protocols (WiFi - Bluetooth - Serial). 

The GUI is developed using Dear ImGui as library.

To connect to SimHub as a serial device you can use com0com to emulate a serial interface. This software emulates an arduino device with custom protocol in SimHub permitting to retrieve the data. After retreiving it, the data is converted into a json object.

To compile the software use: 
```bash
g++.exe -std=c++14 -g -o .\bin\main.exe -static .\src\vJoyClient.cpp .\inc\GUI\backends\imgui_impl_glfw.cpp .\inc\GUI\backends\imgui_impl_opengl3.cpp  .\inc\GUI\imgui*.cpp -I .\inc\GUI\ -I .\inc\GUI\backends\ -L.\lib\amd64\ -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lvJoyInterface -fpermissive -Wno-unused-variable -Wno-unused-parameter -mwindows;
```