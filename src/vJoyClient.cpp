#include "../inc/config.h"
#include "../inc/SG_VolBoard.h"
#include "../inc/SimHubFeeder.h"
#include "../inc/vJoyFeeder.h"
#include "../inc/sg_gui.h"

/* COMPILATION COMMAND:
g++.exe -g -o .\bin\main.exe -static .\src\vJoyClient.cpp .\inc\GUI\backends\imgui_impl_glfw.cpp .\inc\GUI\backends\imgui_impl_opengl3.cpp  .\inc\GUI\imgui*.cpp -I .\inc\GUI\ -I .\inc\GUI\backends\ -L.\lib\amd64\ -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lvJoyInterface -fpermissive -Wno-unused-variable -Wno-unused-parameter -mwindows
*/

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	//TODO: LOAD CONFIG
	VJoyFeeder vJoyFeeder = VJoyFeeder::getInstance();
	SG_VolBoard* volBoard = new SG_VolBoardBLE(string("\\\\.\\COM8"), string("\\\\.\\COM9"), 115200);
	
	SimHubFeeder sh_feeder = SimHubFeeder::getInstance();
	sh_feeder.vol_board = volBoard;

	SG_GUI gui = SG_GUI::getInstance();
	gui.vol_board = volBoard;
	gui.loop();
	
	return 0;
}
