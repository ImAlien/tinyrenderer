/*
 * @Author: Alien
 * @Date: 2023-03-09 14:17:51
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-10 14:33:39
 */
#include "tgaimage.h"
#include "geometry.h"
#include "def.h"

Matrix viewport(const int x, const int y, const int w, const int h);
void projection(const double coeff=0); // coeff = -1/c
Matrix lookat(const Vec3f eye, const Vec3f center, const Vec3f up);


Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P);
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void triangle_zbuffer(std::vector<Vec3i> face, TGAImage& zbuffer, TGAImage &image, float intensity);
// struct IShader {
//     static TGAColor sample2D(const TGAImage &img, Vec2f &uvf) {
//         return img.get(uvf[0] * img.width(), uvf[1] * img.height());
//     }
//     virtual bool fragment(const vec3 bar, TGAColor &color) = 0;
// };

//void triangle(const vec4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer);

