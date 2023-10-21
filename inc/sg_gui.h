#pragma once
#ifndef __SG_GUI__
#define __SG_GUI__


#pragma pack(1)

//SPECIFIC FOR GUI IMPORTS
#include "./GUI/imgui.h"
#include "./GUI/backends/imgui_impl_glfw.h"
#include "./GUI/backends/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "./glfw/include/GLFW/glfw3.h" // Will drag system OpenGL headers

//STANDARD IMPORTS
#include <iostream>
#include <string>
//CUSTOM IMPORTS
#include "./winLibs/SerialPort.hpp"
#include "config.h"
#include "buttons.h"
#include "utils.h"
#include "mediatorStruct.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class GFX_Adapter {
  private: 
    int width, height;
    int xStart, yStart;
    ImColor lastColor, lastbgcolor;
  public:
    GFX_Adapter(int width, int height, int xStart, int yStart) {
        this->width = width;
        this->height = height;
        this->xStart = xStart;
        this->yStart = yStart;
    }
    void cust_drawLCD() {
        fillRect(0, 50, this->width, this->height, 0x0000);
        drawRect(0, 50, this->width, this->height, 0xffff);
    }
    void drawPixel(uint16_t x, uint16_t y, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddCircleFilled(ImVec2(x+xStart, y+yStart), 1, (ImU32)rgb565_to_um32(color));
    } 
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddLine(ImVec2(x0+xStart, y0+yStart), ImVec2(x1+xStart, y1+yStart), ImColor(rgb565_to_um32(color)));
    }
    void drawFastVLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddLine(ImVec2(x0+xStart, y0+yStart), ImVec2(x0+xStart, yStart+y0+length), ImColor(rgb565_to_um32(color)));
    }
    void drawFastHLine(uint8_t x0, uint8_t y0, uint8_t length, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddLine(ImVec2(x0+xStart, y0+yStart), ImVec2(xStart+x0+length, yStart+y0), ImColor(rgb565_to_um32(color)));
    }
    void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRect(ImVec2(x0+xStart, y0+yStart), ImVec2(xStart+x0+w, yStart+y0+h), ImColor(rgb565_to_um32(color)));
    }
    void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(x0+xStart, y0+yStart), ImVec2(xStart+x0+w, yStart+y0+h), ImColor(rgb565_to_um32(color)));
    }
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddCircle(ImVec2(xStart+x0, yStart+y0), r, color, 0);
            
    }
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddCircleFilled(ImVec2(xStart+x0, yStart+y0), r, color, 0);
    }
    void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x0+w, yStart+y0+h), ImColor(rgb565_to_um32(color)), radius);
    }
    void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x0+w, yStart+y0+h), ImColor(rgb565_to_um32(color)), radius);
    }
    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddTriangle(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x1, yStart+y1), ImVec2(xStart+x2, yStart+y2), ImColor(rgb565_to_um32(color)));
    }
    void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddTriangleFilled(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x1, yStart+y1), ImVec2(xStart+x2, yStart+y2), ImColor(rgb565_to_um32(color)));
    }
    void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
        auto draw = ImGui::GetWindowDrawList();
        setCursor(xStart+x, yStart+y);
        setTextColor(color, bg);
        setTextSize(size);
        print("%c", c);
    }
    void setCursor(int16_t x0, int16_t y0) {
        ImGui::SetCursorPos(ImVec2(x0+xStart, y0+yStart));
    }
    void setTextColor(uint16_t color) {
        lastColor = ImColor(rgb565_to_um32(color));
    }
    void setTextColor(uint16_t color, uint16_t backgroundcolor) {
        lastColor = ImColor(rgb565_to_um32(color));
        lastbgcolor = ImColor(rgb565_to_um32(backgroundcolor));
    }
    void setTextSize(uint8_t size) {
        auto draw = ImGui::GetWindowDrawList();
        ImGui::SetWindowFontScale(1+(0.2*size));
    }
    void setTextWrap(boolean w) {
    }
    void fillScreen(uint16_t color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(xStart, yStart), ImVec2(xStart+width, yStart+height), ImColor(rgb565_to_um32(color)));
    }
    template<typename... Args>
    void print(const char* fmt, Args&&... args) {
        ImGui::Text(fmt, args...);
    }
};


class SG_GUI {
    private:
        GLFWwindow* window;
        const char* glsl_version = "#version 130";
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        GFX_Adapter *tft;

