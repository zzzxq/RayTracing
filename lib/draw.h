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
#include "texture.h"
#include <memory>
#include "xyz_rect.h"


using std::shared_ptr;
using std::make_shared;


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

vec3 ray_color(const ray& r, const vec3& background, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0)
        return vec3(0, 0, 0);
    
    if (!world.hit(r, 0.001, infinity, rec))
        return background;
    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p); //自发光项
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))  //光源材质，返回自发光项
        return emitted;
    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}



hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(
        make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
        make_shared<constant_texture>(vec3(0.9, 0.9, 0.9))
    );

    world.add(make_shared<sphere>(vec3(0,-1000,0), 1000, make_shared<lambertian>(checker)));    
    
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = make_shared<constant_texture>(vec3::random() * vec3::random());
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
        vec3(-4, 1, 0), 1.0, make_shared<lambertian>(make_shared<constant_texture>(vec3(0.4, 0.2, 0.1)))));
    world.add(make_shared<sphere>(
        vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}


hittable_list two_spheres() {
    hittable_list objects;
    auto checker = make_shared<checker_texture>(
        make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
        make_shared<constant_texture>(vec3(0.9, 0.9, 0.9)));

    objects.add(make_shared<sphere>(vec3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(vec3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}
hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(vec3(0,-1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list NewSpheres() {
    hittable_list world;
    auto checker = make_shared<checker_texture>(
        make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
        make_shared<constant_texture>(vec3(0.9, 0.9, 0.9)));

    world.add(make_shared<sphere>(vec3(0,-1000,0), 1000, make_shared<lambertian>(checker)));    


    auto earth_texture = make_shared<image_texture>("/home/zxq/RayTracing/texture/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    
    world.add(make_shared<sphere>(vec3(0,1,-3), 1, earth_surface));    

    auto land_texture = make_shared<image_texture>("/home/zxq/RayTracing/texture/land.jpg");
    auto land_surface = make_shared<lambertian>(land_texture);
    world.add(make_shared<sphere>(vec3(2,1,-2), 1, land_surface));


    auto snow_texture = make_shared<image_texture>("/home/zxq/RayTracing/texture/snow.jpg");
    auto snow_surface = make_shared<lambertian>(snow_texture);
    
    world.add(make_shared<sphere>(vec3(4,1,-1), 1, snow_surface)); 

    auto coal_texture = make_shared<image_texture>("/home/zxq/RayTracing/texture/coal.jpg");
    auto coal_surface = make_shared<lambertian>(coal_texture);
    
    world.add(make_shared<sphere>(vec3(6,1,0), 1, coal_surface)); 

    // auto difflight = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(4,4,4)));
    // world.add(make_shared<sphere>(vec3(-1,4,-3), 0.5, difflight));
    // world.add(make_shared<xy_rect>(4, 6, 0, 2, -2, difflight));


    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
    auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    auto green = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
    auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(15, 15, 15)));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));

    
    shared_ptr<hittable> box1 = make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1,  15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);



    return objects;
}

hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
    auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    auto green = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
    auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(7, 7, 7)));

    objects.add(make_shared<yz_rect>(  0, 555,   0, 555, 555, green));
    objects.add(make_shared<yz_rect>(  0, 555,   0, 555,   0,   red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(  0, 555,   0, 555, 555, white));
    objects.add(make_shared<xz_rect>(  0, 555,   0, 555,   0, white));
    objects.add(make_shared<xy_rect>(  0, 555,   0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(vec3(0,0,0), vec3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1,  15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    objects.add(
        make_shared<constant_medium>(box1, 0.01, make_shared<constant_texture>(vec3(0,0,0))));
    objects.add(
        make_shared<constant_medium>(box2, 0.01, make_shared<constant_texture>(vec3(1,1,1))));

    return objects;
}

hittable_list final_scene() {
    hittable_list boxes1;
    auto ground =
        make_shared<lambertian>(make_shared<constant_texture>(vec3(0.48, 0.83, 0.53)));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(vec3(x0,y0,z0), vec3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(7, 7, 7)));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = vec3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material =
        make_shared<lambertian>(make_shared<constant_texture>(vec3(0.7, 0.3, 0.1)));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(vec3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        vec3(0, 150, 145), 50, make_shared<metal>(vec3(0.8, 0.8, 0.9), 10.0)
    ));

    auto boundary = make_shared<sphere>(vec3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(
        boundary, 0.2, make_shared<constant_texture>(vec3(0.2, 0.4, 0.9))
    ));
    boundary = make_shared<sphere>(vec3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(
        boundary, .0001, make_shared<constant_texture>(vec3(1,1,1))));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("/home/zxq/RayTracing/texture/earthmap.jpg"));
    objects.add(make_shared<sphere>(vec3(400,200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(vec3(220,280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(vec3::random(0,165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
            vec3(-100,270,395)
        )
    );

    return objects;
}
#endif
