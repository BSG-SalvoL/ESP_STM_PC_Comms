#include "../inc/vJoyFeeder.h"
#include "../inc/SimHubFeeder.h"
#include "../inc/sg_gui.h"
#include "../inc/config.h"
#include "../inc/buttons.h"
#include "../inc/mediatorStruct.h"
/* COMPILATION COMMAND:
g++.exe -g -o .\bin\main.exe -static .\src\vJoyClient.cpp .\inc\GUI\backends\imgui_impl_glfw.cpp .\inc\GUI\backends\imgui_impl_opengl3.cpp  .\inc\GUI\imgui*.cpp -I .\inc\GUI\ -I .\inc\GUI\backends\ -L.\lib\amd64\ -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lvJoyInterface -fpermissive -Wno-unused-variable -Wno-unused-parameter -mwindows
*/

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	//VJoyFeeder feeder = VJoyFeeder::get_feeder();
	//SimHubFeeder SH_feeder = SimHubFeeder::get_feeder();
	//SH_feeder.vjoyfeeder = &feeder;
	//SH_feeder.Connect();

//TEST VARIABLES
srand(time(NULL));
//END
	SG_GUI* gui = &SG_GUI::get_gui(&mediator.buttons, &mediator.btn_state1, &mediator.btn_state2);
	mediator.gui = (void*)gui;
	gui->loop();
	
	return 0;
}
