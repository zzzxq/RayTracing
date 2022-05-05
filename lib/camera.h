/*************************************************************************
	> File Name: camera.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Thu 21 Apr 2022 04:09:15 PM CST
 ************************************************************************/

#ifndef _CAMERA_H
#define _CAMERA_H

#include"rtweekend.h"

class camera {
public:
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect, 
			double aperture, double focus_dist, double t0 = 0, double t1 = 0) {  //vfov表示上下视角，top to bottom, aperture小孔直径
		origin = lookfrom;
		auto theta = degrees_to_radians(vfov);
		auto half_height = tan(theta / 2);
		auto half_width = aspect * half_height;
		lens_radius = aperture / 2;
		time0 = t0;
		time1 = t1;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
        v = cross(w, u);

		//lower_left_corner = vec3(-half_width, -half_height, -1.0);
		lower_left_corner = origin - half_width * u * focus_dist - half_height * v * focus_dist - w * focus_dist;

		horizontal = 2 * half_width * u * focus_dist;
		vertical = 2 * half_height * v * focus_dist;
	}
	ray get_ray(double s, double t) {
		vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset, 
		lower_left_corner + s * horizontal + t * vertical - origin - offset,
		random_double(time0, time1));
	}
public:
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;	
	vec3 u, v, w;
	double lens_radius;
	double time0, time1;

};


#endif
