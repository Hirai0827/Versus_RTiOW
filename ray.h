//
// Created by ryu19 on 2020/12/30.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "vec3.h"
class ray {
public:
    ray() {}
    ray(const point3& origin,const vec3& direction,const double time = 0.0): orig(origin),dir(direction),tm(time){}
    point3 origin() const {return orig;}
    vec3 direction() const {return dir;}
    double time() const { return tm;}
    point3 at(double t) const {
        return orig + t * dir;
    }
private:
    point3 orig;
    point3 dir;
    double tm;
};

#endif //RAYTRACING_RAY_H
