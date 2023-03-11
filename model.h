/*
 * @Author: Alien
 * @Date: 2023-03-08 22:28:51
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-11 12:01:13
 */
#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
	std::vector<Vec3f> norms_;
	std::vector<Vec2f> uv_;
	TGAImage diffusemap_;
	TGAImage normalmap;          // normal map texture
	TGAImage normalmap_tan;          // normal map texture
    TGAImage specularmap_;        // specular map texture
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f vert(int iface, int nvert);
	Vec3f vn(int idx);
	Vec2f get_uv(int i);
	Vec2i uv(int iface, int nvert);
	Vec3f normal(int iface, int nvert);
	Vec3f normal(const Vec2f &uvf);                     // fetch the normal vector from the normal map texture
	Vec3f normal_tan(const Vec2f &uvf);                     // fetch the normal vector from the normal map texture
	TGAColor diffuse(Vec2i uv);
	TGAColor diffuse(Vec2f uv);
	std::vector<Vec3i> face(int idx);
};

#endif //__MODEL_H__
