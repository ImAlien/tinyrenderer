/*
 * @Author: Alien
 * @Date: 2023-03-08 23:16:19
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-09 00:27:53
 */
#ifndef DEF_H
#define DEF_H

extern int width, height;

#include "geometry.h"

// functions.cpp
Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}
Vec2i convertInt(Vec2f v){
    return Vec2i((int)(v.x + 0.5), int(v.y + 0.5));
}
#endif