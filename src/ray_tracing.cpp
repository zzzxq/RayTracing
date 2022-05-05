/*************************************************************************
	> File Name: ray_tracing.cpp
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Wed 20 Apr 2022 03:07:19 PM CST
 ************************************************************************/

#include <iostream>
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "ray.h"
#include <thread>
#include <vector>
#include "draw.h"
#include <fstream>
#include <cmath>

int numPixelTotal;
int numThread = 1;
float progressDone = 0.0f;
int numPixelRendered = 0;
const int samples_per_pixel = 100;

void RayTracingInOneThread(int k, camera& cam, hittable_list& world)
{

    for (int j = image_height-k; j >= 0; j -= numThread) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);

                color += ray_color(r, world, max_depth);
            }
            auto scale = 1.0 / samples_per_pixel;
            auto r = static_cast<int>(256 * clamp(sqrt(scale * color.x()), 0.0, 0.999));
            auto g = static_cast<int>(256 * clamp(sqrt(scale * color.y()), 0.0, 0.999));
            auto b = static_cast<int>(256 * clamp(sqrt(scale * color.z()), 0.0, 0.999));
            DrawPixel(i, j, r, g, b);
            numPixelRendered++;
        }
    }
}

void RayTracing(camera& cam, hittable_list& world)
{

	framebuffer = new int[image_width * image_height];


    std::ofstream outFile("output_" + std::to_string(image_width) + "x" + std::to_string(image_height) + ".ppm");
    outFile << "P3\n"
            << image_width << " " << image_height << "\n255\n";
    std::vector<std::thread> threads;
    numPixelTotal = image_width * image_height;

    for (int k = 1; k <= numThread; k++)
    {
        threads.push_back(std::thread(RayTracingInOneThread, k, std::ref(cam), std::ref(world)));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
    Framebuffer2File(image_width, image_height, max_depth, framebuffer, outFile, progressDone);
    return;
}


int main() {
    
    //std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    auto aspect_ratio = (double)image_width / image_height;
    //camera cam(vec3(-2,2,1), vec3(0,0,-1), vec3(0, 1, 0), 90, aspect_ratio);
    
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);


    auto world = random_scene();
    //Camera
    RayTracing(cam, world);
    
}

