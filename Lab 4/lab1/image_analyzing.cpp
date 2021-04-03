#include "image_analyzing.h"


void analyze_brightness(HDC hdc,int window_width, int window_height, int image_width,int image_height) {
    COLORREF pixel_color;
    BYTE R, G, B;
    uint32_t line_brightness;

    int graph_width = window_width - image_width;
    int graph_height = window_height - image_height;

    int gw_k = graph_width / 255;

    for (size_t x = 0; x < image_width; x++) {
        line_brightness = 0;
        for (size_t y = 0; y < image_height; y++) {
            pixel_color = GetPixel(hdc, x, y);
            R = GetRValue(pixel_color);
            G = GetGValue(pixel_color);
            B = GetBValue(pixel_color);
            line_brightness += (uint32_t)(R+G+B) / 3;
        }



    }
}