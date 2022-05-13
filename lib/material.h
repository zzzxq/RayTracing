/*************************************************************************
	> File Name: material.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Fri 22 Apr 2022 12:03:08 PM CST
 ************************************************************************/

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "vec3.h"
#include "ray.h"
#include "rtweekend.h"
#include <memory>
#include "texture.h"

using std::shared_ptr;
using std::make_shared;

struct hit_record;
class material {
public:
	virtual vec3 emitted(double u, double v, const vec3& p) const {
        return vec3(0,0,0);
    }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
public:
	lambertian(shared_ptr<texture> a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
		vec3 scatter_direction = rec.normal + random_unit_vector();
		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);  //集中点的
		return true;
	}

public:
	shared_ptr<texture> albedo; //某种材质
};

class metal : public material {
public:
	metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
public:
	vec3 albedo;
	double fuzz;
};

class dielectric : public material {
public:
	dielectric(double ri) : ref_idx(ri) {}

	 virtual bool scatter(
		const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
	) const {
		attenuation = vec3(1.0, 1.0, 1.0);
		double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
		if (etai_over_etat * sin_theta > 1.0 ) {
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected, r_in.time());
			return true;
		}
		double reflect_prob = schlick(cos_theta, etai_over_etat);
		if (random_double() < reflect_prob)
		{
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}
		vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		scattered = ray(rec.p, refracted, r_in.time());
		return true;
	}

	double ref_idx;
};


class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
	) const override{
		return false;
	}

	virtual vec3 emitted(double u, double v, const vec3 &p) const override{
		return emit->value(u, v, p);
	}

public:
	shared_ptr<texture> emit;

};

class isotropic : public material {
public:
	isotropic(shared_ptr<texture> a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

public:
	shared_ptr<texture> albedo;
};





#endif
