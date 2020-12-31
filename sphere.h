//
// Created by ryu19 on 2020/12/30.
//

#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include "hittable.h"
#include "ray.h"

class sphere: public hittable {
public:
    sphere(){}
    sphere(point3 cen,double r,shared_ptr<material> m):center(cen),radius(r),mat_ptr(m){};
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
private:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd)/a;
    //近い方の交点
    if(root < t_min || t_max<root){
        //近い方はレンジ外なので遠い方の交点
        root = (-half_b + sqrtd)/a;
        if(root < t_min || t_max<root){
            return false;
        }
    }
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    return true;
}

#endif //RAYTRACING_SPHERE_H