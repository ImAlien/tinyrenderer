/*
 * @Author: Alien
 * @Date: 2023-03-09 14:34:45
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-09 14:35:46
 */
#include "def.h"

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}
Vec2i convertInt(Vec2f v){
    return Vec2i((int)(v.x + 0.5), int(v.y + 0.5));
}

