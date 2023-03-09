/*
 * @Author: Alien
 * @Date: 2023-03-09 13:59:19
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-09 13:59:20
 */
#include "geometry.h"

template <> template <> vec<4, int>  ::vec(const vec<4, float> &v) : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)), w(int(v.w + .5f)) {}
template <> template <> vec<4, float>::vec(const vec<4, int> &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
template <> template <> vec<3, int>  ::vec(const vec<3, float> &v) : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}
template <> template <> vec<3, float>::vec(const vec<3, int> &v) : x(v.x), y(v.y), z(v.z) {}
template <> template <> vec<2, int>  ::vec(const vec<2, float> &v) : x(int(v.x + .5f)), y(int(v.y + .5f)) {}
template <> template <> vec<2, float>::vec(const vec<2, int> &v) : x(v.x), y(v.y) {}