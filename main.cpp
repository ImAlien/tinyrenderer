/*
 * @Author: Alien
 * @Date: 2023-03-08 10:43:34
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-11 16:09:49
 */
#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "def.h"
#include "my_gl.h"
#include "shader.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
Matrix Projection,ViewPort, ModelView;
mat<4,4,float> uniform_M;   //  Projection*ModelView
mat<4,4,float> uniform_MIT; // (Projection*ModelView).invert_transpose()
int width  = 800;
int height = 800;
int depth = 255;
int *zbuffer = NULL;
Vec3f light_dir = Vec3f(-0.5,0,-1).normalize();
Vec3f eye(1,1,3);
Vec3f center(0,0,1);
Vec3f camera(0,0,3);

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }
    // zbuffer
    //float *zbuffer = new float[width*height];
    //for(int i = 0; i < width * height; i ++) zbuffer[i] = -1e9;
    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::RGB);
    
    ModelView  = lookat(eye, center, Vec3f(0,1,0));
    Projection = Matrix::identity();
    ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4);
    Projection[3][2] = -1.f/(eye-center).norm();
    uniform_M   =  Projection*ModelView;
    uniform_MIT = (Projection*ModelView).invert_transpose();
    GouraudShader shader;
    for (int i=0; i<model->nfaces(); i++) {
        // traverse all face
        // three points' index;
        std::vector<Vec3i> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j][0]);
            screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.);
            world_coords[j]  = v;
        }
        Vec3f n = cross((world_coords[2]-world_coords[0]),(world_coords[1]-world_coords[0]));
        n.normalize();
        float intensity = dot(n, light_dir);
        //if (intensity>0) {
            //triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
            triangle_zbuffer(face, shader, zbuffer, image, intensity);
        //}
    }
    // for (int i=0; i<model->nfaces(); i++) { 
	//     std::vector<int> face = model->face(i); 
	//     Vec2i screen_coords[3]; 
	//     for (int j=0; j<3; j++) { 
	//         Vec3f world_coords = model->vert(face[j]); 
	//         screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
	//     } 
	//     triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
	// }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output_normalmap3.tga");
    zbuffer.flip_vertically();
    zbuffer.write_tga_file("zbuffer.tga");
    delete model;
    return 0;
}

