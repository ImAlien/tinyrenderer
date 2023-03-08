/*
 * @Author: Alien
 * @Date: 2023-03-08 10:43:34
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-08 14:58:03
 */
#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 200;
const int height = 200;


// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     for (float t=0.; t<1.; t+=.1) {
//         int x = x0*(1.-t) + x1*t;
//         int y = y0*(1.-t) + y1*t;
//         image.set(x, y, color);
//     }
// }
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
//     for (int x=x0; x<=x1; x++) { 
//         float t = (x-x0)/(float)(x1-x0); 
//         int y = y0*(1.-t) + y1*t; 
//         image.set(x, y, color); 
//     } 
// }
// version 3: don't consider the pre_message
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
//     bool steep = false; 
//     if (std::abs(x0-x1)<std::abs(y0-y1)) { // if the line is steep, we transpose the image 
//         std::swap(x0, y0); 
//         std::swap(x1, y1); 
//         steep = true; 
//     } 
//     if (x0>x1) { // make it left−to−right 
//         std::swap(x0, x1); 
//         std::swap(y0, y1); 
//     } 
//     for (int x=x0; x<=x1; x++) { 
//         float t = (x-x0)/(float)(x1-x0); 
//         int y = y0*(1.-t) + y1*t; 
//         if (steep) { 
//             image.set(y, x, color); // if transposed, de−transpose 
//         } else { 
//             image.set(x, y, color); 
//         } 
//     } 
// }
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    // float derror = std::abs(dy/float(dx)); 
    // float error = 0; 
    int derror = std::abs(dy) * 2;
    int error = 0;
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            image.set(y, x, color); 
        } else { 
            image.set(x, y, color); 
        } 
        error += derror; 
        if (error> dx) { 
            y += (y1>y0?1:-1); 
            error -= 2*dx; 
        } 
    } 
} 
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     bool steep = false;
//     if (std::abs(x0-x1)<std::abs(y0-y1)) {
//         std::swap(x0, y0);
//         std::swap(x1, y1);
//         steep = true;
//     }
//     if (x0>x1) {
//         std::swap(x0, x1);
//         std::swap(y0, y1);
//     }

//     for (int x=x0; x<=x1; x++) {
//         float t = (x-x0)/(float)(x1-x0);
//         int y = y0*(1.-t) + y1*t;
//         if (steep) {
//             image.set(y, x, color);
//         } else {
//             image.set(x, y, color);
//         }
//     }
// }
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color){
	line(p0.x, p0.y, p1.x, p1.y,image, color);
}
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
    // sort t0, t1, t2 by y
    if(t1.y > t0.y && t1.y > t2.y) {
        std::swap(t0, t1);
    }
    if(t2.y > t0.y && t2.y > t1.y) std::swap(t0, t2);
    if(t2.y > t1.y) std::swap(t2, t1);
    //draw form t0.y to t1.y
    float t = (t1.y - t0.y)/(float)(t2.y-t0.y);  
    int x3 =  t0.x*(1.-t) + t2.x*t ;
    for(int j = t0.y; j >= t1.y; j --){
        float t = (j - t0.y)/(float)(t1.y-t0.y); 
        int x1 = t0.x*(1.-t) + t1.x*t; 
        int x2 = t0.x*(1.-t) + x3*t;
        if(x1 > x2) std::swap(x1, x2);
        for(int i = x1; i <= x2; i ++) image.set(i, j, color);
    }
    // draw from t1.y to t2.y
   for(int j = t1.y; j >= t2.y; j --){
        float t = (t2.y - j)/(float)(t2.y-t1.y); 
        int x1 = t1.x * t  + t2.x*(1. -t); 
        int x2 = x3 * t+ t2.x*(1. - t);
        if(x1 > x2) std::swap(x1, x2);
        for(int i = x1; i <= x2; i ++) image.set(i, j, color);
    } 
}
int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);
    
    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