        bool demo_flag = false;
        int win_x = 1400, win_y = 700;
        int rw_enc_size_x  = 245, rw_enc_size_y = 120;
        int com_size_x = 290, com_size_y = rw_enc_size_y;
        int dbgtmp = 0;

        vector<Button*>* buttons;
        uint64_t *btn_state1, *btn_state2;
        int pov_idx[4]; 
        ImVec4 col_disabled = ImColor(0.4f, 0.4f, 0.4f, 0.5f);
        ImVec4 col_background = ImColor(0.2f, 0.2f, 0.2f, 1.f);
        ImVec4 col_black = ImColor(0.f, 0.f, 0.f, 1.f);
        ImVec4 col_white = ImColor(1.f, 1.f, 1.f, 1.f);
        ImVec4 col_red = ImColor(1.f, 0.f, 0.f, 1.f);
        ImVec4 col_blue = ImColor(0.f, 0.f, 1.f, 1.f);
        ImVec4 col_yellow = ImColor(1.f, 1.f, 0.f, 1.f);
        ImVec4 col_green = ImColor(0.f, 1.f, 0.f, 1.f); 

        SG_GUI(vector<Button*>* btn, uint64_t* state1, uint64_t* state2) {
            this->buttons = btn; 
            this->btn_state1 = state1;
            this->btn_state2 = state2;
            this->tft = new GFX_Adapter(480, 320, 500, 180);
            pov_idx[0] = 1;
            pov_idx[1] = 2;
            pov_idx[2] = 3;
            pov_idx[3] = 4;
            SG_GUI::inst = NULL;
            init_grafics();
        }
    public:
        static SG_GUI* inst;
        static SG_GUI& get_gui(vector<Button*>* btn, uint64_t* state1, uint64_t* state2)
        {
            if(SG_GUI::inst == NULL)
                SG_GUI::inst = new SG_GUI(btn, state1, state2);
            //static SG_GUI instance;
            return *inst;
        }
        
        bool init_grafics() {
            glfwSetErrorCallback(glfw_error_callback);
            if (!glfwInit())
                return false;
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            window = glfwCreateWindow(win_x, win_y, "TheHawkSG - Volante", NULL, NULL);
            if (window == NULL)
                return false;
            glfwMakeContextCurrent(window);
            glfwSwapInterval(1); // Enable vsync
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
            return true;
        }

