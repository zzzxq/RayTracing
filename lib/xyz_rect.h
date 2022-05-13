/*************************************************************************
	> File Name: xy_rect.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Sun 08 May 2022 02:42:33 PM CST
 ************************************************************************/

#ifndef _XY_RECT_H
#define _XY_RECT_H

#include "hittable.h"
#include "material.h"
#include <memory>
#include "aabb.h"
#include "vec3.h"
using std::shared_ptr;

class xy_rect : public hittable {
public:
	xy_rect() {}
	xy_rect(double x0, double x1, double y0, double y1, double k, shared_ptr<material> mat)
		: x0(x0), x1(x1), y0(y0), y1(y1), k(k), mp(mat) {}
	virtual bool hit(const ray &r, double t0, double t1, hit_record& rec) const override;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
		output_box = aabb(vec3(x0, y0, k - 0.0001), vec3(x1, y1, k + 0.0001));
		return true;
	}
public:
	double x0, x1, y0, y1, k;
	shared_ptr<material> mp;
};

bool xy_rect::hit(const ray& r, double t0, double t1, hit_record& rec) const {
	auto t = (k - r.origin().z()) / r.direction().z();
	if (t < t0 || t > t1) 
		return false;
	auto x = r.origin().x() + t * r.direction().x();
	auto y = r.origin().y() + t * r.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1) 
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	vec3 outward_normal = vec3(0, 0, 1);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	return true;



}

class xz_rect: public hittable {
	public:
		xz_rect() {}

		xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, shared_ptr<material> mat)
			: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

		virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;

		virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
			output_box =  aabb(vec3(x0,k-0.0001,z0), vec3(x1, k+0.0001, z1));
			return true;
		}

	public:
		double x0, x1, z0, z1, k;
		shared_ptr<material> mp;

};

class yz_rect: public hittable {
	public:
		yz_rect() {}

		yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, shared_ptr<material> mat)
			: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

		virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;

		virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
			output_box =  aabb(vec3(k-0.0001, y0, z0), vec3(k+0.0001, y1, z1));
			return true;
		}

	public:
		double y0, y1, z0, z1, k;
		shared_ptr<material> mp;

};
 bool xz_rect::hit(const ray& r, double t0, double t1, hit_record& rec) const {
	auto t = (k-r.origin().y()) / r.direction().y();
	if (t < t0 || t > t1)
		return false;
	auto x = r.origin().x() + t*r.direction().x();
	auto z = r.origin().z() + t*r.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x-x0)/(x1-x0);
	rec.v = (z-z0)/(z1-z0);
	rec.t = t;
	vec3 outward_normal = vec3(0, 1, 0);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	return true;
}

bool yz_rect::hit(const ray& r, double t0, double t1, hit_record& rec) const {
	auto t = (k-r.origin().x()) / r.direction().x();
	if (t < t0 || t > t1)
		return false;
	auto y = r.origin().y() + t*r.direction().y();
	auto z = r.origin().z() + t*r.direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y-y0)/(y1-y0);
	rec.v = (z-z0)/(z1-z0);
	rec.t = t;
	vec3 outward_normal = vec3(1, 0, 0);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	return true;
}

class flip_face : public hittable {
	public:
		flip_face(shared_ptr<hittable> p) : ptr(p) {}

		virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
			if (!ptr->hit(r, t_min, t_max, rec))
				return false;

			rec.front_face = !rec.front_face;
			return true;
		}

		virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
			return ptr->bounding_box(t0, t1, output_box);
		}

	public:
		shared_ptr<hittable> ptr;
};

class box : public hittable {
public:
	box() {}
	box(const vec3& p0, const vec3& p1, shared_ptr<material> ptr);
	virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb &output_box) const {
		output_box = aabb(box_min, box_max);
		return true;
	}
public:
	vec3 box_min;
	vec3 box_max;
	hittable_list sides;
};

box::box(const vec3& p0, const vec3 &p1, shared_ptr<material> ptr) {
	box_min = p0;
	box_max = p1;

	sides.add(std::make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	sides.add(std::make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    sides.add(std::make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(std::make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    sides.add(std::make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(std::make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool box::hit(const ray& r, double t0, double t1, hit_record& rec) const {
	return sides.hit(r, t0, t1, rec);
}

class translate : public hittable {
public:
	translate(shared_ptr<hittable> p, const vec3& displacement)
		:ptr(p), offset(displacement) {}
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record &rec) const override;
	virtual bool bounding_box(double t0, double t1, aabb &output_box) const;
public:
	shared_ptr<hittable> ptr;  //指向相应的需要移动的物体
	vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {  //不移动物体，通过移动光线达到移动物体的效果
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (!ptr->hit(moved_r, t_min, t_max, rec))  //调用相应面的hit 函数，用反向移动的光线，判断是否相交
		return false;
	rec.p += offset;  //将最后相交的点正向移动，就是最后的结果
	rec.set_face_normal(moved_r, rec.normal);  //根据光线调整面的法向
	return true;
}

bool translate::bounding_box(double t0, double t1, aabb& output_box) const {
	if (!ptr->bounding_box(t0, t1, output_box)) 
		return false;
	output_box = aabb(output_box.min() + offset, output_box.max() + offset);
	return true;
}

class rotate_y : public hittable {
public:
	rotate_y(shared_ptr<hittable> p, double angle);
	virtual bool hit(const ray &r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
		output_box = bbox;
		return hasbox;
	}

public:
	shared_ptr<hittable> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	aabb bbox;
};
rotate_y::rotate_y(shared_ptr<hittable> p, double angle) :ptr(p) {
	auto radians = degrees_to_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);

	vec3 min( infinity,  infinity,  infinity);
	vec3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

				auto newx =  cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;  //绕着y轴旋转的变换

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = ffmin(min[c], tester[c]);
                    max[c] = ffmax(max[c], tester[c]);
                }
			}
		}
	} 
	bbox = aabb(min, max);
}
bool rotate_y::hit(const ray &r, double t_min, double t_max, hit_record& rec) const {
	vec3 origin = r.origin();
    vec3 direction = r.direction();

    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];  //光线反反向旋转，模拟出物体旋转

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    vec3 p = rec.p;
    vec3 normal = rec.normal;  //计算出旋转后的光线和原物体的交点和法线

	p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

    normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];  //将交点和法线旋转后。得到最终的交点和法线

	rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}

class constant_medium : public hittable {
public:
	constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a) 
		: boundary(b), neg_inv_density(-1 / d) {
			phase_function = std::make_shared<isotropic>(a);
		} 

		virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
		virtual bool bounding_box(double t0, double t1, aabb& output_box) const{
			return boundary->bounding_box(t0, t1, output_box);
		}
public:
	shared_ptr<hittable> boundary;
	shared_ptr<material> phase_function;
	double neg_inv_density;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record rec1, rec2;
	if (!boundary->hit(r, -infinity, infinity, rec1)) {  //进入体积体的时间
		return false;
	}
	if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2)) {  //离开体积体的时间
		return false;
	}
	if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;
	if (rec1.t >= rec2.t)
		return false;
	if (rec1.t < 0)
		rec1.t = 0;
	
	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length; //在体积体内的传播距离
	const auto hit_distance = neg_inv_density * log(random_double());  //光线散射的传输距离

	if (hit_distance > distance_inside_boundary) //散射距离大于传播距离，代表传播出体积体，采样到头
		return false;
	rec.t = rec1. t + hit_distance / ray_length; 
	rec.p = r.at(rec.t);

	rec.normal = vec3(1,0,0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat_ptr = phase_function;

    return true;

}


#endif
