/*
 * @Author: Alien
 * @Date: 2023-03-09 14:17:56
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-10 11:37:54
 */
#include "my_gl.h"

extern Model *model;
extern Vec3f camera;
TGAImage image(width, height, TGAImage::RGB);
extern Matrix Projection;
extern Matrix ViewPort, ModelView;
extern Vec3f light_dir;

// Matrix lookat(Vec3f eye, Vec3f center, Vec3f up)
// {
// 	Vec3f z = (eye - center).normalize();
// 	Vec3f x = cross(up, z).normalize();
// 	Vec3f y = cross(z, x).normalize();

// 	Matrix rotate = Matrix::identity();
// 	Matrix translate = Matrix::identity();
// 	for (int i = 0; i < 3; ++i)
// 	{
// 		rotate[0][i] = x[i];
// 		rotate[1][i] = y[i];
// 		rotate[2][i] = z[i];
// 		translate[i][3] = -eye[i];
// 	}
// 	return rotate * translate;
// }
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

Matrix projection(double fov, double ratio, double n, double f)
{
	float t = (-n) * tanf(fov / 2);
	float r = t * ratio;
	return ortho(-r, r, -t, t, n, f) * perspective(n, f);
}

Matrix perspective(double n, double f)
{
	Matrix ret;
	ret[0][0] = n;
	ret[1][1] = n;
	ret[2][2] = n + f;
	ret[2][3] = -f * n;
	ret[3][2] = 1.0f;
	return ret;
}

Matrix ortho(float l, float r, float b, float t, float n, float f)
{
	Matrix ret = Matrix::identity();
	ret[0][0] = 2.0f / (r - l);
	ret[1][1] = 2.0f / (t - b);
	ret[2][2] = 2.0f / (n - f);
	ret[0][3] = (l + r) / (l - r);
	ret[1][3] = (b + t) / (b - t);
	ret[2][3] = (n + f) / (f - n);
	return ret;
}
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
Matrix viewport(unsigned width, unsigned height)
{
	Matrix ret = Matrix::identity();
	ret[0][0] = width / 2.0f;
	ret[1][1] = height / 2.0f;
	ret[0][3] = (width - 1) / 2.0f;
	ret[1][3] = (height - 1) / 2.0f;
	return ret;
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
//计算质心坐标
Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) 
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1);
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
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
//     if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
//     if (t0.y>t1.y) std::swap(t0, t1);
//     if (t0.y>t2.y) std::swap(t0, t2);
//     if (t1.y>t2.y) std::swap(t1, t2);
//     int total_height = t2.y-t0.y;
//     for (int i=0; i<total_height; i++) {
//         bool second_half = i>t1.y-t0.y || t1.y==t0.y;
//         int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
//         float alpha = (float)i/total_height;
//         float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
//         Vec2i A =               t0 + (t2-t0)*alpha;
//         Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
//         if (A.x>B.x) std::swap(A, B);
//         for (int j=A.x; j<=B.x; j++) {
//             image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
//         }
//     }
// }
void triangle(std::vector<Vec3i> &face, IShader& shader, TGAImage &image, float *zbuffer) {
    Vec3i t0, t1,  t2;
    Vec4f pts[3];
    Vec2i uv0, uv1,  uv2;
    float ity0,ity1,ity2;
    for (int j=0; j<3; j++) {
            pts[j] = shader.vertex(face, j);
            Vec3f v = model->vert(face[j][0]);
            Vec3f vn = model->vn(face[j][2]);
            if(j == 0) {
                t0 = m2v(ViewPort*Projection*ModelView*v2m(v));
                uv0 = model->get_uv(face[j][1]);
                ity0 = dot(vn.normalize(), -light_dir);
            }
            if(j == 1) {
                t1 = m2v(ViewPort*Projection*ModelView*v2m(v));
                uv1 = model->get_uv(face[j][1]);
                ity1 = dot(vn.normalize(), -light_dir);

            }
            if(j == 2) {
                t2 = m2v(ViewPort*Projection*ModelView*v2m(v));
                uv2 = model->get_uv(face[j][1]);
                ity2 = dot(vn.normalize(), -light_dir);
            }
        }
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); }
    if (t0.y>t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); }
    if (t1.y>t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); }

    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec3i A   =               t0  + convertInt(Vec3f(t2-t0  )*alpha);
        Vec3i B   = second_half ? t1  + convertInt(Vec3f(t2-t1  )*beta) : t0  + convertInt(Vec3f(t1-t0  )*beta);
        float ityA =               ity0 +   (ity2-ity0)*alpha;
        float ityB = second_half ? ity1 +   (ity2-ity1)*beta : ity0 +   (ity1-ity0)*beta;
        Vec2i uvA =               uv0 +      (uv2-uv0)*alpha;
        Vec2i uvB = second_half ? uv1 +      (uv2-uv1)*beta : uv0 +      (uv1-uv0)*beta;
        if (A.x>B.x) { std::swap(A, B); std::swap(uvA, uvB); std::swap(ityA,ityB);}
        for (int j=A.x; j<=B.x; j++) {
            float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
            Vec3i   P = Vec3f(A) + Vec3f(B-A)*phi;
            Vec2i uvP =     uvA +   (uvB-uvA)*phi;
            int idx = P.x+P.y*width;
            float ityP =    ityA  + (ityB-ityA)*phi;
            ityP = std::min(1.f, std::abs(ityP)+0.01f);
            if (zbuffer[idx]<P.z) {
                zbuffer[idx] = P.z;
                TGAColor color = model->diffuse(uvP);
                image.set(P.x, P.y, TGAColor(color.r*ityP, color.g*ityP, color.b*ityP));
            }
        }
    }
}

