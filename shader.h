/*
 * @Author: Alien
 * @Date: 2023-03-09 23:56:54
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-10 11:08:00
 */
#ifndef SHADER_H
#define SHADER_H
#include "geometry.h"
#include "model.h"

extern Model* model;
extern Matrix Projection, ViewPort, ModelView;
extern Vec3f light_dir;
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
    GouraudShader(){};
    //接受两个变量，(面，顶点序号)
    virtual Vec4f vertex(std::vector<Vec3i> &face, int nthvert) {
        //根据面序号和顶点序号读取模型对应顶点，并扩展为4维 
        Vec4f gl_Vertex = embed<4>(model->vert(face[nthvert][0]));
        varying_uv.set_col(nthvert, model->get_uv(face[nthvert][1]));
        //变换顶点坐标到屏幕坐标（视角矩阵*投影矩阵*变换矩阵*v）
        mat<4, 4, float> uniform_M = Projection * ModelView;
        mat<4, 4, float> uniform_MIT = (Projection * ModelView).invert_transpose();
        gl_Vertex = ViewPort* uniform_M *gl_Vertex;
        //计算光照强度（顶点法向量*光照方向）
        Vec3f normal = proj<3>(embed<4>(model->vn(face[nthvert][2]))).normalize();
        varying_intensity[nthvert] = std::max(0.f, dot(model->vn(face[nthvert][2]),light_dir) ); // get diffuse lighting intensity
        return gl_Vertex;
    }
    //根据传入的质心坐标，颜色，以及varying_intensity计算出当前像素的颜色
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2f uv = varying_uv * bar;
        TGAColor c = model->diffuse(uv);
        float intensity = dot(varying_intensity,bar);
        color = c*intensity; 
        return false;                              
    }
};

#endif
