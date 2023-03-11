/*
 * @Author: Alien
 * @Date: 2023-03-09 14:17:56
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-11 10:09:21
 */
#include "my_gl.h"

extern Model *model;
extern Matrix Projection,ViewPort, ModelView;
extern mat<4,4,float> uniform_M;   //  Projection*ModelView
extern mat<4,4,float> uniform_MIT; // (Projection*ModelView).invert_transpose()
extern int depth;
extern Vec3f light_dir;

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity();
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}
Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z, x).normalize();
    Matrix rotation = Matrix::identity();
    Matrix translation = Matrix::identity();
    //***矩阵的第四列是用于平移的。因为观察位置从原点变为了center，所以需要将物体平移-center***
    for (int i = 0; i < 3; i++) {
        rotation[i][3] = -center[i];
    }
    //正交矩阵的逆 = 正交矩阵的转置
    //矩阵的第一行即是现在的x
    //矩阵的第二行即是现在的y
    //矩阵的第三行即是现在的z
    //***矩阵的三阶子矩阵是当前视线旋转矩阵的逆矩阵***
    for (int i = 0; i < 3; i++) {
        rotation[0][i] = x[i];
        rotation[1][i] = y[i];
        rotation[2][i] = z[i];
    }
    //这样乘法的效果是先平移物体，再旋转
    Matrix res = rotation*translation;
    return res;
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

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
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);
    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec2i A =               t0 + (t2-t0)*alpha;
        Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}
Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}
Vec4f m2v4(Matrix m){
    return Vec4f(m[0][0],m[1][0],m[2][0],m[3][0]);
}
Matrix v2m(Vec3f v) {
    Matrix m;
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}
void triangle_zbuffer(std::vector<Vec3i> face, TGAImage& zbuffer, TGAImage &image, float intensity) {
    Vec3f pts[3];
    Vec4f pt4[3];
    Vec2f uvs[3];
    float itys[3];
    for (int i=0; i<3; i++) {
        const Vec3f &v = model->vert(face[i][0]);
        pts[i] = m2v(ViewPort*Projection*ModelView*v2m(v));
        pt4[i] = m2v4(ViewPort*Projection*ModelView*v2m(v));
        uvs[i] = model->get_uv(face[i][1]);
        Vec3f vn = model->vn(face[i][2]);
        itys[i] = dot(vn.normalize(), -light_dir);
    }
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3i P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            Vec3f bc_clip;
            for (int i = 0; i < 3; ++i)
            {
                //求α，β，γ,只需要除以pts第四个分量即可
                bc_clip[i] = bc_screen[i] / pt4[i][3];
            }
            float Z_n = 1. / (bc_clip[0] + bc_clip[1] + bc_clip[2]);
            bc_clip = bc_clip*Z_n;
            //bc_clip = bc_clip;
            if (bc_clip.x<0 || bc_clip.y<0 || bc_clip.z<0) continue;
            P.z = 0;
            // z can be calculated by barycentric 
            Vec2f uvf;
            float tot_ity = 0;
            for (int i=0; i<3; i++) {
                P.z += pts[i][2]*bc_clip[i];
                uvf[0] += uvs[i][0] * bc_clip[i];
                uvf[1] += uvs[i][1] * bc_clip[i];
                tot_ity += itys[i]*bc_clip[i];
            }
            tot_ity = std::max(tot_ity,0.f);
            Vec3f n = proj<3>(uniform_MIT*embed<4>(model->normal(uvf))).normalize();
            tot_ity = std::max(0.01f,dot(n,-light_dir));
            if (zbuffer.get(P.x, P.y)[0]<P.z) {
                zbuffer.set(P.x, P.y,TGAColor(P.z ));
                TGAColor color = model->diffuse(uvf);
                image.set(P.x, P.y, TGAColor(color.r*tot_ity, color.g*tot_ity, color.b*tot_ity));
            }
        }
    }
}

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color){
	line(p0.x, p0.y, p1.x, p1.y,image, color);
}