//绘制三角形
void triangle(Vec4f *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer) {
    //初始化三角形边界框
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            //这里pts除以了最后一个分量，实现了透视中的缩放，所以作为边界框
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]/pts[i][3]);
        }
    }
    //当前像素坐标P，颜色color
    Vec2i P;
    TGAColor color;
    //遍历边界框中的每一个像素
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            //c为当前P对应的质心坐标
            //这里pts除以了最后一个分量，实现了透视中的缩放，所以用于判断P是否在三角形内
            Vec3f c = barycentric(proj<2>(pts[0]/pts[0][3]), proj<2>(pts[1]/pts[1][3]), proj<2>(pts[2]/pts[2][3]), proj<2>(P));
            //插值计算P的zbuffer
            //pts[i]为三角形的三个顶点
            //pts[i][2]为三角形的z信息(0~255)
            //pts[i][3]为三角形的投影系数(1-z/c)
            
            float z_P = (pts[0][2]/ pts[0][3])*c.x + (pts[0][2] / pts[1][3]) *c.y + (pts[0][2] / pts[2][3]) *c.z;
            int frag_depth = std::max(0, std::min(255, int(z_P+.5)));
            //P的任一质心分量小于0或者zbuffer小于已有zbuffer，不渲染
            if (c.x<0 || c.y<0 || c.z<0 || zbuffer.get(P.x, P.y).r>frag_depth) continue;
            //调用片元着色器计算当前像素颜色
            bool discard = shader.fragment(c, color);
            if (!discard) {
                //zbuffer
                zbuffer.set(P.x, P.y, TGAColor(frag_depth,frag_depth,frag_depth));
                //为像素设置颜色
                image.set(P.x, P.y, color);
            }
        }
    }
}
Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m;
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}
void triangle_zbuffer(std::vector<Vec3i> face, float *zbuffer, TGAImage &image, float intensity) {
    Vec3f pts[3];
    Vec2f uvs[3];
    Matrix Projection = Matrix::identity();
    Matrix ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4);
    Projection[3][2] = -1.f/camera.z;
    for (int i=0; i<3; i++) {
        Vec3f v = model->vert(face[i][0]);
        pts[i] = m2v(ViewPort*Projection*v2m(v));
        //pts[i] = world2screen(model->vert(face[i][0]));
        uvs[i] = model->get_uv(face[i][1]);
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
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            // z can be calculated by barycentric 
            Vec2f uvf;
            for (int i=0; i<3; i++) {
                P.z += pts[i][2]*bc_screen[i];
                uvf[0] += uvs[i][0] * bc_screen[i];
                uvf[1] += uvs[i][1] * bc_screen[i];
            }
            if (zbuffer[int(P.x+P.y*width)]<P.z) {
                zbuffer[int(P.x+P.y*width)] = P.z;
                TGAColor color = model->diffuse(uvf);
                image.set(P.x, P.y, TGAColor(color.r*intensity, color.g*intensity, color.b*intensity));
            }
        }
    }
}
void triangle_zbuffer(std::vector<Vec3i> &face, IShader& shader, TGAImage &image, float *zbuffer){
    Vec3f pts[3];
    Vec2f uvs[3];
    float itys[3];
    for (int i=0; i<3; i++) {
        Vec3f v = model->vert(face[i][0]);
        pts[i] = m2v(ViewPort*Projection*ModelView*v2m(v));
        //pts[i] = world2screen(model->vert(face[i][0]));
        uvs[i] = model->get_uv(face[i][1]);
        itys[i] = dot(model->vn(face[i][2]).normalize(), -light_dir);
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
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            // z can be calculated by barycentric 
            Vec2f uvf;
            float intensity = 0;
            for (int i=0; i<3; i++) {
                intensity += itys[i]*bc_screen[i];
                P.z += pts[i][2]*bc_screen[i];
                uvf[0] += uvs[i][0] * bc_screen[i];
                uvf[1] += uvs[i][1] * bc_screen[i];
            }
            if (zbuffer[int(P.x+P.y*width)]<P.z) {
                zbuffer[int(P.x+P.y*width)] = P.z;
                TGAColor color = model->diffuse(uvf);
                //image.set(P.x, P.y, TGAColor(color.r*intensity, color.g*intensity, color.b*intensity));
                image.set(P.x, P.y, TGAColor(color.r*255, color.g*255, color.b*255));
            }
        }
    }
}
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color){
	line(p0.x, p0.y, p1.x, p1.y,image, color);
}