        void loop() {
            while (!glfwWindowShouldClose(window)) {
                prepare();
                draw();
                render();
            }
            destroyGUI();
        }
    private:
        void prepare() {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        void draw_STM32COM(int pos_x, int pos_y) {
            vector<char*> v = SerialPort::getComPorts();
            static int item_current = 0;
            char** items = &v[0];
            ImGui::Begin("Select COM Port For STM32"); 
            ImGui::SetWindowSize(ImVec2(1400/2, 650/2), true);                          
            ImGui::SetWindowPos(ImVec2(pos_x, pos_y), true);
            ImGui::Combo("##", &item_current, items, v.size());
            ImGui::SameLine();
            if (ImGui::Button("Reload")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
            
            }
            ImGui::Text("Selected: %s", v[item_current]);
            ImGui::SameLine();
            if(true) // CHECK CONNECTION WITH COM
                ImGui::TextColored(col_green, "Status OK");
            else
                ImGui::TextColored(col_red, "Status ERROR");
            ImGui::End();
        }

        void draw_SimHubCOM(int pos_x, int pos_y) {
            vector<char*> v = SerialPort::getComPorts();
            static int item_current = 0;
            static int item_running = 0;
            static int current_log_item = 0;
            static bool first = true;
            SimHubFeeder* feeder = &mediator.sh_feeder;
            char** items = &v[0];
            if(first) {
                feeder->Connect();
                first = false;
            }
            ImGui::Begin("Select COM Port For SIMHub Feeder");  
            ImGui::SetWindowSize(ImVec2(1400/2, 650/2), true);                       
            ImGui::SetWindowPos(ImVec2(pos_x, pos_y), true);
            ImGui::Combo("##", &item_current, items, v.size());
            ImGui::SameLine();
            if (ImGui::Button("Reload")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
                SIMHUB_Conf.SH_COM_PORT = string(v[item_current]);
                feeder->reloadSH();
                item_running = item_current;
            }
            ImGui::Text("Running on: %s;", v[item_running]);
            ImGui::SameLine();
            if(feeder->Is_Serial_Connected()) // CHECK CONNECTION WITH COM
                ImGui::TextColored(col_green, "COM PORT OPENED SUCCESSFULLY");
            else
                ImGui::TextColored(col_red, "COM ERROR");
            ImGui::Text("SimHub Status: ");
            ImGui::SameLine();
            if(feeder->Is_SH_Connected()) // CHECK CONNECTION WITH SimHub
                ImGui::TextColored(col_green, "OK");
            else
                ImGui::TextColored(col_red, "ERROR OR NOT CONNECTED");
            current_log_item = feeder->get_log().size() - 1;
            ImGui::ListBox("LOG", &current_log_item, &feeder->get_log()[0], feeder->get_log().size(), 10);
            ImGui::End();
        }

        void draw_VjoyStatus(int pos_x, int pos_y) { 
            static bool first = true;
            static VJoyFeeder* feeder = &mediator.vJoyFeeder;
            static vector<const char*> v = feeder->getVJoyDevices();
            vector<const char*>* log = feeder->get_data().log;
            static int current_idx = 0;
            static int current_log_item = 0;
            static std::string test = "TEST";
            if(first) {
                v = feeder->getVJoyDevices();
                feeder->add_to_log(string_format("ID: %d", VJOY_Conf.VJOY_DEVICE_ID));
                first = false;
            }
            if(v[0] == "No Devices Found")
                v = feeder->getVJoyDevices();
            const char** items = &v[0];
            ImGui::Begin("Vjoy Status"); 
            ImGui::SetWindowSize(ImVec2(1400/2, 650/2), true);                          
            ImGui::SetWindowPos(ImVec2(pos_x, pos_y), true);
            const char* combo_preview_value = items[0];  // Pass in the preview value visible before opening the combo (it could be anything)
            if (ImGui::BeginCombo("##", combo_preview_value, 0)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                    const bool is_selected = (current_idx == n);
                    if (ImGui::Selectable(items[n], is_selected)) {
                        current_idx = n;
                        VJOY_Conf.VJOY_DEVICE_ID = current_idx + 1;
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reload")) {
                feeder->add_to_log(string_format("IDX: %d", current_idx));
                v = feeder->getVJoyDevices();
                feeder->reload();
            }
            ImGui::Text("Selected: "); ImGui::SameLine();
            ImGui::Text("%s", v[current_idx]);
            ImGui::SameLine();
            ImGui::Text("Counter: %d", feeder->get_data().counter);
            ImGui::SameLine();
            if(VJoyFeeder::is_acquired()) {// CHECK CONNECTION WITH VjoyDevice
                ImGui::TextColored(col_green, "Status OK");
                feeder->update();
            }
            else
                ImGui::TextColored(col_red, "Status ERROR or Not Connected");
            current_log_item = feeder->get_log().size() - 1;
            ImGui::ListBox("LOG", &current_log_item, &feeder->get_log()[0], feeder->get_log().size(), 10);
            ImGui::End();
        }

        void draw_presets(int pos_x, int pos_y) { //! TO CHANGE ACCORDING TO WHAT IS NECESSARY
            vector<char*> v = SerialPort::getComPorts();
            static int item_current = 0;
            char** items = &v[0];
            ImGui::Begin("Presets (TO MODIFY)"); 
            ImGui::SetWindowSize(ImVec2(1400/2, 650/2), true);                          
            ImGui::SetWindowPos(ImVec2(pos_x, pos_y), true);
            ImGui::Combo("##", &item_current, items, v.size());
            ImGui::SameLine();
            if (ImGui::Button("Reload")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
            
            }
            ImGui::Text("Selected: %s", v[item_current]);
            ImGui::SameLine();
            if(true) // CHECK CONNECTION WITH COM
                ImGui::TextColored(col_green, "Status OK");
            else
                ImGui::TextColored(col_red, "Status ERROR");
            ImGui::End();
        }

        void draw_LCD_EMU() {
            ImGui::Begin("##Dashboard EMULATOR", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);  
            ImGui::SetWindowSize(ImVec2(1400, 650), true);
            ImGui::SetWindowPos(ImVec2(0, 50), true);
            auto drawobj = ImGui::GetWindowDrawList();
            tft->cust_drawLCD();

            // **************************** EMULATE HERE ****************************
            LCD_DATA_V2 data = mediator.sh_feeder.parsedData;
            uint8_t rpm = (data.gear__rpm_leds & 0x0F);
            tft->setCursor(40, 20);
            ImColor coltmp = ImColor(col_red);
            for(int i = 0; i < 15; i++) {
                if(rpm > 0 && rpm >= i) {
                    if(i > 9) tft->fillCircle(25+30*i, 0, 10, ImColor(col_blue));
                    else if(i > 4) {
                        tft->fillCircle(25+30*i, 0, 10, ImColor(col_red));
                    }
                    else tft->fillCircle(25+30*i, 0, 10, ImColor(col_green));
                }
                else
                    tft->drawCircle(25+30*i, 0, 10, ImColor(col_white));
            }
            // **********************************************************************

            ImGui::End();
        }

        void draw_TAB_buttons() {
            JOYSTICK_POSITION_V2 pos = mediator.vJoyFeeder.iReport;
            uint32_t state = pos.lButtons;
            static uint8_t cols = 8, spacing = 4;
            ImGui::Begin("Buttons", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);  
            ImGui::SetWindowSize(ImVec2(1400, 650), true);
            ImGui::SetWindowPos(ImVec2(0, 50), true);
            auto drawobj = ImGui::GetBackgroundDrawList();
            drawobj->AddRectFilled(ImVec2(0, 50), ImVec2(1400, 700), ImColor(col_background));

            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
            ImGui::SliderInt("Columns   ", (int*)&cols, 1, 10); ImGui::SameLine();
            ImGui::SliderInt("Spacing", (int*)&spacing, 0, 6);
            check_range(cols, 1, 10);
            check_range(spacing, 0, 6);
            ImGui::PopItemWidth();

            for (int i = 1; i <= buttons->size(); i++) {
                if(i > 32)
                    state = pos.lButtonsEx1;
                else if(i > 64)
                    state = pos.lButtonsEx2;
                else if(i > 96)
                    state = pos.lButtonsEx3;
                Button* btn = buttons->at(i-1);

                string tmp = std::to_string(i);
                tmp = tmp + std::string(3+spacing-tmp.size(), (char)' ');

                draw_button(btn, tmp.c_str(), state);
                
                if(i % cols != 0)
                    ImGui::SameLine();
            }
            draw_pov(1150, 600, 65);
            for(int i=0; i<4; i++)
                check_range(pov_idx[i], 1, 128);
            
            ImGui::End();
        }

        void draw_button(Button* button, const char* label, uint64_t state) {
            button->change_active_status(button->is_disabled());
            if(button->is_disabled()) {
                ImGui::PushStyleColor(ImGuiCol_CheckMark, col_disabled);
                ImGui::BeginDisabled();
            }
            else if(button->is_pressed(state))
                ImGui::PushStyleColor(ImGuiCol_CheckMark, col_green);
            else
                ImGui::PushStyleColor(ImGuiCol_CheckMark, col_red);
            ImGui::RadioButton(label, button->is_active());
            if(button->is_disabled())
                ImGui::EndDisabled();
            ImGui::PopStyleColor();
        }

        void draw_pov(int posX, int posY, int radius) {
            ImGui::SetCursorPos(ImVec2(posX - 50, posY - (2.5*radius)));
            ImGui::Text("Set POV Buttons");
            auto drawobj = ImGui::GetWindowDrawList();
            drawobj->AddCircle(ImVec2(posX, posY), radius, ImColor(col_white), 0, 2);
            drawobj->AddCircleFilled(ImVec2(posX, posY), radius/7, ImColor(col_red), 0);

            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.055f);
            ImGui::SetCursorPos(ImVec2(posX - 55, posY-(2.2*radius)));
            ImGui::InputInt("POV_TOP", &pov_idx[0]);

            ImGui::SetCursorPos(ImVec2(posX + radius + 10, posY-radius));
            ImGui::InputInt("POV_RIGHT", &pov_idx[1]);

            ImGui::SetCursorPos(ImVec2(posX - 55, posY+(0.4*radius)));
            ImGui::InputInt("POV_DOWN", &pov_idx[2]);

            ImGui::SetCursorPos(ImVec2(posX - (3.1*radius) - 10, posY-radius+2));
            ImGui::Text("POV_LEFT");
            ImGui::SetCursorPos(ImVec2(posX - (2.3*radius), posY-radius));
            ImGui::InputInt("##POV_LEFT", &pov_idx[3]);
            ImGui::PopItemWidth();
        }

        void draw_encoder(int x, int y, string name) {
            vector<char*> v = SerialPort::getComPorts(); //TO CHANGE TO FUNCTIONS 
            static int item_current = 0;
            char** items = &v[0];
            static int begin_i = 0, end_i = 1000; //CHANGE TO CORRECT VALUES
            static bool circular = false; //CHANGE TO CORRECT VALUES
            static int i1 = 0, i2 = 0; //CHANGE TO CORRECT VALUES

            ImGui::Begin(name.c_str());
            ImGui::SetWindowSize(ImVec2(1400/3, 650/2), true); 
            ImGui::SetWindowPos(ImVec2(x, y), true);
            
            ImGui::Text("Actual Value: %d", 100);
            ImGui::DragIntRange2("Range", &begin_i, &end_i, 1, -100, 100, "Min: %d", "Max: %d");
            ImGui::Combo("Function", &item_current, items, v.size());
            ImGui::Checkbox("Circular", &circular);
            ImGui::SameLine(); 
            if (ImGui::Button("Save To EEPROM")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
            
            }

            ImGui::End();
        }

        void draw_rotarySwitch(int x, int y, string name) {
            vector<char*> v = SerialPort::getComPorts(); //TO CHANGE TO FUNCTIONS 
            static int item_current = 0;
            char** items = &v[0];
            static int i1 = 0, i2 = 0; //CHANGE TO CORRECT VALUES
            
            ImGui::Begin(name.c_str());
            ImGui::SetWindowSize(ImVec2(1400/3, 650/2), true); 
            ImGui::SetWindowPos(ImVec2(x, y), true);
            
            ImGui::Text("Actual Value: %d", 12);
            ImGui::SliderInt("Filter", &i1, 0, 255);
            ImGui::Combo("Function", &item_current, items, v.size());
            if (ImGui::Button("Save To EEPROM")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
            
            }

            ImGui::End();
        }

        void draw_EEPROMSettings() {
            ImGui::Begin("EEPROM Settings");
            ImGui::SetWindowSize(ImVec2(win_x, win_y - 50), true); 
            ImGui::SetWindowPos(ImVec2(0, 50), true);
            static bool debug = true, showLogo = true, testMenu = false;
            ImGui::Checkbox("DebugMode", &debug);
            ImGui::Checkbox("ShowLogo", &showLogo);
            ImGui::Checkbox("testMenu", &testMenu);
            ImGui::End();
        }

        void draw() {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Welcome To TheHawkSG Steering Wheel Configurator", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
            ImGui::SetWindowSize(ImVec2(win_x, win_y), true); 
            ImGui::SetWindowPos(ImVec2(0,0), true);
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable;
            if(ImGui::BeginTabBar("TABS", tab_bar_flags)){
                if(ImGui::BeginTabItem("Connections")) {
                    draw_STM32COM(0, 50);
                    draw_SimHubCOM(1400/2, 50);
                    draw_VjoyStatus(0, 50+650/2);
                    draw_presets(1400/2, 50+650/2);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Buttons")) {
                    draw_TAB_buttons();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Rotary Switch")) {
                    draw_rotarySwitch(0, 50,     "Settings For RotarySwitch 1");
                    draw_rotarySwitch(1400/3, 50, "Settings For RotarySwitch 2");
                    draw_rotarySwitch((1400/3)*2, 50, "Settings For RotarySwitch 3");
                    draw_rotarySwitch(0, 50+650/2,     "Settings For RotarySwitch 4");
                    draw_rotarySwitch(1400/3, 50+650/2, "Settings For RotarySwitch 5");
                    draw_rotarySwitch((1400/3)*2, 50+650/2, "Settings For RotarySwitch 6");
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Encoders")) {
                    draw_encoder(0, 50,     "Settings For Encoder 1");
                    draw_encoder(1400/3, 50, "Settings For Encoder 2");
                    draw_encoder((1400/3)*2, 50, "Settings For Encoder 3");
                    draw_encoder(0, 50+650/2,     "Settings For Encoder 4");
                    draw_encoder(1400/3, 50+650/2, "Settings For Encoder 5");
                    draw_encoder((1400/3)*2, 50+650/2, "Settings For Encoder 6");
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("EEPROM")) {
                    draw_EEPROMSettings();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Emulator")) {
                    draw_LCD_EMU();
                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
            }
            ImGui::End();
        }

        void render() {
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        void destroyGUI() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);
            glfwTerminate();
        }
};

SG_GUI* SG_GUI::inst = NULL;



#endif