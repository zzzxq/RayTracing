/*************************************************************************
	> File Name: draw.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Thu 28 Apr 2022 10:35:40 AM CST
 ************************************************************************/

#ifndef _DRAW_H
#define _DRAW_H
#include<iostream>
#include<fstream>
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "ray.h"
#include "moving_sphere.h"
#include "bvh.h"

const int image_width = 800;
const int image_height = 600;
const int max_depth = 50; 
int *framebuffer;

vec3 ray_color(const ray& r, const hittable& world, int depth);
hittable_list random_scene();

void RGB2Color(int &c, const int &r, const int &g, const int &b)
{
    c = (r << 16) | (g << 8) | b;
}

void DrawPixel(int x, int y, int c)
{

    y = image_height - y - 1; //图像的坐标y从 image_height到0， 计算存储的时候要倒过来

    framebuffer[y * image_width + x] = c;
}

void DrawPixel(const int &x, const int &y, const int &r, const int &g, const int &b)
{
    int c;
    RGB2Color(c, r, g, b);
    DrawPixel(x, y, c);
}
void Color2RGB(const int &c, int &r, int &g, int &b)
{
    r = (0xff << 16 & c) >> 16;
    g = (0xff << 8 & c) >> 8;
    b = 0xff & c;
}

void Framebuffer2File(int nx, int ny, int ns, int *fb, std::ofstream &outFile, float &progressDone)
{
    for (int j = (ny - 1); j >= 0; j--)
    {
        progressDone = float(ny - 1 - j) / (ny - 1);
        for (int i = 0; i < nx; i++)
        {
            int x = i;
            int y = ny - 1 - j;
            int c = fb[y * image_width + x];
            int r, g, b = 0;

            Color2RGB(c, r, g, b);
            //std::cout << j << "," << i << std::endl;
            outFile << r << " " << g << " " << b << std::endl;
        }
    }
    std::cout << "done" << std::endl;

}




vec3 ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0) return vec3(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        vec3 attenuation;
        //vec3 target = rec.p + rec.normal + random_unit_vector();
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return vec3(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    world.add(make_shared<sphere>(
        vec3(0,-1000,0), 1000, make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));

    
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    world.add(make_shared<moving_sphere>(
                        center, center + vec3(0, random_double(0, 0.5), 0), 0.0, 1.0, 0.2,
                        make_shared<lambertian>(albedo)));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
                } else {
                    // glass
                    world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }

    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        vec3(-4, 1, 0), 1.0, make_shared<lambertian>(vec3(0.4, 0.2, 0.1))));
    world.add(make_shared<sphere>(
        vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

#endif
