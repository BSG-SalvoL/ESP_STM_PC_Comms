#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <windows.h>

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

void push_to_log_vector(std::vector<const char*> &vec, std::string str) {
    char* buf = new char[str.size()+1];
    strncpy(buf, str.c_str(), str.size());
    buf[str.size()] = '\0';
    vec.push_back(buf);
}

void check_range(int& value, int start, int end) {
    if(value < start)
        value = start;
    if(value > end) 
        value = end;
}

void check_range(uint8_t& value, int start, int end) {
    if(value < start)
        value = start;
    if(value > end) 
        value = end;
}

void check_range(uint16_t& value, int start, int end) {
    if(value < start)
        value = start;
    if(value > end) 
        value = end;
}

void check_range(uint32_t& value, int start, int end) {
    if(value < start)
        value = start;
    if(value > end) 
        value = end;
}

void check_range(uint64_t& value, uint64_t start, uint64_t end) {
    if(value < start)
        value = start;
    if(value > end) 
        value = end;
}

uint16_t rgb_to_565rgb(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t result = (((int)r/255*31) << 11 | ((int)g/255*63) << 5 | ((int)b/255*31));
    return result; 
}

uint32_t rgb565_to_um32(uint16_t col) {
    uint32_t result = 0;
    uint8_t r = ((col & 0b1111100000000000) >> 11); r = (int)((r*255)+15)/31;
    uint8_t g = ((col & 0b0000011111100000) >> 5); g = (int)((g*255)+31)/63;
    uint8_t b = col & 0b0000000000011111; b = (int)((b*255)+15)/31;
    if(col == 0xFFFF)
        printf("%x, %x, %x\n", r, g, b);
    result = (result << 8) | r;
    result = (result << 8) | g;
    result = (result << 8) | b;
    result = (result << 8) | 0xff;
    return result;
}