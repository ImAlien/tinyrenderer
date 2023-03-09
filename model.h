/*
 * @Author: Alien
 * @Date: 2023-03-08 22:28:51
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-09 00:41:03
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
	void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f get_uv(int i);
	Vec2i uv(int iface, int nvert);
	TGAColor diffuse(Vec2i uv);
	TGAColor diffuse(Vec2f uv);
	std::vector<Vec3i> face(int idx);
};

#endif //__MODEL_H__
