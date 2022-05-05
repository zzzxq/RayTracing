/*************************************************************************
	> File Name: aabb.h
	> Author: zxq
	> Mail: 1459190705@qq.com
	> Created Time: Sat 30 Apr 2022 10:17:52 AM CST
 ************************************************************************/

#ifndef _AABB_H
#define _AABB_H
#include "rtweekend.h"
#include "vec3.h"
#include "ray.h"
class aabb {
public:
	aabb() {}
	aabb(const vec3& a, const vec3& b) : _min(a), _max(b) {}
	vec3 min() const { return _min; }
	vec3 max() const { return _max; }

	bool hit(const ray& r, double tmin, double tmax) const {
		for (int i = 0; i < 3; ++i) {
			auto invD = 1.0f / r.direction()[i];
			double t0 = (_min[i] - r.origin()[i]) * invD;
			double t1 = (_max[i] - r.origin()[i]) * invD;

			if (invD < 0.0f) 
				std::swap(t0, t1);

			tmin = t0 > tmin ? t0 : tmin;  //进入时间取两个里面最大的
			tmax = t1 < tmax ? t1 : tmax;  //离开时间取两个里面最小的

			if (tmax <= tmin) 
				return false;
		}
		return true;  //没有考虑t<0的情况
	}
public:
	vec3 _min;
	vec3 _max;
};
aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(ffmin(box0.min().x(), box1.min().x()),
			ffmin(box0.min().y(), box1.min().y()),
			ffmin(box0.min().z(), box1.min().z()));
	vec3 big(ffmax(box0.max().x(), box1.max().x()),
			ffmax(box0.max().y(), box1.max().y()),
			ffmax(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}



#endif
