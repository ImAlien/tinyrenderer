/*
 * @Author: Alien
 * @Date: 2023-03-11 10:20:49
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-11 16:22:18
 */
#ifndef SHADER_H
#define SHADER_H
#include "geometry.h"
#include "model.h"

extern Model* model;
extern Matrix Projection, ViewPort, ModelView;
extern Vec3f light_dir;
extern mat<4, 4, float> uniform_M ;
extern mat<4, 4, float> uniform_MIT ;
//高洛德着色器

struct IShader {
    virtual ~IShader() = default;
    virtual Vec4f vertex(std::vector<Vec3i> &face, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

struct GouraudShader : public IShader {
    //顶点着色器会将数据写入varying_intensity
    //片元着色器从varying_intensity中读取数据
    Vec3f varying_intensity; 
    mat<2, 3, float> varying_uv;
    mat<3,3,float> varying_nrm; // normal per vertex to be interpolated by FS
    mat<4,3,float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
    mat<3,3,float> ndc_tri;     // triangle in normalized device coordinates
    GouraudShader(){};
    //接受两个变量，(面，顶点序号)
    virtual Vec4f vertex(std::vector<Vec3i> &face, int nthvert) {
        //根据面序号和顶点序号读取模型对应顶点，并扩展为4维 
        Vec4f gl_Vertex = embed<4>(model->vert(face[nthvert][0]));
        varying_uv.set_col(nthvert, model->get_uv(face[nthvert][1]));
        Vec3f normal = proj<3>(uniform_MIT*embed<4>(model->vn(face[nthvert][2]), 0.f));
        varying_nrm.set_col(nthvert, proj<3>(uniform_MIT*embed<4>(model->vn(face[nthvert][2]), 0.f)));
        //变换顶点坐标到屏幕坐标（视角矩阵*投影矩阵*变换矩阵*v）
        gl_Vertex = ViewPort* uniform_M *gl_Vertex;
        //计算光照强度（顶点法向量*光照方向）
        varying_intensity[nthvert] = std::max(0.f, dot(normal.normalize(),light_dir) ); // get diffuse lighting intensity
        varying_tri.set_col(nthvert, gl_Vertex);
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex/gl_Vertex[3]));
        return gl_Vertex;
    }
    //根据传入的质心坐标，颜色，以及varying_intensity计算出当前像素的颜色
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2f uv = varying_uv * bar;
        TGAColor c = model->diffuse(uv);
        //float intensity = dot(varying_intensity,bar);
        Vec3f n = proj<3>(uniform_MIT*embed<4>(model->normal(uv))).normalize();
        //std::cout << 4 << std::endl;
        //std::cout << "shader: n" << n << std::endl;
        float tot_ity = std::max(0.01f,dot(n,-light_dir));
        color = c*tot_ity; 
        return false;    
        // Vec3f bn = (varying_nrm*bar).normalize();
        // Vec2f uv = varying_uv*bar;

        // mat<3,3,float> A;
        // A[0] = ndc_tri.col(1) - ndc_tri.col(0);
        // A[1] = ndc_tri.col(2) - ndc_tri.col(0);
        // A[2] = bn;

        // mat<3,3,float> AI = A.invert();

        // Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
        // Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

        // mat<3,3,float> B;
        // B.set_col(0, i.normalize());
        // B.set_col(1, j.normalize());
        // B.set_col(2, bn);

        // Vec3f n = (B*model->normal_tan(uv)).normalize();

        // float diff = std::max(0.f, dot(n,light_dir));
        // color = model->diffuse(uv)*diff;

        // return false;
    }
};

#endif