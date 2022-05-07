/*************************************************************************
	> File Name: moving_sphere.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Thu 28 Apr 2022 10:40:53 PM CST
 ************************************************************************/

#ifndef _MOVING_SPHERE_H
#define _MOVING_SPHERE_H
#include "hittable.h"
#include "ray.h"
#include "material.h"
#include<memory>
#include "vec3.h"
#include "aabb.h"
#include "sphere.h"
using std::shared_ptr;

class moving_sphere : public hittable {
public:
	moving_sphere() {}
	moving_sphere(vec3 cen0, vec3 cen1, double t0, double t1, double r, shared_ptr<material> m)
		: center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {}
	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;
	vec3 center(double time) const;
public:
	vec3 center0, center1;
	double time0, time1;
	double radius;
	shared_ptr<material> mat_ptr;
};
vec3 moving_sphere::center(double time) const{  //线性插值，根据时间获取物体的球心
    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}
bool moving_sphere::hit(
    const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(outward_normal, rec.u, rec.v);
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv(outward_normal, rec.u, rec.v);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    aabb box0(
        center(t0) - vec3(radius, radius, radius),
        center(t0) + vec3(radius, radius, radius));
    aabb box1(
        center(t1) - vec3(radius, radius, radius),
        center(t1) + vec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}


#endif
