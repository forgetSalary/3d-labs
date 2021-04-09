#include "image_analyzing.h"

static void draw_graph_line(HDC hdc,int x,int window_height,int y) {
    int count = window_height - y;
    for (int i = 0; i < count; i++){
        SetPixel(hdc, x, window_height - i, 0);
    }
}

void analyze_brightness(HDC hdc,int window_width, int window_height, int image_width,int image_height) {
    COLORREF pixel_color;
    BYTE R, G, B;
    double line_brightness;

    int graph_height = window_height - image_height;

    int max_line_brightness = 255 * image_height;

    double gh_k = (double)graph_height/(double)max_line_brightness;

    for (size_t x = 0; x < image_width; x++) {
        line_brightness = max_line_brightness;
        for (size_t y = 0; y < image_height; y++) {
            pixel_color = GetPixel(hdc, x, y);
            R = GetRValue(pixel_color);
            G = GetGValue(pixel_color);
            B = GetBValue(pixel_color);
            line_brightness -= (double)(R+G+B) / 3;
        }
        line_brightness = line_brightness*gh_k + graph_height;

        draw_graph_line(hdc, x, window_height, line_brightness);
    }
}


void high_frq_filter(HDC hdc, int image_width, int image_height) {
    int filter_matrix[3][3] = { {0,-1,0},
                                {-1,5,-1},
                                {0,-1,0} };

    COLORREF** new_image = (COLORREF**)malloc(sizeof(COLORREF*)*image_width + sizeof(COLORREF)*image_height*image_width);
    new_image[0] = *new_image + image_width;
    for (size_t i = 1; i < image_height; i++){
        new_image[i] = new_image[0] + i * image_width;
    }

    COLORREF pixels_3x3[3][3];

    for (size_t x = 1; x < image_width-1; x++) {
        for (size_t y = 1; y < image_height-1; y++) {
            for (size_t i = 0; i < 3; i++){
                for (size_t j = 0; j < 3; j++){
                    pixels_3x3[i][j] = GetPixel(hdc, x-1+i, y-1+j);
                }
            }



            new_image = ()
        }
    }
}