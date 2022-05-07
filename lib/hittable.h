/*************************************************************************
	> File Name: hittable.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Thu 21 Apr 2022 10:14:24 AM CST
 ************************************************************************/

#ifndef _HITTABLE_H
#define _HITTABLE_H
#include "ray.h"
#include <memory>
#include "aabb.h"


class material;

struct hit_record {
	vec3 p;
	vec3 normal;
	std::shared_ptr<material> mat_ptr;
	double t;
	double u;
	double v; //uv采样点
	bool front_face;
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;  //从物体内到外的射线，此时与法线dot大于0， 所以法线取相反
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record &rec) const = 0;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
};




#endif
