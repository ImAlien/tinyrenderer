/*
 * @Author: Alien
 * @Date: 2023-03-08 23:16:19
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-09 14:37:33
 */
#ifndef DEF_H
#define DEF_H

extern int width, height;

#include "geometry.h"
#include "model.h"

extern Model *model;
// functions.cpp
Vec3f world2screen(Vec3f v); 
Vec2i convertInt(Vec2f v);

#endif