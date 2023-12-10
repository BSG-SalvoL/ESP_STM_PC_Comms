#pragma once

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
    void drawPixel(uint16_t x, uint16_t y, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddCircleFilled(ImVec2(x+xStart, y+yStart), 1, color);
    } 
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddLine(ImVec2(x0+xStart, y0+yStart), ImVec2(x1+xStart, y1+yStart), color);
    }
    void drawFastVLine(uint16_t x0, uint16_t y0, uint16_t length, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddLine(ImVec2(x0+xStart, y0+yStart), ImVec2(x0+xStart, yStart+y0+length), color);
    }
    void drawFastHLine(uint8_t x0, uint8_t y0, uint8_t length, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddLine(ImVec2(x0+xStart, y0+yStart), ImVec2(xStart+x0+length, yStart+y0), color);
    }
    void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRect(ImVec2(x0+xStart, y0+yStart), ImVec2(xStart+x0+w, yStart+y0+h), color);
    }
    void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(x0+xStart, y0+yStart), ImVec2(xStart+x0+w, yStart+y0+h), color);
    }
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddCircle(ImVec2(xStart+x0, yStart+y0), r, color, 0);
            
    }
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddCircleFilled(ImVec2(xStart+x0, yStart+y0), r, color, 0);
    }
    void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x0+w, yStart+y0+h), color, radius);
    }
    void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x0+w, yStart+y0+h), color, radius);
    }
    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddTriangle(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x1, yStart+y1), ImVec2(xStart+x2, yStart+y2), color);
    }
    void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddTriangleFilled(ImVec2(xStart+x0, yStart+y0), ImVec2(xStart+x1, yStart+y1), ImVec2(xStart+x2, yStart+y2), color);
    }
    void drawChar(uint16_t x, uint16_t y, char c, ImColor color, uint16_t bg, uint8_t size) {
        auto draw = ImGui::GetWindowDrawList();
        setCursor(xStart+x, yStart+y);
        setTextColor(color, bg);
        setTextSize(size);
        print("%c", c);
    }
    void setCursor(int16_t x0, int16_t y0) {
        ImGui::SetCursorPos(ImVec2(x0+xStart, y0+yStart));
    }
    void setTextColor(ImColor color) {
        lastColor = color;
    }
    void setTextColor(ImColor color, ImColor backgroundcolor) {
        lastColor = color;
        lastbgcolor = backgroundcolor;
    }
    void setTextSize(uint8_t size) {
        auto draw = ImGui::GetWindowDrawList();
        ImGui::SetWindowFontScale(1+(0.2*size));
    }
    void setTextWrap(bool w) {

    }
    void fillScreen(ImColor color) {
        auto draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(ImVec2(xStart, yStart), ImVec2(xStart+width, yStart+height), color);
    }
    template<typename... Args>
    void print(const char* fmt, Args&&... args) {
        ImGui::Text(fmt, args...);
    }
};