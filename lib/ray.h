/*************************************************************************
	> File Name: ray.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Wed 20 Apr 2022 09:00:30 PM CST
 ************************************************************************/

#ifndef _RAY_H
#define _RAY_H

#include"vec3.h"

class ray {
public:
	ray() {}
	ray(const vec3& origin, const vec3& direction, double time = 0.0) 
		: orig(origin), dir(direction), tm(time) {}
	vec3 origin() const { return orig; }
	vec3 direction() const { return dir; }
	double time() const { return tm; }
	vec3 at(double t) const {
		return orig + t*dir;
	}

private:
	vec3 orig;
	vec3 dir;
	double tm;
};


#endif
